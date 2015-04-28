using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;

namespace aggregator.Listener
{
    public class Listener
    {
        static System.Net.Sockets.TcpListener m_Listener;
        public Listener()
        {
            if (m_Listener == null)
            {
                System.Net.IPAddress bindingAddress = System.Net.IPAddress.Any;
                System.Net.IPAddress.TryParse(System.Configuration.ConfigurationManager.AppSettings["IPAddress"], out bindingAddress);
                m_Listener = new System.Net.Sockets.TcpListener(System.Net.IPAddress.Any, 5600);
                m_Listener.Start();
                TcpClient tcpC = m_Listener.AcceptTcpClient();
                bool hasMoredata = false;
                int bufferLen = 0;
                while (true)
                {
                    if (tcpC.Available > 0)
                    {
                        byte[] buf = new byte[tcpC.Available];
                        tcpC.Client.Receive(buf);
                        if (!hasMoredata)
                        {
                            bufferLen = BitConverter.ToInt32(buf, 0);
                        }
                        byte[] realBuf = new byte[bufferLen];
                        int bytesReceived = tcpC.Client.Receive(realBuf, bufferLen + 4, SocketFlags.None);
                        Console.WriteLine(System.Text.ASCIIEncoding.ASCII.GetString(realBuf));
                    }
                    System.Threading.Thread.Sleep(5);
                }

            }
        }


    }
}
