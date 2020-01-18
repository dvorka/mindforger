# cmark vs discount > AUTOLINKING <!-- Metadata: type: Outline; tags: mindforger-home; created: 2019-02-16 08:40:05; reads: 258; read: 2019-05-26 09:03:09; revision: 258; modified: 2019-05-26 09:03:09; importance: 0/5; urgency: 0/5; -->
Only cmark can make this:
* a
* [ ] b
* [x] c

:wink:

---

Discount needs:

* e
* [ ] f
* [x] g

[Links](#links)

**Hi** it's *real* pleasure **to do this** again!
# Autolinking <!-- Metadata: type: Note; created: 2019-05-01 11:19:21; reads: 67; read: 2019-05-26 08:54:13; revision: 5; modified: 2019-05-26 08:52:40; -->
Self upper/lower case: Autolinking and autolinking text.

Other Links and links text.
## Note 1 <!-- Metadata: type: Note; created: 2019-02-16 08:40:05; reads: 122; read: 2019-05-26 08:54:16; revision: 12; modified: 2019-05-26 08:46:24; -->
Self note 1.
## Note 2 <!-- Metadata: type: Note; created: 2019-02-18 08:04:19; reads: 106; read: 2019-05-26 08:54:17; revision: 10; modified: 2019-05-26 08:46:27; -->
Self note 2.
# Deep bullet list <!-- Metadata: type: Note; tags: bug; created: 2019-05-21 05:29:44; reads: 26; read: 2019-05-26 08:54:18; revision: 6; modified: 2019-05-26 08:53:04; -->
This is deep bullet list:

* 1
    * 11
        * 111
* 2
    * 22
        * 222
* 3
    * 33
        * 333
## Broken bullets, added trailing space <!-- Metadata: type: Note; tags: bug; created: 2016-12-04 20:23:04; reads: 61; read: 2019-05-26 09:03:09; revision: 9; modified: 2019-05-26 09:03:09; -->

* Mind object
    * methods to:
        * get associations
        * control forgetting level
    * Memory object
        * list of outlines (title, *, !, timestamp, id (filesystem path)) > recursive find of *.md
      * list of notes across all outlines ordered by name
      * map of words from notes title map(word,noteId), noteId outline-filepath#note name as is
   * NOW: transient, created each time MF starts
      * later serialiaze as no whitespace md > easy to debug

* implement JavaFX shell = whispering text box for commands + textual/highlighted view
   * shell allowing to work w/ mind (query, get, ...) > index and API w/o UI (small project), UI will later use same
   * nice thing about shell is that it is about **functionality** and advanced **features**, not about coping
     w/ UI and it might be even faster
   * let me call it bash for mind
      * m8r --shell						... RD_ONLY to start with
         * > list outlines					... all outlines
         * > find outline abc					... find outline that starts/has "abc" in its name
            * Ctrl-o
         * > forget 25%						... hides oldest 25% of outlines/notes
         * > list notes						... all notes
         * > find note abc
            * Ctrl-n
         * > list labels of outlines
            * coding 3x
            * ...
         * > list outlines with "coding" label
         * > list keywords of notes
            * coding 19x
            * ...
         * > list notes with "coding" keyword
         * > list notes of "Git" outline			... outline suffix optional
            * ... tree of notes ...
         * > associations of "Git" outline 			... outline suffix optional
         * > associations of "Git"."Tips and Tricks" note   ... note suffix optional
         * > show "Git"."Tips and Tricks"
         * > use "Git" outline
         * "Git" > list notes
         * "Git" > associations
         * "Git" > show "Tips and Tricks"
            * "Git"."Tips and Tricks" > associations
               * Ctrl-a
            * "Git"."Tips and Tricks" > search
               * Ctrl-s
         * > find text git commit
            * Ctrl-f
         * > show memory dwell					... lists notes from top to deepest
      * m8r --shell						... modifications
         * > edit "Git"."Tips and Tricks"
         * > use "Git" outline
         * "Git" > edit "Tips and Tricks"

* implement JavaFX window skeleton for RD_ONLY - I can play w/ UI forever and it already
  cost me man years of coding time
   * list of outlines (table)
   * MD outline > outline object loader
   * tree-table of notes
   * view single note (possibly MD syntax highlighted)


* finish import of all my resources
   * MindRaider repository - see [import](import.md)
   * import Latex book on mind to repository - see [import](import.md)
   * change M1ndF0rg3r magic word in metadata to METADATA

* graphic navigator - two start screens; all nodes (outline, note, tag, keyword) visible all the time
   * outline tag cloud > tag > outlines (w/ tag) > outline > outline's notes
   * note keyword cloud > keyword > notes



# Links <!-- Metadata: type: Note; created: 2019-03-03 08:50:14; reads: 59; read: 2019-05-26 08:49:31; revision: 2; modified: 2019-03-03 08:51:33; -->
Linking:

* local [note](#note-1)
* [AI](ai/ai.md)
* remote note [tail](linux/ubuntu.md#tail)
## A: Link <!-- Metadata: type: Note; created: 2019-05-06 06:44:31; reads: 43; read: 2019-05-26 08:49:32; revision: 11; modified: 2019-05-26 08:45:57; -->
Non-autolinked: [Links](#links) ... checking links are **not** broken.

* Linked abbrev: A
* Linked name: link
* Linked lower name: Link
# Inlines and Blocks <!-- Metadata: type: Note; tags: bug; created: 2019-05-26 08:46:37; reads: 17; read: 2019-05-26 08:49:57; revision: 8; modified: 2019-05-26 08:49:57; -->
**BUG**: Inlined math: $links inlined$

Math block:
$$
links block
$$

Inlined code `links inlined`

Code block:
```
links block
```

---

Check: links

Check: math
## Math <!-- Metadata: type: Note; tags: bug; created: 2019-05-21 05:27:04; reads: 45; read: 2019-05-26 08:50:10; revision: 7; modified: 2019-05-26 08:50:10; -->
**BUG**: Inlined math $math expressions$ should NOT be linked.
## Code block <!-- Metadata: type: Note; created: 2019-05-13 06:31:27; reads: 43; read: 2019-05-26 08:52:14; revision: 6; modified: 2019-05-26 08:52:14; -->
Before block:
```
1 *
2 ************* Module h2oaicore.mli.mli
3 *
```
After block.

Inlined `code block example`.

Self code block.
# Syntax highlighting: Math <!-- Metadata: type: Note; tags: highlight; created: 2019-05-01 10:54:06; reads: 57; read: 2019-05-26 08:46:20; revision: 12; modified: 2019-05-26 08:10:31; -->
This $math$ syntax (this must be linked math).

This `code block` syntax (BUG this must be linked: code block).

This $$math math$$ syntax.

Multiline...
$$
frac
math
code block
$$
... math block.

---

Bugs:

Text $RR = \frac{p_2}{p_1}$ and the rest.
## frac <!-- Metadata: type: Note; created: 2019-05-13 06:36:42; reads: 23; read: 2019-05-26 08:46:14; revision: 4; modified: 2019-05-26 08:42:24; -->
Self frac.
