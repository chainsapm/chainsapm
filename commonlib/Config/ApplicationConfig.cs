using System.Collections.Generic;

namespace ChainsAPM.Config
{
    public class ApplicationConfig
    {
        public ApplicationGroupConfig ParentApplicationGroup { get; set; }
        public Dictionary<long, Models.Method> MethodList { get; set; }
        public bool IsMethodEntryPoint(long methodid)
        {
            if (MethodList.ContainsKey(methodid))
            {
                if (MethodList[methodid].IsEntryPoint)
                    return true;
            }
            return false;
        }
    }
}
