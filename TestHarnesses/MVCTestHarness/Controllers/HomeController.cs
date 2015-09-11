using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Web;
using System.Web.Mvc;

namespace MVCTestHarness.Controllers {
        public class HomeController : Controller {
                public async Task<ActionResult> Index () {
                        if ( System.Diagnostics.Debugger.IsAttached ) {
                                System.Diagnostics.Debugger.Break ();
                        }
                        return View ();
                }

                public ActionResult About () {
                        ViewBag.Message = "Your application description page.";

                        return View ();
                }

                public ActionResult Contact () {
                        ViewBag.Message = "Your contact page.";

                        return View ();
                }
        }
}