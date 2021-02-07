#pragma once

//Preprocessor handling for all known platforms
//Not all platforms listed are supported
#if defined(__linux__) && !defined(__ANDROID__)
	#define TRISTEON_PLATFORM_LINUX
#elif defined(__ANDROID__)
	#define TRISTEON_PLATFORM_ANDROID
#elif defined(__APPLE__)
	#define TRISTEON_PLATFORM_APPLE
#elif defined(__ros__)
	#define TRISTEON_PLATFORM_ROS
#elif defined(__native_client__)
	#define TRISTEON_PLATFORM_NATIVE_CLIENT
#elif defined(__asmjs__)
	#define TRISTEON_PLATFORM_ASMJS
#elif defined(__Fuchsia__)
	#define TRISTEON_PLATFORM_FUCHSIA
#elif defined(_WIN32) && !defined(_WIN64)
	#define TRISTEON_PLATFORM_WINDOWS_X32
	#define TRISTEON_PLATFORM_WINDOWS
#elif defined(_WIN64)
	#define TRISTEON_PLATFORM_WINDOWS_X64
	#define TRISTEON_PLATFORM_WINDOWS
#endif