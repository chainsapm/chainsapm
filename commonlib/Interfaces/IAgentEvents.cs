using ChainsAPM.Server;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Interfaces
{
    public interface IAgentEvents
    {
        event ConnectionEvent AgentConnected;
        event ConnectionEvent AgentDisconnected;
    }
}
