using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace server.ConsoleServer
{
    class ConsoleServer : ChainsAPM.Server.Server
    {
        public ConsoleServer() : base()
        {

            base.ServerListening += S_ServerListening;
            base.AgentConnected += S_AgentConnected;
        }

        public ConsoleServer(int port) : this(System.Net.IPAddress.Any, port)
        {
        }

        public ConsoleServer(System.Net.IPAddress ip, int port) : this(new System.Net.IPEndPoint(ip, port))
        {
        }

        public ConsoleServer(System.Net.IPEndPoint ip) : this()
        {
            tcpListen = new System.Net.Sockets.TcpListener(ip);
        }

        private void S_AgentConnected(string ConnectionName, System.Net.EndPoint endpoint)
        {
            Console.WriteLine("Agent {0} connected!", ConnectionName);
        }

        private void S_ServerListening(string ConnectionName, System.Net.EndPoint endpoint)
        {
            Console.WriteLine("Server started");
            Console.WriteLine("Listening on {0}", endpoint.ToString());
        }

    }
}
