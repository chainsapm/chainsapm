namespace injectedmethods {

        [CustomAttribute.TypeToReplace ("System.Web.dll", "System.Web.Hosting.PipelineRuntime", "ProcessRequestNotificationHelper")]
        public class WebRequests {
                [System.Runtime.InteropServices.DllImport ("clrprofiler.dll")]
                extern static public void NtvEnterHttp(System.UInt64 mdToken, System.UInt64 methodDef, string request);

                public void RequestEnterActual (System.Web.HttpWorkerRequest workerRequest, System.Web.HttpContext context, string [] requestheaders, string [] requestservervars) {
                        var wr = new System.Web.Hosting.SimpleWorkerRequest ("", "", new System.IO.StringWriter ());
                        RequestEnter (wr , new System.Web.HttpContext (wr), new string [] { "" }, new string [] { "" });
                }

                public static void RequestEnter(System.Web.HttpWorkerRequest workerRequest, System.Web.HttpContext context, string[] requestheaders, string[] requestservervars) {
                        foreach ( var item in context.Request.Headers ) {
                                
                        }
                        NtvEnterHttp (216541651651L, 165460020554L, context.Request.Url.ToString());
                }

                public static void RequestExit(System.Web.HttpWorkerRequest workerRequest, System.Web.HttpContext context, string[] responseheaders, string[] responseservervars) {

                        
                        // Gather response headers, response codes/errors
                }
        }
}
