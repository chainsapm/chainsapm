namespace ChainsAPM.Interfaces
{
    public interface IPacketDisassembler<T, Tdata>
    {
        T ReceiveItem(Tdata data);
        T[] ReceiveItems(Tdata data);
    }

}
