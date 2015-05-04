using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Commands.Common
{
    [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1001:TypesThatOwnDisposableFieldsShouldBeDisposable")]
    public class SendString : Interfaces.ICommand<byte>
    {
        private string m_string;

        private Helpers.Fnv1a32 hashhelper;

        private byte[] m_hash;
        public SendString(string data)
        {
            m_string = data;
            hashhelper = new Helpers.Fnv1a32();
        }
        public SendString(string data, byte[] hash) : this(data)
        {
            m_hash = hash;
        }
        public string Name
        {
            get { return "Add String"; }
        }
        public ushort Code
        {
            get { return 0x0001; }
        }
        public string Description
        {
            get { return "Creates a has of a string and sends it to the server to be referenced later. TH"; }
        }
        public Type CommandType
        {
            get { return typeof(string); }
        }
        public Interfaces.ICommand<byte> Decode(ArraySegment<byte> input)
        {
            if (input.Count != 0)
            {
                int offsetStart = input.Offset;
                int size = BitConverter.ToInt32(input.Array, offsetStart);
                if (input.Count == size)
                {
                    short code = BitConverter.ToInt16(input.Array, offsetStart + 4);
                    if (code == 0x01 | code == 0x02)
                    {
                        // 4 bytes for the len
                        // 1 byte for the code
                        // 4 bytes for the hash (or md token)
                        // x bytes for the string
                        // 2 bytes for the null
                        string s = null;
                        if (code == 0x03)
                        {
                            s = System.Text.UnicodeEncoding.Unicode.GetString(input.Array, offsetStart + 9, size - 11);
                        }
                        else
                        {
                            s = System.Text.UnicodeEncoding.Default.GetString(input.Array, offsetStart + 9, size - 11);
                        }
                        
                        return new SendString(s);
                    }
                }
            }
            return null;
        }
        public byte[] Encode()
        {
            byte[] sBuffer = System.Text.UnicodeEncoding.Default.GetBytes(m_string);
            var buffer = new List<byte>();
            buffer.AddRange(BitConverter.GetBytes(sBuffer.Length + 11)); // 4 bytes for size, 4 for hash, 1 byte for code, 2 bytes for term
            buffer.AddRange(new byte[] { 0x01 });
            if (m_hash == null)
            {
                m_hash = hashhelper.ComputeHash(sBuffer);
            } 
            buffer.AddRange(m_hash);
            buffer.AddRange(sBuffer);
            buffer.AddRange(new byte[] { 0x00, 0x00 });
            return buffer.ToArray();
            
        }
        public string StringData { get { return m_string; } }
    }
}
