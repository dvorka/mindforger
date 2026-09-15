/*
 notebook_tree.cpp     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "notebook_tree.h"

using namespace std;

namespace m8r {

namespace {

bool isAbsolutePath(const string& path)
{
    if(path.empty()) {
        return false;
    }
    if(path[0] == '/' || path[0] == '\\') {
        return true;
    }
    if(path.size() >= 2 && path[1] == ':') {
        return true;
    }
    return false;
}

} // anonymous namespace

std::string NotebookTree::createNotebookTreeKey(
    const set<string>& keys,
    const string& mindDirectoryPath,
    const string& separator
) {
    string key = mindDirectoryPath + separator
            + "notebook-tree-"
            + std::to_string(datetimeNow())
            + ".md";
    while(keys.find(key) != keys.end()) {
        key.insert(key.size()-3, "_");
    }
    return key;
}

std::string NotebookTree::notebookTreeKeyToRelativePath(
    const string& key,
    const string& repositoryDir,
    const string& separator
) {
    string prefix{repositoryDir + separator};
    if(key.compare(0, prefix.size(), prefix) == 0) {
        return key.substr(prefix.size());
    }
    // defensive fallback: key unexpectedly not under the repository
    // root - keep it as is rather than produce a path that cannot be
    // resolved back on load
    return key;
}

std::string NotebookTree::resolveNotebookTreeKey(
    const string& storedKey,
    const string& repositoryDir,
    const string& separator
) {
    if(isAbsolutePath(storedKey)) {
        return storedKey;
    }
    return repositoryDir + separator + storedKey;
}

NotebookTree::NotebookTree(const std::string& name, const std::string& key)
    : Thing{name}
{
    this->key = key;
}

NotebookTree::NotebookTree(const NotebookTree& t)
    : Thing{t.getName()}
{
    this->key = t.key;
}

NotebookTree::~NotebookTree()
{
}

} // m8r namespace
