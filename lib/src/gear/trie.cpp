/*
 trie.cpp     MindForger thinking notebook

 Copyright (C) 2016-2019 Martin Dvorak <martin.dvorak@mindforger.com>

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

void Trie::addWord(string s)
{
    MF_DEBUG("  trie.add(): '" << s << "'" << endl);

    Node* current = root;

    if(!s.size()) {
        // support of empty words is not desired
        // current->setWordMarker();
        return;
    }

    for(size_t i=0; i<s.size(); i++) {
        Node* child = current->findChild(s[i]);

        if(child != nullptr) {
            current = child;
        } else {
            Node* n= new Node();
            n->setContent(s[i]);
            current->appendChild(n);
            current = n;
        }

        if(i == s.size()-1) {
            current->setWordMarker();
        }
    }
}

bool Trie::findWord(string& s) const
{
    if(root->children().empty()) {
        return false;
    } else {
        Node* current = root;
        while(current != nullptr) {
            for(size_t i=0; i<s.size(); i++) {
                Node* n= current->findChild(s[i]);
                if(n == nullptr) {
                    return false;
                }
                current = n;
            }

            if(current->wordMarker()) {
                return true;
            } else {
                return false;
            }
        }
        return false;
    }
}

bool Trie::findLongestPrefixWord(string& s, string& r) const
{
    if(root->children().empty()) {
        return false;
    } else {
        int longestWordSize{};

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

} // m8r namespace
