using System;
using System.Collections.Generic;
using System.Collections.Concurrent;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using RemotingClass.Interfaces;

namespace RemotingClass.Communication.Tcp
{
    public class TcpByteHandler : IPacketHandler<byte[]>, IPacketHandlerEvents<byte[]>
    {
        private ITransport<byte[]> m_Transport;
        private byte[] m_bufferInternal;
        private byte[] m_sendBufferInternal;
        private Queue<byte[]> m_buffers;
        private Queue<byte[]> m_items;
        private object queuelock = new object();
        private object chunklock = new object();
        public bool HasData { get { return m_Transport.HasData; } }

        public ITransport<byte[]> Transport
        {
            get { return m_Transport; }
        }

        public TcpByteHandler(ITransport<byte[]> transport)
        {
            m_Transport = transport;
            m_bufferInternal = new byte[10 * 1024];
            m_sendBufferInternal = new byte[10 * 1024];
            m_buffers = new Queue<byte[]>();
            m_items = new Queue<byte[]>();
        }

        #region IPacketHandler<byte[]> Members

        public byte[][] RecieveData()
        {
            m_Transport.Receive();

            if (m_items.Count > 0)
            {
                List<byte[]> outData = new List<byte[]>();
                lock (chunklock)
                {
                    while (m_items.Count > 0)
                    {
                        outData.Add(m_items.Dequeue());
                    }
                    return outData.ToArray();
                }
            }
            return null;
        }

        public void Received(byte[][] ReceievedData)
        {
            lock (queuelock)
            {
                foreach (var item in ReceievedData)
                {
                    m_buffers.Enqueue(item);
                }
                breakQueueIntoChunks(null);
            }
            // TODO Implement Some stauts or callback
        }

        public void Sent()
        {
            // TODO Implement Some stauts or callback
        }

        public void SendData(byte[] data)
        {
        }
        public void SendDataAsync(byte[] data)
        {
            try
            {
            }
            catch (Exception e)
            {

                throw e;
            }

        }
        #endregion

        private void breakQueueIntoChunks(object none)
        {
            byte[] queueToBreak = new byte[10 * 1024 * 1024];
            int position = 0;
            int finalSize = 0;
            lock (queuelock)
            {
                byte[] queueChunk = new byte[1 * 1024 * 1024];
                while (m_buffers.Count > 0)
                {
                    byte[] localqueueChunk = m_buffers.Dequeue();
                    if ((position + localqueueChunk.Length) > queueChunk.Length)
                    {
                        Array.Resize(ref queueChunk, queueChunk.Length + localqueueChunk.Length);
                    }
                    Array.Copy(localqueueChunk, 0, queueChunk, position, localqueueChunk.Length);
                    position += localqueueChunk.Length;
                }
                //Array.Resize(ref queueChunk, position);

                for (int i = 0; i < queueChunk.Length; )
                {
                    int size = BitConverter.ToInt32(queueChunk, i);
                    if (size == 0)
                    {
                        break;
                    }
                    if ((size + i) <= queueChunk.Length)
                    {
                        Array.Copy(queueChunk, i + 4, queueToBreak, i, size - 4);
                        i += size;
                        finalSize = i;
                    }
                    else
                    {
                        byte[] remainder = new byte[queueChunk.Length - i];
                        Array.Copy(queueChunk, i, remainder, 0, queueChunk.Length - i);
                        m_buffers.Enqueue(remainder);
                    }
                }
            }
            lock (chunklock)
            {
                Array.Resize(ref queueToBreak, finalSize);
                if (queueToBreak != null)
                {
                    for (int i = 0; i < queueToBreak.Length; )
                    {
                        int size = BitConverter.ToInt32(queueToBreak, i);
                        if (size == 0)
                        {
                            break;
                        }
                        var barray = new byte[size];
                        Array.Copy(queueToBreak, i, barray, 0, barray.Length);

                        m_items.Enqueue(barray);


                        i += size;
                    }
                }
            }
        }

        #region IDisposable Members

        public void Dispose()
        {
            Transport.Dispose();
        }

        #endregion
    }
}
