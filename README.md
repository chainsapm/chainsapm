[chainsapm](http://chainsapm.github.io/chainsapm/)
======================

This project is an attempt to bring some opensourcness to the .NET profiling and APM world. Applications like AppNeta, AppDynamics, dynaTrace, Foglight and the like have changed the way we monitor applications. Most of these companies provide some level of free entry level monitoring. Some are in the cloud and some are on premesis.

My goal is to give any organization--large or small--the ability to monitor a number of interconnected systems and provide real time data and analysis on them.

[Scott Hackett](mailto:code@scotthackett.com) provided the idea for the use of ATL boiler plate code for the CLR profiler with his [article](http://www.codeproject.com/Articles/15410/Creating-a-Custom-NET-Profiler) at CodeProect. The code I am using references a lot of resources. If you browse the Wiki you will find the major ones.

Feel free to fork and ask to contribute. The project is a bit adventurous but it will be great fun and a good learning experience for all.

## What does it do?
Right now, not much :smile:. But what it will do is pretty limitless. Here is the outline of what I want in the way of features. Some have been implemented, most have not.

- Simple Web Based Interface
  - View System Info
  - View Process Info
- DocumentDB Backed Data Store
- ELT Method Hooking
  - Track Entry/Exit
  - Gather Arguments
- Event Monitoring
  - GC
  - JIT
  - Assembly Load
  - etc.
- System Information
  - Operating System
  - Hardware (CPU/Disk/Memory)
  - Network Configuration
  - Network Utilization
  - CPU Utilization
  - Disk Utilzation
  - Memory Utilization
- Process Information
  - Image Name
  - Command Line
  - Environment Variables
  - CPU Utilization (app specific)
  - Memory Utilization (app specific)
- Change Tracking / Auditing
  - Assemblies
  - Config Files
  - System Reboots
  - User Logins
  - Application Installations
- IL Rewriting

## How to build
In order to build this project you can clone the repository in it's current state. I will do my best to not have a broken commit at any time. I plan on adding a build script that will compile the entire solution without the need for the Visual Studio IDE.

***Right now only the x64 binaries will work as expected.*** I have not tested the x86 binaries extensively.

1. Clone repository
2. Open chainsapm.sln
1. *Make changes to [Cprofilermain()][profmain] to set the application name filter*
2. *Make changes to [AddCommonFunctions()][commonfunc] to include more functions*
3. Select Debug x64 from the build configurations
4. Select Build from the BUILD menu
5. Wait, since this has NuGet packages it will try to restore them

Steps 3 and 4 are optional and really only need to be completed if you want to test with another application.

## How to run
Once you have built the project you will need to add the environment variable listed above. With this variable set the application will start logging only a select few methods. ***In order to log more data you will need to manually edit the application and rebuild.*** The log files will attempt to be generated in C:\logfiles

>The only caveat here is that profiling will only take place from inside Visual Studio. In order to profile without using the IDE you need to set `COR_ENABLE_PROFILING=0x01`

The instructions below will run the default Debuging behavior with the HelloWorldTestHarness.exe. This is a simple .NET 2.0 console application used to generate data. It has no real meaning or usefulness outside of that.

1. Build application
2. Register the DLL version you need, x64 Debug example:
  1. Open a command prompt
  2. Navigate to the build directory <projecthome>\.output\Debug\x64
  3. Run regsvr32 clrprofiler.dll
2. Set the following environment variable
  - COR_PROFILER=**{41DB4CB9-F3A1-44B2-87DC-52BF4E8E8EB2}**
3. Create the *C:\logfiles\* directory
  - ~~The application **WILL FAIL** if you do not.~~
4. Press F5 or Select Build -> Debug
  - The solution is set to run the HelloWorldTestHarness.exe when using Debug or Release
  - The solution is set to run the Websites and Webservices when using DebugMVC

**NOTE** You can have both the 32bit and 64bit versions of the DLL registered at the same time. If you're not seeing expected results make sure you are rebuilding the proper bitness version.


## Visual Studio 2013
I am using Visual Studio 2013 and you should be able to open the solution in any retail version of Visual Studio. However, if you plan on using the Express editions you should consider rolling all of the .NET projects into one solution to build and leave the C++ project on it's own.

## MSVC 12.0
The platform target for the C++ project is MSVC 12.0. You will need to download and install the MSVC 12.0 Runtime. It can be downloaded from here: http://www.microsoft.com/en-us/download/details.aspx?id=40784

## .NET 4.5
All off the .NET projects (save for some of the testing projects) will be written in .NET 4.5. This version comes on most Windows 8 implementations. If you do not have it please download it from here: http://www.microsoft.com/en-us/download/details.aspx?id=30653

## Troubleshooting

### Random Crashes
This software is under heavy development at this time. It has been tested stable against a number of applications but there are always exceptions.

If you attempt to run this with an application and it fails send an issue request.

### All Applications Being Instrumented
Make sure you do not have COR_ENABLE_PROFILING set to **0x1** in the registry. If you do the profiler will attempt to profile ANY .NET application.

### I want to monitor another application
Make sure you do not unset any of the filters in the  [Cprofilermain()][profmain] method.

### I want to change what is instrumented
Make changes to [AddCommonFunctions()][commonfunc] to include more functions.

The code uses a substring match so if the string matches any part of a method it will be instrumented.

```cpp
this->m_Container->g_FunctionNameSet->insert(TEXT("Main"));
```

Or you can instrument an entire class
```cpp
this->m_Container->g_ClassNameSet->insert(TEXT("System.Threading.ThreadStart"));
```

### My 32bit application isn't monitored
Build the 32bit version of the DLL and make sure you run regsvr32 clrprofiler.dll on the newly built version.
- <projecthome>\.output\Debug\x64
- <projecthome>\.output\Debug\Win32

### I want to run outside of Visual Studio
You have two options here. One, [set the environment][envvar] varible for the user or the system from your Advanced System Properties. Or you can set it temporarily in a command prompt and run your application. If you are monitoring a service you will need to set it in your system wide environment variables; or if you're running as another user, you can set the variables there. However, I would test that out completely.

```
Microsoft Windows [Version 6.1.7601]
Copyright (c) 2009 Microsoft Corporation.  All rights reserved.

C:\>SET COR_ENABLE_PROFILING=0x01

C:\>set COR
COR_ENABLE_PROFILING=0x01
COR_PROFILER={41DB4CB9-F3A1-44B2-87DC-52BF4E8E8EB2}

C:\>
```

[profmain]: https://github.com/chainsapm/chainsapm/blob/01651432d16648da3577eb25c099093fd5b8d642/clrprofiler/profilermain.cpp#L410
[commonfunc]: https://github.com/chainsapm/chainsapm/blob/01651432d16648da3577eb25c099093fd5b8d642/clrprofiler/profilermain.cpp#L466
[envvar]:
