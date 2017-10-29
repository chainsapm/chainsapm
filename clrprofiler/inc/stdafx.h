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

#define WIN32_LEAN_AND_MEAN

#include "resource.h"
// Debug ASSERT
#pragma warning(push, 0)
#include <assert.h>
// ATL LIbraries
#include <atlbase.h>
#include <atlcom.h>
#include <atlctl.h>

// Cor Profiling Libraries
#include <cor.h>
#include <corprof.h>

// STL Libraries
#include <map>
#include <unordered_map>
#include <stack>
#include <array>
#include <string>
#include <sstream>
#include <vector>
#include <queue>
#include <stdexcept>
#include <unordered_set>
#include <locale>
#include <memory>
#include <allocators>



// Statically linked libs
#pragma comment(lib, "corguids.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "metadatastaticlib.lib")
#pragma warning(pop)

//#include <boost/thread.hpp>

#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __TODO__ __FILE__ "("__STR1__(__LINE__)") : message TODO: "
#define TODO(msg) __pragma(message(__TODO__""msg))
