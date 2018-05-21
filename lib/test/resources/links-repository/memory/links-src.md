# Links in memory <!-- Metadata: type: Outline; created: 2018-05-10 10:15:30; reads: 249; read: 2018-05-21 11:39:40; revision: 249; modified: 2018-05-21 11:39:40; importance: 0/5; urgency: 0/5; -->
Manual test for **links** with target(s) in:

* the **same** directory 
* in **subdirectories**

---

Os links:

* [O: ABSOLUTE O link path](/home/dvorka/p/mindforger/git/mindforger/lib/test/resources/links-repository/memory/links-dst.md)
  `/home/dvorka/p/mindforger/git/mindforger/lib/test/resources/links-repository/memory/links-dst.md`
* [O: RELATIVE O link path to SUBDIRECTORY](dst-subdir/links-dst.md)
  `dst-subdir/links-dst.md`
* [O: RELATIVE O link to SAME directory](links-dst.md)
  `links-dst.md`
* [O: RELATIVE O link to SAME directory](./links-dst.md)
  `./links-dst.md`

WIP:

* [O: RELATIVE O link path](~/p/mindforger/git/mindforger/lib/test/resources/links-repository/memory/links-dst.md)
  `~/p/mindforger/git/mindforger/lib/test/resources/links-repository/memory/links-dst.md`
  **NOT SUPPORTED** by QWebView - most probably it is 
  intentional because of security & platform specific reasons

---

Ns links:

* [N: ABSOLUTE O link path](/home/dvorka/p/mindforger/git/mindforger/lib/test/resources/links-repository/memory/links-dst.md#n2)
  `/home/dvorka/p/mindforger/git/mindforger/lib/test/resources/links-repository/memory/links-dst.md#n2`
* [N: RELATIVE in in this O](#n2) `#n2`
* [N: RELATIVE O link path to SUBDIRECTORY](dst-subdir/links-dst.md#n2)
  `dst-subdir/links-dst.md#n2`
* [N: RELATIVE O link to SAME directory](links-dst.md#n2)
  `links-dst.md#n2`
* [N: RELATIVE O link to SAME directory](./links-dst.md#n2)
  `./links-dst.md#n2`


# N1 <!-- Metadata: type: Note; created: 2018-05-11 12:10:07; reads: 27; read: 2018-05-21 11:23:26; revision: 5; modified: 2018-05-21 11:23:26; -->
N1 text.
# N2 <!-- Metadata: type: Note; created: 2018-05-21 11:22:59; reads: 10; read: 2018-05-21 11:23:19; revision: 3; modified: 2018-05-21 11:23:19; -->
N2 text.
# N3 <!-- Metadata: type: Note; created: 2018-05-21 11:23:07; reads: 4; read: 2018-05-21 11:23:12; revision: 2; modified: 2018-05-21 11:23:12; -->
N3 text.
