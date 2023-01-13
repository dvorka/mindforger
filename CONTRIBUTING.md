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
* If possible, limit commit message length to 72 characters.


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
* 4 spaces indentation.
* Python's black like formatting.
* Use `/lib/src` source code as code style reference.

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

Example of how to format code to keep it under 88 columns (Python's black style):

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


# Styleguide: C++ comments
Comments should be used to explain tricky and/or
important code only. Don't use comments to explain
obvious things as comments might diverge from the
actual code (e.g. after code refactoring) and may
cause confusion. Make comments brief, consistent
and concise

Code style:

```cpp
/**
 * @brief Brief class description.
 *
 * Detailed class description which may include
 * `examples` of use, _ASCII diagrams_ or **bullet lists**.
 * Do ~~not~~ worry.
 *
 * Code block:
 *
 *     int codeBlockExampleVariable;
 *
 * @see Text (as sentence) or URL.
 * @see [The link text](http://example.com/)
 * @see [The link text](#MyOtherClass)
 */
class MyClass
{

    int field; // brief field description in the lower case
    /**
	 * @brief Another field.
	 *
     * Field with a long description must be documented using
     * this style of the comment. The text of description to be
     * formed by sentences. Apart to that you can use any formatting
     * syntax from below.
     */
    int anotherField;

public:

...

    /**
     * @brief Brief method description.
     *
     * Detailed method description which may include
     * examples of use, ASCII diagrams or bullet/numbered
     * lists like:
     *
     * 1. The first item.
     * 2. The second item with the reference of `field`.
     * 3. The third item.
     *
     * @param myParam Parameter documentation as sentence(s).
     * @return Return value description as sentence(s).
     *
     * @see [The link text](#MyOtherClass)
     */
    void myMethod(int myParam) { ... }

...

}
```


# Styleguide: Qt

* MindForger uses MVP pattern (see `main_window_presenter.h/.cpp`)
* See `/src/qt` source code for a code style reference.
