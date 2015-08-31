using System;
using ChainsAPM.Interfaces;

namespace ChainsAPM.Models {
        public class EntryPoint:  StackItemBase {

                public MongoDB.Bson.ObjectId _id { get; set; }
                public long GenericID { get; set; }
                public int CurrentDepth { get; set; }
                

                public override bool UpdateStack (IStackItem stackitem) {

                        if ( stackitem.Type == ItemType.Entry )
                                ++CurrentDepth;
                        stackitem.Depth = CurrentDepth;
                        try {
                                return base.UpdateStack (stackitem);
                        }
                        finally {
                                if ( stackitem.Type == ItemType.Exit )
                                        --CurrentDepth;
                        }
                }
        }
}
