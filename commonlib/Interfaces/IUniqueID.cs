using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Interfaces
{
    interface IUniqueID<T> where T : IComparable
    {
        T _id { get; set; }
    }
}
