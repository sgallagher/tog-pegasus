
OpenPegasus - A Manageability Services Broker for the DMTF CIM/WBEM Standards

Author: Mike Brasher, Karl Schopmeyer

Tagline: OpenPegasus is an object manager for DMTF CIM objects written in C++
         and supported by The Open Group

STATUS: Revised June 2003 to match Pegasus release 2.2
Contents:
=========

1.  Overview
2.  Availability of Pegasus
3.  Pegasus major components
4.  Pegasus Dependencies
5.  The Pegasus Directory Structure
6.  Installation
7.  Building Pegasus
8.  Populate the Repository
9.  The MU Utility
10. Notes about Building Pegasus on Linux
11. Notes on building Pegasus with SSL
12. Building Pegasus on Windows 2000 or Windows XP With Microsoft Visual C++
13. Installing the Pegasus HTML Test Client
14. Development with Pegasus and Pegasus Tools
15. Commands
16. Creating SSL certifications.
17. Documentation
18. Participate




1. Overview:
============

OpenPegasus (also refered to as Pegasus):

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

This distribution represents a snapshot of the current work. Currently Pegasus
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

http://www.openpegasus.org


2. Availability of Pegasus:
============================

Pegasus is distributed as open source.  The distribution is available via 
CVS and snapshot images in tar and zip file formats.

The source code from CVS can be found at the following Open Group CVS server;

         cvs.opengroup.org:/cvs/MSB 

using the password authenticating server option (pserve).

Anonymous access for read is with the name and password "anon" as follows:

	%export CVSROOT=:pserver:anon@cvs.opengroup.org:/cvs/MSB
	%cvs login
	  
         When requested, enter the password "anon"

The source tree is in the directory pegasus. To check out the complete Pegasus  
source tree just type:

         cvs co pegasus

A Pegasus directory will be created under the current directory and populated
with the complete source tree and documentation. To get the latest updates 
after a checkout just type this from Pegasus root:

         cvs update -d


Active contributors to Pegasus have write access to the CVS repository.
 If you are interested in contributing back to the Pegasus project, 
(i.e. write (checkin) access to CVS) please request access from either
Martin Kirk (m.kirk@opengroup.org) or Karl Schopmeyer (k.schopmeyer@opengroup.org).


3. Pegasus Major Components:
=============================

The major components of Pegasus are:

Pegasus Server - WBEM/CIM Server with interfaces for providers and clients

Pegasus Repositories - Today Pegasus provides a defined class repository
    interface and a simple file based class repository. It also includes
    an instance repository. Note that these repository were created for
    functionality, not efficieny. It is expected that they will be replaced
    with other implementations of the respository function as the need arises.

Pegasus Client SDK - Tools for building Pegasus clients based on the Pegasus
    C++ interfaces and using the WBEM HTTP/XML protocols or directly
    interfacing with Pegasus.

Pegasus Test Clients - Simple test clients being developed as part of the
    Pegasus development process

Pegasus HTML Test Client - To aid in testing we created a test client for
    Pegasus that uses a WEB server (ex. Apache) with a set of CGI modules and
    HTML to allow the entry of Pegasus operations from a WEB browser as forms
    and the receipt of the response as WEB pages. This has proven useful as a
    test tool and can be used for a wide variety of demonstrations.

Pegasus Provider SDK - Tools for building Pegasus providers using the Pegasus
    C++ interfaces.

Pegasus Providers - Providers to illustrate the use of Pegasus services including 
providers for test and demonstration.

Pegasus Service Extensions -  Common services for use by other Pegasus 
components to extend Pegasus capabilites. 

Pegasus MOF Compiler - FA standalone compiler for MOF files that can be used
to install MOF into the Pegasus schema repository and also to check syntax. 
There is also a tool to extract the MOF from the repository.


4. Pegasus Dependencies:
=========================

We have worked to minimize the dependence of Pegasus on other software
packages and tools. Currently Pegasus has the following dependencies:

1.  GNUMAKE - To simplify the building of Pegasus across multiple platforms we 
    have standardized on a set of build tools including: GNUMAKE. We are using 
    GNUMAKE 3.79.1 successfully both in Windows and Linux environments.

    GNUMAKE is available from

    http://www.gnu.org


2.  MU.EXE - To minimize the difference between Linux and Windows for GNUMAKE,
    we have created a utility called MU.exe.  This utility is required for 
    Pegasus make with ONLY Windows environment. It is provided as an alternative to 
    requiring a number of UNIX utilities (SH, RM, etc.) on the windows platform
    and effectively provides the functions of these utilities that GNUMAKE 
    needs. MU is not required on UNIX or LINUX platforms.

    NOTE: The binary for MU.EXE is not distributed in the Pegasus bin directory.
    You must build it separately.  MU source code is part of the distribution
    in the directory src/utils/MU with its own make file.  You must compile MU
    before you initiate the Pegausu make.

    NOTE: A copy of the binary is made available as a zip file on the Pegasus
    WEB site.

    Again, MU is used ONLY if you are using Windows.


3.  FLEX and BISON - These tools were used to develop the MOF compiler and WQL 
    parser. Anybody intending to recompile the compiler or parser from scratch 
    will be required to have these tools. They are only required if changes need 
    to be made to the files for parsing and compiling.

4.  DOC++ - The Pegasus documentation is taken from a combination of text files
    and the Pegasus header files themselves.  This documentation is formatted 
    with DOC++ and GAWK.  These tools are required if the documentation is to 
    be recreated but we expect that only the core team will be recreating 
    documentaiton.


5. The Pegasus Directory Structure
===================================

Pegasus is distributed as a complete directory structure that should be
installed either from one of the snapshots or from CVS.

This structure is generally as follows

Pegasus                     Pegasus Root directory

    cgi                     Source for the Pegasus WEB Based Test client
        cgi-bin             CGI Directories for WEB demonstration.
                                This directory is normally empty but can
                                be populated from the runtime with the make
                                from cgi.
        htdocs              HTML Pages for Pegasus WEB emonstration
    doc                     Miscellaneous Pegasus Documents.
    DevManual               Source and build files for developers' manual
    mak                     General make files (used by other makes)
    src                     All Pegasus Source Files
        ACEExample          Test directrory with examples of the use of ACE
        Clients             Source for various test clients and client SDK
            CGICLIENT       Pegasus test client that uses a WEB browser
        Pegasus
            Client          Pegasus Client API Tests
                depends
                tests
            Common          Pegasus Common Functions (C++ source and headers
                tests       Test programs for the common functions
            Protocol        Pegasus Client HTTP/XML Protocol Modules
                depends
            Provider        Pegasus Provider interface functions
            Repository      Pegasus Repository Interfaces and Simple Repository
                tests       Tests for Repository Functions
            Server          Pegasus Server Modules
        Providers           Pegasus test and required providers

        Utils

Runtime Directories
     $PEGASUS_HOME$         Home directory for runtime.  All compiler, linker
                            documentation creation, etc. is put here.
     bin                    Destination for executable and DLL modules from
                                Pegasus build
     Manual                 HTML output of the Pegasus Manual
     lib                    Destination for Pegasus LIB modules
     obj                    Destination for object modules
     repository             This Directory contains the created repository


6. Installation
================

Pegasus today is provided only as a source distribution. 

To install Pegasus, you must check it out using CVS (Common Version System)
or download the snapshot. You download, compile, and use it. 

For the snapshot, the installation of Pegasus involves expanding the snapshot 
distribution files, building the runtime, the test files and test clients, and 
building the repository.


7. Building Pegasus
=====================

1.  Check that you have requisite programs (listed in Pegasus Dependencies).

    These include GNU Make, MU.EXE (if using Windows), Flex, and Bison (Flex 
    and Bison only required if changes will be made to the MOF compiler or WQL 
    parser).

    Be sure these are on your path.


2.  Define the following three environment variables:

    PEGASUS_ROOT - this should be the "pegasus" directory you've pulled from CVS

    PEGASUS_HOME - to point to a directory to contain output binary files
    (e.g., set it to $HOME/pegasus_home). Then the output will go into
    $HOME/pegasus_home/bin and $HOME/pegasus_home/lib

    PEGASUS_PLATFORM - this must be set to a supported platform identifier.

    This identifier has the following form:

		 <Operating-System>_<Architecture>_<Compiler>

    For example (Linux on IA32 platform using the GNU Compiler):
                 LINUX_IX86_GNU

    For a complete list, refer to the platform_ make files found in directory 
    pegasus/mak


    Note: if you plan on doing parallel builds, you might consider setting
    PEGASUS_HOME to something like this:

		 $HOME/pegasus_home_LINUX_IX86_GNU

    That way, the output of each build will be placed in its own directory.

3.  Now place $PEGASUS_HOME/bin on your path

    and

    Place $PEGASUS_HOME/lib on your LD_LIBRARY_PATH (for Unix only).
    For RedHat/SuSE/UL, edit /etc/ld.so.conf and add $PEGASUS_HOME/lib

4.  Change to the root of the Pegasus distrubution and type "make"
    (where make refers to GNU make).

5.  Then create the repository, which is needed to serve data.
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
Pegasus client. To execute these tests please refer to the scripts in 
pegasus/mak/BuildMakefile - refer to the prestarttests and poststarttests.

For information on particular installation characteristics, tools, etc. for
each platform see the appropriate sections below:

Generally the build commands are as follows:

    1. There is a Makefile in the Pegasus root directory.   Simply executing
    make in the Pegasus root directory will make everything. "make rebuild"
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

    4. "make clean" removes all object and library files from the structure.


    5. A new build system has been added to Pegasus where a new CVS checkout is done,
       built, and tests are run. Do it by: "make -f mak/BuildMakefile cleanbuild"


8. Populate the Repository
===========================

Before using Pegasus you must populate the repository with the providers. The makefile 
does it all for you, but in case you are wondering what it does or how to do it 
manually:

1. Register the MOF (Managed Object Format) file describing the skeleton of the object.
2. Register a second MOF which only points out which lib*.so file to be loaded when a 
   specific object is activated.

This is done automatically for the providers included in Pegasus by doing:
make repository.

The 'make repository' in pegasus/Schemas does three things
Runs MOF compiler (cimmofl) on:
-Schema v2.7
    Generates the CIM Schema v2.7 in the repository  (skeleton of CIM objects)
-PG_InterOp
    Internal to Pegasus schema for operating (shutdown, add users, etc)
    CIM_Indication’s (SNMP, Events, Alert, Threshold, etc)
-PG_ManagedSystem
    Registers included CIM Providers (libOSProvider.so, libDNSProvider.so, … ) 
    in Pegasus (which are located in src/Providers)

For more information about using the MOF compiler, refer to user's manual on the 
openpegasus.org.  


9. The MU Utility
===================

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


10. Notes about Building Pegasus on Linux
===========================================

No problem. Just make sure you have the environment variables set.


11. Notes about Building Pegasus with SSL
==========================================

To build with SSL you need the OpenSSL libraries and header files. Make sure 
you have them in a standard directory so Pegasus can find them. If that's not 
the case, set the environment varialble OPENSSL_HOME= to point where your OpenSSL
installation is.

Also have the PEGASUS_HAS_SSL=yes variable set. Then just run 'make' in Pegasus
directory and you will have Pegasus with SSL enabled. See section "Creating SSL 
certificates" for more information of how to use SSL.


12. Building Pegasus on Windows 2000 (SP3 or later recommended) or Windows XP
    With Microsoft Visual C++
======================================================================

Today we build Pegasus on Windows using a set of make files contained
in the source distribution, the Microsoft compilers (DevStudio 5.x is not 
supported, Visual Studio 6.0, SP5 supported) and the GNUMAKE make utility.The 
following is the basic setup steps for the environment.

 
Setup the environment variables and path for the Micrososft Visual C compiler. 
Typically this can be done by running the VCVARS32.BAT file supplied with 
Microsoft Visual C++. (contained in the same directory as cl.exe).

For Windows, try the following for an example environment:

    REM call the standard Microsoft .bat for VC 6 setup. 
    call 'C:/Program Files/Microsoft Visual Studio/VC98/Bin/Vcvars32.bat' 
    REM Set debug to something if you want compile in debug mode 
    set PEGASUS_DEBUG=true 
    REM set PEGASUS_ROOT to top of source tree 
    set PEGASUS_ROOT=C:/cimom/pegasus 
    REM set PEGASUS_HOME to where you want repository and executables
    set PEGASUS_HOME=%PEGASUS_ROOT% 
    REM setup the path to the runtime files. 
    set path=%path%;%PEGASUS_HOME%/bin 

13. Installing the Pegasus HTML Test Client:
=============================================

This is a separate test tool that allows Pegasus requests to be initiated from
any WEB browser and that uses a WEB browser, CGI scritps and HTML pages for
the formating and connections. It requires a WEB server, etc.  The
instructions for setting up this environment are maintained in a separate
readme in the CGI directory.


14. Development with Pegasus and Pegasus Tools:
================================================

ATTN: This section needs to be completed.  It should reference the more
complete documentation
ATTN: Write about providers?


15. Commands:
=============

The manpages for each of the commands are in rpm/manLinux/man1.Z directory (on CVS)

To see simple help for each of the commands, use the "-h" flag.
Examples:
bin/cimserver –s (Shuts it down)
bin/cimserver traceLevel=4 traceComponents=ALL (starts server with config flags)
bin/cimprovider –l –s (lists providers and their status)
bin/cimprovider –e –m OperatingSystemModule (enables the OperatingSystem provider)
bin/cimuser –a –u guest –w ThePassword
bin/cimuser –l (lists the users)
bin/tomof CIM_Config (extract CIM_Config from repository and present it in MOF type)


16. Creating SSL certifications
================================

Type these commands in your shell to create the SSL certifications. The PEGASUS_ROOT 
and PEGASUS_HOME have to be set to your respective installation and source directory. 

CN="Common Name"
EMAIL="test@email.address"
HOSTNAME=`uname -n`
sed -e "s/$CN/$HOSTNAME/"  \
    -e "s/$EMAIL/root@$HOSTNAME/" $PEGASUS_ROOT/ssl.cnf \
    > $PEGASUS_HOME/ssl.cnf
chmod 644 $PEGASUS_HOME/ssl.cnf
chown bin $PEGASUS_HOME/ssl.cnf
chgrp bin $PEGASUS_HOME/ssl.cnf

/usr/bin/openssl req -x509 -days 365 -newkey rsa:512 \
 -nodes -config $PEGASUS_HOME/ssl.cnf \
   -keyout $PEGASUS_HOME/key.pem -out $PEGASUS_HOME/cert.pem 

cat $PEGASUS_HOME/key.pem $PEGASUS_HOME/cert.pem > $PEGASUS_HOME/server.pem
rm $PEGASUS_HOME/key.pem $PEGASUS_HOME/cert.pem
cp $PEGASUS_HOME/cert.pem $PEGASUS_HOME/client.pem


17. Documentation:
===================

The documentation is currently in preperation. The preliminary documentation
is not provided with this snapshot but is avialable from the OpenGroup Pegasus
WEB pages. The current documentation is maintained both as a manual created
under the tool DOC++ in the runtime subdirectory manual/html and as other
miscelaneous documentation in the doc directory.

Note that the Pegasus WEB site at The Open Group will be the source of most
documentation in the future and today is the source of most discussion and
design documentation.


18. Participate!
=================

We are looking for people who want to join the Pegasus work group and
contribute to effort of getting this Pegasus off the ground. Please join 
the mailing list by visiting www.openpegasus.org, and click on Mailing Lists.
