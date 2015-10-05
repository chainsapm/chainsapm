using System;
using System.Collections.Generic;
using System.Linq;
using System.Reactive.Linq;
using System.Reactive.Subjects;
using System.Text;
using System.Threading.Tasks;
using ChainsAPM.Commands;
using ChainsAPM.Commands.Agent;
using ChainsAPM.Interfaces;
using ChainsAPM.Models;
using ChainsAPM.Models.Definitions;
using ChainsAPM.Server;

namespace ChainsAPM.Agent {
        partial class Agent : ICommandProcessor {
                public void Process(DefineMethod cmd) {
                        Class refClass = null;
                        if ( ClassList.ContainsKey(cmd.ClassID) ) {
                                refClass = ClassList[cmd.ClassID];
                        }
                        var MethodToAdd = new Method()
                        {
                                MethodName = cmd.MethodName,
                                MethodId = cmd.MethodDef,
                                Class = refClass
                        };
                        if ( !MethodList.ContainsKey(MethodToAdd.MethodId) ) {
                                MethodList.Add(MethodToAdd.MethodId, MethodToAdd);
                        }
                        if ( AgentConfig.InstrumentationPointCollection.ContainsKey(MethodToAdd) ) {
                                MethodToAdd.IsEntryPoint = AgentConfig.InstrumentationPointCollection[MethodToAdd].InstrumentationMethod.IsEntryPoint;
                                AgentConfig.InstrumentationPointCollection[MethodToAdd].InstrumentationMethod = MethodToAdd;
                        }
                }

                public void Process(DefineClass cmd) {
                        if ( !ClassList.ContainsKey(cmd.ClassID) ) {
                                ClassList.Add(cmd.ClassID, new Class()
                                {
                                        ClassId = cmd.ClassID,
                                        ClassName = cmd.ClassName
                                });
                        }


                }
                public void Process(MethodEnter cmd) {
                        if ( MethodList[cmd.MethodDef].ToString().Contains("Response") )
                                Console.WriteLine("Entered {0}\r\n", MethodList[cmd.MethodDef].ToString());

                        if ( !ThreadDepth.ContainsKey(cmd.ThreadID) )
                                ThreadDepth.Add(cmd.ThreadID, 0);
                        else
                                ThreadDepth[cmd.ThreadID]++;

                        if ( !ThreadEntryPointStack.ContainsKey(cmd.ThreadID) )
                                ThreadEntryPointStack.Add(cmd.ThreadID,
                                    new Stack<ChainsAPM.Models.EntryPoint>());

                        if ( AgentConfig.IsMethodEntryPoint(MethodList[cmd.MethodDef]) && ThreadEntryPointStack[cmd.ThreadID].Count == 0 ) {
                                var entry =
                                   new ChainsAPM.Models.EntryPoint()
                                   {
                                           CurrentDepth = 0,
                                           MethodDef = cmd.MethodDef,
                                           StackItemIdentifier = cmd.MethodDef,
                                           OriginalTimeStamp = cmd.TimeStamp.ToFileTimeUtc(),
                                           Started = cmd.TimeStamp,
                                           Name = MethodList.ContainsKey(cmd.MethodDef) ? MethodList[cmd.MethodDef].ToString() : cmd.MethodDef.ToString("X"),
                                           Type = Models.ItemType.Entry,
                                           AgentName = AgentInfo.AgentName,
                                           MachineName = AgentInfo.MachineName,
                                           ApplicationName = AgentConfig.ParentAgentGroup.ParentApplication.ConfigName

                                   };
                                DataStorage.InsertEntryPoint(entry);
                                ThreadEntryPointStack[cmd.ThreadID].Push(entry);
                        } else if ( ThreadEntryPointStack[cmd.ThreadID].Count > 0 ) {
                                ThreadEntryPointStack[cmd.ThreadID].Peek().UpdateStack(new Models.StackItemBase()
                                {
                                        MethodDef = cmd.MethodDef,
                                        StackItemIdentifier = cmd.MethodDef,
                                        OriginalTimeStamp = cmd.TimeStamp.ToFileTimeUtc(),
                                        Started = cmd.TimeStamp,
                                        Name = MethodList.ContainsKey(cmd.MethodDef) ? MethodList[cmd.MethodDef].ToString() : cmd.MethodDef.ToString("X"),
                                        Type = Models.ItemType.Entry
                                });

                        }
                }

                public void Process(MethodsToInstrument agentInfo) {
                        throw new NotImplementedException();
                }

                public void Process(MethodExit cmd) {
                        if ( MethodList[cmd.MethodDef].ToString().Contains("Response") )
                                Console.WriteLine("Exited {0}\r\n", MethodList[cmd.MethodDef].ToString());
                        if ( !ThreadDepth.ContainsKey(cmd.ThreadID) )
                                ThreadDepth.Add(cmd.ThreadID, 0);

                        if ( ThreadDepth[cmd.ThreadID] >= 0 ) {

                                var lastEntry = ThreadEntryPointStack[cmd.ThreadID].Peek();
                                lastEntry.UpdateStack(new Models.StackItemBase()
                                {
                                        MethodDef = cmd.MethodDef,
                                        StackItemIdentifier = cmd.MethodDef,
                                        OriginalTimeStamp = cmd.TimeStamp.ToFileTimeUtc(),
                                        Name = MethodList.ContainsKey(cmd.MethodDef) ? MethodList[cmd.MethodDef].ToString() : cmd.MethodDef.ToString("X"),
                                        Type = Models.ItemType.Exit
                                });

                                if ( ThreadDepth[cmd.ThreadID] > 0 )
                                        ThreadDepth[cmd.ThreadID]--;

                                if ( lastEntry.CurrentDepth == 0 && lastEntry.StackItemIdentifier == cmd.MethodDef ) {
                                        ThreadEntryPointStack[cmd.ThreadID].Pop();
                                        DataStorage.UpdateEntryPoint(lastEntry);
                                        using ( var fw = new System.IO.StreamWriter(string.Format(@"C:\LogFiles\{0}_T{1}.txt", DateTime.Now.Ticks, cmd.ThreadID)) ) {
                                                RecursiveWrite(lastEntry, fw);
                                        }
                                }


                        }
                }

                private void RecursiveWrite(IStackItem cmd, System.IO.StreamWriter fw) {
                        fw.WriteLine("{0:O}\t{2}Method: {1}\t\tElapsed: {3:0.000}ms", cmd.Started, cmd.Name, "".PadLeft((int)cmd.Depth, ' ').PadLeft((int)cmd.Depth, ' '), (cmd.Finished - cmd.Started).TotalMilliseconds);
                        if ( cmd.Children != null ) {
                                foreach ( var si in cmd.Children ) {
                                        RecursiveWrite(si, fw);
                                }
                        }

                }

                public void Process(DefineModule agentInfo) {
                        throw new NotImplementedException();
                }

                public void Process(AgentInformation cmd) {
                        ConnectedTime = DateTime.Now;
                        AgentInfo = cmd;

                        AgentConfig = ConfigAdapter.ReadAgentConfig(cmd);

                        AgentSubscription.OnNext(this);
                        var ListOfMethods = new List<string>();
                        var ListOfClasses = new List<string>();
                        var ListOfMethodSettings = new List<MethodsToInstrument.MethodProperties>();
                        var commandsList = new ChainsAPM.Commands.Agent.MethodsToInstrument(DateTime.Now.ToFileTimeUtc(), ListOfMethodSettings, ListOfClasses, ListOfMethods);

                        foreach ( var MethodToInstrument in AgentConfig.InstrumentationPointCollection ) {
                                commandsList.MethodClassList.Add(MethodToInstrument.Key.Class.ClassName);
                                commandsList.MethodList.Add(MethodToInstrument.Key.MethodName);
                                commandsList.MethodPropList.Add(MethodsToInstrument.MethodProperties.Public);
                        }


                        ConnectionHandler.SendCommand(commandsList);
                }

                public void Process(ACK cmd) {
                        throw new NotImplementedException();
                }

                public void Process (DefineInstrumentationMethods cmd) {
                        throw new NotImplementedException ();
                }

                public void Process (SendInjectionMetadata cmd) {
                        throw new NotImplementedException ();
                }
        }
}
