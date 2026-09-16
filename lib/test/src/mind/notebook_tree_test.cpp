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

#include <map>
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

TEST(NotebookTreeTestCase, KeyRelativePathRoundTrip)
{
    // GIVEN an absolute key living under the repository root
    string repositoryDir{"/home/dvorka/mf-devel/library-trainer"};
    string absoluteKey{
        repositoryDir + FILE_PATH_SEPARATOR
        + "mind" + FILE_PATH_SEPARATOR
        + "notebook-tree-1789452772.md"};

    // WHEN converted to a portable, repository-root-relative path
    string relativeKey = m8r::NotebookTree::notebookTreeKeyToRelativePath(
        absoluteKey, repositoryDir, FILE_PATH_SEPARATOR);

    // THEN it neither contains the repository root nor starts w/ a separator
    EXPECT_EQ(
        string{"mind"} + FILE_PATH_SEPARATOR + "notebook-tree-1789452772.md",
        relativeKey);
    EXPECT_EQ(std::string::npos, relativeKey.find(repositoryDir));

    // WHEN resolved back to the absolute key used at runtime
    string resolvedKey = m8r::NotebookTree::resolveNotebookTreeKey(
        relativeKey, repositoryDir, FILE_PATH_SEPARATOR);

    // THEN it round-trips to the exact original absolute key
    EXPECT_EQ(absoluteKey, resolvedKey);
}

TEST(NotebookTreeTestCase, ResolveKeyBackwardCompatibleWithAbsolutePaths)
{
    // GIVEN a key persisted by an older MindForger version as an
    // absolute path (as found e.g. in a pre-existing repository)
    string legacyAbsoluteKey{
        "/home/dvorka/mf-devel/library-trainer/mind/notebook-tree-1789452772.md"};

    // WHEN resolved against (any) repository root
    string resolvedKey = m8r::NotebookTree::resolveNotebookTreeKey(
        legacyAbsoluteKey, "/home/dvorka/mf-devel/library-trainer", FILE_PATH_SEPARATOR);

    // THEN the absolute value is detected and returned UNCHANGED
    EXPECT_EQ(legacyAbsoluteKey, resolvedKey);
}

TEST(NotebookTreeTestCase, RelativePathFallsBackWhenKeyNotUnderRepository)
{
    // GIVEN a key which (unexpectedly) does NOT live under the repository root
    string key{"/some/other/place/notebook-tree-1.md"};
    string repositoryDir{"/home/dvorka/mf/my-repo"};

    // WHEN converted to a repository-relative path
    string relativeKey = m8r::NotebookTree::notebookTreeKeyToRelativePath(
        key, repositoryDir, FILE_PATH_SEPARATOR);

    // THEN it's returned UNCHANGED rather than producing an unresolvable path
    EXPECT_EQ(key, relativeKey);
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

    // WHEN: remove one
    rc.removeNotebookTree(t1);

    // THEN
    ASSERT_EQ(1, rc.getNotebookTrees().size());
    EXPECT_EQ("Tree 2", rc.getNotebookTrees()[0]->getName());
}

TEST(NotebookTreeTestCase, TouchMovesTreeToFrontOfTheList)
{
    // GIVEN 3 Notebook trees, added in order 1, 2, 3
    m8r::RepositoryConfiguration rc{};
    m8r::NotebookTree* t1 = new m8r::NotebookTree("Tree 1", "/tmp/mf-unit/mind/notebook-tree-1.md");
    m8r::NotebookTree* t2 = new m8r::NotebookTree("Tree 2", "/tmp/mf-unit/mind/notebook-tree-2.md");
    m8r::NotebookTree* t3 = new m8r::NotebookTree("Tree 3", "/tmp/mf-unit/mind/notebook-tree-3.md");
    rc.addNotebookTree(t1);
    rc.addNotebookTree(t2);
    rc.addNotebookTree(t3);
    ASSERT_EQ(3, rc.getNotebookTrees().size());

    // WHEN: the FIRST (oldest) one is touched - e.g. it was just opened
    rc.touchNotebookTree(t1);

    // THEN: it moves to the FRONT, the other 2 keep their relative order
    ASSERT_EQ(3, rc.getNotebookTrees().size());
    EXPECT_EQ("Tree 1", rc.getNotebookTrees()[0]->getName());
    EXPECT_EQ("Tree 2", rc.getNotebookTrees()[1]->getName());
    EXPECT_EQ("Tree 3", rc.getNotebookTrees()[2]->getName());

    // WHEN: the (now) LAST one is touched too
    rc.touchNotebookTree(t3);

    // THEN: it becomes first, the previously-touched Tree 1 comes 2nd
    EXPECT_EQ("Tree 3", rc.getNotebookTrees()[0]->getName());
    EXPECT_EQ("Tree 1", rc.getNotebookTrees()[1]->getName());
    EXPECT_EQ("Tree 2", rc.getNotebookTrees()[2]->getName());
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
    // AND: Key is persisted as a PORTABLE, repository-root-relative path -
    // the repository's absolute location must NOT leak into the file
    EXPECT_NE(
        std::string::npos,
        asString->find(string{"* Key: mind"} + FILE_PATH_SEPARATOR + "notebook-tree-1.md"));
    EXPECT_EQ(std::string::npos, asString->find(box.repositoryPath));
    delete asString;

    // GIVEN load previously saved configuration
    c.setConfigFilePath(box.configPath);

    // WHEN: load previously saved configuration
    bool loaded = configRepresentation.load(c);

    // THEN: assert configuration from loaded file
    ASSERT_TRUE(loaded);
    ASSERT_EQ(2, c.getRepositoryConfiguration().getNotebookTrees().size());
    vector<string> names{};
    map<string,string> nameToKey{};
    for(auto t:c.getRepositoryConfiguration().getNotebookTrees()) {
        names.push_back(t->getName());
        nameToKey[t->getName()] = t->getKey();
    }
    EXPECT_NE(names.end(), std::find(names.begin(), names.end(), "My Work Shelf"));
    EXPECT_NE(names.end(), std::find(names.begin(), names.end(), "My Personal Shelf"));
    // AND: relative Key resolved back to the EXACT original absolute key
    EXPECT_EQ(
        c.getMindPath()+FILE_PATH_SEPARATOR+"notebook-tree-1.md",
        nameToKey["My Work Shelf"]);
    EXPECT_EQ(
        c.getMindPath()+FILE_PATH_SEPARATOR+"notebook-tree-2.md",
        nameToKey["My Personal Shelf"]);
}

TEST(NotebookTreeTestCase, OrderPersistsAcrossSaveAndLoad)
{
    // GIVEN 2 Notebook trees, w/ "My Work Shelf" touched (e.g. opened)
    // AFTER "My Personal Shelf" was added, so it's the front one
    m8r::TestSandbox box{"", true};
    string mdFilename{"custom-repository-notebook-tree-order.md"};
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

    m8r::NotebookTree* personal = new m8r::NotebookTree(
        "My Personal Shelf", c.getMindPath()+FILE_PATH_SEPARATOR+"notebook-tree-1.md");
    c.getRepositoryConfiguration().addNotebookTree(personal);

    m8r::NotebookTree* work = new m8r::NotebookTree(
        "My Work Shelf", c.getMindPath()+FILE_PATH_SEPARATOR+"notebook-tree-2.md");
    c.getRepositoryConfiguration().addNotebookTree(work);

    c.getRepositoryConfiguration().touchNotebookTree(work);
    ASSERT_EQ("My Work Shelf", c.getRepositoryConfiguration().getNotebookTrees()[0]->getName());

    // WHEN: saved and (freshly) reloaded, exactly as happens across an
    // application restart
    configRepresentation.save(c);
    c.setConfigFilePath(box.configPath);
    bool loaded = configRepresentation.load(c);

    // THEN: the order (NOT any timestamp) round-tripped exactly, w/o
    // any separate sort/field needed
    ASSERT_TRUE(loaded);
    ASSERT_EQ(2, c.getRepositoryConfiguration().getNotebookTrees().size());
    EXPECT_EQ("My Work Shelf", c.getRepositoryConfiguration().getNotebookTrees()[0]->getName());
    EXPECT_EQ("My Personal Shelf", c.getRepositoryConfiguration().getNotebookTrees()[1]->getName());
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

TEST(NotebookTreeTestCase, RememberSelfHealsMissingMindDirectory)
{
    // GIVEN a MindForger repository whose mind/ directory is MISSING -
    // this reproduces a pre-existing repository created before Notebook
    // Trees/Shelves existed (mind/ is otherwise only created when a NEW
    // repository is created, see Installer::createEmptyMindForgerRepository)
    string repositoryPath{"/tmp/mf-unit-notebook-tree-no-mind-dir"};
    m8r::removeDirectoryRecursively(repositoryPath.c_str());
    map<string,string> pathToContent{};
    m8r::createEmptyRepository(repositoryPath, pathToContent);

    string mindDir{repositoryPath + FILE_PATH_SEPARATOR + "mind"};
    ASSERT_TRUE(m8r::isDirectoryOrFileExists(mindDir.c_str()));
    m8r::removeDirectoryRecursively(mindDir.c_str());
    ASSERT_FALSE(m8r::isDirectoryOrFileExists(mindDir.c_str()));

    m8r::Repository* repository = new m8r::Repository(
        repositoryPath,
        m8r::Repository::RepositoryType::MINDFORGER,
        m8r::Repository::RepositoryMode::REPOSITORY,
        "",
        false);

    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath("/tmp/mf-unit-notebook-tree-no-mind-dir-cfg.md");
    config.setActiveRepository(
        config.addRepository(repository), repositoryConfigRepresentation
    );

    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();

    // WHEN: a new Notebook tree/shelf is created and persisted, exactly
    // as done by the "Add Notebook Tree" UI action
    set<string> existingKeys{};
    string treeKey = m8r::NotebookTree::createNotebookTreeKey(
        existingKeys, config.getMindPath(), FILE_PATH_SEPARATOR);
    m8r::Outline* tree = mind.notebookTreeNew(treeKey, "Self-Healed Tree");
    mind.notebookTreeRemember(tree);

    // THEN: the missing mind/ directory is (re)created and the tree is
    // ACTUALLY persisted to disk - w/o the fix this silently no-ops,
    // leaving repository-configuration.md referencing a non-existent file
    EXPECT_TRUE(m8r::isDirectoryOrFileExists(mindDir.c_str()));
    EXPECT_TRUE(m8r::isFile(treeKey.c_str()));
}

TEST(NotebookTreeTestCase, ForgetMovesBackingFileToLimboAndEvictsCache)
{
    // GIVEN a MindForger repository with a Notebook tree/shelf
    string repositoryPath{"/tmp/mf-unit-notebook-tree-forget"};
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
    config.setConfigFilePath("/tmp/mf-unit-notebook-tree-forget-cfg.md");
    config.setActiveRepository(
        config.addRepository(repository), repositoryConfigRepresentation
    );

    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();

    set<string> existingKeys{};
    string treeKey = m8r::NotebookTree::createNotebookTreeKey(
        existingKeys, config.getMindPath(), FILE_PATH_SEPARATOR);
    m8r::Outline* tree = mind.notebookTreeNew(treeKey, "ForgottenTree");
    mind.notebookTreeRemember(tree);
    ASSERT_TRUE(m8r::isFile(treeKey.c_str()));

    string expectedLimboKey{
        config.getLimboPath() + FILE_PATH_SEPARATOR + "ForgottenTree.md"};
    ASSERT_FALSE(m8r::isFile(expectedLimboKey.c_str()));

    // WHEN: the Notebook tree is forgotten/deleted, exactly as done by
    // the "Delete Notebook Tree" UI action
    bool forgotten = mind.notebookTreeForget(treeKey);

    // THEN: its backing file is MOVED to Limbo - never left orphaned in
    // mind/, unlike a plain registry-only removal
    ASSERT_TRUE(forgotten);
    EXPECT_FALSE(m8r::isFile(treeKey.c_str()));
    EXPECT_TRUE(m8r::isFile(expectedLimboKey.c_str()));

    // AND: the cache no longer holds the forgotten tree - fetching the
    // SAME key again self-heals with a brand-new EMPTY tree rather than
    // returning stale, cached content (which would prove a leftover
    // cache entry, and risk a later create silently reusing this exact
    // key while it still resolved to the old, forgotten instance)
    m8r::Outline* reGotten = mind.notebookTreeGet(treeKey);
    EXPECT_EQ("Notebook Tree", reGotten->getName());
    EXPECT_EQ(0, reGotten->getNotes().size());
    EXPECT_TRUE(m8r::isFile(treeKey.c_str()));
}

TEST(NotebookTreeTestCase, StaleEntriesCleanupSurvivesStaleParentAndChild)
{
    // GIVEN a MindForger repository and a Notebook tree with 2 NESTED
    // entries - a parent (depth 0) and its child (depth 1) - BOTH
    // stale, i.e. linking to Notebooks that no longer exist
    string repositoryPath{"/tmp/mf-unit-notebook-tree-nested-stale"};
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
    config.setConfigFilePath("/tmp/mf-unit-notebook-tree-nested-stale-cfg.md");
    config.setActiveRepository(
        config.addRepository(repository), repositoryConfigRepresentation
    );

    m8r::Mind mind(config);
    mind.learn();
    mind.think().get();

    // 2 Notebooks, immediately forgotten so their keys resolve to nothing -
    // NOT registered in any tree via notebookTreeAddOutline(), so forgetting
    // them does not itself touch the tree crafted below
    string parentOutlineName{"Parent Notebook"};
    string parentOutlineKey = mind.outlineNew(&parentOutlineName);
    string childOutlineName{"Child Notebook"};
    string childOutlineKey = mind.outlineNew(&childOutlineName);
    ASSERT_TRUE(mind.outlineForget(parentOutlineKey));
    ASSERT_TRUE(mind.outlineForget(childOutlineKey));

    set<string> existingKeys{};
    string treeKey = m8r::NotebookTree::createNotebookTreeKey(
        existingKeys, config.getMindPath(), FILE_PATH_SEPARATOR);
    m8r::Outline* tree = mind.notebookTreeNew(treeKey, "Nested Stale Tree");

    m8r::Note* parentNote = new m8r::Note(*tree->getOutlineDescriptorAsNote());
    parentNote->setName("Stale Parent");
    parentNote->clearLinks();
    parentNote->addLink(new m8r::Link{m8r::LINK_NAME_OUTLINE_KEY, parentOutlineKey});
    parentNote->setDepth(0);
    tree->addNote(parentNote);

    m8r::Note* childNote = new m8r::Note(*tree->getOutlineDescriptorAsNote());
    childNote->setName("Stale Child");
    childNote->clearLinks();
    childNote->addLink(new m8r::Link{m8r::LINK_NAME_OUTLINE_KEY, childOutlineKey});
    childNote->setDepth(1);
    tree->addNote(childNote);

    ASSERT_EQ(2, tree->getNotes().size());

    // WHEN: the tree is fetched - notebookTreeRemoveStaleEntries() runs
    // and (in document order) tries to forgetNote() the stale PARENT
    // first, which deallocates its whole subtree, INCLUDING the stale
    // child also queued for removal
    m8r::Outline* reGotten = mind.notebookTreeGet(treeKey);

    // THEN: no crash / no use-after-free, and BOTH stale entries are gone
    ASSERT_EQ(tree, reGotten);
    EXPECT_EQ(0, reGotten->getNotes().size());
}
