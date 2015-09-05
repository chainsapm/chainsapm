using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ChainsAPM.Models;

namespace ChainsAPM.Interfaces {
        public interface IStackItem {
                ItemType Type { get; set; }
                uint Depth { get; set; }
                string Name { get; set; }
                long OriginalTimeStamp { get; set; }
                List<KeyValuePair<string, string>> Properties { get; set; }
                DateTime Started { get; set; }
                DateTime Finished { get; set; }
                long Elapsed { get; set; }
                Queue<IStackItem> Children { get; set; }
                IStackItem CurrentChild { get; set; }
                long StackItemIdentifier { get; set; }
                bool UpdateStack (IStackItem stackitem);
        }
}
