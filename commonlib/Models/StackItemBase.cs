using System;
using System.Collections.Generic;
using System.Linq;
using ChainsAPM.Interfaces;

namespace ChainsAPM.Models {
        public enum ItemType {
                Entry,
                Exit,
                Informational
        }
        public class StackItemBase : IStackItem {

                public ItemType Type { get; set; }
                public uint Depth { get; set; }
                public string Name { get; set; }
                public long OriginalTimeStamp { get; set; }
                public List<KeyValuePair<string, string>> Properties { get; set; }
                public DateTime Started { get; set; }
                public DateTime Finished { get; set; }
                public long Elapsed { get; set; }
                public Queue<IStackItem> Children { get; set; }
                public IStackItem CurrentChild { get; set; }
                public long StackItemIdentifier { get; set; }
                public long MethodDef { get; set; }
                public string MachineName { get; set; }
                public string AgentName { get; set; }
                public string ProcessName { get; set; }
                public string ApplicationName { get; set; }

                public virtual bool UpdateStack (IStackItem stackitem) {
                        if ( Children == null )
                                Children = new Queue<IStackItem> ();

                        if ( stackitem.Depth == Depth + 1 ) {
                                switch ( stackitem.Type ) {
                                        case ItemType.Entry:
                                                Children.Enqueue (stackitem);
                                                return true;
                                        case ItemType.Exit:
                                        case ItemType.Informational:
                                                return Children.Last ().UpdateStack (stackitem);
                                        default:
                                                return false;
                                }
                        } else if ( stackitem.Depth == Depth ) {
                                switch ( stackitem.Type ) {
                                        case ItemType.Entry:
                                                // TODO add in logic to handle a corruption
                                                return false;
                                        case ItemType.Exit:
                                                if ( stackitem.StackItemIdentifier == StackItemIdentifier ) {
                                                        Elapsed = stackitem.OriginalTimeStamp - OriginalTimeStamp;
                                                        Finished = DateTime.FromFileTimeUtc (stackitem.OriginalTimeStamp);
                                                        Type = stackitem.Type;
                                                }
                                                return true;
                                        case ItemType.Informational:
                                                //Update stack properties?
                                                return true;
                                        default:
                                                return false;
                                }

                        }
                        // Still not at the proper depth
                        try {

                                return Children.Last ().UpdateStack (stackitem);
                        }
                        catch ( Exception ) {

                                throw;
                        }


                }
        }
}

