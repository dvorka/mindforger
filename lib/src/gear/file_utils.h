/*
 file_utils.h     MindForger thinking notebook

 Copyright (C) 2016-2020 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef M8R_FILE_UTILS_H_
#define M8R_FILE_UTILS_H_

#include <sys/types.h>
#include <sys/stat.h>
#include "../config/config.h"

#include <zlib.h>
#if defined(MSDOS) || defined(OS2) || defined(_WIN32) || defined(__CYGWIN__)
  #include <fcntl.h>
  #include <io.h>
  #define SET_BINARY_MODE(file) setmode(fileno(file), O_BINARY)
#else
  #define SET_BINARY_MODE(file)
#endif

#ifdef __APPLE__
  #include <mach-o/dyld.h>
#endif

#include <ctime>
#include <cstdio>
#include <cstring>

#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "../debug.h"
#include "../exceptions.h"
#include "string_utils.h"

#ifdef __linux__
constexpr const auto FILE_PATH_SEPARATOR = "/";
constexpr const auto FILE_PATH_SEPARATOR_CHAR = '/';
#elif _WIN32
constexpr const auto FILE_PATH_SEPARATOR = "\\";
constexpr const auto FILE_PATH_SEPARATOR_CHAR = '\\';
#else
constexpr const auto FILE_PATH_SEPARATOR = "/";
constexpr const auto FILE_PATH_SEPARATOR_CHAR = '/';
#endif

#ifdef __linux__
constexpr const auto SYSTEM_TEMP_DIRECTORY = "/tmp";
#elif _WIN32
constexpr const auto SYSTEM_TEMP_DIRECTORY = "c:\\Windows\\Temp";
#else
constexpr const auto SYSTEM_TEMP_DIRECTORY = "/tmp";
#endif

namespace m8r {

struct File
{
    const std::string name;

    File(const std::string& name)
        : name(name)
    {
    }

    const std::string& getName() const noexcept { return name; }
};

#ifdef __cplusplus
extern "C" {
#endif
int ungzip(const char* srcFile, const char* dstFile);
#ifdef __cplusplus
}
#endif

void pathToDirectoryAndFile(const std::string& path, std::string& directory, std::string& file);
void pathToLinuxDelimiters(const std::string& path, std::string& linuxPath);
bool stringToLines(const std::string* text, std::vector<std::string*>& lines);
bool fileToLines(const std::string* filename, std::vector<std::string*>& lines, size_t& filesize);
std::string* fileToString(const std::string& filename);
void stringToFile(const std::string& filename, const std::string& content);
time_t fileModificationTime(const std::string* filename);
bool copyFile(const std::string& from, const std::string& to);
bool moveFile(const std::string& from, const std::string& to);
void resolvePath(const std::string& path, std::string& resolvedAbsolutePath);
bool isDirectoryOrFileExists(const char* path);
bool isDirectory(const char* path);
bool isFile(const char* path);
bool isPathRelative(const std::string& path);
char* makeTempDirectory(char* dirNamePrefix);
int removeDirectoryRecursively(const char* path);
int copyDirectoryRecursively(const char* srcPath, const char* dstPath, bool extractGz=false);
bool createDirectory(const std::string& path);

/**
 * @brief Get path to the the executable on macOS or windows. Othewise returns nullptr.
 *
 * Method is not reentrant - it returns pointer to the static buffer.
 */
char* getExecutablePath();
} // m8r namespace

#endif /* M8R_FILE_UTILS_H_ */
