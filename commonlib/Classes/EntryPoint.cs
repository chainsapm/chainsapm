using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ChainsAPM.Commands.Agent;

namespace ChainsAPM.Classes
{
    public class EntryPoint : StackItem
    {

        public int CurrentDepth { get; set; }

        public override bool UpdateStack(StackItem stackitem)
        {
            
            if (stackitem.Type == ItemType.Entry)
            {
                stackitem.Depth = ++CurrentDepth;
                if (Children == null)
                {
                    Children = new List<StackItem>();

                }
                CurrentChild.UpdateStack(stackitem);
            }
            return true;
        }
    }
}
