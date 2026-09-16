/*
 csv_test.cpp     MindForger test

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

#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "../../../src/gear/file_utils.h"
#include "../../../src/mind/mind.h"
#include "../../../src/install/installer.h"
#include "../../../src/representations/csv/csv_outline_representation.h"

using namespace std;

namespace {

/**
 * @brief Create repository w/ 2 Outlines and 4 Notes, learn it and return Mind.
 */
m8r::Mind* createCsvTestMind(const string& repositoryDir, const string& configPath)
{
    m8r::removeDirectoryRecursively(repositoryDir.c_str());
    m8r::Installer installer{};
    installer.createEmptyMindForgerRepository(repositoryDir);
    m8r::stringToFile(
        repositoryDir+"/memory/o1.md",
        "# Outline One <!-- Metadata: tags: ml,csv; -->"
        "\nO1 \"quoted\" description."
        "\n## N1 <!-- Metadata: tags: ml; -->"
        "\nT1."
        "\n### N2"
        "\nT2."
        "\n## N3"
        "\nT3."
        "\n");
    m8r::stringToFile(
        repositoryDir+"/memory/o2.md",
        "# Outline Two <!-- Metadata: tags: ml; -->"
        "\nO2."
        "\n## N4"
        "\nT4."
        "\n");

    m8r::MarkdownRepositoryConfigurationRepresentation repositoryConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.clear();
    config.setConfigFilePath(configPath);
    config.setActiveRepository(
        config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(repositoryDir)),
        repositoryConfigRepresentation
    );
    m8r::Mind* mind = new m8r::Mind{config};
    mind->learn();
    mind->think().get();
    return mind;
}

size_t countLines(const string& s)
{
    size_t lines{0};
    for(char c:s) {
        if(c == '\n') {
            lines++;
        }
    }
    return lines;
}

} // anonymous namespace

TEST(CsvTestCase, IncrementalExportEqualsBlockingExport)
{
    // GIVEN
    unique_ptr<m8r::Mind> mind{createCsvTestMind(
        "/tmp/mf-unit-repository-csv-incremental", "/tmp/cfg-csv-incremental.md")};
    string blockingCsv{"/tmp/mf-unit-csv-blocking.csv"};
    string incrementalCsv{"/tmp/mf-unit-csv-incremental.csv"};
    std::remove(blockingCsv.c_str());
    std::remove(incrementalCsv.c_str());
    map<const m8r::Tag*,int> tagsCardinality{};
    mind->getTagsCardinality(tagsCardinality);

    // WHEN
    bool blockingResult = mind->remind().exportToCsv(blockingCsv, tagsCardinality, 1);

    unique_ptr<m8r::CsvOutlinesExport> csvExport{
        mind->remind().createCsvExport(incrementalCsv, tagsCardinality, 1)};
    bool started = csvExport->start();
    vector<float> progress{};
    int steps{0};
    while(csvExport->step(1)) {
        progress.push_back(csvExport->getProgress());
        steps++;
    }

    // THEN
    unique_ptr<string> blocking{m8r::fileToString(blockingCsv)};
    unique_ptr<string> incremental{m8r::fileToString(incrementalCsv)};
    cout << "Incremental CSV export in " << steps << " steps:" << endl << *incremental;

    EXPECT_TRUE(blockingResult);
    EXPECT_TRUE(started);
    EXPECT_EQ(m8r::CsvOutlinesExport::Status::FINISHED, csvExport->getStatus());
    // 2 Os + 4 Ns
    EXPECT_EQ(6, csvExport->getTotalRows());
    EXPECT_EQ(6, csvExport->getExportedRows());
    // last step finishes the export
    EXPECT_EQ(5, steps);
    EXPECT_FLOAT_EQ(1.0f, csvExport->getProgress());
    for(size_t i = 1; i < progress.size(); i++) {
        EXPECT_LT(progress[i-1], progress[i]);
    }
    // header + 6 rows
    EXPECT_EQ(7, countLines(*incremental));
    EXPECT_EQ(*blocking, *incremental);
    // OHE tags columns (order of tags is not deterministic)
    string header = incremental->substr(0, incremental->find('\n'));
    EXPECT_NE(string::npos, header.find(",ml"));
    EXPECT_NE(string::npos, header.find(",csv"));
    // quotes escaping
    EXPECT_NE(string::npos, incremental->find("\"\"quoted\"\""));
}

TEST(CsvTestCase, CancelRemovesIncompleteFile)
{
    // GIVEN
    unique_ptr<m8r::Mind> mind{createCsvTestMind(
        "/tmp/mf-unit-repository-csv-cancel", "/tmp/cfg-csv-cancel.md")};
    string csv{"/tmp/mf-unit-csv-cancel.csv"};
    std::remove(csv.c_str());
    map<const m8r::Tag*,int> tagsCardinality{};
    unique_ptr<m8r::CsvOutlinesExport> csvExport{
        mind->remind().createCsvExport(csv, tagsCardinality, -1)};
    ASSERT_TRUE(csvExport->start());
    ASSERT_TRUE(csvExport->step(2));
    ASSERT_TRUE(m8r::isFile(csv.c_str()));

    // WHEN
    csvExport->cancel();

    // THEN
    cout << "Cancelled after " << csvExport->getExportedRows() << " rows" << endl;
    EXPECT_EQ(m8r::CsvOutlinesExport::Status::CANCELLED, csvExport->getStatus());
    EXPECT_EQ(2, csvExport->getExportedRows());
    EXPECT_FALSE(csvExport->step(1));
    EXPECT_FALSE(m8r::isFile(csv.c_str()));
}

TEST(CsvTestCase, DestroyingRunningExportRemovesIncompleteFile)
{
    // GIVEN
    unique_ptr<m8r::Mind> mind{createCsvTestMind(
        "/tmp/mf-unit-repository-csv-destroy", "/tmp/cfg-csv-destroy.md")};
    string csv{"/tmp/mf-unit-csv-destroy.csv"};
    std::remove(csv.c_str());
    map<const m8r::Tag*,int> tagsCardinality{};
    m8r::CsvOutlinesExport* csvExport
        = mind->remind().createCsvExport(csv, tagsCardinality, -1);
    ASSERT_TRUE(csvExport->start());
    ASSERT_TRUE(csvExport->step(1));

    // WHEN
    delete csvExport;

    // THEN
    EXPECT_FALSE(m8r::isFile(csv.c_str()));
}

TEST(CsvTestCase, ExportToInvalidPathFails)
{
    // GIVEN
    vector<m8r::Outline*> outlines{};
    map<const m8r::Tag*,int> tagsCardinality{};
    m8r::CsvOutlinesExport invalidPathExport{
        outlines, tagsCardinality, "/tmp/mf-unit-csv-nonexistent-dir/export.csv", -1};
    m8r::CsvOutlinesExport emptyPathExport{outlines, tagsCardinality, "", -1};

    // WHEN
    bool invalidPathStarted = invalidPathExport.start();
    bool emptyPathStarted = emptyPathExport.start();

    // THEN
    EXPECT_FALSE(invalidPathStarted);
    EXPECT_EQ(m8r::CsvOutlinesExport::Status::FAILED, invalidPathExport.getStatus());
    EXPECT_FALSE(invalidPathExport.step(1));
    EXPECT_FALSE(emptyPathStarted);
    EXPECT_EQ(m8r::CsvOutlinesExport::Status::FAILED, emptyPathExport.getStatus());
}

TEST(CsvTestCase, ExportOfEmptyMemoryWritesHeaderOnly)
{
    // GIVEN
    string csv{"/tmp/mf-unit-csv-empty.csv"};
    std::remove(csv.c_str());
    vector<m8r::Outline*> outlines{};
    map<const m8r::Tag*,int> tagsCardinality{};
    m8r::CsvOutlinesExport csvExport{outlines, tagsCardinality, csv, -1};

    // WHEN
    bool started = csvExport.start();

    // THEN
    unique_ptr<string> content{m8r::fileToString(csv)};
    EXPECT_TRUE(started);
    EXPECT_EQ(m8r::CsvOutlinesExport::Status::FINISHED, csvExport.getStatus());
    EXPECT_FALSE(csvExport.step(1));
    EXPECT_EQ(0, csvExport.getTotalRows());
    EXPECT_FLOAT_EQ(1.0f, csvExport.getProgress());
    EXPECT_EQ(
        "id,type,title,offset,depth,reads,writes,created,modified,read,description\n",
        *content);
}
