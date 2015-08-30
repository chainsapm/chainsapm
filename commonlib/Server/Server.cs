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

namespace ChainsAPM.Server {
        public delegate void ConnectionEvent (string ConnectionName, System.Net.EndPoint endpoint);

        public class Server : IServerEvents, IAgentEvents {
                object lockConsole = new object ();

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

                private Server () {
                        wCb = new System.Threading.WaitCallback (TimerCallback);
                        concurrentAgentHandlerList = new System.Collections.Concurrent.ConcurrentDictionary<long, Agent.Agent> ();
                        System.Runtime.GCSettings.LatencyMode = System.Runtime.GCLatencyMode.LowLatency;
                }


                public Server (int port) : this (System.Net.IPAddress.Any, port) {
                }

                public Server (System.Net.IPAddress ip, int port) : this (new System.Net.IPEndPoint (ip, port)) {
                }

                public Server (System.Net.IPEndPoint ip) : this () {
                        tcpListen = new System.Net.Sockets.TcpListener (ip);
                }

                public void Start () {
                        tcpListen.Start (200);
                        //listenTimer = new System.Threading.Timer(TimerCallback, tcpListen, 0, 100);
                        System.Threading.ThreadPool.QueueUserWorkItem (wCb, tcpListen);
                        if ( ServerListening != null )
                                ServerListening (Environment.MachineName, tcpListen.LocalEndpoint);

                }

                public void Stop () {
                        tcpListen.Stop ();


                }

                private void TurnOnGCNotification () {
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

                async private void TimerCallback (object objt) {
                        var tcpListen = objt as System.Net.Sockets.TcpListener;
                        tcpListen.Server.UseOnlyOverlappedIO = true;
                        while ( true ) {
                                try {

                                        await tcpListen.AcceptTcpClientAsync ()
                                            .ContinueWith (async (tClient) =>
                                         {
                                                var client = await tClient;
                                                System.Threading.Interlocked.Increment (ref clientsConnected);
                                                Agent.Agent agent = new Agent.Agent (new TcpByteAgentHandler (new TcpByteTransport (client), TcpByteAgentHandler.HandlerType.ReceiveHeavy), this);
                                                concurrentAgentHandlerList.GetOrAdd (agent.GetHashCode (), agent);
                                                agent.AgentSubscription.Subscribe (ag =>
                             {
                                                    ag = ag as Agent.Agent;
                                                    Console.WriteLine ("Agent {0} Connected from {1}.", ag.AgentInfo.AgentName, ag.AgentInfo.MachineName);
                                            });
                                        });

                                }
                                catch ( Exception ) {
                                        break;
                                        //TODO Log fatal exception
                                }
                                finally {
                                }
                        }

                }


        }
}
