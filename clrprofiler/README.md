clrprofiler
===========
---
This is the core class for the .NET profiling API.  This folder contains all of the implementation details for the ICorProfilerCallback interfaces.

This class is responsible for the following
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
