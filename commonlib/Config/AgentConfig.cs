using System;
using System.Collections.Generic;
using ChainsAPM.Interfaces.Config;
using ChainsAPM.Models.Definitions;
using ChainsAPM.Models.Instrumentation;

namespace ChainsAPM.Config {
        public class AgentConfig : IBaseConfig, IInstrumentationMethodGroupConfig, IInstrumentationPointConfig {
                public AgentGroupConfig ParentAgentGroup { get; set; }

                public IDictionary<InstrumentationGroup, bool> InstrumentationGroupCollection { get; set; }

                public IDictionary<Method, InstrumentationPoint> InstrumentationPointCollection { get; set; }

                public string ConfigName {
                        get; set;
                }

                public bool IsMethodEntryPoint (Method methodid) {
                        if ( InstrumentationPointCollection.ContainsKey (methodid) ) {
                                if ( InstrumentationPointCollection [methodid].InstrumentationMethod.IsEntryPoint )
                                        return true;
                        }
                        return ParentAgentGroup.IsMethodEntryPoint (methodid);
                }

        }
}
