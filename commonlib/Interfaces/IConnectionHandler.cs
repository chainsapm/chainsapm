using System;

namespace ChainsAPM.Interfaces
{
    public delegate void HasDataEvent(object sender);
    public delegate void DisconnectedEvent(object sender);

    public interface IConnectionHandler : IDisposable
    {
        
        event HasDataEvent HasData;
        event DisconnectedEvent Disconnected;

        void SendCommand(ICommand<byte> command);
        void SendCommands(ICommand<byte>[] command);

        ICommand<byte>[] GetCommands();
        ICommand<byte> GetCommand();


        bool Disconnect();
        bool Recycle();
        bool Flush();
    }
}
