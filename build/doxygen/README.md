# MindForger Source Code Documentation

MindForger source code is documented using dOxygen: http://www.doxygen.nl

# Generate
Generate documentation:

```
doxygen mindforger.cfg
```
# Setup
Install Doxygen:
```
sudo apt install doxygen
```

# Doxygen Configuration
Configuration has been created using:

```
doxygen -g project-name.cfg
```

Changed from default:

```
PROJECT_NAME = "MindForger" - Specify a project name.
INPUT = ../../lib
USE_MDFILE_AS_MAINPAGE = README.md - specify custom text for main documentation page.
OUTPUT_DIRECTORY = documents - Specify a documentation directory for the processed results.
FULL_PATH_NAMES = NO - Looks cleaner this way. Set to YES for code which has a sensible path hierarchy.
JAVADOC_AUTOBRIEF = YES - Don't need explicit "brief" tag
HIDE_UNDOC_CLASSES = NO - I let doxygen document everything
GENERATE_LATEX = NO - I use HTML docs and have no need for Latex
TAB_SIZE = 4 - Default is 8
OPTIMIZE_OUTPUT_FOR_C = YES - For C and C++ source code
BUILTIN_STL_SUPPORT = YES - Only available in newer versions of dOxygen
EXTRACT_ALL = YES
EXTRACT_LOCAL_CLASSES = YES
RECURSIVE = YES - Search subdirectories
SOURCE_BROWSER = YES - List source files
ALPHABETICAL_INDEX = YES - Generate alphabetic index of classes, structs, unions
GENERATE_TREEVIEW = YES - Generate side panel nav tree (html frame)
TEMPLATE_RELATIONS = YES - Show inheritance relationship for templates
SEARCHENGINE = YES - Creates local search facility.
FILE_PATTERNS = *.cc *.h *.cpp *.hpp - specify file suffixes to document. I leave it blank to take the default.
REFERENCED_BY_RELATION = YES - document all entities associated with functions documented
```

HTML documentation:

```
GENERATE_HTML          = YES
HTML_OUTPUT            = html
HTML_FILE_EXTENSION    = .html
HTML_HEADER            =
HTML_FOOTER            =
HTML_STYLESHEET        =
HTML_ALIGN_MEMBERS     = YES
DISABLE_INDEX          = NO
```

RTF documentation:

```
GENERATE_RTF           = YES
RTF_OUTPUT             = rtf
COMPACT_RTF            = NO
RTF_HYPERLINKS         = NO
RTF_STYLESHEET_FILE    =
RTF_EXTENSIONS_FILE    =
```
# Resources

* https://en.wikipedia.org/wiki/Doxygen
  - Brief Doxygen description in Wikipedia.
* http://www.yolinux.com/TUTORIALS/LinuxTutorialC++CodingStyle.html
  - dOxygen code style
* https://github.com/doxygen/doxygen
  - GitHub
