// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef STRICT
#define STRICT
#endif

#include "targetver.h"

#define _ATL_APARTMENT_THREADED

#define _ATL_NO_AUTOMATIC_NAMESPACE

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit


#define ATL_NO_ASSERT_ON_DESTROY_NONEXISTENT_WINDOW

#include "resource.h"
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>
// Cor Profiling Libraries
#include <cor.h>
#include <corprof.h>
// STL Libraries
#include <map>
#include <stack>
#include <string>
#include <vector>
#include <queue>
#include <exception>
#include <stdexcept>
#include <unordered_set>
#include <iostream>
#include <fstream> 
#include <locale>
#include <memory>
#include <allocators>

// Boost Libraries
#include <boost/date_time.hpp>
#include <boost/format.hpp>
#pragma comment(lib, "corguids.lib")
#pragma comment(lib, "Ws2_32.lib")


//#include <boost/thread.hpp>

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __TODO__ __FILE__ "("__STR1__(__LINE__)") : message TODO: "
#define TODO(msg) __pragma(message(__TODO__""msg))
