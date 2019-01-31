using System.Collections.Generic;
using ChainsAPM.Interfaces.Config;

namespace ChainsAPM.Config
{
    public class ResourceGroupConfig : IBaseConfig
    {
        public List<ResourceConfig> Resources { get; set; }
        public string ConfigName
        {
            get; set;
        }
    }
}
