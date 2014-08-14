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
            Console.ReadLine();
            System.Threading.Thread.CurrentThread.Name = "Main";
            System.Console.WriteLine("Hello World!");
            //System.Collections.SortedList sList = new System.Collections.SortedList(System.Environment.GetEnvironmentVariables());
            //foreach (System.Collections.DictionaryEntry item in sList)
            //{
            //    Console.WriteLine("{0}\t\t={1}", item.Key, item.Value);
            //}
            int i = 0x43434343;
            int b = 0x48484848;

            System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            GC.Collect(0);
            System.Console.WriteLine(AddNumbers(ref i, ref b));
            GC.Collect(1);
            System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            GC.Collect(2);
            System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            System.Console.WriteLine(AddNumbers(ref i, ref b));
            System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            System.Console.WriteLine(AddNumbers(ref i, ref b));
            System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            System.Console.WriteLine(AddNumbers(ref i, ref b));
            System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            System.Console.WriteLine(AddNumbers(ref i, ref b));
            System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
            System.Console.WriteLine(AddNumbers(ref i, ref b));
            System.Console.WriteLine(AddNumbers(ref i, ref b));
            System.Console.WriteLine(AddNumbers(ref i, ref b));
            for (int f = 0; f < 1; f++)
            {

                var t = new System.Threading.Thread(new System.Threading.ThreadStart(
               () =>
               {
                   System.Threading.Thread.CurrentThread.Name = string.Format("Worker Thread {0}", f);
                   System.Console.WriteLine("Hello world from another thread!");
                   System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
                   System.Console.WriteLine(AddNumbers(ref i, ref b));
                   System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
                   System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
                   System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
                   System.Console.WriteLine(AddNumbers(ref i, ref b));
                   System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
                   System.Console.WriteLine(AddNumbers(ref i, ref b));
                   System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
                   System.Console.WriteLine(AddNumbers(ref i, ref b));
                   System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
                   System.Console.WriteLine(AddNumbers(ref i, ref b));
                   System.Console.WriteLine(AddNumbers(0x41414141, 0x42424242));
                   System.Console.WriteLine(AddNumbers(ref i, ref b));
                   System.Console.WriteLine(AddNumbers(ref i, ref b));
                   System.Console.WriteLine(AddNumbers(ref i, ref b));
               }));
                t.Start();
            }

            // var wr = System.Net.HttpWebRequest.Create("http://www.google.com");
            // wr.BeginGetResponse(syncCB, wr);
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
