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

NotebookTree::NotebookTree(const std::string& name, const std::string& key)
    : Thing{name},
      modified{datetimeNow()}
{
    this->key = key;
}

NotebookTree::NotebookTree(const NotebookTree& t)
    : Thing{t.getName()},
      modified{t.modified}
{
    this->key = t.key;
}

NotebookTree::~NotebookTree()
{
}

void NotebookTree::makeModified()
{
    this->modified = datetimeNow();
}

} // m8r namespace
