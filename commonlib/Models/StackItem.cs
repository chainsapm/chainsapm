using System;
using System.Collections.Generic;
using System.Linq;

namespace ChainsAPM.Models
{
    public class StackItem
    {
        public enum ItemType
        {
            Entry,
            Exit,
            Informational
        }
        public ItemType Type { get; set; }
        public int Depth { get; set; }
        public string Name { get; set; }
        public long Id { get; set; }
        public long OriginalTimeStamp { get; set; }
        public List<KeyValuePair<string, string>> Properties { get; set; }
        public DateTime Started { get; set; }
        public DateTime Finished { get; set; }
        public long Elapsed { get; set; }
        public Stack<StackItem> Children { get; set; }
        public StackItem CurrentChild { get; set; }
        public virtual bool UpdateStack(StackItem stackitem)
        {
            if (Children == null)
                Children = new Stack<StackItem>();

            if (stackitem.Depth == Depth + 1)
            {
                if (stackitem.Type == ItemType.Entry)
                {
                    Children.Push(stackitem);
                }
                else
                {
                    var lastItem = Children.Last();
                    if (stackitem.Id == lastItem.Id)
                    {
                        lastItem.Elapsed = stackitem.OriginalTimeStamp - lastItem.OriginalTimeStamp;
                        lastItem.Finished = DateTime.FromFileTimeUtc(stackitem.OriginalTimeStamp);
                    }

                }
            }
            else
            {
                if (Children.Last() == null)
                {
                    // TODO Create a corrupted StackItem representation
                    // Push that corrupt item to the stack
                    // Update the stack Item
                }
                return Children.Last().UpdateStack(stackitem);
            }
            return true;
        }

    }
}
