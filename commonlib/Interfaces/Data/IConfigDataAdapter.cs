using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ChainsAPM.Config;

namespace ChainsAPM.Interfaces.Data {
        public interface IConfigDataAdapter {
                /// <summary>
                /// Gets the top-level config for all resource groups and application groups.
                /// </summary>
                /// <param name="GlobalName"></param>
                /// <returns></returns>
                GlobalConfig ReadGlobalConfig ();
                /// <summary>
                /// Gets the resource group configurations from the global config. There will always be a default resource group, which every application will go if it is not assigned.
                /// </summary>
                /// <param name="GlobalConfig"></param>
                /// /// <param name="ResourceGroup"></param>
                /// <returns></returns>
                ResourceGroupConfig ReadResourceGroupConfig (ResourceConfig ResourceConfig);
                /// <summary>
                /// Gets the resource configuration from the current resource group.
                /// </summary>
                /// <param name="ResourceGroupConfig"></param>
                /// <param name="Resource"></param>
                /// <returns></returns>
                ResourceConfig ReadResourceConfig (Models.Resource.Resource Resource);

                /// <summary>
                /// Gets the current machine configuration from the current resource
                /// </summary>
                /// <param name="ResourceConfig"></param>
                /// <param name="Machine"></param>
                /// <returns></returns>
                MachineConfig ReadMachineConfig (Models.Resource.Machine Machine);

                /// <summary>
                /// Gets the application group config from the global configuration. If there are no group configurations defined all application will be placed in the default group.
                /// </summary>
                /// <param name="GlobalConfig"></param>
                /// <param name="ApplicationGroup"></param>
                /// <returns></returns>
                ApplicationGroupConfig ReadApplicationGroupConfig (ApplicationConfig ApplicationConfig);


                /// <summary>
                /// Gets the individual application configuration from an application group.
                /// </summary>
                /// <param name="AgentGroupConfig"></param>
                /// <returns></returns>
                ApplicationConfig ReadApplicationConfig (AgentGroupConfig AgentGroupConfig);

                /// <summary>
                /// Gets the agent groups belonging to the application. If no groups are defined all agents end up in the default group.
                /// </summary>
                /// <param name="AgentConfig"></param>
                /// <returns></returns>
                AgentGroupConfig ReadAgentGroupConfig (AgentConfig AgentConfig);

                /// <summary>
                /// Gets the configuration on the current agent. If there is no configuration we will try to auto discover.
                /// </summary>
                /// <param name="Agent"></param>
                /// <returns></returns>.
                AgentConfig ReadAgentConfig (Commands.Agent.AgentInformation AgentInformation);

                GlobalConfig ReadServerConfig (AgentConfig AgentGroupConfig);
                GlobalConfig ReadAggregatorConfig (AgentConfig AgentGroupConfig);

        }
}
