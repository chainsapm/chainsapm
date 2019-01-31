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


namespace ChainsAPM
{
    class Program
    {
        static System.Threading.ManualResetEventSlim waitHandl = new System.Threading.ManualResetEventSlim();
        static System.Collections.Concurrent.ConcurrentDictionary<int, int> itemCounter =
            new System.Collections.Concurrent.ConcurrentDictionary<int, int>(8, 128);
        static int counter2 = 0;
        static int counter3 = 0;
        static int totalMessagesSent = 0;
        static object msgLock = new object();
        static int MAX = 1;
        static void Main(string[] args)
        {
            Dictionary<int, ChainsAPM.Interfaces.ICommand<byte>> CommandList = new Dictionary<int, ICommand<byte>>();
            var cmd1 = new ChainsAPM.Commands.Common.SendString("");
            CommandList.Add(cmd1.Code, cmd1);
            CallContext.LogicalSetData("CommandProviders", CommandList);

            for (int i = 0; i < MAX; i++)
            {
                var whand = new System.Threading.ManualResetEventSlim();
                System.Threading.ThreadPool.QueueUserWorkItem(new System.Threading.WaitCallback((object o) =>
                {
                    var rand = new Random();
                    var hostname = System.Configuration.ConfigurationManager.AppSettings["hostname"];
                    var port = int.Parse(System.Configuration.ConfigurationManager.AppSettings["port"]);
                    System.Net.Sockets.TcpClient tcpC = new System.Net.Sockets.TcpClient(hostname, port);
                    TcpByteAgentHandler tcbah = new TcpByteAgentHandler(new TcpByteTransport(tcpC), TcpByteAgentHandler.HandlerType.SendHeavy);

                    itemCounter.GetOrAdd(tcbah.GetHashCode(), System.Threading.Interlocked.Increment(ref counter2));
                    System.Threading.Interlocked.Increment(ref counter3);
                    tcbah.HasData += tcbah_HasData;
                    tcbah.Disconnected += tcbah_Disconnected;

                    tcpC.NoDelay = true;
                    int counter = 0;
                    int msgCounter = 0;
                    int stopCount = 10000;
                    do
                    {
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(5, 1000)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(5, 1000)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(5, 1000)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(5, 1000)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(5, 1000)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(10, 100)));
                        tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString(randString(5, 1000)));
                        msgCounter += 72;
                        System.Threading.Thread.Sleep(rand.Next(5, 25));
                    } while (++counter != stopCount);


                    Console.WriteLine("Messages Sent: {0}", msgCounter);
                    tcbah.SendCommand(new ChainsAPM.Commands.Common.SendString("Done!"));
                    Console.WriteLine("Done Sent!");
                    lock (msgLock)
                    {
                        totalMessagesSent += msgCounter + 1;
                    }

                }));
            }
            waitHandl.Wait();
            Console.WriteLine("Messages Sent: {0}", totalMessagesSent);
        }

        static string randString(int min, int max)
        {
            int totalCount = new Random().Next(min, max);
            var charrand = new Random();
            var sb = new StringBuilder(max);
            for (int i = 0; i < totalCount; i++)
            {
                sb.Append((char)charrand.Next(0x41, 0x5A));
            }
            return sb.ToString();
        }

        static void tcbah_Disconnected(object sender)
        {
            var tcbah = sender as TcpByteAgentHandler;
            Console.WriteLine("Client {0:000} finished.", itemCounter[tcbah.GetHashCode()]);
            Console.WriteLine("Clients {0:000}.", System.Threading.Interlocked.Decrement(ref counter3));
            Task.Factory.StartNew(async () =>
            {
                await Task.Delay(25000);
                tcbah.Disconnect();
            });
        }



        static void tcbah_HasData(object sender)
        {
            var tcbah = sender as TcpByteAgentHandler;
            foreach (var item in tcbah.GetCommands())
            {
                if (item != null)
                {
                }
                if (item is ChainsAPM.Commands.Common.SendString)
                {
                    tcbah.Dispose();
                    if (counter3 == 0)
                    {

                        waitHandl.Set();
                    }
                }

            }
        }

    }
}
