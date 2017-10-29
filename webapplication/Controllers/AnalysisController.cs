using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Web;
using System.Web.Mvc;
using MongoDB.Bson;
using MongoDB.Driver;

namespace webapplication.Controllers {
        public class AnalysisController : Controller {
                public async Task<ActionResult> Index() {
                        var Client = new MongoClient("mongodb://localhost:27017");
                        var Database = Client.GetDatabase("chainsapm");
                        var collection = Database.GetCollection<ChainsAPM.Models.EntryPoint>("entrypoints");
                        var srt = Builders<ChainsAPM.Models.EntryPoint>.Sort.Ascending("Started");
                        return View(await collection.Find(x => true).Sort(srt).ToListAsync());
                }

                public async Task<ActionResult> Details(string id) {
                        var Client = new MongoClient("mongodb://localhost:27017");
                        var Database = Client.GetDatabase("chainsapm");
                        var collection = Database.GetCollection<ChainsAPM.Models.EntryPoint>("entrypoints");
                        return View(await collection.Find(x => x._id == new ObjectId(id)).FirstOrDefaultAsync());
                }
                // GET: Analysis

        }
}