using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Interfaces.Data
{
    public interface IDataAdapter
    {
        bool InsertEntryPoint(IStackItem entry);
        bool UpdateEntryPoint(IStackItem entrypoint);
    }
}
