using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ChainsAPM.Commands.Agent;
using ChainsAPM.Config;
using ChainsAPM.Interfaces;
using ChainsAPM.Interfaces.Config;
using ChainsAPM.Interfaces.Data;
using ChainsAPM.Models.Instrumentation;
using ChainsAPM.Models.Resource;

namespace ChainsAPM.Data {
        class DefaultConfigAdapter : IConfigDataAdapter {
                public AgentConfig ReadAgentConfig (AgentInformation AgentInformation) {
                        var ac = new AgentConfig ()
                        {
                                InstrumentationGroupCollection = new Dictionary<InstrumentationGroup, bool> (),
                                InstrumentationPointCollection = new Dictionary<Models.Definitions.Method, InstrumentationPoint> ()

                        };
                        ac.ParentAgentGroup = ReadAgentGroupConfig (ac);
                        return ac;
                }

                public AgentGroupConfig ReadAgentGroupConfig (AgentConfig AgentConfig) {
                        var ac = new AgentGroupConfig ()
                        {
                                InstrumentationGroupCollection = new Dictionary<InstrumentationGroup, bool> (),
                                InstrumentationPointCollection = new Dictionary<Models.Definitions.Method, InstrumentationPoint> ()

                        };
                        ac.ParentApplication = ReadApplicationConfig (ac);
                        return ac;
                }

                public ApplicationConfig ReadApplicationConfig (AgentGroupConfig AgentGroupConfig) {
                        var ac = new ApplicationConfig ()
                        {
                                InstrumentationGroupCollection = new Dictionary<InstrumentationGroup, bool> (),
                                InstrumentationPointCollection = new Dictionary<Models.Definitions.Method, InstrumentationPoint> ()

                        };
                        ac.ParentApplicationGroup = ReadApplicationGroupConfig (ac);
                        return ac;
                }

                public ApplicationGroupConfig ReadApplicationGroupConfig (ApplicationConfig ApplicationConfig) {
                        var ac = new ApplicationGroupConfig ()
                        {
                                InstrumentationGroupCollection = new Dictionary<InstrumentationGroup, bool> (),
                                InstrumentationPointCollection = new Dictionary<Models.Definitions.Method, InstrumentationPoint> ()
                        };
                        return ac;
                }


                public GlobalConfig ReadAggregatorConfig (AgentConfig AgentGroupConfig) {
                        throw new NotImplementedException ();
                }
                public GlobalConfig ReadGlobalConfig () {
                        throw new NotImplementedException ();
                }

                public MachineConfig ReadMachineConfig (Machine Machine) {
                        throw new NotImplementedException ();
                }

                public ResourceConfig ReadResourceConfig (Resource Resource) {
                        throw new NotImplementedException ();
                }

                public ResourceGroupConfig ReadResourceGroupConfig (ResourceConfig ResourceConfig) {
                        throw new NotImplementedException ();
                }

                public GlobalConfig ReadServerConfig (AgentConfig AgentGroupConfig) {
                        throw new NotImplementedException ();
                }
        }
}
