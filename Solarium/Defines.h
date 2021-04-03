#pragma once

#if _WIN32 || _WIN64
#define PLATFORM_WINDOWS
#else
#if __linux__
#define PLATFORM_LINUX
#elif __APPLE__
#define PLATFORM_MAC
#else
#error "Unable to determine platform."
#endif
#endif

#ifdef PLATFORM_WINDOWS
#define FORCEINLINE __forceinline
#define FORCENOINLINE _declspec(noinline)
#ifdef SOLARIUM_BUILD_LIB
#define SOLARIUM_API __declspec(dllexport)
#else
#define SOLARIUM_API __declspec(dllimport)
#endif

#elif PLATFORM_LINUX || PLATFORM_MAC
#define FORCEINLINE inline
//TODO: Define SOLARIUM_API for linux and mac

#endif

//Assertions
#define ASSERTIONS_ENABLED
#ifdef ASSERTIONS_ENABLED
#include <iostream>

#if _MSC_VER
#include <intrin.h>
#define debugBreak() __debugbreak()
#else
#define debugBreak() __asm { int 3 }
#endif

#define ASSERT(expr) \
{ \
	if ( expr ) \
	{ } \
	else \
	{ \
	reportAssertionFailure(#expr, "", __FILE__, __LINE__); \
	debugBreak(); \
	} \
}

#define ASSERT_MSG(expr, message) \
{ \
	if ( expr ) \
	{ } \
	else \
	{ \
	reportAssertionFailure(#expr, message, __FILE__, __LINE__); \
	debugBreak(); \
	} \
}

#ifdef _DEBUG
#define ASSERT_DEBUG(expr) \
{ \
	if ( expr ) \
	{ } \
	else \
	{ \
	reportAssertionFailure(#expr, "", __FILE__, __LINE__); \
	debugBreak(); \
	} \
}
#else
#define ASSERT_DEBUG(expr)
#endif

FORCEINLINE void reportAssertionFailure(const char* expression, const char* message, const char* file, int line)
{
	std::cerr << "Assertion failure" << expression << ", message: '" << message << "', in file: " << file << ", line: " << line << "\n";
}

#else
#define ASSERT(expr)
#define ASSERT_MSG(expr, message)
#define ASSERT_DEBUG(expr)

#endif

