using System;
using System.Collections.Generic;
using System.Text;

namespace HelloWorldTestHarness {

        class Program {
                static AsyncCallback syncCB = new AsyncCallback (asyncResult);
                static void asyncResult (IAsyncResult result) {
                        System.Net.HttpWebRequest wr = (System.Net.HttpWebRequest)result.AsyncState;
                        wr.EndGetResponse (result);
                       
                        var s = result;
                }

                static int Recursive (int counter, int max) {
                        Console.WriteLine ("Recursive {0}", counter);
                        var wr = System.Net.HttpWebRequest.Create ("http://www.google.com");
                        var resp = wr.GetResponse ();
                        resp.Close ();
                        if ( counter < max ) {

                                System.Threading.Thread.Sleep (2);
                                return Recursive (++counter, max);
                        }
                        return max;
                }

                static void Main (string [] args) {
                        System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch ();
                        var env = Environment.GetEnvironmentVariable ("COR_ENABLE_PROFILING");
                        sw.Start ();
                        System.IO.TextWriter tw = new System.IO.StreamWriter (string.Format (@"C:\Logfiles\logfile_{0}.txt", System.Diagnostics.Process.GetCurrentProcess ().Id), true);
                        var StopTime = DateTime.Now.AddSeconds (5);
                        System.Threading.Thread.CurrentThread.Name = "Main";
                        int loops = 0;

                        System.Console.WriteLine (AddNumbers (0x41414141, 0x42424242));
                        System.Console.WriteLine (AddNumbers (0x41414141, 0x42424242));

                        Recursive (0, 10);
                        Console.WriteLine ("Just before last call.");
                        var wr = System.Net.HttpWebRequest.Create ("http://www.google.com");
                        var resp = wr.GetResponse ();
                        resp.Close ();
                        Console.WriteLine ("After last call.");
                        Console.ReadLine ();
                        sw.Stop ();
                        tw.WriteLine ("HelloWorld.exe ran {1} loops in {0} ms with profiling {2}", sw.ElapsedMilliseconds.ToString (), loops, env == null ? "OFF" : "ON");
                        tw.Flush ();
                        tw.Dispose ();
                        Console.ReadLine ();
                }
                static int AddNumbers (int i, int b) {
                        var wr = System.Net.HttpWebRequest.Create ("http://www.google.com");
                        var resp = wr.GetResponse ();
                        resp.Close ();
                        return i + b;
                }

                static int AddNumbers (ref int i, ref int b) {
                        System.Threading.Thread.Sleep (2);
                        return i + b;
                }
        }
}
