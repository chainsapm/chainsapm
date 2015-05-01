clrprofiler
===========
This is the core class for the .NET profiling API.  This folder contains all of the implementation details for the ICorProfilerCallback interfaces. Below is a quick list of features implemented and those set for the future.

## Current Features

- .NET Function Calls (Enter/Leave/Tail)
- .NET Events
  - Garbage Collection
  - Suspensions
  - Thread Creation
  - Remoting
  - Managed to Unmanaged Transitions
- Network Client
  - Send information to network aggregator or server
  - Recieve updates from the core components
- Performance Monitoring
  - Monitor performance counters on the local machine
- Agent Information
  - Process Information
  - Machine Information

## Future

- IL Re-Writing
