using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;
using System.Web.Services;

namespace WebApplication1
{
    /// <summary>
    /// Summary description for TestInputService
    /// </summary>
    [WebService(Namespace = "http://tempuri.org/")]
    [WebServiceBinding(ConformsTo = WsiProfiles.BasicProfile1_1)]
    [System.ComponentModel.ToolboxItem(false)]
    // To allow this Web Service to be called from script, using ASP.NET AJAX, uncomment the following line. 
    // [System.Web.Script.Services.ScriptService]
    public class TestInputService : System.Web.Services.WebService
    {

        [WebMethod]
        public string HelloWorld()
        {
                        if ( System.Diagnostics.Debugger.IsAttached ) {
                                System.Diagnostics.Debugger.Break ();
                        }
            return "Hello World";
        }
    }
}
