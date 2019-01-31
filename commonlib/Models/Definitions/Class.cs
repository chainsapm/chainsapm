using System;
using System.Text;

namespace ChainsAPM.Models.Definitions
{
    public class Class : IComparable<Class>, IEquatable<Class>
    {

        public Module Module { get; set; }
        public long ClassId { get; set; }
        public string ClassName { get; set; }

        public int CompareTo(Class other)
        {
            return ClassName.CompareTo(other.ClassName);
        }
        public bool Equals(Class other)
        {
            return ClassName.Equals(other.ClassName);
        }

        public override string ToString()
        {
            var sb = new StringBuilder();
            if (Module != null)
            {
                sb.Append(Module);
                sb.Append("!");
            }
            sb.Append(ClassName.Trim());

            return sb.ToString();
        }
        public override int GetHashCode()
        {
            return this.ToString().GetHashCode();
        }
    }
}
