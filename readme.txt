Pegasus - A Manageability Services Broker for the DMTF CIM/WBEM Standards

Author: Mike Brasher, Karl Schopmeyer

Tagline: Pegasus is an object manager for DMTF CIM objects written in C++
         and supported by The Open Group

Overview:
=========

Pegasus is an open-source object manager for DMTF CIM objects. It is written
in C++ and includes the Object manager, a set of defined interfaces, and SDKs
for both client, providers, and services extensions.  It is maintained
consistent with the DMTF CIM and WBEM specifications except for any exceptions
noted in the documentation.

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

Availability of Pegasus:
========================

Pegasus is distributed as open source.  Today the distribution is limited
because we want to reach a predefined level of development before making the
project publically available. The move from limited to open distribution will
be a decision of the Pegasus development team and The Open Group and should
occur before March 2001.

To get on the distribution list for Pegasus today, send an email to

    m.kirk@opengroup.org

    or

    k.schopmeyer@opengroup.org

Pegasus Major Components:
=========================

The major components of Pegasus are:

Pegasus Server - WBEM/CIM Server with interfaces for providers and clients

Pegasus Repositories - Today Pegasus provides a defined class repository
    interface and a simple file based class repository. It also includes
    an instance repository. Note that these repository were created for
    functionality, not efficieny. It is expected that they will be replaced
    with other implementations of the respository function as the need arrses.

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

Pegasus Provider SDK - Tools for build Pegasus clients based on the Pegasus
    C++ interfaces.

Pegasus Providers - A sampling of key providers to make the environment
complete as well as test providers and demonstration providers for the
Pegasus.

Pegasus Service Extensions -  Future (version 1.0)

Pegasus MOF Compiler - FA standalone compiler for MOF files that can be used
to install MOF into the Pegasus schema repository and also to check syntax

Pegasus Dependencies:
=====================

We have worked to minimize the dependence of Pegasus on other software
packages and tools. Currently Pegasus has the following dependencies

1.  GNUMAKE - To simplify the building of Pegasus across multiple platforms we 
    have standardized on a set of build tools including: GNUMAKE. We are using 
    GNUAKE 4.79.1 successfully both in Windows and Linux environments.

    GNUMAKE is available from

    http://www.gnu.org

    ATTN: I think we need to make our version available on the TOG site.

    COMMENT: Unless other users of Pegasus demand it, we will NOT create 
    any build environment for Pegasus that does not require GNUMAKE (e.g., 
    Visual C++). We are open to other developers who want to contribute 
    other build structures for particular environments.

2.  MU.EXE - To minimize the difference between Linux and Windows for GUNMAKE,
    we have created a utility called MU.exe.  This utility is required for 
    Pegasus make with Windows environment. It is provided as an alternative to 
    requiring a number of UNIX utilities (SH, RM, etc.) on the windows platform
    and effectively provides the functions of these utilities that GNUMAKE 
    needs. MU is not required on UNIX or LINUX platforms.

    NOTE: The binary for MU.EXE is not distributed in the Pegasus bin directory.
    You must build it separately.  MU source code is part of the distribution
    in the directory src/utils/MU with its own make file.  You must compile MU
    before you initiate the Pegausu make.

    NOTE: We will make a copy of the binary available on the MSB WEB site to
    eliminate the requirement to build this utility.

3.  FLEX and BISON - These tools were used to develop the compiler, anybody
    intending to recompile the compiler from scratch will be required to have
    these tools.

4.  DOC++ - The Pegasus documentation is taken from a combination of text files
    and the Pegasus header files themselves.  This documentation is formatted 
    with DOC++ and GAWK.  These tools are required if the documentation is to 
    be recreated but we expect that only the core team will be recreating 
    documentaiton.

The Pegasus Directory Structure
===============================

Pegasus is distributed as a complete directory structure that should be
installed either from one of the snapshots or from CVS.

This structure is generally as follows

Pegasus                     Pegasus Root directory


    build                   Default destination for runtime if PEGASUS_HOME
    			    is not specified.  Does not exist if PEGASUS_HOME
    			    is specified


    cgi                     Source for the Pegasus WEB Based Test client
        cgi-bin		    CGI Directories for WEB demonstration.
        		    This directory is normally empty but can
        		    be populated from the runtime with the make
        		    from cgi.
        htdocs		    HTML Pages for Pegasus WEB emonstration
    doc                     Miscellaneous Pegasus Documents.
    	DevManual	    Source and build files for developes manual
    mak                     General make files (used by other makes)
    src                     All Pegasus Source Files
        ACEExample         Test directrory with examples of the use of ACE
        Clients		    Source for various test clients and client SDK
            CGICLIENT       Pegasus test client that uses a WEB browser
        Pegasus
            Client          Pegasus Client API Tests
                depends
                tests
            Common          Pegasus Common Functions (C++ source and headers
                tests       Test programs for the common functions
            Protocol        Pegasus Client HTTP/XML Protocol Modules
                depends
            Provider	    Pegasus Provider interface functions
            Repository      Pegasus Repository Interfaces and Simple
		            Repository
                tests       Tests for Repository Functions
            Server          Pegasus Server Modules
        Providers	    Pegasus test and required providers

        Utils

Runtime Directories
     $PEGASUS_HOME$	    Home directory for runtime.  All compiler, linker
                            documentation creation, etc. is put here.
	bin                 Destination for executable and DLL modules from
				    Pegasus build
	Manual		    HTML output of the Pegasus Manual
	lib		    Destination for Pegasus LIB modules
	obj		    Destination for object modules
        Repository	    This Directory containes the created repository


In the near future we will add directories for:

    Service Extensions
    Servers - Today the server executable is built from /server/test.

Installation:
=============

Pegasus today is provided only as a source distribution.  You download compile
and use it. Pegasus currently compiles and runs under Linux, AIX, HPUX,
Windows NT, and Windows 2000.

The installation of Pegasus involves expanding the snapshot distribution
files, building the runtime, the test files and test clients, and building the
repository.

Building Pegasus:
=================

1.  Check that you have requisite programs (listed in Pegasus Dependencies).

    These include GNU Make, MU.EXE, Flex, and Bison

    Be sure these are on your path.

2.  Define the following two environment variables:

    PEGASUS_HOME - to point to a directory to contain output binary files
    (e.g., set it to $HOME/pegasus_home).

    PEGASUS_PLATFORM - this must be set to a supported platform identifier.
    The following are supported:

	WIN32_IX86_MSVC 
	LINUX_IX86_GNU 
	AIX_RS_IBMCXX 
	HPUX_PARISC_ACC

    This identifier has the following form:

	<Operating-System>_<Architecture>_<Compiler>

    Note: if you plan on doing parallel builds, you might consider setting
    PEGASUS_HOME to something like this:

	$HOME/pegasus_home_LINUX_IX86_GNU

    That way, the output of each build will be placed in its own directory.

3.  Now place $PEGASUS_HOME/bin on your path

    and

    Placd $PEGASUS_HOME/lib on your LD_LIBRARY_PATH (for Unix only).

4.  Now change to the root of the Pegasus distrubution and type "make"
    (where make referes to GNU make).

5.  Then create the repository, which is needed for some test cases -
    "make repository"

6.  To test the build type "make tests".

The following make targets are supported:

    <default> - Build everything.
    clean - Clean out all objects, libs, and executables.
    depend - Create the dependencies.
    repository - Create the repository in $PEGASUS_HOME/repository
    tests  - Execute all tests (except client server tests).
    rebuild - clean, depend, <default>
    world - depend, <default>

The Pegasus Client server tests are executed separately from the above because
they require the initiation of separate process for the Pegasus server and
Pegasus client. These tests can be executed as follows:

    ATTN: Add the procedure for Client/Server Test start here.

For information on particular installation characteristics, tools, etc. for
each platform see the appropriate sections below:

Generally the build commands are as follows:

    1. There is a Makefile in the Pegasus root directory.   Simply executing
    make in the Pegasus root directory will make everything. "Make rebuild"
    will clean and rebuild everything. The "make rebuild" will also populate
    the repository with the current CIM Schemas.

    2. To test a fresh release, go to the pegasus root and type

		"make world".


    This will build dependencies, build binaries, and then run all
    tests except the Client/Server tests.

    3. To execute the basic test suite that is shipped with pegasus type

	   "make tests". This also reintalls the repository.

    Running "make -s tests" suppresses extraneous output such as the
    enter/leave directory messages.

    4. "Make Clean" removes all object and library files from the structure.

The MU Utility
==============

In order to provide a consistent build structure across multiple platforms, we
developed a small utility to provide a consistent set of small utilities
across these platforms. The MU utilityis a simple utility that contains many
commands. For example:


    C:\> mu rm myfile.cpp yourfile.cpp


You may type "mu" to get a list of valid commands. Here are some
of them:


    rm, rmdirhier, mkdirhier, echo, touch, pwd, copy, move, compare depend


The MU utility supports globing (expansion of wildcards) so
you can do things like this:


    C:\> mu rm *.obj *.exe

MU is required to build under the Windows environment.MU is available as part
of the distribution of Pegasus.

Notes about Building Pegasus on Linux:
======================================

ATTN: put something here (but only what is not mentioned above).

Building Pegasus on NT or Windows 2000 With Microsoft Visual C++
================================================================

Today we build Pegasus on Windows NT using a set of make files contained
in the source distribution, the Microsoft compilers (tested so far with
versions 5 and 6) and the GNUMAKE make utility. The following is the basic
set up steps for the

Setup the environment variables and path for the Micrososft Visual C
compiler. Typically this can be done by running the VCVARS32.BAT file
supplied with Microsoft Visual C++. (contained in the same directory
as cl.exe).

Installing the Pegasus HTML Test Client:
========================================

This is a separate test tool that allows Pegasus requests to be initiated from
any WEB browser and that uses a WEB browser, CGI scritps and HTML pages for
the formating and connections. It requires a WEB server, etc.  The
instructions for setting up this environment are maintained in a separate
readme in the CGI directory.

Development with Pegasus and Pegasus Tools:
===========================================

ATTN: This section needs to be completed.  It should reference the more
complete documentation

Documentation:
==============

The documentation is currently in preperation. The preliminary documentation
is not provided with this snapshot but is avialable from the OpenGroup Pegasus
WEB pages. The current documentation is maintained both as a manual created
under the tool DOC++ in the runtime subdirectory manual/html and as other
miscelaneous documentation in the doc directory.

Note that the Pegasus WEB site at The Open Group will be the source of most
documentation in the future and today is the source of most discussion and
design documentation.

Participate!
============

We are looking for people who want to join the Pegasus work group and
contribute to effort of getting this Pegasus off the ground.  Please send
email for details to k.schopmeyer@opengroup.org or m.kirk@opengroup.org or
m.brasher@opengroup.org
