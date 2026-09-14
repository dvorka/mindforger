/*
 notebook_tree.h     MindForger thinking notebook

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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
#ifndef M8R_NOTEBOOK_TREE_H
#define M8R_NOTEBOOK_TREE_H

#include <string>
#include <set>

#include "../gear/datetime_utils.h"
#include "../mind/ontology/thing_class_rel_triple.h"

namespace m8r {

/**
 * @brief NotebookTree - user defined, named tree of Notebooks.
 *
 * A NotebookTree is a lightweight registry entry: its key is the
 * absolute path to its own backing Markdown Outline file (the file
 * holding the actual tree of Notes, each linking to a Notebook), so
 * that the key is naturally unique per repository w/o any separate
 * id generation scheme.
 */
class NotebookTree : public Thing
{
public:
    static std::string createNotebookTreeKey(
        const std::set<std::string>& keys,
        const std::string& mindDirectoryPath,
        const std::string& separator
    );

private:
    /*
     * transient fields
     */

    time_t modified;

public:
    explicit NotebookTree(const std::string& name, const std::string& key);
    explicit NotebookTree(const NotebookTree& t);
    NotebookTree(const NotebookTree&&) = delete;
    NotebookTree& operator=(const NotebookTree&) = delete;
    NotebookTree& operator=(const NotebookTree&&) = delete;
    ~NotebookTree();

    void setKey(const std::string& key) { this->key = key; }

    bool operator<(const NotebookTree& other) const {
        return modified < other.modified;
    }

    bool operator<(NotebookTree* other) const {
        return modified < other->modified;
    }

    time_t getModified() const { return this->modified; }
    void makeModified();
};

}
#endif // M8R_NOTEBOOK_TREE_H
