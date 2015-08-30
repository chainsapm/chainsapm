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
using ChainsAPM.Server;

namespace ChainsAPM.Agent {
        public partial class Agent : IAgentEvents {
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
                        ConnectionHandler.Disconnected += Disconnected;
                        ConnectionHandler.Start ();

                }

                private void Disconnected (object sender) {
                }

                public void HasData (object sender) {
                        if ( sender == ConnectionHandler ) {
                                var arr = ConnectionHandler.GetCommands ();

                                // set up command processor
                                foreach ( var item in arr ) {

                                        System.Threading.Interlocked.Increment (ref messagesRecvd);
                                        if ( item != null ) {

                                                if ( item is ChainsAPM.Commands.Agent.DefineMethod ) {
                                                        Process (item as ChainsAPM.Commands.Agent.DefineMethod);
                                                }

                                                if ( item is ChainsAPM.Commands.Agent.AgentInformation ) {
                                                        Process (item as ChainsAPM.Commands.Agent.AgentInformation);
                                                }

                                                if ( item is ChainsAPM.Commands.Agent.MethodEnter ) {
                                                        Process (item as ChainsAPM.Commands.Agent.MethodEnter);
                                                }

                                                if ( item is ChainsAPM.Commands.Agent.MethodExit ) {
                                                        Process (item as ChainsAPM.Commands.Agent.MethodExit);
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
}
