#ifndef __MACROS__H__
#define __MACROS__H__


#ifdef __cplusplus
#include <SharedPtr.h>
#include <string>

#define MAKE_SHARED_PTR(p) typedef SharedPtr<p> p##Ptr

#if _DEBUG_COMMANDS
#define DEBUG_COMMAND(_x_) 
#else
#define DEBUG_COMMAND 
#endif

#endif


#ifdef __GNUC__
#define DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED(func) func
#endif


#endif

