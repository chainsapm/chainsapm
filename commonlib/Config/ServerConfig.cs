using ChainsAPM.Interfaces.Config;

namespace ChainsAPM.Config {
        public class ServerConfig : IBaseConfig {
                public class DatabaseNode {
                        public System.Reflection.Assembly DatabaseAdapter { get; set; }
                        public string DatabaseName { get; set; }
                        public string DatabaseServer { get; set; }
                        public string DatabasePort { get; set; }
                }
                public class ServerNode {
                        public enum Role {
                                Aggregator,
                                BackEnd,
                                FrontEnd
                        }
                        public string ServerName { get; set; }
                        public Role ServerRole { get; set; }
                        public string IPAddress { get; set; }
                        public int Port { get; set; }
                        public bool UseIPv6 { get; set; }

                }
                public string ConfigName {
                        get; set;
                }
                public ServerNode Server { get; set; }
                public DatabaseNode DocumentDB { get; set; }
                public DatabaseNode RelationalDB { get; set; }


        }
}
