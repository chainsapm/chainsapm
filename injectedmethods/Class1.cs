using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Web;
using System.Web.Hosting;

namespace injectedmethods
{
    public class WebRequests
    {
        public static void RequestEnter(HttpWorkerRequest workerRequest, HttpContext context, string[] requestheaders, string[] requestservervars)
        {
            // Capture user supplied headers, querystrings, URIs, etc., set any response headers here
            context.Response.AddHeader("ChainsAPM", "Test");
        }

        public static void RequestExit(HttpWorkerRequest workerRequest, HttpContext context, string[] responseheaders, string[] responseservervars)
        {
            // Gather response headers, response codes/errors
        }
    }
}
