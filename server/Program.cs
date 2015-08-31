using System;
using CommandLine;
using CommandLine.Text;

namespace server {
        class Program {
                static ConsoleServer.ConsoleServer s, sIPV6;
                static void Main (string [] args) {

                        
                        var options = new server.CommandLineParser.CommandLineParse ();
                        var parser = new CommandLine.Parser ();
                        if ( parser.ParseArguments (args, options) ) {
                                if ( options.StartConsole ) {
                                        AppDomain.CurrentDomain.ProcessExit += ConsoleServerExit;
                                        StartDefaultServer (out s, out sIPV6);
                                } else if ( options.Install ) {
                                        AppDomain.CurrentDomain.ProcessExit += ConsoleServerExit;
                                        StartDefaultServer (out s, out sIPV6);

                                } else {
                                        Console.WriteLine(options.GetUsage ());
                                }
                        } else {
                                Console.WriteLine ("There was a problem using the commandline parser. Please contact support.");
                        }

                }

                private static void ConsoleServerExit (object sender, EventArgs e) {
                        s.Stop ();
                        sIPV6.Stop ();
                }

                private static void StartDefaultServer (out ConsoleServer.ConsoleServer s, out ConsoleServer.ConsoleServer sIPV6) {
                       
                        s = new server.ConsoleServer.ConsoleServer (System.Net.IPAddress.Any, 8080);
                        s.Start ();
                        sIPV6 = new server.ConsoleServer.ConsoleServer(System.Net.IPAddress.IPv6Any, 8080);
                        sIPV6.Start ();
                        char cmd = ' ';
                        while ( cmd != 'x' | cmd != 'q' ) {
                                cmd = (char)Console.Read ();
                        }
                }

        }
}



