using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Models.Instrumentation
{
    public class InstrumentationMethod
    {
        public System.Reflection.Assembly ILAssembly { get; set; }
        public byte[] MethodSignature { get; set; }
        public byte[] LocalsSignature { get; set; }
        public byte[] ILBytes { get; set; }
        public string Method { get; set; }
        public int Offset { get; set; }
        public string Name { get; set; }
        public string Description { get; set; }
        public ICollection<TokenReplacement> TokensToReplace { get; set; }
    }
}
