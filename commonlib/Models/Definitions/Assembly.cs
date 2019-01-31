using System;

namespace ChainsAPM.Models.Definitions
{
    public class Assembly : System.IComparable<Assembly>, IEquatable<Assembly>
    {
        public long AssemblyId { get; set; }
        public string AssemblyName { get; set; }

        public int CompareTo(Assembly other)
        {
            throw new NotImplementedException();
        }

        public bool Equals(Assembly other)
        {
            throw new NotImplementedException();
        }
    }
}
