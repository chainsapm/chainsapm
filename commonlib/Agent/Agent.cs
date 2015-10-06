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
using ChainsAPM.Interfaces.Data;
using ChainsAPM.Models.Definitions;
using ChainsAPM.Server;

namespace ChainsAPM.Agent {
        /// <summary>
        /// The Agent class is the in memory representation of our remote agent. This class will handle all commands to and from the Agent. This class is responsible for creating the callstacks, processing events, thread tracking, etc...
        /// </summary>
        public partial class Agent : IAgentEvents, IConnectedObject {
                public Config.AgentConfig AgentConfig { get; private set; }
                public IConnectionHandler ConnectionHandler { get; set; }
                public IServerEvents ServerEvents { get; set; }
                public AgentInformation AgentInfo { get; set; }

                private IDataAdapter DataStorage { get; set; }
                private IConfigDataAdapter ConfigAdapter { get; set; }

                public Dictionary<long, long> ThreadDepth { get; set; }
                public Dictionary<long, Stack<ChainsAPM.Models.EntryPoint>> ThreadEntryPointStack { get; set; }

                public DateTime ConnectedTime { get; set; }
                public DateTime DisconnectedTime { get; set; }

                public Dictionary<long, Method> MethodList { get; set; }
                public Dictionary<long, Class> ClassList { get; set; }
                public Dictionary<long, Assembly> AssemblyList { get; set; }
                public Dictionary<long, Module> ModuleList { get; set; }

                private long messagesRecvd;

                public event ConnectionEvent AgentConnected;
                public event ConnectionEvent AgentDisconnected;

                public System.Reactive.Subjects.ISubject<Agent> AgentSubscription;

                public ByteCommandLocator CommandProcessor { get; private set; }

                private List<ICommand<byte>> bigList = new List<ICommand<byte>> ();

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

                public Agent (IConnectionHandler connectionHandler, IServerEvents serverEvents, IDataAdapter dataadapter, IConfigDataAdapter configdata) : this () {
                        ConfigAdapter = configdata;
                        ConnectionHandler = connectionHandler;
                        ServerEvents = serverEvents;
                        DataStorage = dataadapter;
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
                                bigList.AddRange (arr);
                                // set up command processor
                                foreach ( var item in arr ) {

                                        System.Threading.Interlocked.Increment (ref messagesRecvd);
                                        if ( item != null ) {

                                                if ( item is DefineMethod ) {
                                                        Process (item as DefineMethod);
                                                }

                                                if ( item is AgentInformation ) {
                                                        Process (item as AgentInformation);
                                                }

                                                if ( item is MethodEnter ) {
                                                        Process (item as MethodEnter);
                                                }

                                                if ( item is MethodExit ) {
                                                        Process (item as MethodExit);
                                                }



                                                if ( item is Commands.Common.SendString ) {
                                                        var it = item as Commands.Common.SendString;
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
