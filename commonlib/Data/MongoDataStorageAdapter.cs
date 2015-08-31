using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ChainsAPM.Interfaces;
using ChainsAPM.Interfaces.Data;
using MongoDB.Driver;

namespace ChainsAPM.Data {
        class MongoDataStorageAdapter : IDataAdapter  {
                private IMongoClient Client;
                private IMongoDatabase Database;
                public MongoDataStorageAdapter() {
                        Client = new MongoClient ("mongodb://localhost:27017");
                        Database = Client.GetDatabase ("chainsapm");
                }

                public bool InsertEntryPoint(IStackItem entry) { 
                        var MongoEntry = entry as ChainsAPM.Models.EntryPoint;
                        var collection = Database.GetCollection<ChainsAPM.Models.EntryPoint> ("entrypoints");
                        collection.InsertOneAsync (MongoEntry);

                        return true;
                }
                public bool UpdateEntryPoint(IStackItem entry)  {
                        var MongoEntry = entry as ChainsAPM.Models.EntryPoint;
                        var collection = Database.GetCollection<ChainsAPM.Models.EntryPoint> ("entrypoints");
                        var filter = Builders<ChainsAPM.Models.EntryPoint>.Filter.Where (x => (x._id == MongoEntry._id));
                        collection.ReplaceOneAsync (filter, MongoEntry);

                        return true;
                }
        }
}
