using System.Collections.Generic;
using System.Collections.ObjectModel;
using ChainsAPM.Interfaces.Config;

namespace ChainsAPM.Config
{
    public  class GlobalConfig : IBaseConfig
    {
        public ObservableCollection<ResourceGroupConfig> ResourceGroups { get; set; }
        public ObservableCollection<ApplicationGroupConfig> ApplicationGroups { get; set; }

                public string ConfigName {
                        get; set;
                }
        }
}
