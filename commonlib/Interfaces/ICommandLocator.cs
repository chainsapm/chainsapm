using System;

namespace ChainsAPM.Interfaces
{
    public interface ICommandLocator<T>
    {
        ICommand<T> ProcessCommands(ICommand<T> command);

        ICommand<T> ProcessData(ArraySegment<T> input);
    }
}
