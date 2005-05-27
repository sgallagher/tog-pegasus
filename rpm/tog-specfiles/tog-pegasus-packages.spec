%package sdk
Summary: The OpenPegasus Software Development Kit
Group: Systems Management/Base
Autoreq: 0
Requires: tog-pegasus >= %{version}

%description sdk
The OpenPegasus WBEM Services for Linux SDK is the developer's kit for the OpenPegasus WBEM
Services for Linux release. It provides Linux C++ developers with the WBEM files required to
build WBEM Clients and Providers. It also supports C provider developers via the CMPI interface.

%if %{PEGASUS_BUILD_TEST_RPM}
%package test
Summary: The OpenPegasus Tests
Group: Systems Management/Base
Autoreq: 0
Requires: tog-pegasus >= %{version} 

%description test
The OpenPegasus WBEM tests for the OpenPegasus %{version} Linux rpm.
%endif

