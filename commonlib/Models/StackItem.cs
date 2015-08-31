using System;
using System.Collections.Generic;
using System.Linq;

namespace ChainsAPM.Models {
        public class StackItem {
                public enum ItemType {
                        Entry,
                        Exit,
                        Informational
                }
                public ItemType Type { get; set; }
                public int Depth { get; set; }
                public string Name { get; set; }
                public long MethodDef { get; set; }
                public long OriginalTimeStamp { get; set; }
                public List<KeyValuePair<string, string>> Properties { get; set; }
                public DateTime Started { get; set; }
                public DateTime Finished { get; set; }
                public long Elapsed { get; set; }
                public Queue<StackItem> Children { get; set; }
                public StackItem CurrentChild { get; set; }
                public virtual bool UpdateStack (StackItem stackitem) {
                        if ( Children == null )
                                Children = new Queue<StackItem> ();

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
                                                if ( stackitem.MethodDef == MethodDef ) {
                                                        Elapsed = stackitem.OriginalTimeStamp - OriginalTimeStamp;
                                                        Finished = DateTime.FromFileTimeUtc (stackitem.OriginalTimeStamp);
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
                        return Children.Last ().UpdateStack (stackitem);

                }
        }
}

