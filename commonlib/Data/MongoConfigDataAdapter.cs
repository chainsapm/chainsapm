using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ChainsAPM.Commands.Agent;
using ChainsAPM.Config;
using ChainsAPM.Interfaces.Data;
using ChainsAPM.Models.Resource;

namespace ChainsAPM.Data
{
    class MongoConfigDataAdapter : IConfigDataAdapter
    {
        public AgentConfig ReadAgentConfig(AgentInformation AgentInformation)
        {
            throw new NotImplementedException();
        }

        public AgentGroupConfig ReadAgentGroupConfig(AgentConfig AgentConfig)
        {
            throw new NotImplementedException();
        }

        public GlobalConfig ReadAggregatorConfig(AgentConfig AgentGroupConfig)
        {
            throw new NotImplementedException();
        }

        public ApplicationConfig ReadApplicationConfig(AgentGroupConfig AgentGroupConfig)
        {
            throw new NotImplementedException();
        }

        public ApplicationGroupConfig ReadApplicationGroupConfig(ApplicationConfig ApplicationConfig)
        {
            throw new NotImplementedException();
        }

        public GlobalConfig ReadGlobalConfig()
        {
            throw new NotImplementedException();
        }

        public MachineConfig ReadMachineConfig(Machine Machine)
        {
            throw new NotImplementedException();
        }

        public ResourceConfig ReadResourceConfig(Resource Resource)
        {
            throw new NotImplementedException();
        }

        public ResourceGroupConfig ReadResourceGroupConfig(ResourceConfig ResourceConfig)
        {
            throw new NotImplementedException();
        }

        public GlobalConfig ReadServerConfig(AgentConfig AgentGroupConfig)
        {
            throw new NotImplementedException();
        }
    }
}
