using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Interfaces.Data {
        public interface IConfigDataAdapter {
                /// <summary>
                /// Gets the top-level config for all resource groups and application groups.
                /// </summary>
                /// <param name="GlobalName"></param>
                /// <returns></returns>
                ChainsAPM.Config.GlobalConfig ReadGlobalConfig (string GlobalName);
                /// <summary>
                /// Gets the resource group configurations from the global config. There will always be a default resource group, which every application will go if it is not assigned.
                /// </summary>
                /// <param name="GlobalConfig"></param>
                /// /// <param name="ResourceGroup"></param>
                /// <returns></returns>
                ChainsAPM.Config.ResourceGroupConfig ReadResourceGroupConfig (ChainsAPM.Config.GlobalConfig GlobalConfig, string ResourceGroup);
                /// <summary>
                /// Gets the resource configuration from the current resource group.
                /// </summary>
                /// <param name="ResourceGroupConfig"></param>
                /// <param name="Resource"></param>
                /// <returns></returns>
                ChainsAPM.Config.ResourceConfig ReadResourceConfig (ChainsAPM.Config.AgentConfig ResourceGroupConfig, string Resource);

                /// <summary>
                /// Gets the current machine configuration from the current resource
                /// </summary>
                /// <param name="ResourceConfig"></param>
                /// <param name="Machine"></param>
                /// <returns></returns>
                ChainsAPM.Config.MachineConfig ReadMachineConfig (ChainsAPM.Config.ResourceConfig ResourceConfig, string Machine);

                /// <summary>
                /// Gets the application group config from the global configuration. If there are no group configurations defined all application will be placed in the default group.
                /// </summary>
                /// <param name="GlobalConfig"></param>
                /// <param name="ApplicationGroup"></param>
                /// <returns></returns>
                ChainsAPM.Config.ApplicationGroupConfig ReadApplicationGroupConfig (ChainsAPM.Config.GlobalConfig GlobalConfig, string ApplicationGroup);


                /// <summary>
                /// Gets the individual application configuration from an application group.
                /// </summary>
                /// <param name="AgentGroupConfig"></param>
                /// <returns></returns>
                ChainsAPM.Config.AgentGroupConfig ReadApplicationConfig (ChainsAPM.Config.ApplicationGroupConfig ApplicationGroupConfig, string Application);

                /// <summary>
                /// Gets the agent groups belonging to the application. If no groups are defined all agents end up in the default group.
                /// </summary>
                /// <param name="AgentGroupConfig"></param>
                /// <returns></returns>
                ChainsAPM.Config.AgentConfig ReadAgentGroupConfig (ChainsAPM.Config.AgentGroupConfig AgentConfig, string AgentGroup);

                /// <summary>
                /// Gets the configuration on the current agent. If there is no configuration we will try to auto discover.
                /// </summary>
                /// <param name="Agent"></param>
                /// <returns></returns>.
                ChainsAPM.Config.AgentConfig ReadAgentConfig (ChainsAPM.Commands.Agent.AgentInformation AgentInformation);
                
                
               
                
               
                
                
                ChainsAPM.Config.GlobalConfig ReadServerConfig (ChainsAPM.Config.AgentConfig AgentGroupConfig);
                ChainsAPM.Config.GlobalConfig ReadAggregatorConfig (ChainsAPM.Config.AgentConfig AgentGroupConfig);

        }
}
