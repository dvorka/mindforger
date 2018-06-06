/*
 main.cpp     MindForger thinking notebook

 Copyright (C) 2016-2018 Martin Dvorak <martin.dvorak@mindforger.com>

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
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include <QtWidgets>

#include "../../lib/src/version.h"
#include "../../lib/src/representations/markdown/markdown_configuration_representation.h"

#include "gear/qutils.h"
#include "i18nl10n.h"
#include "main_window_view.h"
#include "main_window_presenter.h"
#include "look_n_feel.h"

using namespace std;

/**
 * @brief MindForger command line interface.
 */

/* MindForger command line interface description.
 *
 * GUI:
 *
 * $ mindforger
 *   ... lookup repository as follows
 *     1. configured in ~/.mindforger,
 *     2. specified by environment variable MINDFORGER_REPOSITORY,
 *     3. check existence of MindForger repository in default location i.e. ~/mindforger-repository
 *     4. create new MindForger repository in default location i.e. ~/mindforger-repository
 * $ mindforger ~/my-mf-repository
 *   ... MindForger repository
 * $ mindforger ~/books/marathon-training
 *   ... directory structure w/ Markdowns
 * $ mindforger ~/my-mf-repository/memory/plans.md
 *   ... Markdown-based MindForger DSL file
 * $ mindforger ~/books/marathon-training/07-lsd.md
 *   ... Markdown file
 *
 *
 *
 * Options proposal:
 *
 * $ mindforger --theme dark
 *   -t
 * $ mindforger --config forget=25%
 * $ mindforger --config editor-show-syntax-highlighting=true
 *   -c
 * $ mindforger --generate-toc my.md
 *   -T
 * $ mindforger --strip-metadata my.md
 *   -S
 * $ mindforger --shell
 *   -s
 * $ mindforger --version
 *   -V
 * $ mindforger --help
 *   -h
 *
 *
 *
 * Terminal CLI commands proposal:
 *
 * $ mindforger --command LIST outlines
 * $ mindforger -C LIST outlines
 *
 * $ mindforger -C "LIST outlines"
 * $ mindforger -C "FIND outline 'abc'"
 * $ mindforger -C "FIND outline 'a''c'"
 * $ mindforger -C "VIEW outline 'abc'"
 * $ mindforger -C "EDIT outline 'abc'"
 * $ mindforger -C "LIST associations OF outline 'abc'"
 *
 * $ mindforger -C "LIST notes"
 * $ mindforger -C "FIND note 'abc'"
 * $ mindforger -C "VIEW note 'abc'.'efg'"
 * $ mindforger -C "EDIT note 'abc'.'efg'"
 * $ mindforger -C "LIST notes OF outline 'abc'"
 * $ mindforger -C "LIST associations OF note 'abc'.'efg'"
 *
 * $ mindforger -C "FTS 'expr'"
 * $ mindforger -C "FTS 'expr' SCOPE outline 'abc'"
 * $ mindforger -C "FTS 'expr' SCOPE note 'abc'.'efg'"
 */
int main(int argc, char *argv[])
{
    // check whether running in GUI (and not in text console tty)
    char *term = getenv(m8r::ENV_VAR_TERM);
    if(term) {
        if(!strcmp("linux", term)) {
            cerr << endl << QCoreApplication::translate("main", "MindForger CANNOT be run from text console - run it from a terminal in GUI.").toUtf8().constData()
                 << endl;
            exit(1);
        }
    }

    QApplication mindforgerApplication(argc, argv);
#ifdef MF_DEBUG_QRC
    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        MF_DEBUG(it.next() << endl);
    }
#endif
    QApplication::setApplicationName("MindForger");
    QApplication::setApplicationVersion(MINDFORGER_VERSION);
    mindforgerApplication.setWindowIcon(QIcon(":/icons/mindforger-icon.png"));
    m8r::l10n(mindforgerApplication);

    std::string useRepository{};
    QString themeOptionValue{};
    QString configurationFilePath{};
    if(argc > 1) {
        QCommandLineParser parser;
        // process command line as parameters/options are present
        parser.setApplicationDescription("Thinking notebook.");
        parser.addPositionalArgument("[<directory>|<file>]", QCoreApplication::translate("main", "MindForger repository or directory/file with Markdown(s) to open"));
        QCommandLineOption themeOption(QStringList() << "t" << "theme",
                QCoreApplication::translate("main", "Use 'dark', 'light' or other GUI <theme>."),
                QCoreApplication::translate("main", "theme"));
        parser.addOption(themeOption);
        QCommandLineOption configPathOption(QStringList() << "c" << "config-file-path",
                QCoreApplication::translate("main", "Load configuration from given <file>."),
                QCoreApplication::translate("main", "file"));
        parser.addOption(configPathOption);
//        QCommandLineOption generateTocOption(QStringList() << "T" << "generate-toc",
//                QCoreApplication::translate("main", "Generate table of contents for <source> Markdown file."),
//                QCoreApplication::translate("main", "source"));
//        parser.addOption(generateTocOption);
        QCommandLineOption versionOption=parser.addVersionOption();
        QCommandLineOption helpOption=parser.addHelpOption();
        // process the actual command line arguments given by the user
        parser.process(mindforgerApplication);

        if(parser.isSet(helpOption) || parser.isSet(versionOption)) {
            return 0;
        }

        QStringList arguments = parser.positionalArguments();

        if(arguments.size()==1) {
            useRepository.assign(arguments[0].toStdString());
        } else if(arguments.size()>1) {
            // TODO i18n
            cerr << "Error: Too many arguments (" << dec << arguments.size() << ") - at most one directory or file can be specified" << endl;
            return 1;
        }

        if(parser.isSet(themeOption)) {
            themeOptionValue = parser.value(themeOption);
        }

        if(parser.isSet(configPathOption)) {
            configurationFilePath = parser.value(configPathOption);
        }
    }
    // else there are no parameters and options > simply load GUI

    // load configuration
    m8r::MarkdownConfigurationRepresentation mdConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    if(configurationFilePath.size()) {
        config.setConfigFilePath(configurationFilePath.toStdString());
    }
    if(!mdConfigRepresentation.load(config)) {
        mdConfigRepresentation.save(m8r::File{config.getConfigFilePath()});
    }

    m8r::initRandomizer();

    if(!useRepository.empty()) {
        m8r::Repository* r = m8r::RepositoryIndexer::getRepositoryForPath(useRepository);
        if(r) {
            config.setActiveRepository(config.addRepository(r));
        } else {
            if(config.createEmptyMarkdownFile(useRepository)) {
                r = m8r::RepositoryIndexer::getRepositoryForPath(useRepository);
                config.setActiveRepository(config.addRepository(r));
            } else {
                cerr << QCoreApplication::translate("main", "Error: Unable to find given repository/file to open - open MindForger without parameters and create it from menu Mind/New: '").toUtf8().constData()
                     << useRepository
                     << "'"
                     << endl;
                exit(1);
            }
        }
    } else {
        config.findOrCreateDefaultRepository();
    }

    // choose L&F
    m8r::LookAndFeels& lookAndFeels = m8r::LookAndFeels::getInstance();
    lookAndFeels.init(&mindforgerApplication);
    lookAndFeels.setFontPointSize(config.getUiFontPointSize());
    if(!themeOptionValue.isEmpty()) {
        if(lookAndFeels.isThemeNameValid(themeOptionValue)) {
            lookAndFeels.setTheme(themeOptionValue);
        } else {
            cerr << QCoreApplication::translate("main", "Ignoring unknown GUI theme: '").toUtf8().constData()
                 << themeOptionValue.toUtf8().constData()
                 << "'\n";
            lookAndFeels.setTheme(QString::fromStdString(config.getUiThemeName()));
        }
    } else {
        lookAndFeels.setTheme(QString::fromStdString(config.getUiThemeName()));
    }

    // initialize and start UI
    m8r::MainWindowView mainWindowView(lookAndFeels);
    m8r::MainWindowPresenter mainWindowPresenter(mainWindowView);
    mainWindowView.showMaximized();
    mindforgerApplication.font().setPointSize(config.getUiFontPointSize());
    mainWindowPresenter.showInitialView();

    // run application
    return mindforgerApplication.exec();
}
