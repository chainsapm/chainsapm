using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace injectedmethods.CustomAttribute {
        [System.AttributeUsage(AttributeTargets.Class, AllowMultiple = true)]
        public class AssemblyTypeInformation : Attribute {
                public string ModuleToTarget{ get; set; }
                public string TypeToTarget { get; set; }
                public AssemblyTypeInformation(string Module, string Type) {
                        ModuleToTarget = Module;
                        TypeToTarget = Type;
                }
        }

        [System.AttributeUsage (AttributeTargets.Method, AllowMultiple = true)]
        public class LocalVariableFixup : Attribute {
                public int ThisLocalIndex { get; set; }
                public int TargetLocalIndex { get; set; }
                public string TargetLocalType { get; set; }
                public LocalVariableFixup (int ThisIndex, int TargetIndex, string TargetType) {
                        ThisLocalIndex = ThisIndex;
                        TargetLocalIndex = TargetIndex;
                        TargetLocalType = TargetType;
                }
        }
}
