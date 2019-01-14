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
    Node* current = root;

    if(!s.size()) {
        current->setWordMarker(); // an empty word
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

} // m8r namespace
