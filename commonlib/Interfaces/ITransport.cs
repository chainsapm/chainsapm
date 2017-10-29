using System;
using System.Threading.Tasks;

namespace ChainsAPM.Interfaces
{
    public interface ITransport<Tdata> : IDisposable
    {
        System.Net.Sockets.Socket Socket { get;}
        bool HasData { get; }
        Task<bool> Send(Tdata data);
        Task<Tdata> Receive();
        bool Disconnect();
        bool Connected { get; }
    }

}
