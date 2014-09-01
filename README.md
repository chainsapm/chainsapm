chainsapm
======================

This project is an attempt to bring some opensourcness to the .NET profiling and APM world. 

[Scott Hackett](mailto:code@scotthackett.com) provided the idea for the use of ATL boiler plate code for the CLR profiler with his [article](http://www.codeproject.com/Articles/15410/Creating-a-Custom-NET-Profiler) at CodeProect. The code I am using references a lot of resources. If you browse the Wiki you will find the major ones.

Feel free to fork and ask to contribute. The project is a bit adventurous but it will be great fun and a good learning experience for all.

##How to build
In order to build this project you can clone the repository in it's current state. I will do my best to not have a broken commit at any time. I plan on adding a build script that will compile the entire solution without the need for the Visual Studio IDE.

***Right now only the x64 binaries will work as expected.*** I have not compiled and tested the x86 binaries, but I know as of now they are missing the ELT hook directives that point to the x86 versions of code.

1. Clone repository
2. Download boost 1.55.0
3. Extract to C:\boost_1_55_0
2. Open opensource-clrprofiler.sln
1. *Make changes to [Cprofilermain()](https://github.com/jldgit/opensource-clrprofiler/blob/master/clrprofiler/profilermain.cpp#L124) to set the applcation name filter*
2. *Make changes to [AddCommonFunctions()](https://github.com/jldgit/opensource-clrprofiler/blob/master/clrprofiler/profilermain.cpp#L180) to include more functions*
3. Select Debug x64 from the build configurations
4. Select Build from the BUILD menu

Steps 5 and 6 are optional and really only need to be completed if you want to test with another application.

##How to run
Once you have built the project you will need to add two environment variables. With these two varaibles set the application will start logging only a select few methods. ***In order to log more data you will need to manually edit the application and rebuild.*** The log files will attempt to be generated in C:\logfiles

The instructions below will run the default Debuging behavior with the HelloWorldTestHarness.exe. This is a simple .NET 2.0 console application used to generate data. It has no real meaning or usefulness outside of that.

1. Build application
2. Set the following environment variables
  - COR_ENABLE_PROFILING=**0x0**
  - COR_PROFILER=**{41DB4CB9-F3A1-44B2-87DC-52BF4E8E8EB2}**
3. Create the *C:\logfiles\* directory
4. Press F5 or Select Build -> Debug


##Visual Studio 2013
I am using Visual Studio 2013 and you should be able to open the solution in any retail version of Visual Studio. However, if you plan on using the Express editions you should consider rolling all of the .NET projects into one solution to build and leave the C++ project on it's own.

##MSVC 12.0
The platform target for the C++ project is MSVC 12.0. You will need to download and install the MSVC 12.0 Runtime. It can be downloaded from here: http://www.microsoft.com/en-us/download/details.aspx?id=40784

##Boost 1.55 .0
includes are required for this build. So far, I have not had to use any of the compiled libraries. This project will look for them in the **C:\boost_1_55_0** directory. You can download boost from here: http://www.boost.org/users/history/ and look for version 1.55.0

##.NET 4.5
All off the .NET projects (save for some of the testing projects) will be written in .NET 4.5. This version comes on most Windows 8 implementations. If you do not have it please download it from here: http://www.microsoft.com/en-us/download/details.aspx?id=30653

