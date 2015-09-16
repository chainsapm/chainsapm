using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace injectedmethods._System.Web.Hosting {
        [CustomAttribute.TypeToReplace ("System.Web.dll", "System.Web.Hosting.PipelineRuntime", "ProcessRequestNotificationHelper")]
        class PipelineRuntime {
                [System.Runtime.InteropServices.DllImport ("clrprofiler.dll")]
                extern static public void NtvEnterHttp (System.UInt64 mdToken, System.UInt64 methodDef,
                        string method, int responseCode, int subCode, string request, string queryString,
                        string body, string [] headers, string [] cookes, string [] session);

                public void RequestEnterActual (System.Web.HttpWorkerRequest workerRequest, System.Web.HttpContext context) {
                        RequestEnter (workerRequest, context);
                }

                public static void RequestEnter (System.Web.HttpWorkerRequest workerRequest, System.Web.HttpContext context) {
                        if ( context != null ) {
                                List<string> headers = new List<string> ();
                                List<string> cookies = new List<string> ();
                                List<string> session = new List<string> ();
                                List<int> session2 = new List<int> ();
                                string httpMethod = string.Empty;
                                string httpUrl = string.Empty;
                                string httpQueryString = string.Empty;
                                string body = string.Empty;
                                byte [] buffer = null;
                                int statusCode = 0;
                                int substatusCode = 0;
                                if ( context.Response != null ) {
                                        statusCode = context.Response.StatusCode;
                                        substatusCode = context.Response.SubStatusCode;
                                }
                                if ( context.Request != null ) {
                                        if ( context.Request.Headers != null ) {
                                                foreach ( var item in context.Request.Headers ) {
                                                        headers.Add (item.ToString ());
                                                }
                                        }
                                        if ( context.Request.Cookies != null ) {
                                                foreach ( var item in context.Request.Cookies ) {
                                                        cookies.Add (item.ToString ());
                                                }
                                        }
                                        if ( context.Request.InputStream != null ) {
                                                if ( context.Request.InputStream.Length > 0 ) {
                                                        buffer = new byte [context.Request.InputStream.Length];
                                                        context.Request.InputStream.Read (buffer, 0, (int)context.Request.InputStream.Length);
                                                        body = System.Text.UnicodeEncoding.Unicode.GetString (buffer);
                                                }
                                        }
                                        httpMethod = context.Request.HttpMethod;
                                        httpUrl = context.Request.Url.ToString ();
                                        httpQueryString = context.Request.QueryString.ToString ();
                                }
                                foreach ( var item in context.Session ) {
                                        session.Add (item.ToString ());
                                }
                                session2.Add (1);
                                NtvEnterHttp (0xDEADBEEFDEADBEEF, 0x00C0FFEE00C0FFEE, httpMethod, statusCode, substatusCode, httpUrl,
                                       httpQueryString, body, headers.ToArray (), cookies.ToArray (), session.ToArray ());
                        }
                }

                public static void RequestExit (System.Web.HttpWorkerRequest workerRequest, System.Web.HttpContext context, string [] responseheaders, string [] responseservervars) {
                        string s = "s2";
                        string s2 = "s3";
                        string s3 = "s";
                }
        }
}
