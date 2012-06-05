#pragma once

// Debug library
#define STRINGIFY(e) #e
#define ASSERT(expr) \
	do { \
		if (!(expr)) { \
			OutputDebugStringA(__FILE__ "(" STRINGIFY(__LINE__) "): Assertion failed: " #expr); \
			__debugbreak(); \
			std::exit(1); \
		} \
	} while (false)