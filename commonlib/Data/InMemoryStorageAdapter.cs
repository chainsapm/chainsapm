using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ChainsAPM.Interfaces;
using ChainsAPM.Interfaces.Data;
using ChainsAPM.Models;

namespace ChainsAPM.Data
{
    public class InMemoryStorageAdapter : IDataAdapter
    {
        public Dictionary<IComparable, EntryPoint> EntryPoints { get; set; }

        public InMemoryStorageAdapter()
        {
            EntryPoints = new Dictionary<IComparable, EntryPoint>();
        }
        private bool AddEntrypoint(EntryPoint entrypoint, int counter = 10)
        {
            if (counter == -1)
            {
                return false;
            }
            var itemEntry = entrypoint as EntryPoint;
            if (!EntryPoints.ContainsKey(itemEntry.GenericID))
            {
                EntryPoints.Add(itemEntry.GenericID, itemEntry);
                return true;
            }
            ++itemEntry.GenericID;
            return AddEntrypoint(entrypoint, --counter);

        }
        public bool InsertEntryPoint(IStackItem entry)
        {
            try
            {
                var InMemoryEntry = entry as ChainsAPM.Models.EntryPoint;
                if (InMemoryEntry._id.Equals(0))
                {
                    InMemoryEntry.GenericID = DateTime.Now.Ticks;
                }
                return AddEntrypoint(InMemoryEntry);
            }
            catch (Exception)
            {
                return false; // More than likely we have two entrypoints at the same time.
            }
            return false; // More than likely we have two entrypoints at the same time.
        }

        public bool UpdateEntryPoint(IStackItem entry)
        {
            var InMemoryEntry = entry as ChainsAPM.Models.EntryPoint;
            if (InMemoryEntry._id.Equals(0))
            {
                return false; // This has not been inserted yet.
            }
            EntryPoints[InMemoryEntry.GenericID] = InMemoryEntry;
            return true;
        }
    }
}
