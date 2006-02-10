#//%2006////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; Symantec Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-intro.spec
#
%{?!PEGASUS_BUILD_TEST_RPM:   %define PEGASUS_BUILD_TEST_RPM        0}
# do "rpmbuild --define 'PEGASUS_BUILD_TEST_RPM 1'" to build test RPM.
#
%{?!AUTOSTART:   %define AUTOSTART        0}
# Use "rpm -[iU]vh --define 'AUTOSTART 1'" in order to have cimserver enabled
# (chkconfig --level=345 tog-pegasus on) after installation.
#

%define srcRelease 1
Summary:   OpenPegasus WBEM Services for Linux
Name:      tog-pegasus
Group:     Systems Management/Base
License:   Open Group Pegasus Open Source
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
URL:       http://www.openpegasus.org

Source:    %{name}-%{version}-%{srcRelease}.tar.gz

BuildRequires:      bash, sed, grep, coreutils, procps, gcc, gcc-c++
BuildRequires:      libstdc++, make, pam-devel
BuildRequires:      openssl-devel >= 0.9.6, e2fsprogs
Requires:           bash, sed, grep, coreutils, procps, openssl >= 0.9.6, pam
#Requires:          krb5-libs, redhat-lsb, chkconfig, SysVinit, bind-libs
Requires:           e2fsprogs, bind-utils, net-tools
Requires(post):     bash, sed, grep, coreutils, procps, openssl >= 0.9.6, pam
#Requires(post):    krb5-libs, redhat-lsb, chkconfig, SysVinit, bind-libs
Requires(post):     e2fsprogs, bind-utils, net-tools
Requires(pre):      bash, sed, grep, coreutils, procps, openssl >= 0.9.6, pam
#Requires(pre):     krb5-libs, redhat-lsb, chkconfig, SysVinit, bind-libs
Requires(pre):      e2fsprogs, bind-utils, net-tools
Requires(postun):   bash, sed, grep, coreutils, procps, openssl >= 0.9.6, pam
#Requires(postun):  krb5-libs, redhat-lsb, chkconfig, SysVinit, bind-libs
Requires(postun):   e2fsprogs, bind-utils, net-tools

Conflicts: openwbem
Provides: tog-pegasus-cimserver
BuildConflicts: tog-pegasus

%description
OpenPegasus WBEM Services for Linux enables management solutions that deliver
increased control of enterprise resources. WBEM is a platform and resource
independent DMTF standard that defines a common information model and
communication protocol for monitoring and controlling resources from diverse
sources.
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-intro.spec
