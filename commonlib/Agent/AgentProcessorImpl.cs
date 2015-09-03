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
        partial class Agent: ICommandProcessor {
                public void Process (DefineMethod cmd) {
                        //TODO RELPACE WITH METHOD LIST
                        MethodList.Add (cmd.MethodDef, new Method ()
                        {
                                MethodName = cmd.MethodName
                        });
                }

                public void Process (MethodEnter cmd) {
                        if ( !ThreadDepth.ContainsKey (cmd.ThreadID) )
                                ThreadDepth.Add (cmd.ThreadID, 0);

                        if ( !ThreadEntryPointStack.ContainsKey (cmd.ThreadID) )
                                ThreadEntryPointStack.Add (cmd.ThreadID,
                                    new Stack<ChainsAPM.Models.EntryPoint> ());

                        if ( ThreadDepth [cmd.ThreadID] > 0 ) {
                                ThreadEntryPointStack [cmd.ThreadID].Last ().UpdateStack (new Models.StackItemBase ()
                                {
                                        MethodDef = cmd.MethodDef,
                                        OriginalTimeStamp = cmd.TimeStamp.ToFileTimeUtc (),
                                        Started = cmd.TimeStamp,
                                        Name = MethodList.ContainsKey (cmd.MethodDef) ? MethodList [cmd.MethodDef].MethodName : cmd.MethodDef.ToString ("X"),
                                        Type = Models.ItemType.Entry
                                });
                        } else {
                                var entry =
                                    new ChainsAPM.Models.EntryPoint ()
                                    {
                                            CurrentDepth = 0,
                                            MethodDef = cmd.MethodDef,
                                            OriginalTimeStamp = cmd.TimeStamp.ToFileTimeUtc (),
                                            Started = cmd.TimeStamp,
                                            Name = MethodList.ContainsKey (cmd.MethodDef) ? MethodList [cmd.MethodDef].MethodName : cmd.MethodDef.ToString ("X"),
                                            Type = Models.ItemType.Entry

                                    };
                                DataStorage.InsertEntryPoint (entry);
                                ThreadEntryPointStack [cmd.ThreadID].Push (entry);
                        }
                        ThreadDepth [cmd.ThreadID]++;
                }

                public void Process (MethodsToInstrument agentInfo) {
                        throw new NotImplementedException ();
                }

                public void Process (MethodExit cmd) {
                        if ( !ThreadDepth.ContainsKey (cmd.ThreadID) )
                                ThreadDepth.Add (cmd.ThreadID, 0);

                        if ( ThreadDepth [cmd.ThreadID] >= 0 ) {


                                ThreadEntryPointStack [cmd.ThreadID].Last ().UpdateStack (new Models.StackItemBase ()
                                {
                                        MethodDef = cmd.MethodDef,
                                        OriginalTimeStamp = cmd.TimeStamp.ToFileTimeUtc (),
                                        Name = MethodList.ContainsKey (cmd.MethodDef) ? MethodList [cmd.MethodDef].MethodName : cmd.MethodDef.ToString ("X"),
                                        Type = Models.ItemType.Exit
                                });
                        }

                        if ( ThreadDepth [cmd.ThreadID] > 0 )
                                ThreadDepth [cmd.ThreadID]--;

                        if ( ThreadDepth [cmd.ThreadID] == 0 ) {

                                foreach ( var StackItem in ThreadEntryPointStack [cmd.ThreadID] ) {
                                        DataStorage.UpdateEntryPoint(StackItem);
                                }
                                using ( var fw = new System.IO.StreamWriter (string.Format (@"C:\LogFiles\{0}_T{1}.txt", DateTime.Now.Ticks, cmd.ThreadID)) ) {
                                        foreach ( var StackItem in ThreadEntryPointStack [cmd.ThreadID] ) {
                                                RecursiveWrite (StackItem, fw);
                                        }
                                }
                                ThreadDepth.Remove (cmd.ThreadID);
                        }

                }

                private void RecursiveWrite (IStackItem cmd, System.IO.StreamWriter fw) {
                        fw.WriteLine ("{0:O}\tMethod: {1}\tDepth:{2}\tElapsed: {3:0.000}ms", cmd.Started, cmd.Name, cmd.Depth, (cmd.Finished - cmd.Started).TotalMilliseconds);
                        if ( cmd.Children != null ) {
                                foreach ( var si in cmd.Children ) {
                                        RecursiveWrite (si, fw);
                                }
                        }

                }

                public void Process (DefineModule agentInfo) {
                        throw new NotImplementedException ();
                }

                public void Process (AgentInformation cmd) {
                        ConnectedTime = DateTime.Now;
                        AgentInfo = cmd;

                        ConfigAdapter.ReadAgentConfig (cmd);

                        AgentSubscription.OnNext (this);
                        var ListOfMethods = new List<string> ();
                        var ListOfClasses = new List<string> ();
                        var ListOfMethodSettings = new List<MethodsToInstrument.MethodProperties> ();
                        var commandsList = new ChainsAPM.Commands.Agent.MethodsToInstrument (DateTime.Now.ToFileTimeUtc (), ListOfMethodSettings, ListOfClasses, ListOfMethods);

                        foreach ( var MethodToInstrument in AgentConfig.InstrumentationPointCollection ) {
                                commandsList.MethodClassList.Add (MethodToInstrument.Key.Class.ClassName);
                                commandsList.MethodList.Add (MethodToInstrument.Key.MethodName);
                                commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);
                        }
                        

                        ConnectionHandler.SendCommand (commandsList);
                }

                public void Process (ACK cmd) {
                        throw new NotImplementedException ();
                }
        }
}
