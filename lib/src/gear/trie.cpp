/*
 trie.cpp     MindForger thinking notebook

 Copyright (C) 2016-2021 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "trie.h"

namespace m8r {

using namespace std;

Trie::Trie()
{
    root = new Node();
}

Trie::~Trie()
{
    // delete nodes using backtracking
    destroy(root);
}

void Trie::destroy(Node* n)
{    
    for(Node* c:n->mChildren) {
        destroy(c);
    }
    delete n;
}

void Trie::addWord(const string& s)
{
    //MF_DEBUG("trie.add(" << s << ")" << endl);
    if(s.size()) {
        Node* current = root;

        if(!s.size()) {
            // support of empty words is NOT desired
            // current->setWordMarker();
            return;
        }

        for(size_t i=0; i<s.size(); i++) {
            Node* child = current->findChild(s[i]);

            if(child != nullptr) {
                current = child;
            } else {
                Node* n = new Node();
                n->setContent(s[i]);
                current->appendChild(n);
                current = n;
            }

            if(i == s.size()-1) {
                current->setWordMarker();
            }
        }
    }
}

/**
 * @brief Trie::removeWord
 *
 * This method sets word's reference count to 0 (default)
 * or decreases number of references to the word. If
 * reference counter becomes 0, then word is no longer
 * to trie.
 *
 * The method is suboptimal in a sense that nodes
 * are not destroyed immediately, but when the whole trie
 * is destroyed.
 */
bool Trie::removeWord(const string& s, bool decRefCountOnly)
{
    MF_DEBUG("trie.remove(" << s << ")" << endl);
    if(s.size()) {
        if(root->children().empty()) {
            return false;
        } else {
            Node* current = root;
            while(current != nullptr) {
                for(size_t i=0; i<s.size(); i++) {
                    Node* n = current->findChild(s[i]);
                    if(n == nullptr) {
                        return false;
                    }
                    current = n;
                }

                if(current->wordMarker()) {
                    if(decRefCountOnly) {
                        current->decRefCount();
                    } else {
                        current->setRefCount(0);
                    }
                    return true;
                }
            }
        }
    }

    return false;
}

bool Trie::findWord(string& s) const
{
    if(root->children().empty()) {
        return false;
    } else {
        Node* current = root;
        while(current != nullptr) {
            for(size_t i=0; i<s.size(); i++) {
                Node* n = current->findChild(s[i]);
                if(n == nullptr) {
                    return false;
                }
                current = n;
            }

            return current->wordMarker();
        }
        return false;
    }
}

bool Trie::findLongestPrefixWord(const string& s, string& r) const
{
    if(root->children().empty()) {
        return false;
    } else {
        size_t longestWordSize{};

        Node* current = root;
        if(current != nullptr) {
            for(size_t i=0; i<s.size(); i++) {
                Node* n = current->findChild(s[i]);
                if(n == nullptr) {
                    // TODO make this method
                    if(longestWordSize) {
                        r = r.substr(0, longestWordSize);
                        return true;
                    } else {
                        return false;
                    }
                }

                current = n;
                r += s[i];
                if(current->wordMarker()) {
                    longestWordSize = r.size();
                }
            }

            // TODO make this method
            if(longestWordSize) {
                r = r.substr(0, longestWordSize);
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
}

int Trie::print() const
{
    MF_DEBUG("Trie:" << endl);

    int count = 1;
    if(root->children().empty()) {
        MF_DEBUG("  EMPTY" << endl);
    } else {
        string prefix{};
        count = resursivePrint(prefix, root, count);
    }

    MF_DEBUG("Trie nodes: " << count << endl);
    return count;
}

int Trie::resursivePrint(string prefix, const Node* n, int count) const
{
    MF_DEBUG(
        (n->wordMarker()?" >":"  ") <<
        "'" << prefix << "' " <<
        (n->wordMarker()?std::to_string(n->refCount()):"") << endl);

    std::vector<Node*> children = n->children();
    for(Node* n:children) {
        prefix += n->content();
        count = resursivePrint(prefix, n, ++count);
        prefix = prefix.substr(0, prefix.size()-1);
    }

    return count;
}

} // m8r namespace
