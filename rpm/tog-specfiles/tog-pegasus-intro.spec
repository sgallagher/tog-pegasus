Summary: OpenPegasus WBEM Services for Linux
Name: tog-pegasus
Group: Systems Management/Base
Copyright: Open Group Pegasus Open Source
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}
Source: ftp://www.opengroup.org/pegasus/tog-pegasus-%{version}-%{srcRelease}.tar.gz
Requires: openssl >= 0.9.6 lsb >= 1.3
Provides: tog-pegasus-cimserver
BuildRequires: openssl-devel >= 0.9.6

%description
OpenPegasus WBEM Services for Linux enables management solutions that deliver
increased control of enterprise resources. WBEM is a platform and resource
independent DMTF standard that defines a common information model and
communication protocol for monitoring and controlling resources from diverse
sources.
