using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.Remoting;
using System.Runtime.Remoting.Channels;
using System.Runtime.Remoting.Channels.Tcp;
using ChainsAPM.Interfaces;
using ChainsAPM.Communication.Tcp;
using ChainsAPM.Commands.Common;
using System.Runtime.Remoting.Messaging;


namespace ChainsAPM.ConsoleServer
{
    class Program
    {
        static object lockConsole = new object();
        static System.Threading.WaitCallback wCb = new System.Threading.WaitCallback(TimerCallback);

        static System.Collections.Concurrent.ConcurrentDictionary<int, TcpByteAgentHandler> concurrentAgentHandlerList =
            new System.Collections.Concurrent.ConcurrentDictionary<int, TcpByteAgentHandler>();
        static long clientsConnected = 0;
        static long messagesRecvd = 0;

        static void Main(string[] args)
        {
            Dictionary<int, ChainsAPM.Interfaces.ICommand<byte>> CommandList = new Dictionary<int, ICommand<byte>>();
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
            GC.RegisterForFullGCNotification(30, 50);
            System.Runtime.GCSettings.LatencyMode = System.Runtime.GCLatencyMode.LowLatency;
            var timerCheck = new System.Threading.Thread(new System.Threading.ThreadStart(() =>
            {
                Console.WriteLine("GC Notification thread started.");
                while (true)
                {
                    GCNotificationStatus s = GC.WaitForFullGCApproach();
                    if (s == GCNotificationStatus.Succeeded)
                    {
                        Console.WriteLine("GC Notification raised.");
                        foreach (var item in concurrentAgentHandlerList)
                        {
                            item.Value.PauseTimers();
                        }

                    }
                    else if (s == GCNotificationStatus.Canceled)
                    {
                        Console.WriteLine("GC Notification cancelled.");
                        break;
                    }
                    else
                    {
                        // This can occur if a timeout period 
                        // is specified for WaitForFullGCApproach(Timeout)  
                        // or WaitForFullGCComplete(Timeout)   
                        // and the time out period has elapsed. 
                        Console.WriteLine("GC Notification not applicable.");
                        break;
                    }

                    // Check for a notification of a completed collection.
                    s = GC.WaitForFullGCComplete(500);
                    if (s == GCNotificationStatus.Succeeded)
                    {
                        Console.WriteLine("Full GC Complete");
                        foreach (var item in concurrentAgentHandlerList)
                        {
                            item.Value.RestartTimers();
                        }
                    }
                    else if (s == GCNotificationStatus.Canceled)
                    {
                        Console.WriteLine("GC Notification cancelled.");
                        break;
                    }
                    else if (s == GCNotificationStatus.Timeout || s == GCNotificationStatus.NotApplicable)
                    {
                        foreach (var item in concurrentAgentHandlerList)
                        {
                            item.Value.RestartTimers();
                        }
                    }
                    else
                    {
                        // Could be a time out.
                        Console.WriteLine("GC Notification not applicable.");
                        break;
                    }


                    System.Threading.Thread.Sleep(500);
                    // FinalExit is set to true right before   
                    // the main thread cancelled notification.
                }


            }));
            timerCheck.Name = "GC Notification Thread";
            //timerCheck.Start();
            System.Net.Sockets.TcpListener tcpListen = new System.Net.Sockets.TcpListener(System.Net.IPAddress.Any, 8080);
            tcpListen.Start(200);
            //listenTimer = new System.Threading.Timer(TimerCallback, tcpListen, 0, 100);
            System.Threading.ThreadPool.QueueUserWorkItem(wCb, tcpListen);
            Console.WriteLine("Server started");
            Console.WriteLine("Listening on {0}", tcpListen.LocalEndpoint.ToString());
            while (true)
            {
                lock (lockConsole)
                {
                    Console.WriteLine("Current connected clients: {0}\t\tPackets: {1}", clientsConnected, messagesRecvd);
                }
                System.Threading.Thread.Sleep(1000);
            }

        }

        async public static void TimerCallback(object objt)
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
                    //listenTimer.Change(100, 1); // Let's stop the timer from consuming too many threads
                    bool restart = false;
                    for (int i = 0; i < listenList.Count; i++)
                    {
                        var item = listenList[i];
                        var listen = await item;
                        System.Threading.Interlocked.Increment(ref clientsConnected);
                        if (item.IsCompleted)
                        {

                            TcpByteAgentHandler tcbah = new TcpByteAgentHandler(new TcpByteTransport(listen), TcpByteAgentHandler.HandlerType.ReceiveHeavy);
                            tcbah.AddCommand(new ChainsAPM.Commands.Common.SendString(""));
                            tcbah.HasData += tcbah_HasDataEvent;
                            tcbah.Disconnected += tcbah_Disconnected;
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

        static void tcbah_Disconnected(object sender)
        {

            System.Threading.Interlocked.Decrement(ref clientsConnected);
            var tcbah = sender as TcpByteAgentHandler;
            lock (lockConsole)
            {
                tcbah.DisconnectedTime = DateTime.Now;
                Console.WriteLine("Agent {0} disconnected. It was connected for {1}", tcbah.AgentInfo.AgentName, (tcbah.DisconnectedTime - tcbah.ConnectedTime));
            }

            var fstream = System.IO.File.CreateText(string.Format(@"C:\Logfiles\{0}_{1}.txt", tcbah.AgentInfo.AgentName, tcbah.ConnectedTime.ToFileTime()));
            foreach (var item in tcbah.ThreadEntryPoint)
            {
                fstream.WriteLine("Starting Thread {0:X}", item.Key);
                foreach (var tpe_list in item.Value)
                {
                    fstream.WriteLine("{0}{1}", "".PadLeft((int)tpe_list.Item1), tcbah.FunctionList[tpe_list.Item2]);
                }
            }
            fstream.Flush();
            fstream.Close();
            TcpByteAgentHandler refOut = null;
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
                    tcbah.Disconnect();
                });
            }

        }

        static void tcbah_HasDataEvent(object sender)
        {
            var tcbah = sender as TcpByteAgentHandler;
            var stringCmd = new ChainsAPM.Commands.Common.SendString("Done!");
            var arr = tcbah.GetCommands();
            foreach (var item in arr)
            {
                System.Threading.Interlocked.Increment(ref messagesRecvd);
                if (item != null)
                {
                    if (item is ChainsAPM.Commands.Agent.DefineFunction)
                    {
                        var DefFunc = item as ChainsAPM.Commands.Agent.DefineFunction;
                        tcbah.FunctionList.Add(DefFunc.FunctionID, DefFunc.FunctionName);
                    }
                    if (item is ChainsAPM.Commands.Agent.AgentInformation)
                    {
                        tcbah.AgentInfo = item as ChainsAPM.Commands.Agent.AgentInformation;
                        tcbah.ConnectedTime = DateTime.Now;
                        Console.WriteLine("Agent {0} connected with version {1} from machine {2}", ((ChainsAPM.Commands.Agent.AgentInformation)item).AgentName, ((ChainsAPM.Commands.Agent.AgentInformation)item).Version, ((ChainsAPM.Commands.Agent.AgentInformation)item).MachineName);
                    }
                    if (item is ChainsAPM.Commands.Agent.FunctionEnterQuick)
                    {
                        var feq = item as ChainsAPM.Commands.Agent.FunctionEnterQuick;
                        if (!tcbah.ThreadDepth.ContainsKey(feq.ThreadID))
                            tcbah.ThreadDepth.Add(feq.ThreadID, 0);

                        if (!tcbah.ThreadEntryPoint.ContainsKey(feq.ThreadID))
                            tcbah.ThreadEntryPoint.Add(feq.ThreadID, new List<Tuple<long, long>>());

                        tcbah.ThreadEntryPoint[feq.ThreadID].Add(new Tuple<long, long>(tcbah.ThreadDepth[feq.ThreadID], feq.FunctionID));
                        tcbah.ThreadDepth[feq.ThreadID]++;
                    }

                    if (item is ChainsAPM.Commands.Agent.FunctionTailQuick)
                    {
                        var feq = item as ChainsAPM.Commands.Agent.FunctionTailQuick;
                        if (!tcbah.ThreadDepth.ContainsKey(feq.ThreadID))
                            tcbah.ThreadDepth.Add(feq.ThreadID, 0);

                        if (tcbah.ThreadDepth[feq.ThreadID] > 0)
                        {
                            tcbah.ThreadDepth[feq.ThreadID]--;
                        }


                        if (!tcbah.ThreadEntryPoint.ContainsKey(feq.ThreadID))
                            tcbah.ThreadEntryPoint.Add(feq.ThreadID, new List<Tuple<long, long>>());

                        tcbah.ThreadEntryPoint[feq.ThreadID].Add(new Tuple<long, long>(tcbah.ThreadDepth[feq.ThreadID], feq.FunctionID));


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

                        if (!tcbah.ThreadEntryPoint.ContainsKey(feq.ThreadID))
                            tcbah.ThreadEntryPoint.Add(feq.ThreadID, new List<Tuple<long, long>>());

                        tcbah.ThreadEntryPoint[feq.ThreadID].Add(new Tuple<long, long>(tcbah.ThreadDepth[feq.ThreadID], feq.FunctionID));

                    }
                    if (item is ChainsAPM.Commands.Common.SendString)
                    {
                        if (((ChainsAPM.Commands.Common.SendString)item).StringData == "Done!")
                        {
                            tcbah.SendCommand(stringCmd);
                            tcbah.Dispose();
                        }
                    }
                }

            }
        }
    }
}


