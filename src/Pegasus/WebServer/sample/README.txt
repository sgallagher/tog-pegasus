WebServer Demo
==============

Requirements:
-------------
- Pegasus build with WebServer module enabled
   (flag 'PEGASUS_ENABLE_PROTOCOL_WEB')
- Files for the web interface have to be encoded in UTF-8,
   and placed in the web-root. The web-root depends on the current
   operating system.


Build:
------
- set PEGASUS_ENABLE_PROTOCOL_WEB
- run make target 'all' on Makefile in the project's root
- run make target 'copy_webroot' on WebServer-module's Makefile


Configuration:
--------------
 The WebServer uses a default configuration which can be overridden with
 entries from the OpenPegasus configuration file.

 OpenPegasus Config-File:
  'cimserver_current.conf'

 Parameters:
   webRoot      : WebServer's web-root directory, default is a sub-directory
                    called 'www' of the OpenPegasus installation directory.
   indexFile    : The file to serve as index file for requests pointing to the
                    web-root directory '/', default is index.html
   mimeTypesFile: Absolute path to mimeTypes-file containing a mapping of
                  file-extension to MIME-type.


Notice:
-------
 1) It is not possible to place symbolic links in the web-root.
 2) Only files with defined mimeType will be served.
 3) File-Encodings different to UTF-8 may cause trouble.
