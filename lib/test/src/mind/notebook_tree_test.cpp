/*
 notebook_tree_test.cpp     MindForger test

 Copyright (C) 2016-2026 Martin Dvorak <martin.dvorak@mindforger.com>

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

#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "../test_utils.h"
#include "../../../src/model/notebook_tree.h"
#include "../../../src/mind/mind.h"
#include "../../../src/representations/markdown/markdown_configuration_representation.h"

using namespace std;

TEST(NotebookTreeTestCase, KeyGenerationIsUnique)
{
    // GIVEN
    set<string> existingKeys{};
    string mindPath{"/tmp/mf-unit-notebook-tree/mind"};

    // WHEN
    string key1 = m8r::NotebookTree::createNotebookTreeKey(existingKeys, mindPath, FILE_PATH_SEPARATOR);
    existingKeys.insert(key1);
    string key2 = m8r::NotebookTree::createNotebookTreeKey(existingKeys, mindPath, FILE_PATH_SEPARATOR);

    // THEN
    ASSERT_NE(key1, key2);
    EXPECT_NE(std::string::npos, key1.find(mindPath));
    EXPECT_NE(std::string::npos, key1.find(".md"));
}

TEST(NotebookTreeTestCase, RepositoryConfigurationAddRemoveGet)
{
    // GIVEN
    m8r::RepositoryConfiguration rc{};
    m8r::NotebookTree* t1 = new m8r::NotebookTree("Tree 1", "/tmp/mf-unit/mind/notebook-tree-1.md");
    m8r::NotebookTree* t2 = new m8r::NotebookTree("Tree 2", "/tmp/mf-unit/mind/notebook-tree-2.md");

    // WHEN
    rc.addNotebookTree(t1);
    rc.addNotebookTree(t2);

    // THEN
    ASSERT_EQ(2, rc.getNotebookTrees().size());
    rc.sortNotebookTrees();
    ASSERT_EQ(2, rc.getNotebookTrees().size());

    // WHEN: remove one
    rc.removeNotebookTree(t1);

    // THEN
    ASSERT_EQ(1, rc.getNotebookTrees().size());
    EXPECT_EQ("Tree 2", rc.getNotebookTrees()[0]->getName());
}

TEST(NotebookTreeTestCase, ParseSaveAndLoad)
{
    // GIVEN
    m8r::TestSandbox box{"", true};
    string mdFilename{"custom-repository-notebook-tree-single-file.md"};
    box.addMdFile(mdFilename);

    m8r::MarkdownConfigurationRepresentation configRepresentation{};
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& c = m8r::Configuration::getInstance();
    c.clear();

    // WHEN: save configuration file WITH notebook trees
    c.setConfigFilePath(box.configPath);
    m8r::Repository* r = new m8r::Repository{
        box.repositoryPath,
        m8r::Repository::RepositoryType::MINDFORGER,
        m8r::Repository::RepositoryMode::REPOSITORY,
        mdFilename
    };
    c.setActiveRepository(c.addRepository(r), repositoryConfigRepresentation);

    c.getRepositoryConfiguration().addNotebookTree(
        new m8r::NotebookTree("My Work Shelf", c.getMindPath()+FILE_PATH_SEPARATOR+"notebook-tree-1.md"));
    c.getRepositoryConfiguration().addNotebookTree(
        new m8r::NotebookTree("My Personal Shelf", c.getMindPath()+FILE_PATH_SEPARATOR+"notebook-tree-2.md"));

    configRepresentation.save(c);

    // THEN: assert serialized configuration
    ASSERT_TRUE(c.getRepositoryConfigFilePath().size());
    string* asString = m8r::fileToString(c.getRepositoryConfigFilePath());
    EXPECT_NE(std::string::npos, asString->find("Notebook Shelves"));
    EXPECT_NE(std::string::npos, asString->find("Notebook shelf name: My Work Shelf"));
    EXPECT_NE(std::string::npos, asString->find("Notebook shelf name: My Personal Shelf"));
    delete asString;

    // GIVEN load previously saved configuration
    c.setConfigFilePath(box.configPath);

    // WHEN: load previously saved configuration
    bool loaded = configRepresentation.load(c);

    // THEN: assert configuration from loaded file
    ASSERT_TRUE(loaded);
    ASSERT_EQ(2, c.getRepositoryConfiguration().getNotebookTrees().size());
    vector<string> names{};
    for(auto t:c.getRepositoryConfiguration().getNotebookTrees()) {
        names.push_back(t->getName());
    }
    EXPECT_NE(names.end(), std::find(names.begin(), names.end(), "My Work Shelf"));
    EXPECT_NE(names.end(), std::find(names.begin(), names.end(), "My Personal Shelf"));
}

TEST(NotebookTreeTestCase, MigrateLegacyNotebooksMap)
{
    // GIVEN a repository with a LEGACY outlines-map.md and NO registered Notebook trees
    m8r::TestSandbox box{"", true};
    string mdFilename{"custom-repository-legacy-map-single-file.md"};
    box.addMdFile(mdFilename);

    m8r::MarkdownConfigurationRepresentation configRepresentation{};
    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& c = m8r::Configuration::getInstance();
    c.clear();

    c.setConfigFilePath(box.configPath);
    m8r::Repository* r = new m8r::Repository{
        box.repositoryPath,
        m8r::Repository::RepositoryType::MINDFORGER,
        m8r::Repository::RepositoryMode::REPOSITORY,
        mdFilename
    };
    c.setActiveRepository(c.addRepository(r), repositoryConfigRepresentation);

    // NO notebook trees registered
    c.getRepositoryConfiguration().clearNotebookTrees();
    configRepresentation.save(c);

    // legacy outlines-map.md file exists on disk
    ASSERT_TRUE(c.getOutlinesMapPath().size());
    std::ofstream out(c.getOutlinesMapPath());
    out << "# Notebooks Map" << endl;
    out.close();

    // WHEN: repository configuration is (re)loaded
    bool loaded = repositoryConfigRepresentation.load(c);

    // THEN: exactly one Notebook tree was self-healed, pointing at the legacy file
    ASSERT_TRUE(loaded);
    ASSERT_EQ(1, c.getRepositoryConfiguration().getNotebookTrees().size());
    EXPECT_EQ(c.getOutlinesMapPath(), c.getRepositoryConfiguration().getNotebookTrees()[0]->getKey());
}

TEST(NotebookTreeTestCase, NewTreeStartsEmptyAndPersists)
{
    // GIVEN a MindForger repository with a Notebook
    string repositoryPath{"/tmp/mf-unit-notebook-tree-new"};
    m8r::removeDirectoryRecursively(repositoryPath.c_str());
    map<string,string> pathToContent{};
    m8r::createEmptyRepository(repositoryPath, pathToContent);

    m8r::Repository* repository = new m8r::Repository(
        repositoryPath,
        m8r::Repository::RepositoryType::MINDFORGER,
        m8r::Repository::RepositoryMode::REPOSITORY,
        "",
        false);

    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/mf-unit-notebook-tree-new-cfg.md");
    config.setActiveRepository(
        config.addRepository(repository), repositoryConfigRepresentation
    );

    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();

    // WHEN: a new, empty Notebook tree is created and persisted
    set<string> existingKeys{};
    string treeKey = m8r::NotebookTree::createNotebookTreeKey(
        existingKeys, config.getMindPath(), FILE_PATH_SEPARATOR);
    m8r::Outline* tree = mind.notebookTreeNew(treeKey, "My New Tree");
    mind.notebookTreeRemember(tree);

    // THEN: it is EMPTY by default - no automatic population of Notebooks
    EXPECT_EQ(0, tree->getNotes().size());
    EXPECT_TRUE(m8r::isFile(treeKey.c_str()));

    // WHEN: it's fetched again via the cache
    m8r::Outline* cachedTree = mind.notebookTreeGet(treeKey);

    // THEN: it's the SAME empty tree
    EXPECT_EQ(tree, cachedTree);
    EXPECT_EQ(0, cachedTree->getNotes().size());
}

TEST(NotebookTreeTestCase, AddOutlineAndForgetCascade)
{
    // GIVEN a MindForger repository with 2 Notebooks
    string repositoryPath{"/tmp/mf-unit-notebook-tree-cascade"};
    m8r::removeDirectoryRecursively(repositoryPath.c_str());
    map<string,string> pathToContent{};
    m8r::createEmptyRepository(repositoryPath, pathToContent);

    m8r::Repository* repository = new m8r::Repository(
        repositoryPath,
        m8r::Repository::RepositoryType::MINDFORGER,
        m8r::Repository::RepositoryMode::REPOSITORY,
        "",
        false);

    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/mf-unit-notebook-tree-cascade-cfg.md");
    config.setActiveRepository(
        config.addRepository(repository), repositoryConfigRepresentation
    );

    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();

    string outlineName{"Notebook to be organized"};
    string outlineKey = mind.outlineNew(&outlineName);
    m8r::Outline* o = mind.remind().getOutline(outlineKey);
    ASSERT_NE(nullptr, o);

    // WHEN: the Notebook is added to TWO different Notebook trees
    set<string> existingKeys{};
    string treeKeyA = m8r::NotebookTree::createNotebookTreeKey(
        existingKeys, config.getMindPath(), FILE_PATH_SEPARATOR);
    existingKeys.insert(treeKeyA);
    string treeKeyB = m8r::NotebookTree::createNotebookTreeKey(
        existingKeys, config.getMindPath(), FILE_PATH_SEPARATOR);

    m8r::Outline* treeA = mind.notebookTreeNew(treeKeyA, "Tree A");
    m8r::Outline* treeB = mind.notebookTreeNew(treeKeyB, "Tree B");
    mind.notebookTreeAddOutline(treeA, o);
    mind.notebookTreeAddOutline(treeB, o);
    mind.notebookTreeRemember(treeA);
    mind.notebookTreeRemember(treeB);

    ASSERT_EQ(1, treeA->getNotes().size());
    ASSERT_EQ(1, treeB->getNotes().size());

    // AND: both trees are registered in the repository configuration
    // (notebookTreeRemoveOutlineFromAll iterates the REGISTRY, not the cache)
    config.getRepositoryConfiguration().addNotebookTree(new m8r::NotebookTree("Tree A", treeKeyA));
    config.getRepositoryConfiguration().addNotebookTree(new m8r::NotebookTree("Tree B", treeKeyB));

    // WHEN: the Notebook is forgotten
    bool forgotten = mind.outlineForget(o->getKey());

    // THEN: it's removed from BOTH Notebook trees
    ASSERT_TRUE(forgotten);
    m8r::Outline* reloadedTreeA = mind.notebookTreeGet(treeKeyA);
    m8r::Outline* reloadedTreeB = mind.notebookTreeGet(treeKeyB);
    EXPECT_EQ(0, reloadedTreeA->getNotes().size());
    EXPECT_EQ(0, reloadedTreeB->getNotes().size());
}
