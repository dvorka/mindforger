/*
 filesystem_information.cpp     MindForger thinking notebook

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
#include "filesystem_information.h"

using namespace std;

namespace m8r {

FilesystemInformationSource::FilesystemInformationSource(
    string& sourcePath,
    FilesystemPersistence& persistence,
    MarkdownDocumentRepresentation& mdDocumentRepresentation
)
    : InformationSource{SourceType::FILESYSTEM, sourcePath},
      persistence{persistence},
      mdDocumentRepresentation{mdDocumentRepresentation}
{
}

FilesystemInformationSource::~FilesystemInformationSource()
{
}

bool FilesystemInformationSource::indexToMemory(Repository& repository)
{
    if(!isDirectory(locator.c_str())) {
        MF_DEBUG("Error: filesystem information resource cannot be indexed to memory as its locator path '" << locator << "' does not exist");
        return false;
    }

    if(repository.getType() != Repository::RepositoryType::MINDFORGER
       || repository.getMode() != Repository::RepositoryMode::REPOSITORY
    ) {
        MF_DEBUG("Error: filesystem information resource cannot be indexed as active directory is not of MINDFORGER/REPOSITORY type");
        return false;
    }

    string memoryPath{repository.getDir()+FILE_PATH_SEPARATOR+DIRNAME_MEMORY};
    if(!isDirectory(memoryPath.c_str())) {
        MF_DEBUG("Error: filesystem information resource cannot be indexed to memory path '" << memoryPath << "' as this directory does not exist");
        return false;
    }

    string memoryLibIndexPath{memoryPath};
    memoryLibIndexPath += FILE_PATH_SEPARATOR;
    memoryLibIndexPath += FilesystemInformationSource::DIR_MEMORY_M1ndF0rg3rL1br8ry;
    createDirectory(memoryLibIndexPath);

    string memoryInformationSourceIndexPath{memoryLibIndexPath};
    memoryInformationSourceIndexPath += FILE_PATH_SEPARATOR;
    memoryInformationSourceIndexPath += normalizeToNcName(this->locator, '_');
    createDirectory(memoryInformationSourceIndexPath);

    indexDirectoryToMemory(locator, memoryPath);

    string outlinePathInMemory{};
    string relativeDocLibPath{};
    string outlineDir{};
    string outlineFilename{};
    for(auto f:this->pdfs) {
        MF_DEBUG("  " << *f << endl);
        relativeDocLibPath.assign(f->substr(locator.size()+1));
        MF_DEBUG("    " << relativeDocLibPath << endl);

        outlinePathInMemory.assign(memoryInformationSourceIndexPath);
        outlinePathInMemory += FILE_PATH_SEPARATOR;
        outlinePathInMemory += relativeDocLibPath;
        outlinePathInMemory += FILE_EXTENSION_MD_MD;
        MF_DEBUG("      " << outlinePathInMemory << endl);

        pathToDirectoryAndFile(outlinePathInMemory, outlineDir, outlineFilename);
        if(outlineDir.size()) {
            // TODO create directory including parent directories
            createDirectory(outlineDir);
        }

        persistence.save(
            mdDocumentRepresentation.to(*f, outlinePathInMemory)
        );
    }

    return true;
}

void FilesystemInformationSource::indexDirectoryToMemory(
    const string& directory,
    const string& memoryPath
) {
    MF_DEBUG(endl << "INDEXING information source DIR: '" << directory << "' to memory DIR: '" << memoryPath << "'");
    DIR* dir;
    if((dir = opendir(directory.c_str()))) {
        const struct dirent *entry;
        if((entry = readdir(dir))) {
            string path;
            string* ppath;
            do {
                if(entry->d_type == DT_DIR) {
                    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                        continue;
                    }
                    MF_DEBUG(endl << "DIVE> " << directory.c_str() << "//" << entry->d_name);
                    path.assign(directory);
                    path += FILE_PATH_SEPARATOR;
                    path += entry->d_name;

                    indexDirectoryToMemory(path, memoryPath);
                } else {
                    MF_DEBUG(endl << "  FILE: " << directory.c_str() << "//" << entry->d_name);
                    ppath = new string{directory};
                    ppath->append(FILE_PATH_SEPARATOR);
                    ppath->append(entry->d_name);

                    if(RepositoryIndexer::fileHasPdfExtension(*ppath)) {
                        pdfs.insert(ppath);
                    }
                }
            } while ((entry = readdir(dir)) != 0);
            closedir(dir);
        }
    }
}

} // m8r namespace
