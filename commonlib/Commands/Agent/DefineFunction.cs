using System;
using System.Collections.Generic;

namespace ChainsAPM.Commands.Agent
{
    public class DefineFunction : Interfaces.ICommand<byte>
    {
        public long FunctionID { get; set; }
        public long ClassID { get; set; }
        public string FunctionName { get; set; }
        public DateTime TimeStamp { get; set; }

        public DefineFunction() : this(0, 0, "", 0)
        {
        }
        public DefineFunction(long functionid, long classid, string functioname, long timestamp)
        {
            FunctionID = functionid;
            ClassID = classid;
            FunctionName = functioname;
            TimeStamp = DateTime.FromFileTimeUtc(timestamp);
        }
        public string Name
        {
            get { return "Function Enter Quick"; }
        }
        public ushort Code
        {
            get { return 0x001B; }
        }
        public string Description
        {
            get { return "Event that represents a quick function enter--meaning there were no paramters captured by the agent."; }
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
                    short code = segstream.GetInt16();
                    if (code == Code)
                    {

                        var timestamp = segstream.GetInt64();
                        var function = segstream.GetInt64();
                        var classname = segstream.GetInt64();
                        var strlen = segstream.GetInt32();
                        var functionname = segstream.GetUnicode(strlen);
                        var term = segstream.GetInt16();

                        if (term != 0)
                        {
                            throw new System.Runtime.Serialization.SerializationException("Terminator is a non zero value. Please check the incoming byte stream for possible errors.");
                        }
                        return new DefineFunction(function, classname, functionname, timestamp);
                    }
                    else
                    {
                        throw new System.Runtime.Serialization.SerializationException("Invalid command code detected. Please check the incoming byte stream for possible errors.");
                    }
                }
                else
                {
                    throw new System.Runtime.Serialization.SerializationException("Size of message does not match size of byte stream. Please check the incoming byte stream for possible errors.");
                }
            }
            else
            {
                throw new System.Runtime.Serialization.SerializationException("Size of message is zero. Please check the incoming byte stream for possible errors. ");
            }
        }
        public byte[] Encode()
        {
            var buffer = new List<byte>(31);
            buffer.AddRange(BitConverter.GetBytes(31)); // 4 bytes for size, 2 byte for code, 8 bytes for timestamp, 8 bytes for data, 8 bytes for data, 2 bytes for term
            buffer.AddRange(BitConverter.GetBytes(Code));
            buffer.AddRange(BitConverter.GetBytes(TimeStamp.ToFileTimeUtc()));
            buffer.AddRange(BitConverter.GetBytes(FunctionID));
            buffer.AddRange(BitConverter.GetBytes(ClassID));
            buffer.AddRange(BitConverter.GetBytes(FunctionName.Length));
            buffer.AddRange(System.Text.UnicodeEncoding.Unicode.GetBytes(FunctionName)); 
            buffer.AddRange(BitConverter.GetBytes((short)0));
            return buffer.ToArray();
            
        }
    }
}
