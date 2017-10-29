using System;
using System.Collections.Generic;

namespace ChainsAPM.Commands.Common
{
    [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Design", "CA1001:TypesThatOwnDisposableFieldsShouldBeDisposable")]
    public class SendString : Interfaces.ICommand<byte>
    {
        private string m_string;

        private Helpers.Fnv1a64 hashhelper;

        private byte[] m_hash;

        public SendString() : this("")
        {

        }

        public SendString(string data)
        {
            m_string = data;
            hashhelper = new Helpers.Fnv1a64();
        }
        public SendString(string data, byte[] hash)
            : this(data)
        {
            m_hash = hash;
        }
        public string Name
        {
            get { return "Add String"; }
        }
        public ushort Code
        {
            get { return 0x0011; }
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
                Helpers.ArraySegmentStream segstream = new Helpers.ArraySegmentStream(input);
                int size = segstream.GetInt32();
                if (input.Count == size)
                {
                    int code = segstream.GetInt16();
                    if (code == Code | code == Code + 1)
                    {
                        var strlen = segstream.GetInt32();
                        var hashcode = segstream.GetInt64();
                        string s = null;
                        if (code == Code + 1)
                        {
                            s = segstream.GetUnicode(strlen);
                        }
                        else
                        {
                            s = segstream.GetASCII(strlen);
                        }

                        return new SendString(s);
                    }
                }
            }
            return null;
        }
        public byte[] Encode()
        {
            byte[] sBuffer = System.Text.UnicodeEncoding.Unicode.GetBytes(m_string);
            var buffer = new List<byte>();
            buffer.AddRange(BitConverter.GetBytes((sBuffer.Length) + 20)); // 4 bytes for size, 2 byte for code, 4 bytes for strlen, 8 bytes for hash, Xbytes for string 2 bytes for term
            buffer.AddRange(BitConverter.GetBytes((short)3));
            if (m_hash == null)
            {
                m_hash = hashhelper.ComputeHash(sBuffer);
            }
            buffer.AddRange(BitConverter.GetBytes(m_string.Length));
            buffer.AddRange(m_hash);
            buffer.AddRange(sBuffer);
            buffer.AddRange(new byte[] { 0x00, 0x00 });
            return buffer.ToArray();

        }
        public string StringData { get { return m_string; } }
    }
}
