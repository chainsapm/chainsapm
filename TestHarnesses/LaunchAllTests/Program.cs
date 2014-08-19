using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LaunchAllTests
{
    class Program
    {
        static void Main(string[] args)
        {
            //COR_ENABLE_PROFILING=0x1
            //Environment.SetEnvironmentVariable("COR_ENABLE_PROFILING", "0x1");
            System.Diagnostics.Process p = new System.Diagnostics.Process();
            p.StartInfo.UseShellExecute = false;
            if (p.StartInfo.EnvironmentVariables.ContainsKey("COR_ENABLE_PROFILING"))
                p.StartInfo.EnvironmentVariables["COR_ENABLE_PROFILING"] = "0x1";
            else
                p.StartInfo.EnvironmentVariables.Add("COR_ENABLE_PROFILING", "0x1");
            p.StartInfo.FileName = ".\\HelloWorldTestHarness.exe";
            p.Start();
            Console.ReadLine();
            p.Start();
            Console.ReadLine();
            p.Start();
            Console.ReadLine();
            p.Start();
            Console.ReadLine();
            p.Start();
            Console.ReadLine();
        }
    }
}
