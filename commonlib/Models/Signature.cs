using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ChainsAPM.Models
{
    public class Signature
    {
        public List<Parameter> Parameters { get; set; }
        public override string ToString()
        {
            var sb = new StringBuilder();
            sb.Append("(");
            foreach (var item in Parameters)
            {
                if (Parameters.Last() == item)
                    sb.Append(string.Format("{0}, ",item));
                else
                    sb.Append(string.Format("{0}", item));

            }
            sb.Append(")");
            return sb.ToString();
        }
    }
}
