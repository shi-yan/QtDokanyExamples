# QtDokanyExamples
Dokany filesystem examples using Qt

This example project aims at providing a dokany drive [https://github.com/dokan-dev/dokany] implementation with Qt.

There are needs to write a dokany client on platforms other than windows. For example, when implementing a network drive, you want to implement a windows dokany server to receive requests from the dokany driver. The requests will then be forwarded to a client app running on linux or mac. The client app will carry out the filesystem operations and send the results back to the dokany server.

Although the Mirror example enclosed in the dokany project serves as a good document, its extensive use of windows api leads to poor portability. I therefore started this project to provide an example without using windows api and runable on other platform (the client part).

There are two examples enclosed in this project. One is a Mirror that can mount one of the local folders as a harddrive. One is a network drive example which can mount a remote folder as a harddrive.

== How to build ==

== How to run the mirror example ==

== How to run the network drive example on a single machine ==

== How to run the network drive example on two machines ==

== How to run the network drive example without the ui ==

== General architecture of the code ==