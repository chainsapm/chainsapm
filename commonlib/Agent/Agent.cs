using ChainsAPM.Commands;
using ChainsAPM.Commands.Agent;
using ChainsAPM.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ChainsAPM.Server;
using System.Reactive.Linq;
using System.Reactive.Subjects;
using ChainsAPM.Models;

namespace ChainsAPM.Agent {
        public class Agent : IAgentEvents {
                public Config.AgentConfig AgentConfig { get; private set; }
                public IConnectionHandler ConnectionHandler { get; set; }
                public IServerEvents ServerEvents { get; set; }
                public AgentInformation AgentInfo { get; set; }

                public Dictionary<long, long> ThreadDepth { get; set; }
                public Dictionary<long, Stack<ChainsAPM.Models.EntryPoint>> ThreadEntryPointStack { get; set; }

                public DateTime ConnectedTime { get; set; }
                public DateTime DisconnectedTime { get; set; }

                public Dictionary<long, Models.Method> MethodList { get; set; }
                public Dictionary<long, Models.Class> ClassList { get; set; }
                public Dictionary<long, Models.Assembly> AssemblyList { get; set; }
                public Dictionary<long, Models.Module> ModuleList { get; set; }

                private long messagesRecvd;

                public event ConnectionEvent AgentConnected;
                public event ConnectionEvent AgentDisconnected;

                public System.Reactive.Subjects.ISubject<Agent> AgentSubscription;

                public ByteCommandLocator CommandProcessor { get; private set; }

                private Agent () {
                        ThreadDepth = new Dictionary<long, long> ();
                        ThreadEntryPointStack = new Dictionary<long, Stack<Models.EntryPoint>> ();
                        CommandProcessor = new ByteCommandLocator ();
                        CommandProcessor.CreateList ();
                        AgentSubscription = new Subject<Agent> ();
                        MethodList = new Dictionary<long, Method> (); // TODO set by agent cache
                        ClassList = new Dictionary<long, Class> (); // TODO set by agent cache
                        AssemblyList = new Dictionary<long, Assembly> (); // TODO set by agent cache
                        ModuleList = new Dictionary<long, Module> (); // TODO set by agent cache
                }

                public Agent (IConnectionHandler connectionHandler, IServerEvents serverEvents) : this () {
                        ConnectionHandler = connectionHandler;
                        ServerEvents = serverEvents;
                        ConnectionHandler.SetProcessor (CommandProcessor);
                        ConnectionHandler.HasData += HasData;

                        //ConnectionHandler.Disconnected += tcbah_Disconnected;
                        ConnectionHandler.Start ();

                }

                public void HasData (object sender) {
                        if ( sender == ConnectionHandler ) {
                                var arr = ConnectionHandler.GetCommands ();

                                // set up command processor
                                foreach ( var item in arr ) {

                                        System.Threading.Interlocked.Increment (ref messagesRecvd);
                                        if ( item != null ) {
                                                if ( item is ChainsAPM.Commands.Agent.DefineFunction ) {
                                                        var DefFunc = item as ChainsAPM.Commands.Agent.DefineFunction;
                                                        //TODO RELPACE WITH METHOD LIST
                                                        MethodList.Add (DefFunc.FunctionID, new Method ()
                                                        {
                                                                MethodName = DefFunc.FunctionName
                                                        });
                                                }

                                                if ( item is ChainsAPM.Commands.Agent.AgentInformation ) {
                                                        AgentInfo = item as ChainsAPM.Commands.Agent.AgentInformation;
                                                        ConnectedTime = DateTime.Now;
                                                        AgentSubscription.OnNext (this);
                                                        
                                                        var ListOfMethods = new List<string> ();
                                                        var ListOfMethodSettings = new List<SendListOfMethods.MethodSettings> ();
                                                        var commandsList = new ChainsAPM.Commands.Agent.SendListOfMethods (DateTime.Now.ToFileTimeUtc(), ListOfMethodSettings, ListOfMethods);
                                                        commandsList.ListOfMethods.Add ("mscorlib!System.Data::Open()");
                                                        commandsList.ListOfMethodSettings.Add (SendListOfMethods.MethodSettings.Method | SendListOfMethods.MethodSettings.Public);
                                                        commandsList.ListOfMethods.Add ("mscorlib!System.Data::Close()");
                                                        commandsList.ListOfMethodSettings.Add (SendListOfMethods.MethodSettings.Method | SendListOfMethods.MethodSettings.Public);
                                                        commandsList.ListOfMethods.Add ("mscorlib!System.Return::Reset()");
                                                        commandsList.ListOfMethodSettings.Add (SendListOfMethods.MethodSettings.Method | SendListOfMethods.MethodSettings.Private | SendListOfMethods.MethodSettings.Protected);
                                                        commandsList.ListOfMethods.Add ("mscorlib!BlahBlahBlah.Data::Hahah()");
                                                        commandsList.ListOfMethodSettings.Add (SendListOfMethods.MethodSettings.Method | SendListOfMethods.MethodSettings.Public | SendListOfMethods.MethodSettings.Assembly);
                                                        commandsList.ListOfMethods.Add ("mscorlib!System.Custom::Open()");
                                                        commandsList.ListOfMethodSettings.Add (SendListOfMethods.MethodSettings.Method);
                                                        ConnectionHandler.SendCommand (commandsList);
                                                }

                                                if ( item is ChainsAPM.Commands.Agent.FunctionEnterQuick ) {
                                                        var feq = item as ChainsAPM.Commands.Agent.FunctionEnterQuick;
                                                        if ( !ThreadDepth.ContainsKey (feq.ThreadID) )
                                                                ThreadDepth.Add (feq.ThreadID, 0);

                                                        if ( !ThreadEntryPointStack.ContainsKey (feq.ThreadID) )
                                                                ThreadEntryPointStack.Add (feq.ThreadID,
                                                                    new Stack<ChainsAPM.Models.EntryPoint> ());
                                                        else {
                                                                if ( ThreadDepth [feq.ThreadID] > 0 ) {
                                                                        ThreadEntryPointStack [feq.ThreadID].Last ().UpdateStack (new Models.StackItem ()
                                                                        {
                                                                                Id = feq.FunctionID,
                                                                                OriginalTimeStamp = feq.TimeStamp.ToFileTimeUtc (),
                                                                                Name = MethodList.ContainsKey (feq.FunctionID) ? MethodList [feq.FunctionID].MethodName : feq.FunctionID.ToString ("X"),
                                                                                Type = Models.StackItem.ItemType.Entry
                                                                        });
                                                                }
                                                                ThreadEntryPointStack [feq.ThreadID].Push (
                                                                    new ChainsAPM.Models.EntryPoint ()
                                                                    {
                                                                            CurrentDepth = 0,
                                                                            Id = feq.FunctionID,
                                                                            OriginalTimeStamp = feq.TimeStamp.ToFileTimeUtc (),
                                                                            Name = MethodList.ContainsKey (feq.FunctionID) ? MethodList [feq.FunctionID].MethodName : feq.FunctionID.ToString ("X"),
                                                                            Type = Models.StackItem.ItemType.Entry

                                                                    });
                                                        }
                                                }
                                        }

                                        if ( item is ChainsAPM.Commands.Agent.FunctionLeaveQuick ) {
                                                var feq = item as ChainsAPM.Commands.Agent.FunctionLeaveQuick;
                                                if ( !ThreadDepth.ContainsKey (feq.ThreadID) )
                                                        ThreadDepth.Add (feq.ThreadID, 0);

                                                if ( ThreadDepth [feq.ThreadID] > 0 ) {
                                                        ThreadDepth [feq.ThreadID]--;
                                                }

                                                if ( ThreadDepth [feq.ThreadID] == 0 ) {
                                                        using ( var fw = new System.IO.StreamWriter (string.Format (@"C:\LogFiles\{0}_T{1}.txt", DateTime.Now.Ticks, feq.ThreadID)) ) {
                                                                foreach ( var StackItem in ThreadEntryPointStack [feq.ThreadID] ) {
                                                                        // TODO FIX THIS TO PROPERLY UPDATE THE EXIT
                                                                        //fw.WriteLine("{0}{1}", "".PadLeft((int)StackItem.Item1, ' '), StackItem.Item2);
                                                                }
                                                        }
                                                        ThreadDepth.Remove (feq.ThreadID);
                                                }

                                        }
                                        if ( item is ChainsAPM.Commands.Common.SendString ) {
                                                var it = item as ChainsAPM.Commands.Common.SendString;
                                                Console.WriteLine ("Agent {0} has sent string {1}", AgentInfo.AgentName, it.StringData);
                                                if ( ((ChainsAPM.Commands.Common.SendString)item).StringData == "Done!" ) {
                                                        //TODO add in proper shutdown commands
                                                        //ConnectionHandler.SendCommand(stringCmd);
                                                        ConnectionHandler.Dispose ();
                                                }
                                        }
                                }
                        }
                }
        }
}
