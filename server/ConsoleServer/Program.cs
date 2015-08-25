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
using System.Threading;

namespace ChainsAPM.ConsoleServer
{
    class Program
    {
        static void Main(string[] args)
        {
            

            var s = new Server.Server(8080);
            
            s.ServerListening += new Server.ConnectionEvent(S_ServerListening);
            s.AgentConnected += S_AgentConnected;


            s.Start();


            var sIPV6 = new Server.Server(System.Net.IPAddress.IPv6Any, 8080);
            sIPV6.ServerListening += new Server.ConnectionEvent(S_ServerListening);
            sIPV6.AgentConnected += S_AgentConnected;


            sIPV6.Start();
            Console.ReadLine();

            s.Stop();
            sIPV6.Stop();
        }

        private static void S_AgentConnected(string ConnectionName, System.Net.EndPoint endpoint)
        {
            Console.WriteLine("Agent {0} connected!", ConnectionName);
        }

        private static void S_ServerListening(string ConnectionName, System.Net.EndPoint endpoint)
        {
            Console.WriteLine("Server started");
            Console.WriteLine("Listening on {0}", endpoint.ToString());
        }

    }
}



