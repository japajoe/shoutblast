#ifndef SHOUTBLAST_PLATFORM_HPP
#define SHOUTBLAST_PLATFORM_HPP

#ifndef SB_PLATFORM_HPP
#define SB_PLATFORM_HPP

#if defined(_WIN32) || defined(_WIN64)
	#define SB_PLATFORM_WINDOWS
#endif

#if defined(__linux__)
	#define SB_PLATFORM_LINUX
#endif

#if defined(__FreeBSD__)
	#define SB_PLATFORM_BSD
#endif

#if defined(__APPLE__)
	#define SB_PLATFORM_MAC
#endif

#if defined(__linux__) || defined(__FreeBSD__) || defined(__APPLE__)
	#define SB_PLATFORM_UNIX
#endif

#endif

#endif