# Contribute to MindForger

MindForger is free and open source software. Feel free to **contribute** - any help 
with MindForger development will be **highly appreciated**!

* **Bugs and Suggestions**
    * Submit bugs, issues, ideas and enhancements.
* **Translations**
    * Translate MindForger to your language.
* **Platform support**
    * Port MindForger to your favorite OS or distribution.
* **Code**
    * Submit pull request/patch with implementation of a feature you missed.
* **Integration**
    * How-to or code enabling integration with your (favorite) project.
* **Enhancements** 
    * Submit performance, efficiency and/or productivity enhancements suggestions (code, bug or docs)
* **Documentation** 
    * Write a document, block post; create YouTube video, ...

Don't hesitate to contact [me](mailto:martin.dvorak@mindforger.com).
# Code of Conduct
This project and everyone participating in it is governed by the 
[MindForger Code of Conduct](./CODE_OF_CONDUCT.md). By participating, you are expected to uphold this
code.
# Styleguide: Git Commit Messages
* Commit messages must provide concise change description.
* Reference issues and pull requests related to the commit.
* Close bugs with issue references in commit messages.
* Limit commit message length to 72 characters, but only if reasonable.
# Styleguide: C++
Code style:
  
* Use `.h` extension for header files.
* Use `.cpp` extension for class files.
* Use `lower_case_with_unserscores` source code file names.
* Spaces, no tabs.
* No trailing whitespaces.
* Use `{}` with constructor having 0/1 parameter, () otherwise.
* `CamelCase` class names (no underscores).
* Lines to have at most 88 columns.
* See `/lib/src` source code for as code style reference.

Example of class `{}` style:

```cpp
class MyClass
{

...

public:

...

    void myFunction(int myParam) { ... }
    
    int myMultiLineFunction() const { 
        ...
    }

...

}
```

Example of source code `{}` style:

```cpp
...

bool MyClass::myFunction(const QString& myArg)
{
    if(myCondition()) {
       ...
    }
    ...
}
```

Example of how to format code to keep it under 88 columns:

```cpp
void MainWindowPresenter::doActionOrganizerMoveNoteToNextVisibleQuadrant(Note* note)
{
    if(OrlojPresenterFacets::FACET_KANBAN == orloj->getFacet()) {
        doActionKanbanMoveNoteCommon(
            note,
            orloj->getKanban()->moveToNextVisibleColumn(note),
            orloj
        );
    } else {
        if(!EisenhowerMatrix::isEisenhowMatrixOrganizer(
                orloj->getOrganizer()->getOrganizer()
           )
           && myFunction()
           && yourCondition
         ) {
            doActionOrganizerMoveNoteCommon(
                note,
                orloj->getOrganizer()->moveToNextVisibleQuadrant(note),
                orloj
            );
        } else {
            statusBar->showError(
                "Notebooks/notes cannot be moved around quadrants of "
                "Eisenhower Matrix"
            );
        }
    }
}

```
# Styleguide: Qt

* MindForger uses MVP pattern (see `main_window_presenter.h/.cpp`)
* See `/src/qt` source code for a code style reference.
