/*
 filesystem_persistence.cpp     MindForger thinking notebook

 Copyright (C) 2016-2022 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include "filesystem_persistence.h"

#include <sys/stat.h>

using namespace std;

namespace m8r {

string FilesystemPersistence::getUniqueDirOrFileName(
    const string& directory,
    const string* text,
    const string& extension
) {
    string filename = normalizeToNcName(*text, '-');
    string fullname{};
    struct stat buffer{};

    for(int discriminator=0; (fullname.size()?!stat(fullname.c_str(),&buffer):true); discriminator++) {
        fullname =
            directory +
            FILE_PATH_SEPARATOR +
            filename +
            (discriminator?"-"+std::to_string(discriminator):"") +
            extension;
    }

    return fullname;
}

FilesystemPersistence::FilesystemPersistence(MarkdownOutlineRepresentation& mdRepresentation, HtmlOutlineRepresentation& htmlRepresentation)
    : mdRepresentation(mdRepresentation), htmlRepresentation(htmlRepresentation)
{
}

FilesystemPersistence::~FilesystemPersistence()
{
}

void FilesystemPersistence::load(Stencil* stencil)
{
    if(stencil) {
        ifstream t{stencil->getFilePath()};
        string* str = new string{};
        t.seekg(0, std::ios::end);
        str->reserve(t.tellg());
        t.seekg(0, std::ios::beg);
        str->assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

        stencil->setContent(str);
    }
}

string FilesystemPersistence::createFileName(
    const string& directory,
    const string* text,
    const string& extension
) {
    return FilesystemPersistence::getUniqueDirOrFileName(
        directory, text, extension
    );
}

bool FilesystemPersistence::isWriteable(const std::string& outlineKey)
{
    MF_DEBUG("Checking writeability of O: " << outlineKey << endl);
    if(!outlineKey.size()) {
        MF_DEBUG("  O key is empty" << endl);
        return false;
    }
    std::ofstream out(outlineKey);
    if(!out.is_open()) {
        MF_DEBUG("  O is RD_ONLY" << endl);
        return false;
    }
    MF_DEBUG("  O opened: " << boolalpha << out.is_open() << endl);
    out.close();
    return true;
}

void FilesystemPersistence::save(Outline* outline)
{
    string* text = mdRepresentation.to(outline);
    if(text!=nullptr) {
        MF_DEBUG("Saving O: " << outline->getKey() << endl);
        std::ofstream out(outline->getKey());
        MF_DEBUG("  O opened: " << boolalpha << out.is_open() << endl);
        out << *text;
        MF_DEBUG("  O written: " << &out << endl);
        out.close();
        MF_DEBUG("O saved: " << &out << endl);
        delete text;

        outline->clearDirty();
    }
}

void FilesystemPersistence::saveAsHtml(Outline* outline, const string& fileName)
{
    string* text = new string{};
        htmlRepresentation.to(
        outline,
        text,
        true,
        false,
        true,
        false
    );
    std::ofstream out(fileName);
    out << *text;
    out.close();
    delete text;
}

} // m8r namespace
