using System;
using System.Text;

namespace ChainsAPM.Models.Definitions {
        public class Method : System.IComparable<Method>, IEquatable<Method> {
                public Assembly Assembly { get; set; }
                public Module Module { get; set; }
                public Class Class { get; set; }
                public long MethodId { get; set; }
                public string MethodName { get; set; }
                public Signature Signature { get; set; }
                public bool IsEntryPoint { get; set; }
                public bool AlwaysCreateNew { get; set; }

                public override string ToString () {
                        var sb = new StringBuilder ();
                        sb.Append (MethodName);
                        sb.Append (Signature);
                        return sb.ToString ();
                }

                public string FullName () {
                        var sb = new StringBuilder ();
                        sb.Append (Assembly);
                        sb.Append (" ");
                        sb.Append (Module);
                        sb.Append ("!");
                        sb.Append (Class);
                        sb.Append ("::");
                        sb.Append (this);
                        return sb.ToString ();
                }

                public bool Equals (Method other) {
                        if ( MethodName.Equals(other.MethodName)   
                                && Class.Equals(other.Class) ) {
                                return true;
                        }
                        return false;
                }

                public int CompareTo (Method other) {
                        return MethodName.CompareTo (other.MethodName);
                }
        }
}
