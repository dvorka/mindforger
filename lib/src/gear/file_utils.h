/*
 file_utils.h     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../definitions.h"

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

namespace filesystem {

    struct File
    {
        static const std::string EXTENSION_HTML;
        static const std::string EXTENSION_CSV;

        static const std::string EXTENSION_MD_MD;
        static const std::string EXTENSION_MD_MARKDOWN;
        static const std::string EXTENSION_MD_MDOWN;
        static const std::string EXTENSION_MD_MKDN;

        static const std::string EXTENSION_PDF;
        static const std::string EXTENSION_PDF_UPPER;

        static const std::string EXTENSION_TXT;

        /**
         * @brief Does file has one of supported Markdown extensions?
         */
        static bool fileHasMarkdownExtension(const std::string& filename);
        // TODO: PDF extension >> make it MIME

        /**
         * @brief Does file has one of supported PDF extensions?
         */
        static bool fileHasPdfExtension(const std::string& filename);
        // TODO: text extension >> make it MIME

        /**
         * @brief Does file has one of supported text file extensions?
         */
        static bool fileHasTextExtension(const std::string& filename);
        // TODO instead of hard-coded extensions use MIME types to support/work
        //      support and work with (new) extensions dynamically
        // TODO extensions as content type parsers could be registered dynamically
        //      just by name specification

    public:
        const std::string name;

        File(const std::string& name)
            : name(name)
        {
        }

        const std::string& getName() const noexcept { return name; }
    };

    /**
     * @brief The filesystem path class.
     *
     * Implementation of filesystem path for C++ 11-.
     *
     * @see https://en.cppreference.com/w/cpp/filesystem/path
     */
    class Path
    {
    private:
        std::string path;

    public:
        explicit Path(const std::string& path);
        explicit Path(const char* path);
        explicit Path(File& file);
        Path(const Path& other) {
            this->path = other.path;
        }
        Path(const Path&& other) {
            this->path = other.path;
        }
        Path& operator=(const Path& other) {
            this->path = other.path;
            return *this;
        }
        Path& operator=(const Path&& other) {
            this->path = other.path;
            return *this;
        }
        ~Path();

        Path& operator/(const std::string& fileOrDirName) {
            if(fileOrDirName.size()) {
                this->path += FILE_PATH_SEPARATOR;
                this->path += fileOrDirName;
            }
            return *this;
        }

        /**
         * @brief Operator of cast to std::string.
         */
        operator std::string() const {
            return path;
        }

        /**
         * @brief Return path as std::string.
         * @return Path as std::string.
         */
        std::string toString() const { return path; }
        /**
         * @brief Return path as C string.
         * @return Path as C string.
         */
        const char* toCString() const { return path.c_str(); }

        void clear() { this->path.clear(); }

    };

    /**
     * @brief << operator ensuring autocast to std::string (must be defined as function).
     */
    std::ostream& operator<<(std::ostream& out, const Path& p);

} // namespace: filesystem

#ifdef __cplusplus
extern "C" {
#endif
int ungzip(const char* srcFile, const char* dstFile);
#ifdef __cplusplus
}
#endif

void pathToDirectoryAndFile(const std::string& path, std::string& directory, std::string& file);
void pathToLinuxDelimiters(const std::string& path, std::string& linuxPath);
std::string platformSpecificPath(const char* path);
std::string& getSystemTempPath();
std::string getNewTempFilePath(const std::string& extension);
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
