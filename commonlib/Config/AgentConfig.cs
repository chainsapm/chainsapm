using System.Collections.Generic;

namespace ChainsAPM.Config
{
    public class AgentConfig
    {
        public AgentGroupConfig ParentAgentGroup { get; set; }
        public Dictionary<long,  Models.Method> MethodList{ get; set; }
        bool IsMethodEntryPoint(long methodid)
        {
            if (MethodList.ContainsKey(methodid))
            {
                if (MethodList[methodid].IsEntryPoint)
                    return true;
            }
            return ParentAgentGroup.IsMethodEntryPoint(methodid);
        }
    }
}
