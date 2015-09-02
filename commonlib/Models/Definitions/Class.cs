using System;

namespace ChainsAPM.Models.Definitions {
    public class Class : IComparable<Class>, IEquatable<Class> 
    {
        public long ClassId { get; set; }
        public string ClassName { get; set; }

                public int CompareTo (Class other) {
                        return ClassName.CompareTo (other.ClassName);
                }

                public bool Equals (Class other) {
                        return ClassName.Equals (other.ClassName);
                }
        }
}
