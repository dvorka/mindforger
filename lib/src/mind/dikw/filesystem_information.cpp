/*
 filesystem_information.cpp     MindForger thinking notebook

 Copyright (C) 2016-2025 Martin Dvorak <martin.dvorak@mindforger.com>

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
using namespace m8r::filesystem;

namespace m8r {

constexpr const auto META_SRC_PATH_PREFIX = "Source directory: ";

std::vector<FilesystemInformationSource*> FilesystemInformationSource::findInformationSources(
    Configuration &config,
    Mind& mind,
    MarkdownDocumentRepresentation& mdDocumentRepresentation
) {
    vector<FilesystemInformationSource*> infoSrc;

    if(config.getActiveRepository()->getType() != Repository::RepositoryType::MINDFORGER
       || config.getActiveRepository()->getMode() != Repository::RepositoryMode::REPOSITORY
    ) {
        MF_DEBUG(
            "Error: filesystem information resource cannot be indexed as "
            "active directory is not of MINDFORGER/REPOSITORY type");
        return infoSrc;
    }

    string libraryPath{
        config.getMemoryPath()+FILE_PATH_SEPARATOR+DIR_MEMORY_M1ndF0rg3rL1br8ry
    };
    if(!isDirectory(libraryPath.c_str())) {
        MF_DEBUG(
            "Error: memory's library path '" << libraryPath << "' does not exist");
        return infoSrc;
    }

    MF_DEBUG(
        endl <<
        "SEARCHING existing information sources in memory's library path: '" <<
        libraryPath << "'");
    DIR* dir;
    if((dir = opendir(libraryPath.c_str()))) {
        const struct dirent *entry;
        if((entry = readdir(dir))) {
            string path;
            do {
                if(entry->d_type == DT_DIR) {
                    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                        continue;
                    }
                    MF_DEBUG(
                        endl << "LIBRARY SOURCE> " << libraryPath.c_str() << "/" << entry->d_name);
                    path.assign(libraryPath);
                    path += FILE_PATH_SEPARATOR;
                    path += entry->d_name;

                    string metaPath{
                        path+FILE_PATH_SEPARATOR+FILE_META_M1ndF0rg3rL1br8ryM3t8
                    };
                    MF_DEBUG(endl << "  Metadata file path: '" << metaPath << "'");
                    if(!isFile(metaPath.c_str())) {
                        continue;
                    }

                    vector<string*> metaLines{};
                    size_t fileSize{};
                    fileToLines(&metaPath, metaLines, fileSize);

                    if(metaLines.size() >= 2 && metaLines[2]) {
                        string* line=metaLines[2];
                        string libSrcPath = line->substr(strlen(META_SRC_PATH_PREFIX));
                        MF_DEBUG(
                            endl << "    Library source dir: '" << libSrcPath << "'");

                        if(!isDirectory(libSrcPath.c_str())) {
                            continue;
                        }

                        MF_DEBUG(endl << "      VALID");
                        FilesystemInformationSource* src = new FilesystemInformationSource{
                            libSrcPath,
                            mind,
                            mdDocumentRepresentation,
                        };
                        src->setMfPath(path);
                        infoSrc.push_back(src);
                    }
                }
            } while ((entry = readdir(dir)) != 0);
            closedir(dir);
        }
    }

    return infoSrc;
}

FilesystemInformationSource::FilesystemInformationSource(
    string& sourcePath,
    Mind& mind,
    MarkdownDocumentRepresentation& mdDocumentRepresentation
)
    : InformationSource{SourceType::FILESYSTEM, sourcePath},
      mind{mind},
      mdDocumentRepresentation{mdDocumentRepresentation},
      mfPath{}
{
}

FilesystemInformationSource::~FilesystemInformationSource()
{
    for(auto p:this->txts) {
        delete p;
    }
    for(auto p:this->pdfs_paths) {
        delete p;
    }
    for(auto p:this->htmls) {
        delete p;
    }

    for(auto p:this->msOfficeWords) {
        delete p;
    }
    for(auto p:this->msOfficeExcel) {
        delete p;
    }
    for(auto p:this->msOfficePowerpoint) {
        delete p;
    }

    for(auto p:this->openOfficeWords) {
        delete p;
    }
    for(auto p:this->openOfficeExcel) {
        delete p;
    }
    for(auto p:this->openOfficePowerpoint) {
        delete p;
    }
}

FilesystemInformationSource::ErrorCode FilesystemInformationSource::indexToMemory(
    Repository& repository, bool synchronize
) {
    MF_DEBUG("Indexing LIBRARY documents to memory:" << endl);

    if(!isDirectory(locator.c_str())) {
        MF_DEBUG(
            "Error: filesystem information resource cannot be indexed to memory "
            "as its locator path '" << locator << "' does not exist");
        return FilesystemInformationSource::ErrorCode::INVALID_LOCATOR;
    }

    if(repository.getType() != Repository::RepositoryType::MINDFORGER
       || repository.getMode() != Repository::RepositoryMode::REPOSITORY
    ) {
        MF_DEBUG(
            "Error: filesystem information resource cannot be indexed as "
            "active directory is not of MINDFORGER/REPOSITORY type");
        return FilesystemInformationSource::ErrorCode::NOT_MINDFORGER_REPOSITORY;
    }

    string memoryPath{repository.getDir()+FILE_PATH_SEPARATOR+DIRNAME_MEMORY};
    if(!isDirectory(memoryPath.c_str())) {
        MF_DEBUG(
            "Error: filesystem information resource cannot be indexed to "
            "memory path '" << memoryPath << "' as this directory does not exist");
        return FilesystemInformationSource::ErrorCode::INVALID_MEMORY_PATH;
    }

    string memoryLibIndexPath{memoryPath};
    memoryLibIndexPath += FILE_PATH_SEPARATOR;
    memoryLibIndexPath += FilesystemInformationSource::DIR_MEMORY_M1ndF0rg3rL1br8ry;
    if(!isDirectory(memoryLibIndexPath.c_str())) {
        createDirectory(memoryLibIndexPath);
    }

    string memoryInformationSourceIndexPath{memoryLibIndexPath};
    memoryInformationSourceIndexPath += FILE_PATH_SEPARATOR;
    memoryInformationSourceIndexPath += normalizeToNcName(this->locator, '_');
    MF_DEBUG("  Library path in memory: " << memoryInformationSourceIndexPath << endl);
    if(!synchronize && isDirectory(memoryInformationSourceIndexPath.c_str())) {
        return ErrorCode::LIBRARY_ALREADY_EXISTS;
    } else {
        createDirectory(memoryInformationSourceIndexPath);
    }

    indexDirectoryToMemory(locator, memoryPath);

    string outlinePathInMemory{};
    string relativeDocLibPath{};
    string outlineDir{};
    string outlineFilename{};
    for(auto pdf_path:this->pdfs_paths) {
        MF_DEBUG("  " << *pdf_path << endl);
        relativeDocLibPath.assign(pdf_path->substr(locator.size()+1));
        MF_DEBUG("    " << relativeDocLibPath << endl);

        outlinePathInMemory.assign(memoryInformationSourceIndexPath);
        outlinePathInMemory += FILE_PATH_SEPARATOR;
        outlinePathInMemory += relativeDocLibPath;
        outlinePathInMemory += File::EXTENSION_MD_MD;
        MF_DEBUG("      " << outlinePathInMemory << endl);

        pathToDirectoryAndFile(outlinePathInMemory, outlineDir, outlineFilename);
        if(outlineDir.size() && !isDirectory(outlineDir.c_str())) {
            MF_DEBUG("      creating dir including parent dirs: " << outlinePathInMemory << endl);
            createDirectories(outlineDir);
        }

        if(!isFile(outlinePathInMemory.c_str())) {
            Outline* o=mdDocumentRepresentation.to(*pdf_path, outlinePathInMemory);
            mind.outlineNew(o);
        } else {
            MF_DEBUG("      SKIPPING creation of O as it already EXISTS");
        }
    }

    string metaPath{
        memoryInformationSourceIndexPath
        + FILE_PATH_SEPARATOR
        + FILE_META_M1ndF0rg3rL1br8ryM3t8
    };
    saveMetadata(metaPath, locator);

    return ErrorCode::SUCCESS;
}

void FilesystemInformationSource::indexDirectoryToMemory(
    const string& directory,
    const string& memoryPath
) {
    MF_DEBUG(
        endl <<
        "INDEXING information source DIR: '" <<
        directory << "' to memory DIR: '" << memoryPath << "'");
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
                    MF_DEBUG(
                        endl << "DIVE> " << directory.c_str() << "//" << entry->d_name);
                    path.assign(directory);
                    path += FILE_PATH_SEPARATOR;
                    path += entry->d_name;

                    indexDirectoryToMemory(path, memoryPath);
                } else {
                    MF_DEBUG(
                        endl << "  FILE: " << directory.c_str() << "//" << entry->d_name);
                    ppath = new string{directory};
                    ppath->append(FILE_PATH_SEPARATOR);
                    ppath->append(entry->d_name);

                    if(File::fileHasPdfExtension(*ppath)) {
                        pdfs_paths.insert(ppath);
                    } else {
                        delete ppath;
                        ppath = nullptr;
                    }
                }
            } while ((entry = readdir(dir)) != 0);
            closedir(dir);
        }
    }
}

void FilesystemInformationSource::saveMetadata(
    string& metaPath,
    string& librarySrcPath
) {
    std::ofstream out(metaPath);
    out << "# MindForger Library Metadata" << endl
    << "Library name: My library" << endl
    << META_SRC_PATH_PREFIX << librarySrcPath << endl;
    out.close();

}

} // m8r namespace
