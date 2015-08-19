using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Classes
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
        public int Depth{ get; set; }
        public string Name { get; set; }
        public long Id { get; set; }
        public long OriginalTimeStamp { get; set; }
        public List<KeyValuePair<string, string>> Properties { get; set; }
        public DateTime Started { get; set; }
        public DateTime Finished { get; set; }
        public long Elapsed { get; set; }
        public List<StackItem> Children { get; set; }
        public StackItem CurrentChild { get; set; }
        public virtual bool UpdateStack(StackItem stackitem)
        {
            if (stackitem.Depth == Depth + 1)
            {
                if (Children == null)
                {
                    Children = new List<StackItem>();
                    CurrentChild = stackitem;
                    Children.Add(stackitem);
                }
                CurrentChild.UpdateStack(stackitem);
            }
            if (stackitem.Depth == Depth)
            {
                if (stackitem.Type == ItemType.Exit)
                {
                    if (Id == stackitem.Id)
                    {
                        Finished = DateTime.FromFileTimeUtc(stackitem.OriginalTimeStamp);
                        Elapsed = stackitem.OriginalTimeStamp - OriginalTimeStamp;
                    }
                }
                if (stackitem.Type == ItemType.Entry)
                {
                    if (Children == null)
                    {
                        Children = new List<StackItem>();
                        CurrentChild = stackitem;
                        Children.Add(stackitem);
                    } else
                    {
                        //if (CurrentChild)
                        //{

                        //}
                    }
                    
                }
            }
            
            return true;
        }
    }
}
