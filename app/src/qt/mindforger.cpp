/*
 Martin Dvorak <martin.dvorak@mindforger.com>

 The author disclaims copyright to this source code. In place of a legal notice,
 here is a blessing:

    May you do good and not evil.
    May you find forgiveness for yourself and forgive others.
    May you share freely, never taking more than you give.

  mindforger.cpp     Main file of MindForger code base.
*/
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#ifndef _WIN32
  #include <getopt.h>
#else
  #include "../../deps/getopt/getopt.h"
#endif
#include <QtWidgets>

#include "../../lib/src/version.h"
#include "../../lib/src/gear/file_utils.h"
#include "../../lib/src/representations/markdown/markdown_configuration_representation.h"
#include "../../lib/src/representations/markdown/markdown_repository_configuration_representation.h"

#include "gear/qutils.h"
#include "i18nl10n.h"
#include "main_window_view.h"
#include "main_window_presenter.h"
#include "look_n_feel.h"

using namespace std;
using namespace m8r::filesystem;

/**
 * @brief main MindForger function and command line interface.
 *
 * MindForger command line interface description:
 *
 * ```
 * $ mindforger
 *   ... looks up repository as follows
 *     1. repository configured in ~/.mindforger,
 *     2. specified by environment variable MINDFORGER_REPOSITORY,
 *     3. checks existence of MindForger repository in default location
 *        i.e. ~/mindforger-repository
 *     4. creates new MindForger repository in default location
 *        i.e. ~/mindforger-repository
 * $ mindforger ~/my-mf-repository
 *   ... start MindForger with given repository
 * $ mindforger ~/books/marathon-training
 *   ... directory structure w/ Markdowns
 * $ mindforger ~/my-mf-repository/memory/plans.md
 *   ... Markdown-based MindForger DSL file
 * $ mindforger ~/books/marathon-training/07-lsd.md
 *   ... Markdown file
 * ```
 */

/*
 * Options proposal:
 *
 * ```
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
 * ```
 *
 * Terminal CLI commands proposal
 * (Docker inspiration > switch entity and action):
 *
 * ```
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
 * ```
 *
 * MindForger documentation is generated with DoXygen's JavaDoc style.
 *
 * @see https://www.doxygen.nl/manual/markdown.html
 * @see https://www.doxygen.nl/manual/docblocks.html#docexamples
 */
int main(int argc, char* argv[])
{
    // check whether running in GUI (and not in text console tty)
#if !defined(__APPLE__) && !defined(_WIN32)
    char* term = getenv(m8r::ENV_VAR_DISPLAY);
    if(!term || !strlen(term)) {
        cerr << endl
             << QCoreApplication::translate(
                    "main",
                    "MindForger CANNOT be run from text console "
                    "- set DISPLAY environment variable or run "
                    "MindForger from GUI."
                ).toUtf8().constData()
             << endl;
        exit(1);
    }
    // default terminal macOS environment: TERM=xterm-256color DISPLAY=
#endif

    // Stupid & ugly: QWebEngine @ macOS requires extra parameter(s) to
    // allow access to local files (like images) to QApplication (security).
    // Parameter differ Qt version to Qt version as diffent versions
    // bundle different Chromium versions.
    //
    // Qt 5.9.9
    //  --disable-web-security           ... same origin
    //
    // Qt 5.15.2 ~ Chrome 83 ~ I did NOT find params to enable filesystem access
    //  --user-data-dir=<HOME>           ... user data dir
    //  --disable-web-security           ... same origin
    //  --disable-site-isolation-trials
    //
    // QApplication args assembly:
    //  - order in ^ examples matters
    //
    // Additional hints:
    // - HTML is rendered by QWebEngine on macOS
    // - QWebEngine is Chromium wrapper
    //   https://doc.qt.io/qt-5/qtwebengine-overview.html
    //   https://doc.qt.io/qt-5/qtwebengine-features.html
    //
    // Debugging:
    //
    // - DEBUG_WEBENGINE_SECURITY_QT_5_15
//#define DEBUG_WEBENGINE_SECURITY_QT_5_15
    //
    // Resources:
    // - https://doc.qt.io/qt-5/qtwebengine-index.html
    // - https://doc.qt.io/qt-5/qtwebengine-debugging.html#using-command-line-arguments
    // - https://wiki.qt.io/QtWebEngine/ChromiumVersions
    // - https://stackoverflow.com/questions/35432749/disable-web-security-in-chrome-48
    //   ^ changes in required parameters with disable-web-security
    //
#if defined(__APPLE__) || defined(_WIN32)
  #if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    char ARG_DISABLE_WEB_SECURITY[] = "--disable-web-security";
    int newArgc = argc + 1 + 1;
    char** newArgv = new char*[static_cast<size_t>(newArgc)];
    newArgv[0] = argv[0];
    newArgv[1] = ARG_DISABLE_WEB_SECURITY;
    for(int i=2; i<newArgc-1; i++) {
        newArgv[i] = argv[i-1];
    }
    newArgv[newArgc-1] = nullptr;
  #else
    // IMPROVE: new Chromium @ QWebEngine requires more parameters, but
    //   I didn't find working configuration which would allow loading
    //   of images on macOS BigSur
    char ARG_DISABLE_WEB_SECURITY[] = "--disable-web-security";
    char ARG_USER_DATA_DIR[] = "--user-data-dir=/Users/dvorka/tmp";
    char ARG_SINGLE_PROCESS[] = "--single-process";
    char ARG_NO_SANDBOX[] = "--no-sandbox";
    char ARG_DISABLE_ISOLATION_TRIALS[] = "--disable-site-isolation-trials";
    char ARG_ALLOW_FILE_ACCESS_FROM_FILES[] = "--allow-file-access-from-files";

    #ifdef DEBUG_WEBENGINE_SECURITY_QT_5_15
        int newArgc = 6;
        char** newArgv = new char*[static_cast<size_t>(newArgc)];
        newArgv[0] = argv[0];
        newArgv[1] = "--args";
        newArgv[2] = ARG_USER_DATA_DIR;
        newArgv[3] = ARG_DISABLE_WEB_SECURITY;
        newArgv[4] = "/Users/dvorka/mf-devel/mf-copy";
        newArgv[5] = NULL;

    #else // DEBUG_WEBENGINE_SECURITY_QT_5_15
        char* injectWebEngineArgv[] = {
            ARG_USER_DATA_DIR,
            ARG_DISABLE_WEB_SECURITY,
            ARG_DISABLE_ISOLATION_TRIALS,
            ARG_SINGLE_PROCESS,
            ARG_NO_SANDBOX,
            ARG_ALLOW_FILE_ACCESS_FROM_FILES
        };
        int injectWebEngineArgc = 6;

        int newArgc = argc + injectWebEngineArgc + 1;
        char** newArgv = new char*[static_cast<size_t>(newArgc)];
        newArgv[0] = argv[0];
        int i=1;
        for (int v=0; v<injectWebEngineArgc; i++, v++) {
            newArgv[i] = injectWebEngineArgv[v];
        }
        for(int v=1; v<argc; v++, i++) {
            newArgv[i] = argv[v];
        }
        newArgv[newArgc-1] = nullptr;
    #endif // DEBUG_WEBENGINE_SECURITY_QT_5_15
  #endif // Qt version

  #ifdef DO_MF_DEBUG
    MF_DEBUG("argv: " << newArgc << endl);
    for(int i=0; i<newArgc; i++) {
        if(newArgv[i] == nullptr) {
            MF_DEBUG("  " << i << " NULL" << endl);
            break;
        }
        MF_DEBUG("  " << i << " " << newArgv[i] << endl);
    }
  #endif

    QApplication mindforgerApplication(newArgc, newArgv);
#else
    QApplication mindforgerApplication(argc, argv);
#endif

#ifdef MF_DEBUG_QRC
    QDirIterator it(":", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        MF_DEBUG(it.next() << endl);
    }
#endif
    QApplication::setApplicationName("MindForger");
    QApplication::setApplicationVersion(MINDFORGER_VERSION);
    mindforgerApplication.setWindowIcon(QIcon(":/icons/mindforger-icon.png"));

#ifndef DEBUG_WEBENGINE_SECURITY_QT_5_15
    std::string useRepository{};
    QString themeOptionValue{};
    QString configurationFilePath{};
    if(argc > 1) {
        QCommandLineParser parser;
        // process command line as parameters/options are present
        parser.setApplicationDescription("Thinking notebook.");
        parser.addPositionalArgument(
            "[<directory>|<file>]",
            QCoreApplication::translate(
                 "main",
                 "MindForger workspace or directory/file with Markdown(s) to open"
            )
        );
        QCommandLineOption themeOption(QStringList() << "t" << "theme",
            QCoreApplication::translate(
                "main", "Use 'dark', 'light' or other GUI <theme>."
            ),
            QCoreApplication::translate("main", "theme")
        );
        parser.addOption(themeOption);
        QCommandLineOption configPathOption(QStringList() << "c" << "config-file-path",
            QCoreApplication::translate(
                "main", "Load configuration from given <file>."
            ),
            QCoreApplication::translate("main", "file")
        );
        parser.addOption(configPathOption);

#if defined(__APPLE__) || defined(_WIN32)
        // command line options passed to WebEngine to disable security
        QCommandLineOption macosDisableSecurityOption(QStringList() << "S" << "disable-web-security",
            QCoreApplication::translate("main", "Disable WebEngine security to allow loading of images on macOS.")
        );
        parser.addOption(macosDisableSecurityOption);

      #if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        QCommandLineOption macosSingleProcess(QStringList() << "P" << "single-process",
            QCoreApplication::translate("main", "Disable WebEngine security by running single process on macOS.")
        );
        parser.addOption(macosSingleProcess);
        QCommandLineOption macosNoSandbox(QStringList() << "N" << "no-sandbox",
            QCoreApplication::translate("main", "Disable WebEngine security by disabling sandbox on macOS.")
        );
        parser.addOption(macosNoSandbox);
        QCommandLineOption macosUserDataDir(QStringList() << "U" << "user-data-dir",
            QCoreApplication::translate("main", "Disable WebEngine security by user data dir specification on macOS."),
            QCoreApplication::translate("main", "file")

        );
        parser.addOption(macosUserDataDir);
        QCommandLineOption macosSiteIsolation(QStringList() << "I" << "disable-site-isolation-trials",
            QCoreApplication::translate("main", "Disable WebEngine security via site isolation trials on macOS.")
        );
        parser.addOption(macosSiteIsolation);
        QCommandLineOption macosAcessFileFromFile(QStringList() << "F" << "allow-file-access-from-files",
            QCoreApplication::translate("main", "Disable WebEngine security via acess file from file on macOS.")
        );
        parser.addOption(macosAcessFileFromFile);
      #endif
#endif
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
            cerr << "Error: Too many arguments ("
                 << dec
                 << arguments.size()
                 << ") - at most one directory or file can be specified"
                 << endl;
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
# else // DEBUG_WEBENGINE_SECURITY_QT_5_15
    QString configurationFilePath{"/Users/dvorka/.mindforger.md"};
    QString themeOptionValue{};
    std::string useRepository{};
#endif // DEBUG_WEBENGINE_SECURITY_QT_5_15

    // load configuration
    m8r::MarkdownConfigurationRepresentation mdConfigRepresentation{};
    m8r::Configuration& config = m8r::Configuration::getInstance();
    if(configurationFilePath.size()) {
        config.setConfigFilePath(configurationFilePath.toStdString());
    }
    if(!mdConfigRepresentation.load(config)) {
        MF_DEBUG(
            "MindForger is unable to load MD configuration from: " <<
            config.getConfigFilePath() << endl
        );

        mdConfigRepresentation.save(File{config.getConfigFilePath()});
    }

    // l10n is initalized after configuration to be configurable by MF
    m8r::l10n(mindforgerApplication);

    m8r::initRandomizer();

    m8r::MarkdownRepositoryConfigurationRepresentation mdRepositoryCfgRepresentation{};
    if(!useRepository.empty()) {
        m8r::Repository* r = m8r::RepositoryIndexer::getRepositoryForPath(
            useRepository
        );
        if(r) {
            config.setActiveRepository(
                config.addRepository(r), mdRepositoryCfgRepresentation
            );
        } else {
            if(config.createEmptyMarkdownFile(useRepository)) {
                r = m8r::RepositoryIndexer::getRepositoryForPath(useRepository);
                config.setActiveRepository(
                    config.addRepository(r), mdRepositoryCfgRepresentation
                );
            } else {
                cerr << QCoreApplication::translate(
                            "main",
                            "Error: Unable to find given workspace/file "
                            "to open - open MindForger without parameters "
                            "and create it from menu Mind/New: '"
                        ).toUtf8().constData()
                     << useRepository
                     << "'"
                     << endl;
                exit(1);
            }
        }
    } else {
        config.findOrCreateDefaultRepository(mdRepositoryCfgRepresentation);
    }

    // L&F
    m8r::LookAndFeels& lookAndFeels = m8r::LookAndFeels::getInstance();
    lookAndFeels.init(&mindforgerApplication);
    lookAndFeels.setFontPointSize(config.getUiFontPointSize());
    if(!themeOptionValue.isEmpty()) {
        if(lookAndFeels.isThemeNameValid(themeOptionValue)) {
            lookAndFeels.setTheme(themeOptionValue);
        } else {
            cerr << QCoreApplication::translate(
                        "main", "Ignoring unknown GUI theme: '"
                    ).toUtf8().constData()
                 << themeOptionValue.toUtf8().constData()
                 << "'"
                 << endl;
            lookAndFeels.setTheme(QString::fromStdString(config.getUiThemeName()));
        }
    } else {
        lookAndFeels.setTheme(QString::fromStdString(config.getUiThemeName()));
    }

    // spell check
    MF_DEBUG("Spell check:" << endl);
    // prepare dir(s) for CUSTOM dictionaries
    string customDictionaryPath{m8r::getSystemMindForgerConfigPath()};
    customDictionaryPath += FILE_PATH_SEPARATOR;
    customDictionaryPath += "dictionaries";
    QDir qDictionaryDir{QString::fromStdString(customDictionaryPath)};
    if (!qDictionaryDir.exists()) {
        qDictionaryDir.mkpath(qDictionaryDir.path());
    }
    DictionaryManager::setPath(QString::fromStdString(customDictionaryPath));
#ifdef _WIN32
    // Hunspell is not installed+configured on Win - set dirs for dictionary lookup
    vector<string> searchDirPaths{};
    string searchDirPath{};
    // search custom dictionaries dir
    searchDirPaths.push_back(DictionaryManager::path().toStdString());
    // search home dir
    searchDirPath.assign(m8r::getHomeDirectoryPath());
    searchDirPath += FILE_PATH_SEPARATOR;
    searchDirPath += "dictionaries";
    searchDirPaths.push_back(searchDirPath);
    // search application settings
    searchDirPath.assign(m8r::getHomeDirectoryPath());
    searchDirPath += FILE_PATH_SEPARATOR;
    searchDirPath += "AppData";
    searchDirPath += FILE_PATH_SEPARATOR;
    searchDirPath += "Local";
    searchDirPath += FILE_PATH_SEPARATOR;
    searchDirPath += "dictionaries";
    searchDirPaths.push_back(searchDirPath);
    // search local settings
    searchDirPath.assign(m8r::getHomeDirectoryPath());
    searchDirPath += FILE_PATH_SEPARATOR;
    searchDirPath += "Local Settings";
    searchDirPath += FILE_PATH_SEPARATOR;
    searchDirPath += "dictionaries";
    searchDirPaths.push_back(searchDirPath);
    // search paths assembly
    QStringList spellCheckDictionaryDirs{};
    MF_DEBUG("  Adding dirs to search path:" << endl);
    for(auto d:searchDirPaths) {
        MF_DEBUG("    " << d << endl);
        qDictionaryDir = QDir{QString::fromStdString(searchDirPath)};
        if(qDictionaryDir.exists()) {
            MF_DEBUG("      ADDED" << endl);
            spellCheckDictionaryDirs.append(qDictionaryDir.path());
        }
    }
    // register paths under "dict" alias
    QDir::setSearchPaths("dict", spellCheckDictionaryDirs);
#else
    // use "hunspell -D" to determine dictionary search paths on Linux systems
#endif
    // MF must search for dictionaries REGARDLESS spell check is enabled or not
    // so that it can be enabled later OR dictionaries how to can be provided
    DictionaryManager::instance().addProviders();
    // ^ load available standard dictionaries
    QStringList languages = DictionaryManager::instance().availableDictionaries();
    MF_DEBUG("  Available language dictionaries:" << endl);
    // set the default dictionary language
    if(languages.size()) {
        languages.sort();
        // set language based on configuration
        string defaultLanguage = config.getUiEditorSpellCheckDefaultLanguage();
        bool isDefaultLangAvailable{false};
        foreach(auto l, languages) {
            config.addUiEditorSpellCheckLanguage(l.toStdString());
            MF_DEBUG("    '" << l.toStdString() << "'");
            if(l.toStdString() == defaultLanguage) {
               isDefaultLangAvailable = true;
               MF_DEBUG(" (default)");
            }
            MF_DEBUG(endl);
        }
        if(!isDefaultLangAvailable) {
            // try to set English > US English > fallback to the first available language
            string commonLangs[] = {"en", "en_US", "en_GB", languages[0].toStdString()};
            for(auto cl:commonLangs) {
                if(languages.contains("en")) {
                    defaultLanguage.assign("en");
                    break;
                }
            }
            if(!defaultLanguage.size()) {
                defaultLanguage.assign(languages[0].toStdString());
            }
            MF_DEBUG("  Setting FALLBACK language to : " << defaultLanguage << endl);
        }
        DictionaryManager::instance().setDefaultLanguage(
            QString::fromStdString(defaultLanguage)
        );
        MF_DEBUG("  Spell check language set to: '" << defaultLanguage << "'" << endl);
        config.setUiEditorSpellCheckDefaultLanguage(defaultLanguage);
    } else {
        MF_DEBUG("  No spell check dictionaries available!" << endl);
        // disable spell check (lang, langs and live spell check)
        if(config.isUiEditorLiveSpellCheck()) {
            config.clearUiEditorSpellCheckLanguages();
        }
    }
    // mdConfigRepresentation->save(config);

    // initialize and start UI
    m8r::MainWindowView mainWindowView(lookAndFeels);
    m8r::MainWindowPresenter mainWindowPresenter(mainWindowView);
#ifdef __APPLE__
    mindforgerApplication.font().setPointSize(config.getUiFontPointSize());
    // ugly & stupid shown() > shownMaximized() to workaround start w/ window wider
    // than screen on macOS
    mainWindowView.show();
    mainWindowPresenter.showInitialView();
    mainWindowView.showMaximized();
#else
    mainWindowView.showMaximized();
    mindforgerApplication.font().setPointSize(config.getUiFontPointSize());
    mainWindowPresenter.showInitialView();
#endif

    // run application
    return mindforgerApplication.exec();
}
