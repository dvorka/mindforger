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

#include "lib/src/version.h"
#include "lib/src/config/configuration.h"

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
 *   ... use repository specified using
 *       --repository parameter,
 *       or configured in ~/.mindforger,
 *       or specified by environment variable MINDFORGER_REPOSITORY,
 *       or check existence of default ~/mindforger-repository
 *       otherwise exit
 *
 *
 * Terminal CLI commands:
 *
 * $ mindforger LIST outlines
 * $ mindforger FIND outline "abc"
 * $ mindforger VIEW outline "abc"
 * $ mindforger EDIT outline "abc"
 * $ mindforger LIST associations OF outline "abc"
 *
 * $ mindforger LIST notes
 * $ mindforger FIND note "abc"
 * $ mindforger VIEW note "abc"."efg"
 * $ mindforger EDIT note "abc"."efg"
 * $ mindforger LIST notes OF outline "abc"
 * $ mindforger LIST associations OF note "abc"."efg"
 *
 * $ mindforger FTS "expr"
 * $ mindforger FTS "expr" OF outline "abc"
 * $ mindforger FTS "expr" OF note "abc"."efg"
 *
 *
 * MindForger options:
 *
 * TODO $ mindforger /a/directory    ... a MD directory to index (non-MF)
 * TODO $ mindforger /a/file.md      ... a MD file to show (non-MF)
 *
 * $ mindforger --repository ~/my-repository
 * $ mindforger -r ~/my-repository
 * $ mindforger --theme dark
 * $ mindforger --generate-toc my.md
 * $ mindforger --config forget=25%
 * $ mindforger --shell
 * $ mindforger --help
 */
int main(int argc, char *argv[])
{
    QApplication mindforgerApplication(argc, argv);

    std::string useRepository{};
    QString themeOptionValue{};
    if(argc > 1) {
        QCommandLineParser parser;
        // process command line as parameters/options are present
        parser.setApplicationDescription("Thinking notebook.");
        parser.addPositionalArgument("[<command>]", QCoreApplication::translate("main", "A MindForger command like LIST, FIND, VIEW, EDIT or FTS."));
        parser.addPositionalArgument("[<args>]", QCoreApplication::translate("main", "Command arguments."));
        QCommandLineOption repositoryOption(QStringList() << "r" << "repository",
                QCoreApplication::translate("main", "Load given MindForger or Markdown <repository>."),
                QCoreApplication::translate("main", "repository"));
        parser.addOption(repositoryOption);
        QCommandLineOption themeOption(QStringList() << "t" << "theme",
                QCoreApplication::translate("main", "Use yin or yang GUI <theme>."),
                QCoreApplication::translate("main", "theme"));
        parser.addOption(themeOption);
        QCommandLineOption generateTocOption(QStringList() << "T" << "generate-toc",
                QCoreApplication::translate("main", "Generate table of contents for <source> Markdown file."),
                QCoreApplication::translate("main", "source"));
        parser.addOption(generateTocOption);
        QCommandLineOption versionOption=parser.addVersionOption();
        QCommandLineOption helpOption=parser.addHelpOption();
        // process the actual command line arguments given by the user
        parser.process(mindforgerApplication);

        if(parser.isSet(helpOption) || parser.isSet(versionOption)) {
            return 0;
        }

        if(parser.isSet(repositoryOption)) {
            QString repositoryOptionValue = parser.value(repositoryOption);
            useRepository.assign(repositoryOptionValue.toStdString());
        }

        if(parser.isSet(themeOption)) {
            themeOptionValue = parser.value(themeOption);
        }

        QString tocOfMarkdownFile = parser.value(generateTocOption);
        if(!tocOfMarkdownFile.isEmpty()) {
            cout << "Generating table of contents for " << tocOfMarkdownFile.toUtf8().constData();

            // ... TODO to be finished ...

            return 0;
        }
    }
    // else there are no parameters and options > simply load GUI

    QApplication::setApplicationName("MindForger");
    QApplication::setApplicationVersion(MINDFORGER_VERSION);
    m8r::initRandomizer();

    // load configuration & update it w/ CLI settings
    m8r::Configuration& config = m8r::Configuration::getInstance();
    config.load();
    if(!useRepository.empty()) {
        config.setActiveRepository(config.addRepository(m8r::RepositoryIndexer::getRepositoryForPath(useRepository)));
    } else {
        config.findOrCreateDefaultRepository();
    }

    // setup application
    mindforgerApplication.setWindowIcon(QIcon(":/icons/minforger-icon.png"));
    m8r::l8n(mindforgerApplication);

    // choose L&F
    m8r::LookAndFeels& lookAndFeels = m8r::LookAndFeels::getInstance();
    lookAndFeels.init(&mindforgerApplication);
    lookAndFeels.setFontPointSize(config.getFontPointSize());
    if(!themeOptionValue.isEmpty()) {
        if(lookAndFeels.isThemeNameValid(themeOptionValue)) {
            lookAndFeels.setTheme(themeOptionValue);
        } else {
            cerr << QCoreApplication::translate("main", "Ignoring unknown GUI theme: '").toUtf8().constData()
                 << themeOptionValue.toUtf8().constData()
                 << "'";
        }
    }

    // initialize and start UI
    m8r::MainWindowView mainWindowView(lookAndFeels);
    m8r::MainWindowPresenter mainWindowPresenter(mainWindowView);
    mainWindowView.showMaximized();
    mainWindowPresenter.doActionViewOutlines();
    mindforgerApplication.font().setPointSize(config.getFontPointSize());

    // run application
    return mindforgerApplication.exec();
}
