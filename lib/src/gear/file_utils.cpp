/*
 file_utils.cpp     MindForger thinking notebook

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
#include "file_utils.h"

using namespace std;

namespace m8r {

void pathToDirectoryAndFile(const std::string& path, std::string& directory, std::string& file)
{
    if(!path.empty()) {
        size_t found;
        // IMPROVE complete the code
        found=path.find_last_of(FILE_PATH_SEPARATOR);
        if(found == string::npos) {
            directory = ".";
        } else {
            directory = path.substr(0,found);
        }

        file = path.substr(found+1);
    }
}

void pathToLinuxDelimiters(const std::string& path, std::string& linuxPath)
{
    if(!path.empty()) {
        linuxPath.assign(path);
        std::replace(linuxPath.begin(), linuxPath.end(), '\\', '/');
    }
}

bool stringToLines(const string* text, vector<string*>& lines)
{
    if(text && !text->empty()) {
        istringstream input{*text};
        string line;
        while(getline(input, line)) {
            // IMPROVE heap allocation possibly expensive
            lines.push_back(new string{line});
        }
        return true;
    }

    return false;
}

bool fileToLines(const string* filename, vector<string*>& lines, size_t &fileSize)
{
    ifstream infile(*filename);
    string line;
    while(getline(infile, line)) {
        fileSize+=line.size()+1;
        // IMPROVE heap allocation possibly expensive
        lines.push_back(new string{line});
    }
    infile.close();
    return fileSize>0;
}

string* fileToString(const string& filename)
{
    ifstream is(filename);
    string* s = new string{};

    is.seekg(0, ios::end);
    s->reserve(is.tellg());
    is.seekg(0, ios::beg);
    s->assign((istreambuf_iterator<char>(is)),istreambuf_iterator<char>());

    return s;
}

void stringToFile(const string& filename, const string& content)
{
    ofstream out(filename);
    out << content;
    out.close();
}

time_t fileModificationTime(const string* filename)
{
#ifdef __linux__
    // IMPROVE stat t_stat{}; doesn't compile for me - ignore IDE warning for this - value initializer {} MUST present
    typedef struct stat attrs;
    attrs t_stat{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // crazy initializer required by older GCC versions
    stat(filename->c_str(), &t_stat);
    return t_stat.st_mtime; // modification time ~ file content modification; st_ctime ~ file metata change (more sensitive)
#elif _WIN32
    time_t tMod = 0;
    FILETIME ft;
    HANDLE hFile;
    ULARGE_INTEGER ull;
    hFile = CreateFileA(filename->c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if(hFile != INVALID_HANDLE_VALUE) {
        if(GetFileTime(hFile, nullptr, nullptr, &ft)) {
            // Convert the last-write time to local time.
            ull.LowPart = ft.dwLowDateTime;
            ull.HighPart = ft.dwHighDateTime;
            tMod = ull.QuadPart / 10000000ULL - 11644473600ULL;
        }
        CloseHandle(hFile);
    }
    return tMod;
#else
    // IMPROVE complete the code
    typedef struct stat attrs;
    attrs t_stat{};
    stat(filename->c_str(), &t_stat);
    return t_stat.st_mtime; // modification time ~ file content modification; st_ctime ~ file metata change (more sensitive)
#endif
}

bool copyFile(const string &from, const string &to)
{
    ifstream  src(from, ios::binary);
    ofstream  dst(to,   ios::binary);
    dst << src.rdbuf();
    return true;
}

bool moveFile(const string &from, const string &to)
{
    copyFile(from,to);
    if(remove(from.c_str())) {
      return false;
    } else {
        return true;
    }
}

#define BUFSIZE 4096

void resolvePath(const std::string& path, std::string& resolvedAbsolutePath)
{
#ifdef _WIN32
    char  buffer[BUFSIZE] = "";
    if(GetFullPathNameA(path.c_str(), BUFSIZE, buffer, nullptr)) {
        resolvedAbsolutePath.assign(buffer);

    } else {
        cerr << "Error: unable to resolve path '" << path << "'" << endl;
        resolvedAbsolutePath.assign(path);
    }


#else
    // output buffer MUST be set to NULL (check realpath manpage)
    char * rp = realpath(path.c_str(), NULL);
    if(!rp) {
        cerr << "Error: unable to resolve path '" << path << "'" << endl;
        resolvedAbsolutePath.assign(path);
    } else {
        resolvedAbsolutePath.assign(rp);
        free(rp);
    }
#endif //_WIN32
}

bool isDirectoryOrFileExists(const char* path)
{
    struct stat info;
    if(stat(path, &info)) {
        return false;
    } else {
        return true;
    }
}

bool isDirectory(const char* path)
{
    struct stat info;
    if(stat(path, &info)) {
        return false;
    } else if(S_ISDIR(info.st_mode)) {
        return true;
    } else {
        return false;
    }
}

bool isFile(const char* path)
{
    struct stat info;
    if(stat(path, &info)) {
        return false;
    } else if(S_ISDIR(info.st_mode)) {
        return false;
    } else {
        return true;
    }
}

bool isPathRelative(const string& path)
{
    // IMPROVE relative vs. absolute is platform specific (remind Windows; new C++ standards have methods for this)
    if(path.size() && path.at(0) == '/') {
        return true;
    } else {
        return false;
    }
}

bool createDirectory(const string& path) {
#ifdef _WIN32
    int e = _mkdir(path.c_str());
#else
    int e = mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
#endif

    if(e) {
        cerr << "Failed to create directory '" << path << "' with error " << e;
        return false;
    } else {
        return true;
    }
}

char* makeTempDirectory(char* dirNamePrefix)
{
#ifdef _WIN32
    char *ret = nullptr;
    char  *tempPathBuffer = new char[MAX_PATH];
    UUID uuid;
    RPC_CSTR uuidStr;
    GetTempPathA(MAX_PATH, tempPathBuffer);
    strcat(tempPathBuffer, FILE_PATH_SEPARATOR);
    if (strlen(tempPathBuffer) + strlen(dirNamePrefix) < MAX_PATH) {
        strcat(tempPathBuffer, dirNamePrefix);
        UuidCreate(&uuid);
        UuidToStringA(&uuid, &uuidStr);
        if (strlen(tempPathBuffer) + strlen((char*)uuidStr) < MAX_PATH) {
             strcat(tempPathBuffer, (char*)uuidStr);
             if (CreateDirectoryA(tempPathBuffer, nullptr)) {
                 ret = tempPathBuffer;
             }
        }
        RpcStringFreeA(&uuidStr);
    }
    if (ret == nullptr) {
        delete [] tempPathBuffer;
    }
    return ret;
#else
    char *tmpl = new char[100];
    tmpl[0] = 0;
    strcat(tmpl, SYSTEM_TEMP_DIRECTORY);
    strcat(tmpl, FILE_PATH_SEPARATOR);
    strcat(tmpl, dirNamePrefix);
    strcat(tmpl, "XXXXXX");
    return mkdtemp(tmpl);
#endif
}

int removeDirectoryRecursively(const char* path)
{
   DIR* d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;
   if(d) {
       struct dirent* p;
       r = 0;
       while(!r && (p=readdir(d))) {
           int r2 = -1;
           char *buf;
           size_t len;
           // skip the names "." and ".." as I don't want to recurse on them
           if(!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
               continue;
           }
           len = path_len + strlen(p->d_name) + 2;
           buf = new char[len];
           if(buf) {
               struct stat statbuf;
               // IMPROVE MF_DEBUG
               snprintf(buf, len, "%s/%s", path, p->d_name);
               if(!stat(buf, &statbuf)) {
                   if(S_ISDIR(statbuf.st_mode)) {
                       r2 = removeDirectoryRecursively(buf);
                   } else {
                       r2 = unlink(buf);
                   }
               }
               delete[] buf;
           }
           r = r2;
       }
       closedir(d);
   }
   if(!r) {
#ifdef _WIN32
       r = _rmdir(path);
#else
       r = rmdir(path);
#endif
   }

   return r;
}





#ifdef GZIP_DEFLATE_VIA_ZIP_LIBRARY

void unzip(const char* srcFile, const char* dstFile)
{
    int err = 0;
    zip* z = zip_open(srcFile, 0, &err);

    struct zip_stat zipStat;
    zip_stat_init(&zipStat);
    zip_stat(z, dstFile, 0, &zipStat);

    char* contents = new char[zipStat.size];

    zip_file* f = zip_fopen(z, dstFile, 0);
    const zip_int64_t did_read = zip_fread(f, contents, zipStat.size);
    if(did_read > 0) {
        zip_fclose(f);
    }

    zip_close(z);

    delete[] contents;
}

#endif

#ifdef GZIP_DEFLATE_VIA_ZLIB_PIPE_

int ungzipFile(FILE* source, FILE* dest)
{
    #define CHUNK 16384

    /* avoid end-of-line conversions */
    SET_BINARY_MODE(source);
    SET_BINARY_MODE(dest);

    int ret;
    unsigned have;
    z_stream strm;
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit(&strm);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        strm.avail_in = fread(in, 1, CHUNK, source);
        if (ferror(source)) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = CHUNK;
            strm.next_out = out;
            ret = inflate(&strm, Z_NO_FLUSH);


            // IMPROVE don't want to crash assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            if(ret == Z_STREAM_ERROR) return Z_STREAM_ERROR;


            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }
            have = CHUNK - strm.avail_out;
            if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

int ungzip(const char* srcFile, const char* dstFile)
{
    if(isDirectoryOrFileExists(srcFile) && !isDirectoryOrFileExists(dstFile)) {
        FILE* srcFILE = fopen(srcFile, "r");
        FILE* dstFILE = fopen(dstFile, "r+");
        int r = ungzipFile(srcFILE, dstFILE);

        // IMPROVE polish error messages - debug/cerr
        if(r != Z_OK) {
            fputs("zpipe: ", stderr);
            switch (r) {
            case Z_ERRNO:
                if (ferror(stdin))
                    fputs("error reading stdin\n", stderr);
                if (ferror(stdout))
                    fputs("error writing stdout\n", stderr);
                break;
            case Z_STREAM_ERROR:
                fputs("invalid compression level\n", stderr);
                break;
            case Z_DATA_ERROR:
                fputs("invalid or incomplete deflate data\n", stderr);
                break;
            case Z_MEM_ERROR:
                fputs("out of memory\n", stderr);
                break;
            case Z_VERSION_ERROR:
                fputs("zlib version mismatch!\n", stderr);
            }
        }
        return r;
    }

    return Z_STREAM_ERROR;
}

#endif

// TODO this code must be completely rewritten
#ifdef __cplusplus
extern "C" {
#endif

/* The following macro calls a zlib routine and checks the return
   value. If the return value ("status") is not OK, it prints an error
   message and exits the program. Zlib's error statuses are all less
   than zero. */

#define GZIP_CALL_ZLIB(x) {                                                  \
        int status;                                                     \
        status = x;                                                     \
        if (status < 0) {                                               \
            fprintf (stderr,                                            \
                     "%s:%d: %s returned a bad status of %d.\n",        \
                     __FILE__, __LINE__, #x, status);                   \
            exit (EXIT_FAILURE);                                        \
        }                                                               \
    }

/* if "test" is true, print an error message and halt execution. */

#define GZIP_FAIL(test,message) {                             \
        if (test) {                                      \
            inflateEnd (& strm);                         \
            fprintf (stderr, "%s:%d: " message           \
                     " file '%s' failed: %s\n",          \
                     __FILE__, __LINE__, srcFile,      \
                     strerror (errno));                  \
            exit (EXIT_FAILURE);                         \
        }                                                \
    }


int ungzip(const char* srcFile, const char* dstFile)
{
    UNUSED_ARG(dstFile);

    // IMPROVE make these variables
#define CHUNK 0x4000
#define windowBits 15
#define ENABLE_ZLIB_GZIP 32

    FILE* srcFILE;
    FILE* dstFILE;
    z_stream strm = {nullptr,0,0,nullptr,0,0,nullptr,nullptr,nullptr,nullptr,nullptr,0,0,0};
    unsigned char in[CHUNK];
    unsigned char out[CHUNK];

    strm.zalloc = nullptr;
    strm.zfree = nullptr;
    strm.opaque = nullptr;
    strm.next_in = in;
    strm.avail_in = 0;
    GZIP_CALL_ZLIB (inflateInit2 (& strm, windowBits | ENABLE_ZLIB_GZIP));

    // IMPROVE error reporting
    if(!isDirectoryOrFileExists(srcFile) || isDirectoryOrFileExists(dstFile)) {
        return Z_STREAM_ERROR;
    }

    // open src file
    srcFILE = fopen (srcFile, "rb");
    GZIP_FAIL (! srcFILE, "open");

    // open dst file
    dstFILE = fopen(dstFile, "wb");
    GZIP_FAIL (! dstFILE, "open");

    while (1) {
        size_t bytes_read;
        int zlib_status;

        bytes_read = fread (in, sizeof (char), sizeof (in), srcFILE);
        GZIP_FAIL (ferror (srcFILE), "read");
        strm.avail_in = static_cast<uInt>(bytes_read);
        strm.next_in = in;
        do {
            unsigned have;
            strm.avail_out = CHUNK;
            strm.next_out = out;
            zlib_status = inflate (& strm, Z_NO_FLUSH);

            switch (zlib_status) {
            case Z_OK:
            case Z_STREAM_END:
            case Z_BUF_ERROR:
                break;
            default:
                inflateEnd (& strm);
                fprintf (stderr, "Gzip error %d in '%s'.\n", zlib_status, srcFile);
                return -1;
            }

            have = CHUNK - strm.avail_out;

            // write deflated data
            fwrite(out, sizeof (unsigned char), have, dstFILE);
        } while (strm.avail_out == 0);

        if (feof (srcFILE)) {
            inflateEnd (& strm);
            break;
        }
    }
    GZIP_FAIL (fclose (srcFILE), "close");
    GZIP_FAIL (fclose (dstFILE), "close");
    return 0;
}

#ifdef __cplusplus
}
#endif

// IMPROVE error handling to be fixed
int copyDirectoryRecursively(const char* srcPath, const char* dstPath, bool extractGz)
{
    DIR *d = opendir(srcPath);
    size_t srcPathLen = strlen(srcPath);
    size_t dstPathLen = strlen(dstPath);
    int r = -1;
    if(d) {
        MF_DEBUG("DIR: " << dstPath << endl);
        if(!isDirectoryOrFileExists(dstPath)) createDirectory(string{dstPath});

        struct dirent *p;
        r = 0;
        while(!r && (p=readdir(d))) {
            int r2 = -1;
            char *srcBuf, *dstBuf;
            size_t srcLen, dstLen;
            // skip the names "." and ".." as I don't want to recurse on them
            if(!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
                continue;
            }
            srcLen = srcPathLen + strlen(p->d_name) + 2;
            dstLen = dstPathLen + strlen(p->d_name) + 2;
            srcBuf = new char[srcLen];
            dstBuf = new char[dstLen];
            if(srcBuf) {
                struct stat statbuf;
                // IMPROVE MF_DEBUG
                snprintf(srcBuf, srcLen, "%s/%s", srcPath, p->d_name);
                snprintf(dstBuf, dstLen, "%s/%s", dstPath, p->d_name);
                if(!stat(srcBuf, &statbuf)) {
                    if(S_ISDIR(statbuf.st_mode)) {
                        r2 = copyDirectoryRecursively(srcBuf, dstBuf, extractGz);
                    } else {
                        MF_DEBUG("FILE: " << dstBuf << endl);
                        if(!isDirectoryOrFileExists(dstBuf)) {
                            copyFile(string{srcBuf}, string{dstBuf});
                            if(extractGz) {
                                if(stringEndsWith(dstBuf,".gz") && strlen(dstBuf)>3) {
                                    char* dstExt = new char[strlen(dstBuf)+1];
                                    strcpy(dstExt, dstBuf);
                                    if(strlen(dstExt)>3) {
                                        dstExt[strlen(dstExt)-3] = 0;
                                        MF_DEBUG("  ungzip: '" << dstBuf << "' > '" << dstExt << "'" << endl);
                                        ungzip(dstBuf, dstExt);
                                    }
                                    delete[] dstExt;
                                }
                            }
                        }
                        r2 = 0;
                    }
                }
                delete[] srcBuf;
                delete[] dstBuf;
            }
            r = r2;
        }
        closedir(d);
    }

    return r;
}

char* getExecutablePath() {
#ifdef __APPLE__
    static char exePath[2048];
    uint32_t len = sizeof(exePath);
    if(_NSGetExecutablePath(exePath, &len) != 0) {
        // buffer too small
        exePath[0] = '\0';
    } else {
        // resolve symlinks, ., .. if possible
        char *canonicalPath = realpath(exePath, NULL);
        if(canonicalPath != NULL) {
            strncpy(exePath,canonicalPath,len);
            free(canonicalPath);
        }
    }
    return exePath;
#elif defined(_WIN32)
    static char exePath[MAX_PATH+1];
    GetModuleFileNameA( nullptr, exePath, MAX_PATH );
    return exePath;
#else
    return nullptr;
#endif

}

} // m8r namespace
