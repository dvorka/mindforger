/*
 file_utils.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifdef __linux__
        found=path.find_last_of("/");
#elif _WIN32
        found=path.find_last_of("\\");
#else
        // IMPROVE complete the code
        found=path.find_last_of("/");
#endif
        if(found == string::npos) {
            directory = ".";
        } else {
            directory = path.substr(0,found);
        }

        file = path.substr(found+1);
    }
}

bool stringToLines(const string* text, vector<string*>& lines)
{
    if(text && text->size()) {
        istringstream input{*text};
        string line;
        while(getline(input, line)) {
            // IMPROVE heap allocation possibly expensive
            lines.push_back(new string{line});
        }
        return true;
    } else {
        return false;
    }
}

bool fileToLines(const string* filename, vector<string*>& lines, unsigned long int &fileSize)
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
    // IMPROVE windows code goes here
#else
    // IMPROVE complete the code
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

void resolvePath(const std::string& path, std::string& resolvedAbsolutePath)
{
    // output buffer MUST be set to NULL (check realpath manpage)
    char * rp = realpath(path.c_str(), NULL);
    if(!rp) {
        cerr << "Error: unable to resolve path '" << path << "'" << endl;
        resolvedAbsolutePath.assign(path);
    } else {
        resolvedAbsolutePath.assign(rp);
        free(rp);
    }
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

char* makeTempDirectory(char* dirNamePrefix)
{
    char tmpl[100];
    tmpl[0] = 0;
    strcat(tmpl, SYSTEM_TEMP_DIRECTORY);
    strcat(tmpl, FILE_PATH_SEPARATOR);
    strcat(tmpl, dirNamePrefix);
    strcat(tmpl, "XXXXXX");
    return mkdtemp(tmpl);
}

int removeDirectoryRecursively(const char *path)
{
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;
   if(d) {
      struct dirent *p;
      r = 0;
      while(!r && (p=readdir(d))) {
          int r2 = -1;
          char *buf;
          size_t len;
          // skip the names "." and ".." as I don't want to recurse on them
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) {
             continue;
          }
          len = path_len + strlen(p->d_name) + 2;
          buf = new char[len];
          if(buf) {
             struct stat statbuf;
             // IMPROVE MF_DEBUG
             snprintf(buf, len, "%s/%s", path, p->d_name);
             if (!stat(buf, &statbuf)) {
                if (S_ISDIR(statbuf.st_mode)) {
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
      r = rmdir(path);
   }

   return r;
}

} // m8r namespace
