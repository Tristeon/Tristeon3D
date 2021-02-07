#pragma once

//Compiler handling for all known compilers
//Not all compilers listed are supported
#if defined(__MINGW32__) && !defined(__MINGW64__)
	#define TRISTEON_COMPILER_MINGW32
	#define TRISTEON_COMPILER_MINGW
#elif defined(__MINGW64__)
	#define TRISTEON_COMPILER_MINGW64
	#define TRISTEON_COMPILER_MINGW
#elif defined(__EMSCRIPTEN__)
	#define TRISTEON_COMPILER_EMSCRIPTEN
#elif defined(__clang__)
	#define TRISTEON_COMPILER_CLANG
#elif defined(__GNUC__)
	#define TRISTEON_COMPILER_GCC
#elif defined(_MSC_VER)
	#define TRISTEON_COMPILER_MSVC
#endif