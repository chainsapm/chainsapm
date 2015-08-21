using ChainsAPM.Communication.Tcp;
using ChainsAPM.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Server
{
    public delegate void ConnectionEvent(string ConnectionName, System.Net.EndPoint endpoint);

    public class Server
    {
        object lockConsole = new object();

        static System.Collections.Concurrent.ConcurrentDictionary<int, Agent.Agent> concurrentAgentHandlerList =
          new System.Collections.Concurrent.ConcurrentDictionary<int, Agent.Agent>();

        long clientsConnected = 0;
        long messagesRecvd = 0;

        public event ConnectionEvent ServerListening;
        public event ConnectionEvent AgentConnected;
        public event ConnectionEvent AgentDisconnected;

        public long ClientsConnected { get { return clientsConnected; } }
        public long MessagesReceived { get { return messagesRecvd; } }

        private System.Net.Sockets.TcpListener tcpListen;

        private Config.AgentConfig testAgentConfig;

        System.Threading.WaitCallback wCb;

        private Server()
        {
            wCb = new System.Threading.WaitCallback(TimerCallback);


            Dictionary<int, ChainsAPM.Interfaces.ICommand<byte>> CommandList
                = new Dictionary<int, ICommand<byte>>();

            var cmd1 = new ChainsAPM.Commands.Common.SendString("");
            var cmd2 = new ChainsAPM.Commands.Agent.FunctionEnterQuick(0, 0, 0);
            var cmd3 = new ChainsAPM.Commands.Agent.FunctionLeaveQuick(0, 0, 0);
            var cmd4 = new ChainsAPM.Commands.Agent.AgentInformation();
            var cmd5 = new ChainsAPM.Commands.Agent.FunctionTailQuick(0, 0, 0);
            var cmd6 = new ChainsAPM.Commands.Agent.DefineFunction(0, 0, "", 0);

            CommandList.Add(cmd1.Code, cmd1);
            CommandList.Add(cmd1.Code + 1, cmd1); // SendString handles Unicode and ASCII
            CommandList.Add(cmd2.Code, cmd2);
            CommandList.Add(cmd3.Code, cmd3);
            CommandList.Add(cmd4.Code, cmd4);
            CommandList.Add(cmd5.Code, cmd5);
            CommandList.Add(cmd6.Code, cmd6);
            CallContext.LogicalSetData("CommandProviders", CommandList);

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
            while (true)
            {
                try
                {
                    var tcpListen = objt as System.Net.Sockets.TcpListener;
                    var listenList = new List<Task<System.Net.Sockets.TcpClient>>();
                    tcpListen.Server.UseOnlyOverlappedIO = true;
                    while (tcpListen.Pending())
                    {
                        listenList.Add(tcpListen.AcceptTcpClientAsync());
                    }
                    bool restart = false;
                    for (int i = 0; i < listenList.Count; i++)
                    {
                        var item = listenList[i];
                        var listen = await item;
                        System.Threading.Interlocked.Increment(ref clientsConnected);
                        if (item.IsCompleted)
                        {
                            Agent.Agent tcbah = new Agent.Agent(new TcpByteAgentHandler(new TcpByteTransport(listen), TcpByteAgentHandler.HandlerType.ReceiveHeavy));

                            tcbah.ConnectionHandler.HasData += tcbah_HasDataEvent;
                            tcbah.ConnectionHandler.Disconnected += tcbah_Disconnected;
                            concurrentAgentHandlerList.GetOrAdd(tcbah.GetHashCode(), tcbah);
                        }

                        if (i == listenList.Count - 1 && restart == true)
                        {
                            i = 0;
                            restart = false;
                        }
                    }

                }
                catch (Exception)
                {
                    throw;
                }
                finally
                {
                }
                System.Threading.Thread.Sleep(10);
            }

        }

        private void tcbah_Disconnected(object sender)
        {

            System.Threading.Interlocked.Decrement(ref clientsConnected);
            var tcbah = sender as Agent.Agent;
            lock (lockConsole)
            {
                tcbah.DisconnectedTime = DateTime.Now;
                Console.WriteLine("Agent {0} disconnected. It was connected for {1}", tcbah.AgentInfo.AgentName, (tcbah.DisconnectedTime - tcbah.ConnectedTime));
            }

            var fstream = System.IO.File.CreateText(string.Format(@"C:\Logfiles\{0}_{1}.txt", tcbah.AgentInfo.AgentName, tcbah.ConnectedTime.ToFileTime()));
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
            concurrentAgentHandlerList.TryRemove(tcbah.GetHashCode(), out refOut);
            if (refOut != null)
            {
                lock (lockConsole)
                {
                    Console.WriteLine("<<<<Agent {0} removed from list.", tcbah.AgentInfo.AgentName);
                }
                Task.Factory.StartNew(async () =>
                {
                    await Task.Delay(30000);
                    tcbah.ConnectionHandler.Disconnect();
                });
            }

        }

        private void tcbah_HasDataEvent(object sender)
        {
            var tcbah = sender as Agent.Agent;
            var stringCmd = new ChainsAPM.Commands.Common.SendString("Done!");
            var arr = tcbah.ConnectionHandler.GetCommands();
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
                        tcbah.AgentInfo = item as ChainsAPM.Commands.Agent.AgentInformation;
                        tcbah.ConnectedTime = DateTime.Now;
                        if (AgentConnected != null)
                            AgentConnected(tcbah.AgentInfo.AgentName, null);
                        var okCmd = new ChainsAPM.Commands.Common.SendString("OK!");
                        tcbah.ConnectionHandler.SendCommand(okCmd);
                    }
                    if (item is ChainsAPM.Commands.Agent.FunctionEnterQuick)
                    {
                        var feq = item as ChainsAPM.Commands.Agent.FunctionEnterQuick;
                        if (!tcbah.ThreadDepth.ContainsKey(feq.ThreadID))
                            tcbah.ThreadDepth.Add(feq.ThreadID, 0);

                        if (!tcbah.ThreadEntryPointStack.ContainsKey(feq.ThreadID))
                            tcbah.ThreadEntryPointStack.Add(feq.ThreadID,
                                new Stack<ChainsAPM.Models.EntryPoint>());
                        else
                        {
                            if (true)
                            {
                                tcbah.ThreadEntryPointStack[feq.ThreadID].Last();
                            }
                            tcbah.ThreadEntryPointStack[feq.ThreadID].Push(
                                new ChainsAPM.Models.EntryPoint()
                                {
                                    CurrentDepth = 0,
                                    Id = feq.FunctionID,
                                    OriginalTimeStamp = feq.TimeStamp.ToFileTimeUtc(),
                                    //TODO RELPACE WITH METHOD LIST
                                    //Name = tcbah.FunctionList[feq.FunctionID],
                                    Type = Models.StackItem.ItemType.Entry

                                });
                        }
                    }
                }

                if (item is ChainsAPM.Commands.Agent.FunctionLeaveQuick)
                {
                    var feq = item as ChainsAPM.Commands.Agent.FunctionLeaveQuick;
                    if (!tcbah.ThreadDepth.ContainsKey(feq.ThreadID))
                        tcbah.ThreadDepth.Add(feq.ThreadID, 0);

                    if (tcbah.ThreadDepth[feq.ThreadID] > 0)
                    {
                        tcbah.ThreadDepth[feq.ThreadID]--;
                    }

                    if (tcbah.ThreadDepth[feq.ThreadID] == 0)
                    {
                        using (var fw = new System.IO.StreamWriter(string.Format(@"C:\LogFiles\{0}_T{1}.txt", DateTime.Now.Ticks, feq.ThreadID)))
                        {
                            foreach (var StackItem in tcbah.ThreadEntryPointStack[feq.ThreadID])
                            {
                                // TODO FIX THIS TO PROPERLY UPDATE THE EXIT
                                //fw.WriteLine("{0}{1}", "".PadLeft((int)StackItem.Item1, ' '), StackItem.Item2);
                            }
                        }
                        tcbah.ThreadDepth.Remove(feq.ThreadID);
                    }

                }
                if (item is ChainsAPM.Commands.Common.SendString)
                {
                    var it = item as ChainsAPM.Commands.Common.SendString;
                    Console.WriteLine("Agent {0} has sent string {1}", tcbah.AgentInfo.AgentName, it.StringData);
                    if (((ChainsAPM.Commands.Common.SendString)item).StringData == "Done!")
                    {
                        tcbah.ConnectionHandler.SendCommand(stringCmd);
                        tcbah.ConnectionHandler.Dispose();
                    }
                }
            }

        }
    }
}
