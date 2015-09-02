using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ChainsAPM.Models.Instrumentation {
        public class TokenReplacement {
                public enum TokenDefEnum {
                        TypeRef,
                        TypeDef,
                        MemberRef,
                        MemberDef
                }
                public Int64 Token { get; set; }
                public TokenDefEnum TokenDef { get; set; }
        }
}
