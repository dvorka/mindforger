# Contribute to MindForger

MindForger is free and open source software. Feel free to **contribute** - any help 
with MindForger development will be **highly appreciated**!

* **Bugs and Suggestions** ... submit bugs, issues, ideas and enhancements.
* **Translations** ... translate MindForger to your language.
* **Platform support** ... port MindForger to your favorite OS or distribution.
* **Code** ... submit pull request/patch with implementation of a feature you missed.
* **Integration** ... how-to or code enabling integration with your (favorite) project.
* **Enhancements** ... submit performance, efficiency and/or productivity enhancements suggestions (code, bug or docs)
* **Documentation** ... write a document, block post; create YouTube video, ...

Don't hesitate to contact [me](mailto:martin.dvorak@mindforger.com).



# Code of Conduct
This project and everyone participating in it is governed by the 
[MindForger Code of Conduct](./CODE_OF_CONDUCT.md). By participating, you are expected to uphold this
code. Please [report](mailto:martin.dvorak@mindforger.com) unacceptable behavior.



# Styleguide: Git Commit Messages

* Limit commit message to 72 characters or less.
* Reference issues and pull requests - especially when you are closing an issue with the commit.
* Consider starting the commit message with an applicable emoji:
    * :art: `:art:` when improving the format/structure of the code
    * :racehorse: `:racehorse:` when improving performance
    * :non-potable_water: `:non-potable_water:` when plugging memory leaks
    * :memo: `:memo:` when writing docs
    * :penguin: `:penguin:` when fixing something on Linux
    * :apple: `:apple:` when fixing something on macOS
    * :checkered_flag: `:checkered_flag:` when fixing something on Windows
    * :bug: `:bug:` when fixing a bug
    * :fire: `:fire:` when removing code or files
    * :green_heart: `:green_heart:` when fixing the CI build
    * :white_check_mark: `:white_check_mark:` when adding tests
    * :lock: `:lock:` when dealing with security



# Styleguide: C++

* .h extension for header files.
* .cpp extension for class files.
* lower_case_with_unserscores class files names.
* Spaces, no tabs.
* No trailing whitespaces.
* {} used w/ constructor having 0/1 parameter, () otherwise.
* CamelCase class names (no underscores).
* See `/lib/src` source code for as code style reference.



# Styleguide: Qt

* MindForger uses MVP pattern (see `main_window_presenter.h/cpp`)
* See `/src/qt` source code for as code style reference.
