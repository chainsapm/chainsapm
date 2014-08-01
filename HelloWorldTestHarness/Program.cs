using System;
using System.Collections.Generic;
using System.Text;

namespace HelloWorldTestHarness
{
    class Program
    {
        static void Main(string[] args)
        {
            System.Collections.SortedList sList = new System.Collections.SortedList(System.Environment.GetEnvironmentVariables());
            foreach (System.Collections.DictionaryEntry item in sList)
            {
                Console.WriteLine("{0}\t\t={1}", item.Key, item.Value);
            }
            Console.ReadLine();
        }
    }
}
