using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace injectedmethods.CustomAttribute {
        [System.AttributeUsage(System.AttributeTargets.Class)]
        public class TypeToReplace : Attribute {
                public string ModuleToTarget{ get; set; }
                public string TypeToTarget { get; set; }
                public string MethodToTarget { get; set; }
                public TypeToReplace(string Module, string Type, string Method) {
                        ModuleToTarget = Module;
                        TypeToTarget = Type;
                        MethodToTarget = Method;
                }
        }
}
