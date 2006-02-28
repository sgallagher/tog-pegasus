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
