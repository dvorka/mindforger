/*
 trie.h     MindForger thinking notebook

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
#ifndef M8R_TRIE_H
#define M8R_TRIE_H

#include <vector>
#include <string>

#include "../debug.h"

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
        // >1 it is word with given references, 0 it's char inside a word
        int mRefCount;
        std::vector<Node*> mChildren;

    public:
        explicit Node() {
            mContent = ' ';
            mRefCount = 0;
        }
        ~Node() {}
        char content() const { return mContent; }
        void setContent(char c) { mContent = c; }
        bool wordMarker() const { return mRefCount>0; }
        int refCount() const { return mRefCount; }
        int decRefCount() { if(mRefCount > 0) { mRefCount--; }; return mRefCount; }
        void setRefCount(int refCount) { mRefCount=refCount; }
        void setWordMarker() { ++mRefCount; }
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
    Trie& operator=(const Trie&) = delete;
    Trie& operator=(const Trie&&) = delete;
    ~Trie();

    bool empty() const { return root->children().empty(); }

    void addWord(const std::string& s);
    /**
     * @brief Is the word known to trie?
     */
    bool findWord(std::string& s) const;
    /**
     * @brief Find longest word which is prefix of s.
     */
    bool findLongestPrefixWord(const std::string& s, std::string& r) const;
    /**
     * @brief Remove word from trie.
     */
    bool removeWord(const std::string& s, bool decRefCountOnly=false);

    /**
     * @brief Print trie (backgracking).
     */
    int print() const;

private:
    int resursivePrint(std::string prefix, const Node* n, int count) const;
    void destroy(Node* n);
};

}
#endif // M8R_TRIE_H
