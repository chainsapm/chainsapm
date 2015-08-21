using ChainsAPM.Commands;
using ChainsAPM.Commands.Agent;
using ChainsAPM.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Agent
{
    public class Agent
    {
        public Config.AgentConfig AgentConfig { get; private set; }
        public IConnectionHandler ConnectionHandler { get; set; }
        public AgentInformation AgentInfo { get; set; }
        public Dictionary<long, long> ThreadDepth { get; set; }
        public Dictionary<long, Stack<ChainsAPM.Models.EntryPoint>> ThreadEntryPointStack { get; set; }
        public DateTime ConnectedTime { get; set; }
        public DateTime DisconnectedTime { get; set; }

        public Processor CommandProcessor { get; private set; }

        private Agent()
        {
            ThreadDepth = new Dictionary<long, long>();
            ThreadEntryPointStack = new Dictionary<long, Stack<Models.EntryPoint>>();

            CommandProcessor = new Processor();

            CommandProcessor.CreateList();
        }

        public Agent(IConnectionHandler connectionHandler)
        {
            ConnectionHandler = connectionHandler;
        }
    }
}
