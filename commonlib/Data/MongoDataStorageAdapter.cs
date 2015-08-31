using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ChainsAPM.Interfaces;
using MongoDB.Driver;

namespace ChainsAPM.Data {
        class MongoDataStorageAdapter : IDocumentDBDataAdapter {
                public MongoDataStorageAdapter() {

                }

                public bool InsertEntryPoint (ChainsAPM.Models.EntryPoint entry) {
                        var client = new MongoClient ("mongodb://localhost:27017");
                        var db = client.GetDatabase ("chainsapm");
                        var collection = db.GetCollection<ChainsAPM.Models.EntryPoint> ("entrypoints");
                        collection.InsertOneAsync (entry);

                        return true;
                }
                public bool UpdateEntryPoint (ChainsAPM.Models.EntryPoint entry) {
                        var client = new MongoClient ("mongodb://localhost:27017");
                        var db = client.GetDatabase ("chainsapm");
                        var collection = db.GetCollection<ChainsAPM.Models.EntryPoint> ("entrypoints");
                        var filter = Builders<ChainsAPM.Models.EntryPoint>.Filter.Where (x => x._id == entry._id);
                        collection.ReplaceOneAsync (filter, entry);

                        return true;
                }
        }
}
