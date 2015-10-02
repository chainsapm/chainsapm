using System;
using System.Collections.Generic;
using System.Linq;
using System.Security;
using System.Text;
using System.Threading.Tasks;

namespace injectedmethods._System.Web.Hosting {

        [CustomAttribute.AssemblyTypeInformation ("System.Web.dll", "System.Web.Hosting.PipelineRuntime")]
        class PipelineRuntime {
                [SecurityCritical]
                internal static extern long GetSystemTimeAsFileTime ();

                [System.Runtime.InteropServices.DllImport ("clrprofiler.dll")]
                extern static public void NtvEnterHttp (System.UInt64 mdToken, System.UInt64 methodDef,
                        string method, int responseCode, int subCode, string request, string queryString,
                        string body, string [] headers, string [] cookes, string [] session);

                [System.Runtime.InteropServices.DllImport ("clrprofiler.dll")]
                extern static public void NtvExitSimple (System.UInt64 mdToken, System.UInt64 methodDef);

                [CustomAttribute.LocalVariableFixup (0, 0, "System.Web.Hosting.IIS7WorkerRequest"), CustomAttribute.LocalVariableFixup (1, 1, "System.Web.HttpContext")]
                public void ProcessRequestNotificationHelper_Enter_Inject () {
                        System.Web.HttpWorkerRequest workerRequest = null;
                        System.Web.HttpContext context = null;
                        ProcessRequestNotificationHelper_Enter (workerRequest, context);
                }
                public void ProcessRequestNotificationHelper_Exit_Inject () {
                        ProcessRequestNotificationHelper_Exit ();
                }
                [CustomAttribute.LocalVariableFixup (0, 0, "System.Web.Hosting.IIS7WorkerRequest"), CustomAttribute.LocalVariableFixup (1, 1, "System.Web.HttpContext")]
                public void ProcessRequestNotificationHelper_Info_Inject () {
                        System.Web.HttpWorkerRequest workerRequest = null;
                        System.Web.HttpContext context = null;
                        ProcessRequestNotificationHelper_Info (workerRequest, context);
                }
                public void ProcessRequestNotificationHelper_Info (System.Web.HttpWorkerRequest workerRequest, System.Web.HttpContext context) {
                        if ( context != null ) {
                                IList<string> headers = new List<string> ();
                                IList<string> cookies = new List<string> ();
                                IList<string> session = new List<string> ();
                                IList<int> session2 = new List<int> ();
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
                [CustomAttribute.InsertOffset(1)]
                public void ProcessRequestNotificationHelper_Enter (System.Web.HttpWorkerRequest workerRequest, System.Web.HttpContext context) {
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
                public void ProcessRequestNotificationHelper_Exit () {
                        NtvExitSimple (0xDEADBEEFDEADBEEF, 0x00C0FFEE00C0FFEE);
                }

                public void DateTimeDummy () {
                        ulong dtutc = (ulong)DateTime.Now.ToFileTimeUtc ();
                }

                public string IfteST1 () {
                        int i = new Random ().Next (0, 10);
                        string s = "test";
                        i += 2;
                        int j = new Random ().Next (0, 10);
                        j += i;
                        s += j.ToString ();
                        if ( j < 2 ) {
                                s = "2";
                        } else {
                                s = (j + i).ToString ();
                        }
                        string s2 = s + "out_";
                        return s2;
                }
                public string IfTest2 () {
                        int i = new Random ().Next (0, 10);
                        string s = "test";
                        i += 2;
                        int j = new Random ().Next (0, 10);
                        j += i;
                        s += j.ToString ();
                        if ( j == 2 ) {
                                s = "2";
                        } else if ( j == 3 ) {
                                s = "3";
                        } else if ( j == 4 ) {
                                s = "3";
                        } else {
                                s = (j + i).ToString ();
                        }
                                string s2 = s + "out_";
                                return s2;
                        }
                }
        }
