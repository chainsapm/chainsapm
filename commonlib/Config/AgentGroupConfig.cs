using System.Collections.Generic;

namespace ChainsAPM.Config
{
    public class AgentGroupConfig
    {
        public Dictionary<long, Models.Method> MethodList { get; set; }
        public ApplicationConfig ParentApplication { get; set; }

        public bool IsMethodEntryPoint(long methodid)
        {
            if (MethodList.ContainsKey(methodid))
            {
                if (MethodList[methodid].IsEntryPoint)
                    return true;
            }
            return ParentApplication.IsMethodEntryPoint(methodid);
        }
    }
}
