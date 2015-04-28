using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace aggregator.Connection
{
    class Connection
    {
        private System.Net.Sockets.Socket m_socket;
        

        public int MyProperty { get; set; }

        public Connection()
        {

        }

        public Connection(System.Net.Sockets.Socket socket)
            : this()
        {
            if (socket == null)
            {
                throw new ArgumentNullException("socket", "You must supply a socket for this operation.");
            }
            m_socket = socket;

        }

        public void SendPackets(byte[] ByteArray)
        {
            var eargs = new System.Net.Sockets.SocketAsyncEventArgs();
            eargs.SetBuffer(ByteArray, 0, ByteArray.Length);
            var async = m_socket.SendAsync(eargs);
            if (!async)
            {
                // Do nothing for now
            }
        }

        public void ReceievePackets(object disector)
        {
            try
            {
                if (m_socket.Available > 0)
                {
                    var eargs = new System.Net.Sockets.SocketAsyncEventArgs();
                    eargs.Completed += eargs_Completed;
                    eargs.UserToken = disector;
                    var async = m_socket.ReceiveAsync(eargs);
                    if (!async)
                    {
                        this.eargs_Completed(this, eargs);
                    }
                }
            }
            catch (Exception)
            {
                
                throw;
            }
            
        }

        private void eargs_Completed(object sender, System.Net.Sockets.SocketAsyncEventArgs e)
        {
            var disect = e.UserToken as aggregator.NetworkPackets.IPacketDisector;
            disect.PushPackets(e.Buffer);
        }
    }
}
