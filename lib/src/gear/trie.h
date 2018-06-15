/*
 trie.h     MindForger thinking notebook

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
#ifndef M8R_TRIE_H
#define M8R_TRIE_H

#include <vector>
#include <string>

// TODO remove
#include <iostream>

namespace m8r {

/**
 * @brief Trie.
 *
 * This implementation has been inspired by an http://www.sourcetricks.com example.
 */
class Trie
{
private:
    class Node {
        friend class Trie;

    private:
        char mContent;
        bool mMarker;
        std::vector<Node*> mChildren;

    public:
        explicit Node() {
            mContent = ' ';
            mMarker = false;
        }
        ~Node() {}
        char content() const { return mContent; }
        void setContent(char c) { mContent = c; }
        bool wordMarker() const { return mMarker; }
        void setWordMarker() { mMarker = true; }
        void appendChild(Node* child) { mChildren.push_back(child); }
        std::vector<Node*> children() const { return mChildren; }

        // IMPROVE sort children once trie filled AND use binary search here O(n) -> O(log(n))
        Node* findChild(char c) {
            for(Node* n:mChildren) {
                if(n->content()==c) {
                    return n;
                }
            }
            return nullptr;
        }
    };

    Node* root;

public:
    explicit Trie();
    Trie(const Trie&) = delete;
    Trie(const Trie&&) = delete;
    Trie &operator=(const Trie&) = delete;
    Trie &operator=(const Trie&&) = delete;
    ~Trie();

    bool empty() const { return root->children().empty(); }

    void addWord(std::string s);
    bool findWord(std::string& s) const;
    void deleteWord(std::string s);

private:
    void destroy(Node* n);
};

}
#endif // M8R_TRIE_H
