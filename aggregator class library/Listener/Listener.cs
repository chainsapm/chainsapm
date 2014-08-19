using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace aggregator.Listener
{
    class Listener
    {
        static System.Net.Sockets.TcpListener m_Listener;
        public Listener()
        {
            if (m_Listener == null)
            {
                System.Net.IPAddress bindingAddress = System.Net.IPAddress.Any;
                System.Net.IPAddress.TryParse(System.Configuration.ConfigurationManager.AppSettings["IPAddress"], out bindingAddress);
                m_Listener = new System.Net.Sockets.TcpListener(System.Net.IPAddress.Any, 5600);
            }
        }
    }
}
