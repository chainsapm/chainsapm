using System;

namespace ChainsAPM.Commands.Agent
{
    public class AgentInformation : Interfaces.ICommand<byte>
    {
        public enum Capabilities
        {
            PROFILE = 0x1, // Can attach to CLR profiler
            FILE_TRANSFER = 0x2, // Can send files
            PERFORMANCE_COUNTERS = 0x4, // Can access perf counters
            AGENT_REGISTRY = 0x8, // Can modify the agent definitions
            FULL_REGISTRY = 0x10, // Can modify any registry
            IL_REWRITE = 0x20, // Can modify .NET code before it's JITted
            RESERVED0 = 0x40,
            RESERVED1 = 0x80,
            RESERVED2 = 0x100,
            RESERVED3 = 0x200,
            RESERVED4 = 0x400,
            RESERVED5 = 0x800,
            RESERVED6 = 0x1000,
            RESERVED7 = 0x2000,
            RESERVED8 = 0x4000,
            RESERVED9 = 0x8000
        }

        public Capabilities AgentCapabilities { get; set; }
        public byte AgentMajor { get; set; }
        public byte AgentMinor { get; set; }
        public byte AgentIncremental { get; set; }
        public string AgentName { get; set; }

        public int AgentNameHash { get; set; }

        public string MachineName { get; set; }
        public int MachineNameHash { get; set; }

        public string Version { get { return string.Format("{0}.{1}.{2}", AgentMajor, AgentMinor, AgentIncremental); } }
        public AgentInformation()
        {

        }

        public AgentInformation(Capabilities capabilties, byte major, byte minor, byte incremental, string name, int hash, string machine, int machinehash)
        {
            AgentCapabilities = capabilties;
            AgentMajor = major;
            AgentMinor = minor;
            AgentIncremental = incremental;
            AgentNameHash = hash;
            AgentName = name;
            MachineNameHash = machinehash;
            MachineName = machine;
        }

        public string Name
        {
            get { return "Agent Information"; }
        }
        public ushort Code
        {
            get { return 0x0005; }
        }
        public string Description
        {
            get { return "Event that transfers the agent information."; }
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
                        var decodeAgentCapabilities = (Capabilities)segstream.GetInt32();
                       
                        var decodeAgentMajor = segstream.GetByte();
                        var decodeAgentMinor = segstream.GetByte();
                        var decodeAgentIncremental = segstream.GetByte();

                        var stringlenmachineName = segstream.GetInt32();
                        var decodeMachineNameHash = segstream.GetInt32();
                        var decodeMachineName = segstream.GetUnicode(stringlenmachineName);

                        var stringlen = segstream.GetInt32();
                        var decodeAgentNameHash = segstream.GetInt32();
                        var decodeAgentName = segstream.GetUnicode(stringlen);
                        var term = segstream.GetInt16();
                        if (term != 0)
                        {
                            throw new System.Runtime.Serialization.SerializationException("Terminator is a non zero value. Please check the incoming byte stream for possible errors.");
                        }
                        return new AgentInformation(decodeAgentCapabilities, decodeAgentMajor, decodeAgentMinor, decodeAgentIncremental, decodeAgentName, decodeAgentNameHash, decodeMachineName, decodeMachineNameHash);
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
            return null;
        }
    }

}
