using System;
using System.Text;

namespace ChainsAPM.Models.Definitions {
        public class Method : System.IComparable<Method>, IEquatable<Method> {
               
                public Class Class { get; set; }
                public long MethodId { get; set; }
                public string MethodName { get; set; }
                public Signature Signature { get; set; }
                public bool IsEntryPoint { get; set; }
                public bool AlwaysCreateNew { get; set; }

                public override string ToString () {
                        var sb = new StringBuilder ();
                        if ( Class != null ) {
                                sb.Append (Class);
                                sb.Append ("::");
                        }
                        sb.Append (MethodName.Trim());
                        sb.Append (Signature);
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

                public override int GetHashCode () {
                        return this.ToString ().GetHashCode ();
                        
                }
        }
}
