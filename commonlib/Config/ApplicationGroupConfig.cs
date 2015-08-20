namespace ChainsAPM.Config
{
    public class ApplicationGroupConfig
    {
        public MachineConfig ParentMachine { get; set; }
        bool IsFunctionEntryPoint(string functionName)
        {
            if (functionName == "Recursive")
                return true;

            return false;
        }
    }
}
