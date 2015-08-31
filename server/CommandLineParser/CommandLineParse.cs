using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CommandLine;
using CommandLine.Text;

namespace server.CommandLineParser {
        class CommandLineParse {
                [Option ('c', "console", Required = false, HelpText = "Start a standalone console version of the ChainsAPM server.")]
                public bool StartConsole { get; set; }
                [Option ('f', "config", Required = false, HelpText = "Specify confile file for server start.")]
                public string Configfile { get; set; }
                [Option ('i', "install", Required = false, HelpText = "Install instance of the ChainsAPM server as a Windows service.")]
                public bool Install { get; set; }
                [Option ('l', "log", Required = false, HelpText = "Location of log file. Default is current directory.")]
                public string LogFile{ get; set; }
                [Option ('a', "ipaddress", Required = false, HelpText = "IP address to bind to.")]
                public string IPAddress { get; set; }
                [Option ('p', "port", Required = false, HelpText = "Port to listen on.")]
                public int Port { get; set; }
                [Option ('6', "ipv6", Required = false, HelpText = "Allow usage of IPv6.")]
                public bool UseIPV6 { get; set; }
                [HelpOption]
                public string GetUsage () {
                        var help = new HelpText
                        {
                                Heading = new HeadingInfo ("ChainsAPM", "0.0.1"),
                                Copyright = new CopyrightInfo ("James Davis (@debugthings)", 2015),
                                AdditionalNewLineAfterOption = false,
                                AddDashesToOption = true
                        };
                        help.AddPreOptionsLine ("");
                        help.AddPreOptionsLine ("Usage: chainsapmsrv --console");
                        help.AddOptions (this);
                        return help;
                }

        }
}
