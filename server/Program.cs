using System;
using CommandLine;
using CommandLine.Text;

namespace server {
        class Program {
                static ConsoleServer.ConsoleServer s, sIPV6;
                static void Main (string [] args) {

                        var typ = typeof (ConsoleServer.ConsoleServer);
                        var subc = typ.IsSubclassOf (typeof (ChainsAPM.Server.Server));

                        var options = new server.CommandLineParser.CommandLineParse ();
                        var parser = new CommandLine.Parser ();
                        if ( parser.ParseArguments (args, options) ) {
                                if ( options.StartConsole ) {
                                        var ip = System.Net.IPAddress.Any;
                                        System.Net.IPAddress outip = null;
                                        var parsed = System.Net.IPAddress.TryParse (options.IPAddress, out outip);
                                        if (parsed && outip != System.Net.IPAddress.Loopback) {
                                                ip = outip;
                                        }
                                        int useport = options.Port;
                                        AppDomain.CurrentDomain.ProcessExit += ConsoleServerExit;
                                        StartServer (ip, useport);
                                        if ( options.UseIPV6 ) {
                                                var ipv6 = System.Net.IPAddress.IPv6Any;
                                                parsed = System.Net.IPAddress.TryParse (options.IPAddress, out outip);
                                                if ( parsed && outip != System.Net.IPAddress.IPv6Loopback ) {
                                                        ipv6 = outip;
                                                }
                                                StartServer (ipv6, useport);
                                        }
                                        char cmd = ' ';
                                        while ( cmd != 'x' | cmd != 'q' ) {
                                                cmd = (char)Console.Read ();
                                        }
                                } else if ( options.Install ) {


                                } else {
                                        Console.WriteLine (options.GetUsage ());
                                }
                        } else {
                                Console.WriteLine ("There was a problem using the commandline parser. Please contact support.");
                        }

                }

                private static void ConsoleServerExit (object sender, EventArgs e) {
                        s.Stop ();
                        sIPV6.Stop ();
                }

                private static void StartServer (System.Net.IPAddress ip, int port) {

                        s = new server.ConsoleServer.ConsoleServer (ip, port);
                        s.Start ();
                }


        }
}



