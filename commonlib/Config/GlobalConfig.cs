using System.Collections.Generic;
using System.Collections.ObjectModel;

namespace ChainsAPM.Config
{
    public  class GlobalConfig
    {
        public ObservableCollection<ResourceGroupConfig> ResourceGroups { get; set; }
        public ObservableCollection<ApplicationGroupConfig> ApplicationGroups { get; set; }
    }
}
