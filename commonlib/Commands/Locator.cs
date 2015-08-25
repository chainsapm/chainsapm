using ChainsAPM.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Commands
{
    public class ByteCommandLocator : ICommandLocator<byte>
    {
        private Dictionary<int, ICommand<byte>> CommandList;

        public ByteCommandLocator()
        {
            CommandList = new Dictionary<int, ICommand<byte>>();
        }

        public void CreateList()
        {
            try
            {
                var all_classes = AppDomain.CurrentDomain.GetAssemblies()
                        .SelectMany(assem => assem.GetTypes())
                        .Where(t => t.IsClass && t.GetInterface("ICommand`1") != null);

                foreach (var item in all_classes)
                {
                    ICommand<byte> cmd = Activator.CreateInstance(item) as ICommand<byte>;
                    CommandList.Add(cmd.Code, cmd);
                }
            }
            catch (Exception)
            {
                // TODO Logging
                throw;
            }
        }

        public ICommand<byte> ProcessCommands(ICommand<byte> command)
        {
            throw new NotImplementedException();
        }

        public ICommand<byte> ProcessData(ArraySegment<byte> command)
        {
            var size = BitConverter.ToInt32(command.Array, command.Offset);
            var code = command.Array[command.Offset + 4];
            return CommandList[code].Decode(command);
        }
    }
}
