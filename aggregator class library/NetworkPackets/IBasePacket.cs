using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace aggregator.NetworkPackets
{
    enum PacketType
    {
        StackItem,
        Information,
        Event,
        NodeUpdate,
        AggregagtorUpdate,
        RelayUpdate,
        ServerUpdate
    }

    [Serializable]
    class IBaseItem
    {
        UInt64 NodeID;
        UInt64 TimeStamp;
        PacketType PacketType;
    }

   
}
