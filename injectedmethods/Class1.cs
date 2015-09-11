using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Web;
using System.Web.Hosting;

namespace injectedmethods {

        public class WebRequests {
                [DllImport("clrprofiler.dll")]
                extern static public void NtvEnterHttp(UInt64 mdToken, UInt64 methodDef, string request);
                public static void RequestEnter(HttpWorkerRequest workerRequest, HttpContext context, string[] requestheaders, string[] requestservervars) {
                        NtvEnterHttp(216541651651L, 165460020554L, context.Request.Url.ToString());
                }

                public static void RequestExit(HttpWorkerRequest workerRequest, HttpContext context, string[] responseheaders, string[] responseservervars) {
                        // Gather response headers, response codes/errors
                }
        }
}
