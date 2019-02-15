#ifndef CONFIG_H
#define CONFIG_H

#ifdef _WIN32
#include <Windows.h>
//typedef uint8_t   u_int8_t;
//typedef uint16_t  u_int16_t;
//typedef uint32_t  u_int32_t;
typedef short int   u_int8_t;
typedef int u_int16_t;
typedef long int  u_int32_t;
typedef unsigned int uint;
#include "../../../build/windows/dirent/dirent.h"
#include <io.h>
#include <direct.h>
#else
#include <dirent.h>
#include <sys/dir.h>
#include <unistd.h>
#endif // _WIN32

#endif // CONFIG_H
