using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Interfaces.Config
{
    public interface IInstrumentationMethodGroupConfig
    {
        IDictionary<ChainsAPM.Models.Instrumentation.InstrumentationGroup, bool> InstrumentationGroupCollection { get; set; }
    }
}
