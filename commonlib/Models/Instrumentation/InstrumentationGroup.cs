using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Models.Instrumentation {
        public class InstrumentationGroup {
                ICollection<InstrumentationPoint> InstrumentationPoints {get; set; }
                public string GroupName { get; set; }
        }
}
