using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Models.Instrumentation
{
    public class InstrumentationPoint
    {
        public Models.Definitions.Method InstrumentationMethod { get; set; }
        /// <summary>
        /// Defines the method to place at the entrypoint of our montired method.
        /// </summary>
        public InstrumentationMethod EnterMethod { get; set; }
        /// <summary>
        /// Defines the method that will be placed at the exits of the monitored method
        /// </summary>
        public InstrumentationMethod ExitMethod { get; set; }
        /// <summary>
        /// Collection that will be used to provide additional details not able to be captured at method entry.
        /// </summary>
        public ICollection<InstrumentationMethod> InformationalMethods { get; set; }
    }
}
