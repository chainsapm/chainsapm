using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ChainsAPM.Commands.Agent;
using ChainsAPM.Config;
using ChainsAPM.Interfaces;
using ChainsAPM.Interfaces.Config;
using ChainsAPM.Interfaces.Data;
using ChainsAPM.Models.Instrumentation;
using ChainsAPM.Models.Resource;

namespace ChainsAPM.Data {
        class DefaultConfigAdapter : IConfigDataAdapter {
                public AgentConfig ReadAgentConfig (AgentInformation AgentInformation) {
                        
                        var ac = new AgentConfig ()
                        {
                                InstrumentationGroupCollection = new Dictionary<InstrumentationGroup, bool> (),
                                InstrumentationPointCollection = new Dictionary<Models.Definitions.Method, InstrumentationPoint> ()

                        };
                        InstrumentationGroup igData = new InstrumentationGroup ()
                        {
                                GroupName = "Generic Database",
                                InstrumentationPoints = new List<InstrumentationPoint> ()
                                
                        };
                        igData.InstrumentationPoints.Add (new InstrumentationPoint ()
                        {
                                InstrumentationMethod = new Models.Definitions.Method ()
                                {
                                        MethodName = "Open",
                                        Class = new Models.Definitions.Class()
                                        {
                                                ClassName = "System.Data.IDbConnection"
                                        }
                                }
                        });
                        igData.InstrumentationPoints.Add (new InstrumentationPoint ()
                        {
                                InstrumentationMethod = new Models.Definitions.Method ()
                                {
                                        MethodName = "Close",
                                        Class = new Models.Definitions.Class ()
                                        {
                                                ClassName = "System.Data.IDbConnection"
                                        }
                                }
                        });
                        igData.InstrumentationPoints.Add (new InstrumentationPoint ()
                        {
                                InstrumentationMethod = new Models.Definitions.Method ()
                                {
                                        MethodName = "CreateCommand",
                                        Class = new Models.Definitions.Class ()
                                        {
                                                ClassName = "System.Data.IDbConnection"
                                        }
                                }
                        });

                        ac.InstrumentationGroupCollection.Add (igData, true);

                        InstrumentationGroup igSqlClient = new InstrumentationGroup ()
                        {
                                GroupName = "SqlClient",
                                InstrumentationPoints = new List<InstrumentationPoint> ()

                        };
                        igSqlClient.InstrumentationPoints.Add (new InstrumentationPoint ()
                        {
                                InstrumentationMethod = new Models.Definitions.Method ()
                                {
                                        MethodName = "Open",
                                        Class = new Models.Definitions.Class ()
                                        {
                                                ClassName = "System.Data.SqlClient"
                                        }
                                }
                        });
                        igSqlClient.InstrumentationPoints.Add (new InstrumentationPoint ()
                        {
                                InstrumentationMethod = new Models.Definitions.Method ()
                                {
                                        MethodName = "Close",
                                        Class = new Models.Definitions.Class ()
                                        {
                                                ClassName = "System.Data.SqlClient"
                                        }
                                }
                        });
                        igSqlClient.InstrumentationPoints.Add (new InstrumentationPoint ()
                        {
                                InstrumentationMethod = new Models.Definitions.Method ()
                                {
                                        MethodName = "CreateCommand",
                                        Class = new Models.Definitions.Class ()
                                        {
                                                ClassName = "System.Data.SqlClient"
                                        }
                                }
                        });
                        ac.InstrumentationGroupCollection.Add (igSqlClient, true);

                        InstrumentationGroup igThreading = new InstrumentationGroup ()
                        {
                                GroupName = "Generic Threading",
                                InstrumentationPoints = new List<InstrumentationPoint> ()

                        };
                        igThreading.InstrumentationPoints.Add (new InstrumentationPoint ()
                        {
                                InstrumentationMethod = new Models.Definitions.Method ()
                                {
                                        MethodName = "Start",
                                        Class = new Models.Definitions.Class ()
                                        {
                                                ClassName = "System.Threading.Thread"
                                        }
                                }
                        });

                        igThreading.InstrumentationPoints.Add (new InstrumentationPoint ()
                        {
                                InstrumentationMethod = new Models.Definitions.Method ()
                                {
                                        MethodName = "Sleep",
                                        Class = new Models.Definitions.Class ()
                                        {
                                                ClassName = "System.Threading.Thread"
                                        }
                                },
                                
                        });

                        igThreading.InstrumentationPoints.Add (new InstrumentationPoint ()
                        {
                                InstrumentationMethod = new Models.Definitions.Method ()
                                {
                                        MethodName = "SpinWait",
                                        Class = new Models.Definitions.Class ()
                                        {
                                                ClassName = "System.Threading.Thread"
                                        }
                                }
                        });

                        igThreading.InstrumentationPoints.Add (new InstrumentationPoint ()
                        {
                                InstrumentationMethod = new Models.Definitions.Method ()
                                {
                                        MethodName = "WaitOne",
                                        Class = new Models.Definitions.Class ()
                                        {
                                                ClassName = "System.Threading.WaitHandle"
                                        }
                                }
                        });
                        igThreading.InstrumentationPoints.Add (new InstrumentationPoint ()
                        {
                                InstrumentationMethod = new Models.Definitions.Method ()
                                {
                                        MethodName = "WaitMultiple",
                                        Class = new Models.Definitions.Class ()
                                        {
                                                ClassName = "System.Threading.WaitHandle"
                                        }
                                }
                        });
                        ac.InstrumentationGroupCollection.Add (igSqlClient, true);


                        //commandsList.MethodClassList.Add ("System.Data.SqlClient.SqlCommand");
                        //commandsList.MethodList.Add ("Prepare");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);
                        //commandsList.MethodClassList.Add ("System.Data.SqlClient.SqlCommand");
                        //commandsList.MethodList.Add ("ExecuteReader");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);
                        //commandsList.MethodClassList.Add ("System.Data.SqlClient.SqlCommand");
                        //commandsList.MethodList.Add ("ExecuteScalar");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);
                        //commandsList.MethodClassList.Add ("System.Data.SqlClient.SqlCommand");
                        //commandsList.MethodList.Add ("ExecuteNonQuery");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);

                        //commandsList.MethodClassList.Add ("System.Data.SqlClient.SqlConnection");
                        //commandsList.MethodList.Add ("Open");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);
                        //commandsList.MethodClassList.Add ("System.Data.SqlClient.SqlConnection");
                        //commandsList.MethodList.Add ("Close");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);
                        //commandsList.MethodClassList.Add ("System.Data.SqlClient.SqlConnection");
                        //commandsList.MethodList.Add ("Dispose");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);

                        //commandsList.MethodClassList.Add ("System.Data.SqlClient.SqlConnection");
                        //commandsList.MethodList.Add ("EnlistTransaction");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);
                        //commandsList.MethodClassList.Add ("System.Data.SqlClient.SqlConnection");
                        //commandsList.MethodList.Add ("EnlistDistributedTransaction");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);

                        //commandsList.MethodClassList.Add ("System.Net.HttpWebRequest");
                        //commandsList.MethodList.Add ("Create");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);
                        //commandsList.MethodClassList.Add ("System.Net.HttpWebRequest");
                        //commandsList.MethodList.Add ("GetResponse");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);

                        //commandsList.MethodClassList.Add ("HelloWorldTestHarness.Program");
                        //commandsList.MethodList.Add ("AddNumbers");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);

                        //commandsList.MethodClassList.Add ("HelloWorldTestHarness.Program");
                        //commandsList.MethodList.Add ("Recursive");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);

                        //commandsList.MethodClassList.Add ("HelloWorldTestHarness.Program");
                        //commandsList.MethodList.Add ("Main");
                        //commandsList.MethodPropList.Add (MethodsToInstrument.MethodProperties.Public);


                        ac.ParentAgentGroup = ReadAgentGroupConfig (ac);
                        return ac;
                }

                public AgentGroupConfig ReadAgentGroupConfig (AgentConfig AgentConfig) {
                        var ac = new AgentGroupConfig ()
                        {
                                InstrumentationGroupCollection = new Dictionary<InstrumentationGroup, bool> (),
                                InstrumentationPointCollection = new Dictionary<Models.Definitions.Method, InstrumentationPoint> ()

                        };
                        ac.ParentApplication = ReadApplicationConfig (ac);
                        return ac;
                }

                public ApplicationConfig ReadApplicationConfig (AgentGroupConfig AgentGroupConfig) {
                        var ac = new ApplicationConfig ()
                        {
                                InstrumentationGroupCollection = new Dictionary<InstrumentationGroup, bool> (),
                                InstrumentationPointCollection = new Dictionary<Models.Definitions.Method, InstrumentationPoint> ()

                        };
                        ac.ParentApplicationGroup = ReadApplicationGroupConfig (ac);
                        return ac;
                }

                public ApplicationGroupConfig ReadApplicationGroupConfig (ApplicationConfig ApplicationConfig) {
                        var ac = new ApplicationGroupConfig ()
                        {
                                InstrumentationGroupCollection = new Dictionary<InstrumentationGroup, bool> (),
                                InstrumentationPointCollection = new Dictionary<Models.Definitions.Method, InstrumentationPoint> ()
                        };
                        return ac;
                }


                public GlobalConfig ReadAggregatorConfig (AgentConfig AgentGroupConfig) {
                        throw new NotImplementedException ();
                }
                public GlobalConfig ReadGlobalConfig () {
                        throw new NotImplementedException ();
                }

                public MachineConfig ReadMachineConfig (Machine Machine) {
                        throw new NotImplementedException ();
                }

                public ResourceConfig ReadResourceConfig (Resource Resource) {
                        throw new NotImplementedException ();
                }

                public ResourceGroupConfig ReadResourceGroupConfig (ResourceConfig ResourceConfig) {
                        throw new NotImplementedException ();
                }

                public GlobalConfig ReadServerConfig (AgentConfig AgentGroupConfig) {
                        throw new NotImplementedException ();
                }
        }
}
