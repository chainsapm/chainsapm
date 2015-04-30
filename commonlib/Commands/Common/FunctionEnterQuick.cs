using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Commands.Common
{
    public class FunctionEnterQuick : Interfaces.ICommand<byte>
    {
        private Int64 m_string;

        public FunctionEnterQuick(Int64 data)
        {
            m_string = data;
        }
        public string Name
        {
            get { return "Function Enter Quick"; }
        }
        public ushort Code
        {
            get { return 0x0002; }
        }
        public string Description
        {
            get { return "Event that represents a quick function enter--meaning there were no paramters captured by the agent."; }
        }
        public Type CommandType
        {
            get { return typeof(string); }
        }
        public Interfaces.ICommand<byte> Decode(ArraySegment<byte> input2)
        {
            byte[] input = input2.ToArray();
            if (input.Length != 0)
            {
                int size = BitConverter.ToInt32(input, 0);
                if (input.Length == size)
                {
                    byte code = input[4];
                    if (code == 0x02)
                    {
                        // 4 bytes for the len
                        // 1 byte for the code
                        // 4 bytes for the hash (or md token)
                        // x bytes for the string
                        // 2 bytes for the null
                        var s = BitConverter.ToInt64(input, 5);
                        return new FunctionEnterQuick(s);
                    }
                }
            }
            return null;
        }
        public byte[] Encode()
        {
            var buffer = new List<byte>();
            buffer.AddRange(BitConverter.GetBytes(15)); // 4 bytes for size, 1 byte for code, 8 bytes for data, 2 bytes for term
            buffer.AddRange(BitConverter.GetBytes(m_string)); // 4 bytes for size, 1 byte for code, 8 bytes for data, 2 bytes for term
            buffer.AddRange(new byte[] { 0x01 });
            buffer.AddRange(new byte[] { 0x00, 0x00 });
            return buffer.ToArray();
            
        }
        public long StringData { get { return m_string; } }
    }
}
