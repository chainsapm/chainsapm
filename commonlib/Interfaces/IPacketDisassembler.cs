using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Interfaces
{
    public interface IPacketDisassembler<T, Tdata>
    {
        T ReceiveItem(Tdata data);
        T[] ReceiveItems(Tdata data);
    }

}
