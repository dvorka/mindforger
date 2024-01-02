/*
 tag_scope_aspect.cpp     MindForger thinking notebook

 Copyright (C) 2016-2024 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "tag_scope_aspect.h"

namespace m8r {

TagsScopeAspect::TagsScopeAspect(Ontology& ontology)
    : ontology(ontology)
{
}

TagsScopeAspect::~TagsScopeAspect()
{
}

bool TagsScopeAspect::isOutOfScope(const Outline* o) const
{
    return !inScope(o->getTags());
}

bool TagsScopeAspect::isOutOfScope(const Note* n) const
{
    return !inScope(n->getTags());
}

bool TagsScopeAspect::isInScope(const Outline* o) const
{
    return inScope(o->getTags());
}

bool TagsScopeAspect::isInScope(const Note* n) const
{
    return inScope(n->getTags());
}

bool TagsScopeAspect::inScope(const std::vector<const Tag*>* thingTags) const
{
    bool hasAllTags=true;
    for(size_t i=0; i<tags.size(); i++) {
        if(std::find(
                    thingTags->begin(),
                    thingTags->end(),
                    tags.at(i)) == thingTags->end())
        {
            hasAllTags=false;
            break;
        }
    }

    return hasAllTags;
}

} // m8r namespace
