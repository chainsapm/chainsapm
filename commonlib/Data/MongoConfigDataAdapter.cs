using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ChainsAPM.Commands.Agent;
using ChainsAPM.Config;
using ChainsAPM.Interfaces.Data;

namespace ChainsAPM.Data {
        class MongoConfigDataAdapter : IConfigDataAdapter {
                public AgentConfig ReadAgentConfig (AgentInformation AgentInformation) {
                        throw new NotImplementedException ();
                }

                public AgentConfig ReadAgentGroupConfig (AgentGroupConfig AgentConfig, string AgentGroup) {
                        throw new NotImplementedException ();
                }

                public GlobalConfig ReadAggregatorConfig (AgentConfig AgentGroupConfig) {
                        throw new NotImplementedException ();
                }

                public AgentGroupConfig ReadApplicationConfig (ApplicationGroupConfig ApplicationGroupConfig, string Application) {
                        throw new NotImplementedException ();
                }

                public ApplicationGroupConfig ReadApplicationGroupConfig (GlobalConfig GlobalConfig, string ApplicationGroup) {
                        throw new NotImplementedException ();
                }

                public GlobalConfig ReadGlobalConfig (string GlobalName) {
                        throw new NotImplementedException ();
                }

                public MachineConfig ReadMachineConfig (ResourceConfig ResourceConfig, string Machine) {
                        throw new NotImplementedException ();
                }

                public ResourceConfig ReadResourceConfig (AgentConfig ResourceGroupConfig, string Resource) {
                        throw new NotImplementedException ();
                }

                public ResourceGroupConfig ReadResourceGroupConfig (GlobalConfig GlobalConfig, string ResourceGroup) {
                        throw new NotImplementedException ();
                }

                public GlobalConfig ReadServerConfig (AgentConfig AgentGroupConfig) {
                        throw new NotImplementedException ();
                }
        }
}
