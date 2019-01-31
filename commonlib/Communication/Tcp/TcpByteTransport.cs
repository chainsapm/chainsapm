using ChainsAPM.Interfaces;
using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Threading.Tasks;


namespace ChainsAPM.Communication.Tcp
{

    public sealed class SocketAwaitable : INotifyCompletion
    {
        private readonly static Action SENTINEL = () => { };

        internal bool m_wasCompleted;
        internal Action m_continuation;
        internal SocketAsyncEventArgs m_eventArgs;

        public SocketAwaitable(SocketAsyncEventArgs eventArgs)
        {
            if (eventArgs == null) throw new ArgumentNullException("eventArgs");
            m_eventArgs = eventArgs;
            eventArgs.Completed += delegate
            {
                var prev = m_continuation ?? Interlocked.CompareExchange(
                    ref m_continuation, SENTINEL, null);
                if (prev != null) prev();
            };
        }

        internal void Reset()
        {
            m_wasCompleted = false;
            m_continuation = null;
        }

        public SocketAwaitable GetAwaiter() { return this; }

        public bool IsCompleted { get { return m_wasCompleted; } }

        public void OnCompleted(Action continuation)
        {
            if (m_continuation == SENTINEL ||
                Interlocked.CompareExchange(
                    ref m_continuation, continuation, null) == SENTINEL)
            {
                Task.Run(continuation);
            }
        }

        public void GetResult()
        {
            if (m_eventArgs.SocketError != SocketError.Success)
                throw new SocketException((int)m_eventArgs.SocketError);
        }
    }
    public static class SocketExtensions
    {
        public static SocketAwaitable ReceiveAsync(this Socket socket,
            SocketAwaitable awaitable)
        {
            awaitable.Reset();
            if (!socket.ReceiveAsync(awaitable.m_eventArgs))
                awaitable.m_wasCompleted = true;
            return awaitable;
        }

        public static SocketAwaitable SendAsync(this Socket socket,
            SocketAwaitable awaitable)
        {
            awaitable.Reset();
            if (!socket.SendAsync(awaitable.m_eventArgs))
                awaitable.m_wasCompleted = true;
            return awaitable;
        }

        // ... 
    }
    public class TcpByteTransport : ITransport<byte[]>, IDisposable
    {
        public System.Net.Sockets.NetworkStream m_Socket;
        System.Net.Sockets.TcpClient m_Client;
        private byte[] internalBuffer;
        private Queue<byte[]> m_InboundQueue;
        private object m_QueueLock;
        private object inRecv;
        private bool disposed;
        public System.Net.Sockets.Socket Socket { get { return m_Client.Client; } }


        public TcpByteTransport(System.Net.Sockets.TcpClient socket)
        {
            socket.ReceiveTimeout = 5000;
            socket.SendTimeout = 5000;
            socket.ReceiveBufferSize = 1 * 1024 * 1024;
            socket.SendBufferSize = 1 * 1024 * 1024;
            m_Client = socket;
            m_Socket = socket.GetStream();
            socket.Client.UseOnlyOverlappedIO = true;
            internalBuffer = new byte[10 * 1024 * 1024];
            m_InboundQueue = new Queue<byte[]>();
            m_QueueLock = new object();
            socket.Client.Blocking = false;
            inRecv = new object();

        }


        public async Task<bool> Send(byte[] data)
        {

            if (!disposed)
            {
                if (m_Client.Connected)
                {
                    byte[] dataToSend = new byte[data.Length + 8];
                    Array.Copy(BitConverter.GetBytes(dataToSend.Length), dataToSend, 4); // PackageLength
                    Array.Copy(data, 0, dataToSend, 4, data.Length); // Messages
                    Array.Copy(new byte[] { 0xCC, 0xCC, 0xCC, 0xCC }, 0, dataToSend, data.Length + 4, 4); // PostAmble

                    try
                    {
                        await m_Socket.WriteAsync(dataToSend, 0, dataToSend.Length);
                    }
                    catch (Exception)
                    {
                        disposed = true;
                    }

                    return true;
                }
            }
            return false;
        }

        public async Task<byte[]> Receive()
        {

            if (!disposed)
            {

                // Reusable SocketAsyncEventArgs and awaitable wrapper 



                try
                {
                    var bytes = await m_Socket.ReadAsync(internalBuffer, 0, internalBuffer.Length);
                    if (bytes > 0)
                    {
                        byte[] queueBuffer = new byte[bytes];
                        Array.Copy(internalBuffer, queueBuffer, bytes);
                        return queueBuffer;
                    }
                }
                catch (Exception)
                {
                    disposed = true;
                }
            }
            return null;
        }

        public bool Disconnect()
        {
            try
            {

                Dispose();
            }
            catch (Exception)
            {
                return false;
            }
            return true;
        }



        public void Dispose()
        {
            if (!disposed)
            {
                m_Client.Close();
            }

            disposed = true;
        }


        #region ITransport<byte[]> Members


        public bool Connected
        {
            get
            {
                try
                {
                    return m_Client.Client.Connected;
                }
                catch (Exception)
                {

                    return false;
                }

            }
        }

        #endregion

        #region ITransport<byte[]> Members

        public bool HasData
        {
            get
            {
                lock (m_QueueLock)
                {
                    return m_InboundQueue.Count > 0;
                }
            }

        }

        #endregion
    }
}
