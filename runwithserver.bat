start "Chains APM Server" /i chainsapmsvr.exe
regsvr32 clrprofiler.dll
COR_ENABLE_PROFILING=0x1
start "HelloWorldTest" HelloWorldTestHarness.exe