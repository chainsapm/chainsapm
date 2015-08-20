namespace ChainsAPM.Models
{
    public class Parameter
    {
        public enum RefOrVal
        {
            @ref,
            @out
        }
        public string Type { get; set; }
        public string Name { get; set; }
        public RefOrVal AccessType { get; set; }

        public override string ToString()
        {
            return string.Format("{0} {1} {1}", AccessType, Type, Name);
        }
    }
}
