using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Interfaces {
        interface IDocumentDBDataAdapter {
                bool InsertEntryPoint (ChainsAPM.Models.EntryPoint entrypoint);
                bool UpdateEntryPoint (ChainsAPM.Models.EntryPoint entrypoint);
        }
}
