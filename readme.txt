Pegasus - A Manageability Services Broker for the DMTF CIM/WBEM Standards

Author: Mike Brasher, Karl Schopmeyer

--------------------------------------------------------------------------------

$Log: readme.txt,v $
Revision 1.3  2001/01/22 15:09:29  mike
Reworking indentation and breaking width of lines below 80 columns (not finished). Also
inserted comments and suggestions following "MEB:" annotations.


Revision 1.1  2001/01/15 04:26:04  karl
added Readme

--------------------------------------------------------------------------------

Tagline: Pegasus is an object manager for DMTF CIM objects written in C++
         and supported buy The Open Group
         
Pegasus is an object manager for DMTF CIM objects. It is written in C++ and
includes the Object manager, a set of defined interfaces, and SDKs for both
client, providers, and services extensions.  It is maintained consistent with
the DMTF CIM and WBEM specifications except for any exceptions noted in the
documentation.

This distribution represents a work in progress towards building a Pegasus
release. Pegasus is open source and is covered under the following license.
This version is incomplete and is directed towards evaluators and developers
of the Pegasus Architecture.

Pegasus is being developed and maintained under the auspices of The Open
Group. Pegasus is maintained under the license defined in the doc directory
(LICENSE) of this release.  This licensing is intended to support as wide a
distribution as possible with minimal demands on the users.

This distribution represnts a snapshot of the current work. Currently Pegasus
is in phase 1 of a multiphase development project. This snapshot is primarily
for developers and for evaluation of the project.

More information on this project, access to the CVS, and documentation on
Pegasus are available from the OpenGroup WEB site.

There are separate files in the release for

	History of releases - HISTORY
	What's new for this release - WHATSNEW
	What's Broken - BUGS

In addition, the roadmap for Pegasus and further information on the project is
available on The Opengroup Pegasus WEB Site.

http://www.opengroup.org/MSB

Availability of Pegasus
=======================

Pegasus is distributed as open source.  Today the distribution is limited
because we want to reach a predefined level of development before making the
project publically available. The move from limited to open distribution will
be a decision of the Pegasus development team and The Open Group and should
occur before March 2001.

To get on the distribution list for Pegasus today, send an email to

m.kirk@opengroup.org

or

k.schopmeyer@opengroup.org

Pegasus Major Components
========================

The major components of Pegasus are:

Pegasus Server - WBEM/CIM Server with interfaces for providers and clients

Pegasus Repository - Today Pegasus provides a defined class repository 
    interface and a simple file based class repository. Future will include 
    the object [MEB: instance?] repository

Pegasus Client SDK - Tools for building Pegasus clients based on the Pegasus 
    C++ interfaces and using the WBEM HTTP/XML protocols or directly 
    interfacing with Pegasus.

Pegasus Test Clients - Simple test clients being developed as part of the 
    Pegasus development process

Pegasus HTML Test Client - To aid in testing we created a test client for 
    Pegasus that uses a WEB server (ex. Apache) with a set of CGI modules and 
    HTML to allow the entry of Pegasus operations from a WEB browser as forms 
    and the recept of the response as WEB pages. This has proven useful as a 
    test tool and can be used for a wide variety of demonstrations.

Pegasus Providers - Future (Version 1.0)

Pegasus Service Extensions -  Future (version 1.0)

Pegasus MOF Compiler - Future (Version 0.9)

Pegasus Dependencies
====================

We have worked to minimize the dependence of Pegasus on other software
packages and tools. Currently Pegasus has the following dependencies

1. ACE_Wrappers communication package - The ACE Wrappers package is used for 
communication and some low level functions within the MSB, the Pegasus client 
SDK, and the Pegasus Provider SDK.  ACE is available from the following site.

    http://ace.cs.wustl.edu/cvsweb/ace-cvs.cgi/ACE_wrappers/

Currently we are using version 5.1 of ACE_Wrappers both in Linux and Windows.

    http://www.cs.wustl.edu/~schmidt/ACE_wrappers/ACE-5.1.tar.gz

COMMENTS: Pegasus only uses limited facilities from ACE.  However, today we 
demand the installation of the complete ACE library. One of the action items 
for the future is to provide a more limited ACE library and possibly even to 
provide an environment independent of ACE. This will depend on the demands of 
the users.

2. To simplify the building of Pegasus across multiple platforms we have
standardized on a set of build tools including: GNUMAKE. We are using GNUAKE
4.79.1 successfully both in Windows and Linux environments.

GNUMAKE is available from

    http://www.gnu.org

ATTN: I think we need to make our version available on the TOG site.

COMMENT: Unless other users of Pegasus demand it, we will NOT create any build 
environment for Pegasus that does not require GNUMAKE (e.g., Visual C++). We 
are open to other developers who want to contribute other build structures for 
particular environments.


3. MU.EXE - To minimize the difference between Linux and Windows for GUNMAKE,
we have created a utility called MU.exe.  The source for this is available
with the distribution and the binary is distributed in the bin directory of
the the snapshots for now.

[MEB: the binary for MU.EXE is not distributed in the bin directory; you
have to build it at this time]

[MEB: MU.EXE is only needed on Windows]

[STOPPED]


The Pegasus Directory Structure
===============================

Pegasus is distributed as a complete directory structure that should be 
installed either from one of the snapshots or from CVS.

This structure is generally as follows

Pegasus                     Root directory
    bin
    build                   Destination for all intermediate files from build
        bin                 Destination for executable and DLL modules from 
			    Pegasus build
        lib
        obj
    cgi-bin                 Source and make for the Pegasus WEB Based Test client software
    doc                     Miscelaneous Pegasus Documents. Includes the DMTF XML for CIM 2.4
    html                    HTML files for the Browser test client.
    mak                     General make files (used by the root make and other makes)
    Repository		    This Directory containes the created repository
    src                     All Pegasus Source Files
        ACEExamples         Test directrory with examples of the use of ACE (developers)
        Pegasus
            CGI             CGI files for the WEB test client
                CGIClient
            Client          Pegasus Client SDK and Test client using the SDK
                depends
                tests
            Common          Pegasus Common Functions (C++ source and headers
                tests       Test programs for the common functions
            Protocol        Pegasus Client HTTP/XML Protocol Modules
                depends
            Repository      Pegasus Repository Interfaces and Simple Repository
                tests       Tests for Repository Functions
            Server          Pegasus Server Modules
                depends
                tests

        Utils
    manual                  Pegasus User/developer manual source modules
        HTML                Output from the Pegasus Manual compilartion.

In the near future we will add directories for:

    Service Extensions
    Providers


Installation
============

Pegasus today is provided only as a source distribution.  You download compile and use it.
Pegasus currently compiles and runs under both Linux and Windows NT environments.

The installation of Pegasus involves expanding the snapshot distribution files, building the
runtime, the test files and test clients, and building the repository.

NOTE: Since the compiler integration is not complete today, the class repository is populated
from the XML defintions for the complete CIM schema.  This process is included in the build
process so that at the end of a complete build, the class repository is completely populated
from the CIM 2.4 release schema.


Building from a Distribution --- General
========================================

Pegasus is buildable and runnable on either Linux or WIndows Platforms today.
It uses a common build system for the two platforms based on build files and
GUNMAKE.

Generally we support four targets in our make system:


            C:\> make clean
            C:\> make depend
            C:\> make
            C:\> make tests

The Pegasus Client server tests are executed separately from the above because they require
the initiation of separate process for the Pegasus server and Pegasus client. These tests can
be executed as follows:

    ATTN: Add the procedure for Client/Server Test start here.


For information on particular installation characteristics, tools, etc. for each platform see
the appropriate sections below:

Generally the build commands are as follows:

	1. There is a Makefile in the Pegasus root.   Simply executing make in the Pegasus
	root directory will make everything.

	2. In order to provide a working class repository for Pegasus until the MOF compiler
	is integrated, There is a utility to load the CIM Schema from XML into the repository.
	This executes the LoadRepository executable which is defined in

		pegasus/src/Pegasus/Repository/tests/LoadRepository

	with the binary in pegasus/bin. This program will load the CIM XML repository
	definitions into the class repository.

	3. To test a fresh release, go to the pegasus root and type

		"make world".


	This will clean, build dependencies, build binaries, and then run all
	tests except the Client/Server tests.

	4. To execute the basic test suite that is shipped with pegasus type

	   "make tests"

	5. To run the Client/Server tests provided, simply type

	   "make tests"

	Running "make -s tests" suppresses extraneous output such as the
	enter/leave directory messages.

	6. "Make Clean" removes all object and library files from the
	structure.

Build Variables
===============

The build system depends on the following environment variables

    PEGASUS_ACE_ROOT - required (points to ACE_wrappers directory). This must be set
    before you initiate make

    PEGASUS_BUILD - optional (points to build directory; defaults to build
    directory under root of pegasus distribution). This must be set before you iniiate
    make

    PEGASUS_OS_TYPE - optional. The is currently set to windows.  If you build on Linux
    you must change this before initiating make.

 COMMENTS: In the future, we will probably automate this more.  However, setting these
 variables today is manual.


The MU Utility
==============

In order to provide a consistent build structure across multiple platforms, we developed a
small utility to provide a consistent set of small utilities across these platforms. The MU
utilityis a simple utility that contains many commands. For example:


    C:\> mu rm myfile.cpp yourfile.cpp


You may type "mu" to get a list of valid commands. Here are some
of them:


    rm, rmdirhier, mkdirhier, echo, touch, pwd, copy, move, compare depend


The MU utility supports globing (expansion of wildcards) so
you can do things like this:


    C:\> mu rm *.obj *.exe

MU is required to build under the Windows environment.MU is available as part of the
distribution of Pegasus.

Building Pegausu on Linux
=========================

1. Obtain, unpack and build the ACE_Wrappers package for Linux.

2. Define an environment variable called ACE_ROOT  that points to the root of the ACE_WRAPPERS
distribution. For example

IMPORT ACE_ROOT=\local\ACE_Wrappers

ATTN: Mike.  what is a viable directory for Linux (what do you use)

3. Define an environment variable PEGASUS_ROOT that points to the
root of the Pegasus Distribution

4. Append %PEGASUS_ROOT%/bin to your path

5. Unpack the ace distribution.

ATTN: Unpack with what since ZIP.

5. Build using the make commands defined in the section on General Build.

	make world
	make tests

Building Pegasus on NT or Windows 2000 With Visual C++
=======================================================

Today we build Pegasus on Windows NT using a set of make files contained
in the source distribution, the Microsoft compilers (tested so far with
versions 5 and 6) and the GNUMAKE make utility. The following is the basic
set up steps for the


1.	Setup the environment variables and path for the Micrososft Visual C compiler.
Typically this can be done by running the VCVARS32.BAT file supplied with Microsoft Visual
C++. (contained in the same directory as cl.exe).

2.	Define an environment variable called ACE_ROOT that points to the
root of the ACE_Wrappers distribution. For example:

    C:\> set PEGASUS_ACE_ROOT=c:/ACE_wrappers

Now append %ACE_ROOT%/bin to the path like this:

    C:\> set path=%path%;%PEGASUS_ACE_ROOT%/bin

3.	Define an environment variable called PEGASUS_ROOT that points to
the root of the Pegasus distribution. For example:

    C:\> set PEGASUS_ROOT=c:/pegasus

Now append %PEGASUS_ROOT%/bin to your path like this:

    set path=%path%;%PEGASUS_ROOT%/bin

4.  Unpack and build the ACE_Wrappers distribution for windows.

As an example, we created a file called %ACE_ROOT%/ace/config.h
containing the following:

    #include "ace/config-win32.h"

and then opened this DSW file in MSVC++ IDE:

    %ACE_ROOT%/ace/ace.dsw

From the build menu we picked "aced.dll". For me it built without complaint.

BUG: there is still one open problem that has not been resolved.  With
the Visucal C++ version 6 compiler, the ace libraries default to

	acemfc.dll

With the version 5 visual C, apparently they default to the names

	ace.dll
	or
	aced.dll (debug verions

We will correct this in a future snapshot.


5. Unpack the Pegasus distribution.  On Windows systems, PKzip25 or WINZIP can be used.
Be certain to use the option that expands the directory tree of the files.  This expansionwill
create a directory called ./pegasus

If you are using pkzip25.exe do this:
    C:\> pkzip25 -extract -recurse -directories pegasus.zip

This will create a directory tree called ./pegasus under the
current directory.

ATTN: Define for Linux also

6.  Change directory  to %PEGASUS_ROOT% and type "make world". See the general build section
for the full set of make commands.

This builds Pegasus and all of its examples.

7.  To verify that it built okay, run the tests

	make tests

Installing the Pegasus HTML Test Client
=======================================

This is a separate test tool that allows Pegasus requests to be initiated from any WEB browser
and that uses a WEB browser, CGI scritps and HTML pages for the formating and connections. It
requires a WEB server, etc.  The instructions for setting up this environment are maintained
in a separate readme in the CGI directory.

Development with Pegasus and Pegasus Tools
==========================================

ATTN: This section needs to be completed.  It should reference the more
complete documentation

===================
Documentation

The documentation is currently in preperation. The preliminary documentation is not provided
with this snapshot but is avialable from the OpenGroup Pegasus WEB pages. The current
documentation is maintained both as a manual created under the tool DOC++ in the
subdirectory manual/html and as other miscelaneous documentation in the doc directory.

Note that the Pegasus WEB site at The Open Group will be the source of most documentation in
the future and today is the source of most discussion and design documentation.

Participate!
============
We are looking for people who want to join the effort of getting this
next release off the ground.  Please send me email for details.












