using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Interfaces.Config
{
    public interface IInstrumentationMethodConfig
    {
        IDictionary<ChainsAPM.Models.Instrumentation.InstrumentationMethod, bool> InstrumentationMethodCollection { get; set; }
    }
}
