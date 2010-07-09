# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-buildRequires.spec
#
BuildRequires:      bash, sed, grep, coreutils, procps, gcc, gcc-c++
BuildRequires:      libstdc++, make, pam-devel
BuildRequires:      openssl-devel >= 0.9.6, e2fsprogs
%if %{EXTERNAL_SLP_REQUESTED}
BuildRequires:      openslp
Requires:           openslp
%endif

