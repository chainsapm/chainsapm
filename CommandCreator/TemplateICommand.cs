using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CommandCreator
{
    public partial class CommandInterface : CommandInterfaceBase
    {

        public List<NetworkCommandTreeItemModel> Commands { get; set; }
        public CommandInterface(List<NetworkCommandTreeItemModel> cmds)
        {
            Commands = cmds;
        }
    }
}
