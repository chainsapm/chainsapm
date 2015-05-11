using System;
using System.Collections.Generic;
using System.Text;

namespace HelloWorldTestHarness
{
    
    class Program
    {
        static AsyncCallback syncCB = new AsyncCallback(asyncResult);
        static void asyncResult(IAsyncResult result)
        {
            System.Net.HttpWebRequest wr = (System.Net.HttpWebRequest)result.AsyncState;
            wr.EndGetResponse(result);
            var s = result;
        }
        static void Main(string[] args)
        {
            System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();
            var env = Environment.GetEnvironmentVariable("COR_ENABLE_PROFILING");
            sw.Start();
            System.IO.TextWriter tw = new System.IO.StreamWriter(@"C:\Logfiles\logfile.txt", true);
            var StopTime = DateTime.Now.AddSeconds(5);
            System.Threading.Thread.CurrentThread.Name = "Main";
            int loops = 0;
            var t = System.Threading.Tasks.Task.Run(() =>
            {
                int i = 0x43434343;
                int b = 0x48484848;
                System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
                System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
                System.Console.WriteLine(AddNumbers(ref i, ref b));
                System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
                System.Console.WriteLine(AddNumbers(ref i, ref b));
                System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
                System.Console.WriteLine(AddNumbers(ref i, ref b));
                System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
                System.Console.WriteLine(AddNumbers(ref i, ref b));
            });
            System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //while (loops < 100)
            //{
            //    ++loops;
            //    //Console.ReadLine();
            //    System.Console.WriteLine("Hello World!");
            //    //System.Collections.SortedList sList = new System.Collections.SortedList(System.Environment.GetEnvironmentVariables());
            //    //foreach (System.Collections.DictionaryEntry item in sList)
            //    //{
            //    //    Console.WriteLine("{0}\t\t={1}", item.Key, item.Value);
            //    //}
            //    int i = 0x43434343;
            //    int b = 0x48484848;

            //    System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //    GC.Collect(0);
            //    System.Console.WriteLine(AddNumbers(ref i, ref b));
            //    GC.Collect(1);
            //    System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //    GC.Collect(2);
            //    System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //    System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //    System.Console.WriteLine(AddNumbers(ref i, ref b));
            //    System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //    System.Console.WriteLine(AddNumbers(ref i, ref b));
            //    System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //    System.Console.WriteLine(AddNumbers(ref i, ref b));
            //    System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //    System.Console.WriteLine(AddNumbers(ref i, ref b));
            //    System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //    System.Console.WriteLine(AddNumbers(ref i, ref b));
            //    System.Console.WriteLine(AddNumbers(ref i, ref b));
            //    System.Console.WriteLine(AddNumbers(ref i, ref b));
            //    for (int f = 0; f < 8; f++)
            //    {

            //        var t = new System.Threading.Thread(new System.Threading.ThreadStart(
            //       () =>
            //       {
            //           System.Threading.Thread.CurrentThread.Name = string.Format("Worker Thread {0}", f);
            //           System.Console.WriteLine("Hello world from another thread!");
            //           System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //           System.Console.WriteLine(AddNumbers(ref i, ref b));
            //           System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //           System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //           System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //           System.Console.WriteLine(AddNumbers(ref i, ref b));
            //           System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //           System.Console.WriteLine(AddNumbers(ref i, ref b));
            //           System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //           System.Console.WriteLine(AddNumbers(ref i, ref b));
            //           System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //           System.Console.WriteLine(AddNumbers(ref i, ref b));
            //           System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            //           System.Console.WriteLine(AddNumbers(ref i, ref b));
            //           System.Console.WriteLine(AddNumbers(ref i, ref b));
            //           System.Console.WriteLine(AddNumbers(ref i, ref b));
            //       }));
            //        t.Start();
            //    }
            //}
            // var wr = System.Net.HttpWebRequest.Create("http://www.google.com");
            // wr.BeginGetResponse(syncCB, wr);
            //Console.ReadLine();
            sw.Stop();
            tw.WriteLine("HelloWorld.exe ran {1} loops in {0} ms with profiling {2}", sw.ElapsedMilliseconds.ToString(), loops, env == null ? "OFF": "ON");
            tw.Flush();
            tw.Dispose();
            Console.ReadLine();
        }
        static int AddNumbers(int i, int b)
        {
            System.Threading.Thread.Sleep(2);
            return i + b;
        }

        static int AddNumbers(ref int i, ref int b)
        {
            System.Threading.Thread.Sleep(2);
            return i + b;
        }
    }
}
