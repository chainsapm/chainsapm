using ChainsAPM.Commands;
using ChainsAPM.Communication.Tcp;
using ChainsAPM.Interfaces;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Reactive.Linq;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Server
{
    public delegate void ConnectionEvent(string ConnectionName, System.Net.EndPoint endpoint);

    public class Server : IServerEvents, IAgentEvents
    {
        object lockConsole = new object();

        private System.Collections.Concurrent.ConcurrentDictionary<long, Agent.Agent> concurrentAgentHandlerList;


        long clientsConnected = 0;
        long messagesRecvd = 0;

        public event ConnectionEvent ServerListening;
        public event ConnectionEvent ServerShutdown;

        public event ConnectionEvent AgentConnected;
        public event ConnectionEvent AgentDisconnected;

        public long ClientsConnected { get { return clientsConnected; } }
        public long MessagesReceived { get { return messagesRecvd; } }



        private System.Net.Sockets.TcpListener tcpListen;

        System.Threading.WaitCallback wCb;

        private Server()
        {
            wCb = new System.Threading.WaitCallback(TimerCallback);
            concurrentAgentHandlerList = new System.Collections.Concurrent.ConcurrentDictionary<long, Agent.Agent>();
            System.Runtime.GCSettings.LatencyMode = System.Runtime.GCLatencyMode.LowLatency;
        }


        public Server(int port) : this(System.Net.IPAddress.Any, port)
        {
        }

        public Server(System.Net.IPAddress ip, int port) : this(new System.Net.IPEndPoint(ip, port))
        {
        }

        public Server(System.Net.IPEndPoint ip) : this()
        {
            tcpListen = new System.Net.Sockets.TcpListener(ip);
        }

        public void Start()
        {
            tcpListen.Start(200);
            //listenTimer = new System.Threading.Timer(TimerCallback, tcpListen, 0, 100);
            System.Threading.ThreadPool.QueueUserWorkItem(wCb, tcpListen);
            if (ServerListening != null)
                ServerListening(Environment.MachineName, tcpListen.LocalEndpoint);

        }

        public void Stop()
        {
            tcpListen.Stop();


        }

        private void TurnOnGCNotification()
        {
            //GC.RegisterForFullGCNotification(30, 50);
            //var timerCheck = new System.Threading.Thread(new System.Threading.ThreadStart(() =>
            //{
            //    Console.WriteLine("GC Notification thread started.");
            //    while (true)
            //    {
            //        GCNotificationStatus s = GC.WaitForFullGCApproach();
            //        if (s == GCNotificationStatus.Succeeded)
            //        {
            //            Console.WriteLine("GC Notification raised.");
            //            foreach (var item in concurrentAgentHandlerList)
            //            {
            //                item.Value.PauseTimers();
            //            }

            //        }
            //        else if (s == GCNotificationStatus.Canceled)
            //        {
            //            Console.WriteLine("GC Notification cancelled.");
            //            break;
            //        }
            //        else
            //        {
            //            // This can occur if a timeout period 
            //            // is specified for WaitForFullGCApproach(Timeout)  
            //            // or WaitForFullGCComplete(Timeout)   
            //            // and the time out period has elapsed. 
            //            Console.WriteLine("GC Notification not applicable.");
            //            break;
            //        }

            //        // Check for a notification of a completed collection.
            //        s = GC.WaitForFullGCComplete(500);
            //        if (s == GCNotificationStatus.Succeeded)
            //        {
            //            Console.WriteLine("Full GC Complete");
            //            foreach (var item in concurrentAgentHandlerList)
            //            {
            //                item.Value.RestartTimers();
            //            }
            //        }
            //        else if (s == GCNotificationStatus.Canceled)
            //        {
            //            Console.WriteLine("GC Notification cancelled.");
            //            break;
            //        }
            //        else if (s == GCNotificationStatus.Timeout || s == GCNotificationStatus.NotApplicable)
            //        {
            //            foreach (var item in concurrentAgentHandlerList)
            //            {
            //                item.Value.RestartTimers();
            //            }
            //        }
            //        else
            //        {
            //            // Could be a time out.
            //            Console.WriteLine("GC Notification not applicable.");
            //            break;
            //        }


            //        System.Threading.Thread.Sleep(500);
            //        // FinalExit is set to true right before   
            //        // the main thread cancelled notification.
            //    }


            //}));
            //timerCheck.Name = "GC Notification Thread";
            ////timerCheck.Start();
        }

        async private void TimerCallback(object objt)
        {
            var tcpListen = objt as System.Net.Sockets.TcpListener;
            tcpListen.Server.UseOnlyOverlappedIO = true;
            while (true)
            {
                try
                {

                    await tcpListen.AcceptTcpClientAsync()
                        .ContinueWith(async (tClient) =>
                    {
                        var client = await tClient;
                        System.Threading.Interlocked.Increment(ref clientsConnected);
                        Agent.Agent agent = new Agent.Agent(new TcpByteAgentHandler(new TcpByteTransport(client), TcpByteAgentHandler.HandlerType.ReceiveHeavy), this);
                        concurrentAgentHandlerList.GetOrAdd(agent.GetHashCode(), agent);
                        agent.AgentSubscription.Subscribe(ag =>
                        {
                            ag = ag as Agent.Agent;
                            Console.WriteLine("Agent {0} Connected from {1}.", ag.AgentInfo.AgentName, ag.AgentInfo.MachineName);
                        });
                    });
                   
                }
                catch (Exception)
                {
                    break;
                    //TODO Log fatal exception
                }
                finally
                {
                }
            }

        }

        private void tcbah_Disconnected(object sender)
        {

            System.Threading.Interlocked.Decrement(ref clientsConnected);
            var agent = sender as Agent.Agent;
            lock (lockConsole)
            {
                agent.DisconnectedTime = DateTime.Now;
                Console.WriteLine("Agent {0} disconnected. It was connected for {1}", agent.AgentInfo.AgentName, (agent.DisconnectedTime - agent.ConnectedTime));
            }

            var fstream = System.IO.File.CreateText(string.Format(@"C:\Logfiles\{0}_{1}.txt", agent.AgentInfo.AgentName, agent.ConnectedTime.ToFileTime()));
            // TODO FIX THIS TO INCLUDE THE PROPER ENTRYPOINTS AND METHOD LIST
            //foreach (var item in tcbah.ThreadEntryPoint)
            //{
            //    fstream.WriteLine("Starting Thread {0:X}", item.Key);
            //    foreach (var tpe_list in item.Value)
            //    {
            //        fstream.WriteLine("{0}{1}", "".PadLeft((int)tpe_list.Item1), tcbah.FunctionList[tpe_list.Item2]);
            //    }
            //}

            fstream.Flush();
            fstream.Close();
            Agent.Agent refOut = null;
            concurrentAgentHandlerList.TryRemove(agent.GetHashCode(), out refOut);

            if (refOut != null)
            {
                lock (lockConsole)
                {
                    Console.WriteLine("<<<<Agent {0} removed from list.", agent.AgentInfo.AgentName);
                }
                Task.Factory.StartNew(async () =>
                {
                    await Task.Delay(30000);
                    agent.ConnectionHandler.Disconnect();
                });
            }

        }

        private void tcbah_HasDataEvent(object sender)
        {
            var agent = sender as Agent.Agent;
            var arr = agent.ConnectionHandler.GetCommands();
            // set up command processor
            foreach (var item in arr)
            {

                System.Threading.Interlocked.Increment(ref messagesRecvd);
                if (item != null)
                {
                    if (item is ChainsAPM.Commands.Agent.DefineFunction)
                    {
                        var DefFunc = item as ChainsAPM.Commands.Agent.DefineFunction;
                        //TODO RELPACE WITH METHOD LIST
                        //tcbah.FunctionList.Add(DefFunc.FunctionID, DefFunc.FunctionName);
                    }

                    if (item is ChainsAPM.Commands.Agent.AgentInformation)
                    {
                        agent.AgentInfo = item as ChainsAPM.Commands.Agent.AgentInformation;
                        agent.ConnectedTime = DateTime.Now;
                        if (AgentConnected != null)
                            AgentConnected(agent.AgentInfo.AgentName, null);
                        var okCmd = new ChainsAPM.Commands.Common.SendString("OK!");
                        agent.ConnectionHandler.SendCommand(okCmd);
                    }
                    if (item is ChainsAPM.Commands.Agent.FunctionEnterQuick)
                    {
                        var feq = item as ChainsAPM.Commands.Agent.FunctionEnterQuick;
                        if (!agent.ThreadDepth.ContainsKey(feq.ThreadID))
                            agent.ThreadDepth.Add(feq.ThreadID, 0);

                        if (!agent.ThreadEntryPointStack.ContainsKey(feq.ThreadID))
                            agent.ThreadEntryPointStack.Add(feq.ThreadID,
                                new Stack<ChainsAPM.Models.EntryPoint>());
                        else
                        {
                            if (true)
                            {
                                agent.ThreadEntryPointStack[feq.ThreadID].Last();
                            }
                            agent.ThreadEntryPointStack[feq.ThreadID].Push(
                                new ChainsAPM.Models.EntryPoint()
                                {
                                    CurrentDepth = 0,
                                    Id = feq.FunctionID,
                                    OriginalTimeStamp = feq.TimeStamp.ToFileTimeUtc(),
                                    //TODO RELPACE WITH METHOD LIST
                                    //Name = agent.FunctionList[feq.FunctionID],
                                    Type = Models.StackItem.ItemType.Entry

                                });
                        }
                    }
                }

                if (item is ChainsAPM.Commands.Agent.FunctionLeaveQuick)
                {
                    var feq = item as ChainsAPM.Commands.Agent.FunctionLeaveQuick;
                    if (!agent.ThreadDepth.ContainsKey(feq.ThreadID))
                        agent.ThreadDepth.Add(feq.ThreadID, 0);

                    if (agent.ThreadDepth[feq.ThreadID] > 0)
                    {
                        agent.ThreadDepth[feq.ThreadID]--;
                    }

                    if (agent.ThreadDepth[feq.ThreadID] == 0)
                    {
                        using (var fw = new System.IO.StreamWriter(string.Format(@"C:\LogFiles\{0}_T{1}.txt", DateTime.Now.Ticks, feq.ThreadID)))
                        {
                            foreach (var StackItem in agent.ThreadEntryPointStack[feq.ThreadID])
                            {
                                // TODO FIX THIS TO PROPERLY UPDATE THE EXIT
                                //fw.WriteLine("{0}{1}", "".PadLeft((int)StackItem.Item1, ' '), StackItem.Item2);
                            }
                        }
                        agent.ThreadDepth.Remove(feq.ThreadID);
                    }

                }
                if (item is ChainsAPM.Commands.Common.SendString)
                {
                    var it = item as ChainsAPM.Commands.Common.SendString;
                    Console.WriteLine("Agent {0} has sent string {1}", agent.AgentInfo.AgentName, it.StringData);
                    if (((ChainsAPM.Commands.Common.SendString)item).StringData == "Done!")
                    {
                        //TODO add in proper shutdown commands
                        //agent.ConnectionHandler.SendCommand(stringCmd);
                        agent.ConnectionHandler.Dispose();
                    }
                }
            }

        }
    }
}
