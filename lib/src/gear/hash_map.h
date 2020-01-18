/*
 hash_map.h     MindForger thinking notebook

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
#ifndef M8R_HASH_MAP_H_
#define M8R_HASH_MAP_H_

#include <cstring>
#include <iostream>
#include <vector>

namespace m8r {

/**
 * Hash map and set.
 */
template<class VALUE>
class HashMap
{
private:
    struct Entry {
        char* key;
        VALUE* value;
        struct Entry* next;
    };

    int tableSize;
    Entry** table;
    int entrySize;

    bool freeValues;

public:
    HashMap(bool deleteValuesOnDestruction=false, int size=10007);
    HashMap(const HashMap&) = delete;
    HashMap(const HashMap&&) = delete;
    HashMap &operator=(const HashMap&) = delete;
    HashMap &operator=(const HashMap&&) = delete;
    virtual ~HashMap();

    void put(const char* key, VALUE* value);
    void add(const char* key) { put(key, nullptr); }
    VALUE* get(const char* key);
    bool contains(const char* key) { return (get(key)!=nullptr); }
    std::vector<char*>* keys();

    void put(const std::string* key, VALUE* value) { put(key->c_str(), value); }
    void add(const std::string* key) { put(key, nullptr); }
    VALUE* get(const std::string* key) { return get(key->c_str()); }
    bool contains(const std::string* key) { return (get(key)!=nullptr); }

    int size() const { return entrySize; }
    void stat();

private:
    unsigned int hash(const char *key)
    {
        unsigned int result=5381;
        for(int i=0; key[i]!='\0'; i++) {
            result=result*33+key[i];
        }
        result = result^(result>>16);

        return result%tableSize;
    }
};

template<class VALUE>
HashMap<VALUE>::HashMap(bool freeValues, int tableSize)
{
    this->tableSize = tableSize;
    this->table = new Entry*[tableSize];
    for(int slot=0; slot<tableSize; slot++) {
        table[slot] = nullptr;
    }
    this->freeValues = freeValues;
    this->entrySize = entrySize;
}

template<class VALUE>
HashMap<VALUE>::~HashMap()
{
    Entry *entry, *kill;
    for(int slot{}; slot<tableSize; slot++) {
        entry = table[slot];
        while(entry) {
            kill = entry;
            entry = entry->next;
            delete kill->key;
            if(freeValues) {
                delete kill->value;
            }
            delete kill;
        }
    }

    if(table) {
        delete table;
        table = nullptr;
    }
}

template<class VALUE>
void HashMap<VALUE>::put(const char* key, VALUE* value)
{
    int slot = hash(key);
    Entry *entry = table[slot];
    while(entry && strcmp(entry->key, key)) {
        entry = entry->next;
    }
    if(entry) {
        entry->value = value;
    } else {
        entry = new Entry();
        entry->key = new char[strlen(key)+1];
        strcpy(entry->key, key);
        entry->value=value;
        entry->next=table[slot];
        table[slot]=entry;
        entrySize++;
    }
}

template<class VALUE>
VALUE* HashMap<VALUE>::get(const char* key)
{
    int slot = hash(key);
    Entry *ptr = table[slot];
    while(ptr && strcmp(ptr->key, key)) {
        ptr = ptr->next;
    }

    return ptr?ptr->value:nullptr;
}

template<class VALUE>
std::vector<char*>* HashMap<VALUE>::keys()
{
    if(entrySize) {
        std::vector<char*>* result = new std::vector<char*>{};
        int i{};
        Entry *p{};
        for(i=0; i<tableSize; i++) {
            p=table[i];
            while(p) {
                result->push_back(p->key);
                p=p->next;
            }
        }
        return result;
    } else {
        return nullptr;
    }
}

template<class VALUE>
void HashMap<VALUE>::stat()
{
    Entry *entry{};
    for(int slot=0; slot<tableSize; slot++) {
        for(entry=table[slot]; entry!=nullptr; entry=entry->next) {
            std::cout << std::endl << entry->key;
        }
    }
}

} // m8r namespace

#endif /* M8R_HASH_MAP_H_ */
