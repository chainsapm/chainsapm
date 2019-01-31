using System;
using System.Text;

namespace ChainsAPM.Models.Definitions
{
    public class Module : IComparable<Module>, IEquatable<Module>
    {

        public Assembly Assembly { get; set; }

        public Int64 ModuleID { get; set; }
        public string ModuleName { get; set; }
        public int CompareTo(Module other)
        {
            throw new NotImplementedException();
        }

        public bool Equals(Module other)
        {
            throw new NotImplementedException();
        }

        public override string ToString()
        {

            var sb = new StringBuilder();
            if (Assembly != null)
            {
                sb.Append(Assembly);
                sb.Append(" ");
            }
            sb.Append(ModuleName.Trim());
            return sb.ToString();
        }
    }
}
