namespace ChainsAPM.Models {
        public class EntryPoint : StackItem {

                public MongoDB.Bson.ObjectId _id { get; set; }
                public int CurrentDepth { get; set; }
                

                public override bool UpdateStack (StackItem stackitem) {

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
