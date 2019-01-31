using System.Collections.Generic;
using ChainsAPM.Interfaces.Config;
using ChainsAPM.Models.Definitions;
using ChainsAPM.Models.Instrumentation;

namespace ChainsAPM.Config
{
    public class ApplicationConfig : IBaseConfig, IInstrumentationMethodGroupConfig, IInstrumentationPointConfig
    {
        public ApplicationGroupConfig ParentApplicationGroup { get; set; }
        public IDictionary<InstrumentationGroup, bool> InstrumentationGroupCollection { get; set; }

        public IDictionary<Method, InstrumentationPoint> InstrumentationPointCollection { get; set; }
        public string ConfigName
        {
            get; set;
        }

        public bool IsMethodEntryPoint(Models.Definitions.Method methodid)
        {
            if (InstrumentationPointCollection.ContainsKey(methodid))
            {
                if (InstrumentationPointCollection[methodid].InstrumentationMethod.IsEntryPoint)
                    return true;
            }
            return ParentApplicationGroup.IsMethodEntryPoint(methodid);
        }
    }
}
