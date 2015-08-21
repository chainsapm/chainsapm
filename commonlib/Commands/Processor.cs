using ChainsAPM.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Remoting.Messaging;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Commands
{
    public class Processor
    {
        private Dictionary<int, ICommand<byte>> CommandList;

        public Processor()
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

                CallContext.LogicalSetData("CommandProviders", CommandList);
            }
            catch (Exception)
            {
                // TODO Logging
                throw;
            }
        }
    }
}
