using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Interfaces.Config {
        public interface IInstrumentationPointConfig {
                IDictionary<Models.Definitions.Method, Models.Instrumentation.InstrumentationPoint> InstrumentationPointCollection { get; set; }
        }
}
