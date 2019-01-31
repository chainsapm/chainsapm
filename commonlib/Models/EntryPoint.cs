using System;
using ChainsAPM.Interfaces;

namespace ChainsAPM.Models
{
    public class EntryPoint : StackItemBase
    {

        public MongoDB.Bson.ObjectId _id { get; set; }
        public long GenericID { get; set; }
        public uint CurrentDepth { get; set; }

        private object lockobj = new object();


        public override bool UpdateStack(IStackItem stackitem)
        {

            lock (lockobj)
            {
                if (stackitem.Type == ItemType.Entry)
                    ++CurrentDepth;
                stackitem.Depth = CurrentDepth;
                try
                {
                    return base.UpdateStack(stackitem);
                }
                finally
                {
                    if (stackitem.Type == ItemType.Exit && CurrentDepth != 0)
                    {
                        --CurrentDepth;
                    }

                }
            }
        }
    }
}
