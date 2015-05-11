using System;
using System.Collections.Generic;
using System.Collections.Concurrent;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ChainsAPM.Interfaces;
using ChainsAPM.Commands.Common;
using ChainsAPM.Commands.Agent;
using System.Runtime.Remoting.Messaging;


namespace ChainsAPM.Communication.Tcp
{
    public class TcpByteAgentHandler : IConnectionHandler, IDisposable
    {
        public enum HandlerType
        {
            SendHeavy,
            ReceiveHeavy,
            Balanced
        }
        public AgentInformation AgentInfo { get; set; }
        public DateTime ConnectedTime { get; set; }
        public DateTime DisconnectedTime { get; set; }

        private Dictionary<int, Interfaces.ICommand<byte>> CommandList;
        private object cmdListLock;

        public Dictionary<long, string> StringList;
        public Dictionary<long, string> FunctionList;

        public Dictionary<long, long> ThreadDepth;

        // Thread Id, sequence, threadid
        public Dictionary<long, List<Tuple<long, long>>> ThreadEntryPoint;

        private ITransport<byte[]> m_PacketHandler;
        
        private object lockingOutbound;
        private object lockingInbound;
        private Queue<byte[]> blockingOutboundQueue;
        private System.Threading.Timer sendTimer; // Let's keep this guy around
        private System.Threading.Timer recvTimer; // Let's keep this guy around
        public delegate void HasDataEvent(object sender);
        public event HasDataEvent HasData;
        public delegate void DisconnectedEvent(object sender);
        public event DisconnectedEvent Disconnected;
        object timerLock = new object();
        public int messagesSent = 0;
        int MAX_SENDBUFFER = 1024 * 70; // Keep this out of the LOH
        private Queue<byte[]> m_buffers;
        private Queue<ArraySegment<byte>> m_items;
        private object queuelock = new object();
        private object chunklock = new object();
        private bool disconnected = false;
        private int sendTimerInterval = 250;
        private int recvTimerInterval = 250;
        private bool timersSuspended = false;
        private bool inSend = false;
        private bool inRecv = false;
        private List<byte> chunkList;

        public TcpByteAgentHandler(ITransport<byte[]> packethand, HandlerType handType = HandlerType.Balanced)
        {
            m_PacketHandler = packethand;
            blockingOutboundQueue = new Queue<byte[]>();
            lockingOutbound = new object();
            lockingInbound = new object();
            m_buffers = new Queue<byte[]>();
            m_items = new Queue<ArraySegment<byte>>();
            chunkList = new List<byte>(MAX_SENDBUFFER);
            int sendTimerInterval = 250;
            int recvTimerInterval = 250;
            CommandList = CallContext.LogicalGetData("CommandProviders") as Dictionary<int, Interfaces.ICommand<byte>>;
            cmdListLock = new object();

            StringList = new Dictionary<long, string>();
            FunctionList = new Dictionary<long, string>();

            ThreadDepth = new Dictionary<long, long>();

            // Thread Id, sequence, threadid
            ThreadEntryPoint = new Dictionary<long, List<Tuple<long, long>>>();

            switch (handType)
            {
                case HandlerType.SendHeavy:
                    sendTimerInterval /= 2;
                    recvTimerInterval *= 2;
                    break;
                case HandlerType.ReceiveHeavy:
                    recvTimerInterval /= 2;
                    sendTimerInterval *= 2;
                    break;
                default:
                    break;
            }
            sendTimer = new System.Threading.Timer(new System.Threading.TimerCallback(async (object o) =>
            {
                if (!inSend)
                {
                    inSend = true;
                    await SendData();
                    inSend = false;
                }


            }), null, sendTimerInterval, sendTimerInterval);

            recvTimer = new System.Threading.Timer(new System.Threading.TimerCallback(async (object o) =>
            {
                if (!inRecv)
                {
                    inRecv = true;
                    await RecvData();
                    ExtractData();
                    inRecv = false;
                }
            }), null, recvTimerInterval, recvTimerInterval);
        }

        public void PauseTimers()
        {
            lock (timerLock)
            {
                if (!timersSuspended)
                {

                    try
                    {
                        sendTimer.Change(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
                    }
                    catch (Exception)
                    {

                    }
                    try
                    {
                        recvTimer.Change(System.Threading.Timeout.Infinite, System.Threading.Timeout.Infinite);
                    }
                    catch (Exception)
                    {

                    }
                    timersSuspended = true;
                }
            }
        }

        public void RestartTimers()
        {
            lock (timerLock)
            {
                if (timersSuspended)
                {
                    try
                    {
                        sendTimer.Change(sendTimerInterval, sendTimerInterval);
                    }
                    catch (Exception)
                    {

                    }
                    try
                    {
                        recvTimer.Change(recvTimerInterval, recvTimerInterval);
                    }
                    catch (Exception)
                    {

                    }
                    timersSuspended = false;
                }
            }
        }
        private async void SendRecvData(object none)
        {

            try
            {

                await SendData();

                await RecvData();

                ExtractData();

            }
            finally
            {

            }
        }

        private async Task RecvData()
        {
            byte[] bytes = null;
            try
            {
                bytes = await m_PacketHandler.Receive();
            }
            catch (Exception)
            {

                throw;
            }

            lock (queuelock)
            {
                if (bytes != null)
                {
                    m_buffers.Enqueue(bytes);
                }

            }
        }

        private async Task SendData()
        {
            List<byte> sendArray = null;
            lock (lockingOutbound)
            {
                if (blockingOutboundQueue.Count > 0)
                {
                    sendArray = new List<byte>(MAX_SENDBUFFER);
                    try
                    {
                        while (blockingOutboundQueue.Count > 0)
                        {
                            sendArray.AddRange(blockingOutboundQueue.Dequeue());
                        }
                    }
                    catch (Exception)
                    {
                        sendArray = null;
                    }
                    finally
                    {
                    }
                }
            }
            if (sendArray != null)
            {
                try
                {
                    await m_PacketHandler.Send(sendArray.ToArray());
                }
                catch (Exception)
                {
                    //TODO Add logging
                }
            }
        }

        public void AddCommand(ICommand<byte> command)
        {
            lock (cmdListLock)
            {
                if (!CommandList.ContainsKey(command.Code))
                {
                    CommandList.Add(command.Code, command);
                }

            }

        }
        public void SendCommand(ICommand<byte> command)
        {
            lock (lockingOutbound)
            {
                blockingOutboundQueue.Enqueue(command.Encode());
                ++messagesSent;
            }
        }

        public ICommand<byte> GetCommand()
        {
            ArraySegment<byte> command = new ArraySegment<byte>();
            lock (chunklock)
            {
                command = m_items.Dequeue();
            }
            if (command != null)
            {
                var size = BitConverter.ToInt32(command.Array, command.Offset);
                var code = command.Array[command.Offset + 4];
                return CommandList[code].Decode(command);
            }
            return null;

        }

        public void SendCommands(ICommand<byte>[] command)
        {
            lock (lockingOutbound)
            {
                foreach (var item in command)
                {
                    blockingOutboundQueue.Enqueue(item.Encode());
                }
                ++messagesSent;
            }
        }

        public ICommand<byte>[] GetCommands()
        {

            List<ICommand<byte>> outList = new List<ICommand<byte>>();
            lock (chunklock)
            {
                ArraySegment<byte> command = new ArraySegment<byte>();
                while (m_items.Count > 0)
                {
                    command = m_items.Dequeue();

                    if (command != null)
                    {
                        var size = BitConverter.ToInt32(command.Array, command.Offset);
                        var code = command.Array[command.Offset + 4];
                        outList.Add(CommandList[code].Decode(command));
                    }
                }
            }
            return outList.ToArray();
        }

        #region IConnectionHandler Members

        public bool Disconnect()
        {
            try
            {
                m_PacketHandler.Disconnect();
            }
            catch (Exception)
            {

            }
            return true;
        }

        public bool Recycle()
        {
            // TODO Implement method to tell agent to recycle connection
            return true;

        }

        public bool Flush()
        {
            SendRecvData(null);
            return true;
        }

        #endregion

        #region IDisposable Members

        public void Dispose()
        {
            Task.Factory.StartNew(async () =>
                {
                    await Task.Delay(Math.Max(sendTimerInterval, recvTimerInterval) * 3);
                    sendTimer.Dispose();
                });

            if (!disconnected)
            {
                disconnected = true;
                Disconnected(this);
            }

        }

        #endregion

        public void Received(byte[][] ReceievedData)
        {
            lock (queuelock)
            {
                foreach (var item in ReceievedData)
                {
                    m_buffers.Enqueue(item);
                }

            }
        }

        private void ExtractData()
        {

            int position = 0;
            int finalSize = 0;
            byte[] queueToBreak = null;
            List<ArraySegment<byte>> segmentList = new List<ArraySegment<byte>>();
            lock (queuelock)
            {
                if (m_buffers.Count > 0)
                {

                    while (m_buffers.Count > 0)
                    {
                        byte[] localqueueChunk = m_buffers.Dequeue();
                        chunkList.AddRange(localqueueChunk);
                        position += localqueueChunk.Length;
                    }

                    byte[] queueChunk = chunkList.ToArray();
                    chunkList.Clear();
                    queueToBreak = new byte[queueChunk.Length];
                    segmentList = new List<ArraySegment<byte>>();
                    for (int i = 0; i < queueChunk.Length; )
                    {
                        int size = 0;
                        if (queueChunk.Length - 4 >= i + 4)
                        {
                            size = BitConverter.ToInt32(queueChunk, i);
                        }
                        if (size > 0 && queueChunk.Length >= i + size && BitConverter.ToUInt32(queueChunk, (i + size) - 4) == 0xCCCCCCCC)
                        {

                            segmentList.Add(new ArraySegment<byte>(queueChunk, i + 4, size - 8));
                            finalSize += (size - 8);
                            i += size;
                        }
                        else
                        {
                            byte[] remainder = new byte[queueChunk.Length - i];
                            Array.Copy(queueChunk, i, remainder, 0, queueChunk.Length - i);
                            m_buffers.Enqueue(remainder);
                            break;
                        }
                    }
                }
            }
            lock (chunklock)
            {
                if (finalSize > 0)
                {
                    var listOfSizes = new List<int>();
                    Array.Resize(ref queueToBreak, finalSize);
                    foreach (var item in segmentList)
                    {
                        for (int i = 0; i < item.Count; )
                        {
                            var startOffset = i + item.Offset;
                            int size = BitConverter.ToInt32(item.Array, startOffset);
                            if (size > 0 && size < 4096)
                            {
                                listOfSizes.Add(size);
                                var segment = new ArraySegment<byte>(item.Array, startOffset, size);
                                m_items.Enqueue(segment);
                                i += size;
                            }
                            else
                            {
                                break;
                            }
                        }
                    }
                    HasData(this);
                }
            }
        }
        public void Sent()
        {
            throw new NotImplementedException();
        }
    }
}
