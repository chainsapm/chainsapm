namespace ChainsAPM.Interfaces
{
    public interface IPacketAssembler<T, Tdata>
    {
        /// <summary>
        /// Output an instance of <see cref="{Tdata}"/> of a single type of <see cref="{T}"/>.
        /// </summary>
        /// <param name="ObjectToSend"></param>
        /// <returns></returns>
        Tdata SendItem(T ObjectToSend);
        /// <summary>
        /// Output an instance of <see cref="{Tdata}"/> with a mulitple type <see cref="{T[]}"/>.
        /// </summary>
        /// <param name="ObjectToSend"></param>
        /// <returns></returns>
        Tdata SendItems(T[] ObjectToSend);
    }
}
