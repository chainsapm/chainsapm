using System;
using System.Collections.Generic;
using ChainsAPM.Commands.Agent;
using ChainsAPM.Config;
using ChainsAPM.Interfaces.Data;
using ChainsAPM.Models.Instrumentation;
using ChainsAPM.Models.Resource;

namespace ChainsAPM.Data
{
    class DefaultConfigAdapter : IConfigDataAdapter
    {
        public AgentConfig ReadAgentConfig(AgentInformation AgentInformation)
        {

            var ac = new AgentConfig()
            {
                InstrumentationGroupCollection = new Dictionary<InstrumentationGroup, bool>(),
                InstrumentationPointCollection = new Dictionary<Models.Definitions.Method, InstrumentationPoint>(),
                ConfigName = "DefaultAgentConfig"

            };
            #region Generic IDbConnection
            InstrumentationGroup igData = new InstrumentationGroup()
            {
                GroupName = "Generic Database",
                InstrumentationPoints = new List<InstrumentationPoint>()

            };

            var idbConnectionClass = new Models.Definitions.Class()
            {
                ClassName = "System.Data.IDbConnection"
            };

            igData.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Open",
                    Class = idbConnectionClass
                }
            });
            igData.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Close",
                    Class = idbConnectionClass
                }
            });
            igData.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "CreateCommand",
                    Class = idbConnectionClass
                }
            });

            ac.InstrumentationGroupCollection.Add(igData, true);
            #endregion

            #region SqlClient Specific
            InstrumentationGroup igSqlClient = new InstrumentationGroup()
            {
                GroupName = "SqlClient",
                InstrumentationPoints = new List<InstrumentationPoint>()

            };

            var sqlClientClass = new Models.Definitions.Class()
            {
                ClassName = "System.Data.SqlClient"
            };
            igSqlClient.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Open",
                    Class = sqlClientClass
                }
            });
            igSqlClient.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Close",
                    Class = sqlClientClass
                }
            });
            igSqlClient.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "CreateCommand",
                    Class = sqlClientClass
                }
            });

            InstrumentationGroup igSqlClommand = new InstrumentationGroup()
            {
                GroupName = "SqlCommand",
                InstrumentationPoints = new List<InstrumentationPoint>()

            };

            var sqlCommandClass = new Models.Definitions.Class()
            {
                ClassName = "System.Data.SqlCommand"
            };

            igSqlClommand.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Prepare",
                    Class = sqlCommandClass
                }
            });

            igSqlClommand.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "ExecuteReader",
                    Class = sqlCommandClass
                }
            });

            igSqlClommand.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "ExecuteScalar",
                    Class = sqlCommandClass
                }
            });


            igSqlClommand.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "ExecuteNonQuery",
                    Class = sqlCommandClass
                }
            });


            InstrumentationGroup igSqlClonnection = new InstrumentationGroup()
            {
                GroupName = "SqlConnection",
                InstrumentationPoints = new List<InstrumentationPoint>()

            };

            var sqlConnectionClass = new Models.Definitions.Class()
            {
                ClassName = "System.Data.SqlConnection"
            };

            igSqlClonnection.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Open",
                    Class = sqlConnectionClass
                }
            });

            igSqlClonnection.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Close",
                    Class = sqlConnectionClass
                }
            });

            igSqlClonnection.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Dispose",
                    Class = sqlConnectionClass
                }
            });

            igSqlClonnection.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "EnlistTransaction",
                    Class = sqlConnectionClass
                }
            });

            igSqlClonnection.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "EnlistDistributedTransaction",
                    Class = sqlConnectionClass
                }
            });

            ac.InstrumentationGroupCollection.Add(igSqlClient, true);
            ac.InstrumentationGroupCollection.Add(igSqlClommand, true);
            ac.InstrumentationGroupCollection.Add(igSqlClonnection, true);

            #endregion

            #region Threading
            InstrumentationGroup igThreading = new InstrumentationGroup()
            {
                GroupName = "Generic Threading",
                InstrumentationPoints = new List<InstrumentationPoint>()

            };

            var threadingClass = new Models.Definitions.Class()
            {
                ClassName = "System.Threading.Thread"
            };

            igThreading.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Start",
                    Class = threadingClass
                }
            });

            igThreading.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Sleep",
                    Class = threadingClass
                },

            });

            igThreading.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "SpinWait",
                    Class = threadingClass
                }
            });

            igThreading.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "WaitOne",
                    Class = threadingClass
                }
            });
            igThreading.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "WaitMultiple",
                    Class = threadingClass
                }
            });
            ac.InstrumentationGroupCollection.Add(igThreading, true);
            #endregion

            #region Generic WebRequests
            InstrumentationGroup igWebRequests = new InstrumentationGroup()
            {
                GroupName = "Generic WebRequests",
                InstrumentationPoints = new List<InstrumentationPoint>()

            };

            var webRequestsClass = new Models.Definitions.Class()
            {
                ClassName = "System.Net.HttpWebRequest"
            };

            igWebRequests.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Create",
                    Class = webRequestsClass
                }
            });

            igWebRequests.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "GetResponse",
                    Class = webRequestsClass
                }
            });



            igWebRequests.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "BeginGetResponse",
                    Class = webRequestsClass
                }
            });

            igWebRequests.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "EndGetResponse",
                    Class = webRequestsClass
                }
            });

            igWebRequests.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "_ReadAResult",
                    Class = webRequestsClass
                }
            });

            igWebRequests.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "BeginSubmitRequest",
                    Class = webRequestsClass
                }
            });

            igWebRequests.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "EndSubmitRequest",
                    Class = webRequestsClass
                }
            });

            igWebRequests.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "SetRequestSubmitted",
                    Class = webRequestsClass
                }
            });

            igWebRequests.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "CheckDefferedCallDone",
                    Class = webRequestsClass
                }
            });


            igWebRequests.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "CheckProtocol",
                    Class = webRequestsClass
                }
            });

            igWebRequests.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "CheckWriteSideResponseProcessing",
                    Class = webRequestsClass
                }
            });



            ac.InstrumentationGroupCollection.Add(igWebRequests, true);
            #endregion

            #region HelloWorldTestHarness.exe
            InstrumentationGroup igMyApplication = new InstrumentationGroup()
            {
                GroupName = "HelloWorldTestHarness.exe",
                InstrumentationPoints = new List<InstrumentationPoint>()

            };

            var helloClass = new Models.Definitions.Class()
            {
                ClassName = "HelloWorldTestHarness.Program"
            };

            igMyApplication.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Main",
                    Class = helloClass,
                    IsEntryPoint = true
                }
            });

            igMyApplication.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Recursive",
                    Class = helloClass,
                }
            });
            igMyApplication.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "AddNumbers",
                    Class = helloClass,
                }
            });

            ac.InstrumentationGroupCollection.Add(igMyApplication, true);
            #endregion

            #region ASP.NET and MVC
            InstrumentationGroup igASPNET = new InstrumentationGroup()
            {
                GroupName = "ASP.NET",
                InstrumentationPoints = new List<InstrumentationPoint>()

            };

            var pipelineClass = new Models.Definitions.Class()
            {
                ClassName = "System.Web.Hosting.PipelineRuntime"
            };


            igASPNET.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "ProcessRequestNotification",
                    Class = pipelineClass,
                    IsEntryPoint = true
                }
            });

            igASPNET.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "ProcessRequestNotificationHelper",
                    Class = pipelineClass
                }
            });

            var mvcAsyncHandlerClass = new Models.Definitions.Class()
            {
                ClassName = "System.Web.Mvc.MvcHandler.System.Web.IHttpAsyncHandler"

            };
            igASPNET.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "EndProcessRequest",
                    Class = mvcAsyncHandlerClass
                }
            });


            //System.Web.Mvc.MvcHandler.EndProcessRequest (System.IAsyncResult)
            var mvcHandlerClass = new Models.Definitions.Class()
            {
                ClassName = "System.Web.Mvc.MvcHandler"

            };
            igASPNET.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "EndProcessRequest",
                    Class = mvcHandlerClass
                }
            });

            //System.Web.Mvc.ActionMethodDispatcher.Execute
            var mvcActionDispatcherClass = new Models.Definitions.Class()
            {
                ClassName = "System.Web.Mvc.ActionMethodDispatcher"

            };
            igASPNET.InstrumentationPoints.Add(new InstrumentationPoint()
            {
                InstrumentationMethod = new Models.Definitions.Method()
                {
                    MethodName = "Execute",
                    Class = pipelineClass
                }
            });


            ac.InstrumentationGroupCollection.Add(igASPNET, true);
            #endregion

            foreach (var InstrumentationGrp in ac.InstrumentationGroupCollection)
            {
                if (InstrumentationGrp.Value)
                {
                    foreach (var InstrumentationPt in InstrumentationGrp.Key.InstrumentationPoints)
                    {
                        ac.InstrumentationPointCollection.Add(InstrumentationPt.InstrumentationMethod, InstrumentationPt);
                    }
                }

            }

            ac.ParentAgentGroup = ReadAgentGroupConfig(ac);

            return ac;
        }

        public AgentGroupConfig ReadAgentGroupConfig(AgentConfig AgentConfig)
        {
            var ac = new AgentGroupConfig()
            {
                InstrumentationGroupCollection = new Dictionary<InstrumentationGroup, bool>(),
                InstrumentationPointCollection = new Dictionary<Models.Definitions.Method, InstrumentationPoint>(),
                ConfigName = "DefaultAgentGroupConfig"

            };
            ac.ParentApplication = ReadApplicationConfig(ac);
            return ac;
        }

        public ApplicationConfig ReadApplicationConfig(AgentGroupConfig AgentGroupConfig)
        {
            var ac = new ApplicationConfig()
            {
                InstrumentationGroupCollection = new Dictionary<InstrumentationGroup, bool>(),
                InstrumentationPointCollection = new Dictionary<Models.Definitions.Method, InstrumentationPoint>(),
                ConfigName = "DefaultApplicationConfig"

            };
            ac.ParentApplicationGroup = ReadApplicationGroupConfig(ac);
            return ac;
        }

        public ApplicationGroupConfig ReadApplicationGroupConfig(ApplicationConfig ApplicationConfig)
        {
            var ac = new ApplicationGroupConfig()
            {
                InstrumentationGroupCollection = new Dictionary<InstrumentationGroup, bool>(),
                InstrumentationPointCollection = new Dictionary<Models.Definitions.Method, InstrumentationPoint>(),
                ConfigName = "DefaultApplicationGroupConfig"
            };
            return ac;
        }


        public GlobalConfig ReadAggregatorConfig(AgentConfig AgentGroupConfig)
        {
            throw new NotImplementedException();
        }
        public GlobalConfig ReadGlobalConfig()
        {
            throw new NotImplementedException();
        }

        public MachineConfig ReadMachineConfig(Machine Machine)
        {
            throw new NotImplementedException();
        }

        public ResourceConfig ReadResourceConfig(Resource Resource)
        {
            throw new NotImplementedException();
        }

        public ResourceGroupConfig ReadResourceGroupConfig(ResourceConfig ResourceConfig)
        {
            throw new NotImplementedException();
        }

        public GlobalConfig ReadServerConfig(AgentConfig AgentGroupConfig)
        {
            throw new NotImplementedException();
        }
    }
}
