#
# spec file for package pegasus-wbem (Version 2.3.2)
#
# Copyright (c) 2004 SUSE LINUX AG, Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments via http://www.suse.de/feedback/
#

# neededforbuild  doc++ gpp libgpp openssl openssl-devel pam-devel

BuildRequires: aaa_base acl attr bash bind-utils bison bzip2 coreutils cpio cpp cracklib cvs cyrus-sasl db devs diffutils e2fsprogs file filesystem fillup findutils flex gawk gdbm-devel glibc glibc-devel glibc-locale gpm grep groff gzip info insserv kbd less libacl libattr libgcc libselinux libstdc++ libxcrypt m4 make man mktemp module-init-tools ncurses ncurses-devel net-tools netcfg openldap2-client openssl pam pam-modules patch permissions popt procinfo procps psmisc pwdutils rcs readline sed strace syslogd sysvinit tar tcpd texinfo timezone unzip util-linux vim zlib zlib-devel autoconf automake binutils doc++ gcc gcc-c++ gdbm gettext libstdc++-devel libtool openssl-devel pam-devel perl rpm

Name:         pegasus-wbem
URL:          http://www.openpegasus.org/
Summary:      An open-source implementation of the DMTF CIM and WBEM standards
Version:      2.3.2
Release:      35.25
Group:        System/Management
License:      Other uncritical OpenSource License, Other License(s), see package
Provides:     cim-server
Source:       ftp://www.opengroup.org/pegasus/pegasus-%{version}.tar.bz2
Source1:      pegasus-wbem
Source2:      loadmof.sh
Source3:      rmmof.sh
PreReq:       openssl, /bin/cat, /bin/chmod, /bin/chgrp, /bin/cp, %insserv_prereq
BuildRoot:    %{_tmppath}/%{name}-%{version}-build
Conflicts:    openwbem
Patch1:       pegasus-wbem.dif
Patch2:       pegasus-2.3.2.dif
Patch3:       lib64.dif
Patch4:       pegasus-2.3.2-semfix.dif

%description
Pegasus is an open-source implementation of the DMTF CIM and WBEM
standards. It is designed to be portable and highly modular. It is
coded in C++ so that it effectively translates the object concepts of
the CIM objects into a programming model but still retains the speed
and efficiency of a compiled language.



Authors:
--------
    The Open Group

%package devel
Summary:      Header files and additional documentation for Pegasus WBEM
Group:        System/Management
Requires:     pegasus-wbem >= %{version}, openssl-devel >= 0.9.7

%description devel
Header files and additional documentation for Pegasus WBEM



Authors:
--------
    The Open Group


%preun
%stop_on_removal pegasus-wbem 
%prep
%setup -q -n pegasus-%{version}
%patch1
%patch2 -p1
%ifarch x86_64 ppc64 s390x
%patch3
%endif
%patch4 -p1

%build
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;
export PEGASUS_ROOT=$RPM_BUILD_DIR/pegasus-$RPM_PACKAGE_VERSION
export PEGASUS_HOME=$PEGASUS_ROOT/pegasus-build
ln -sf $RPM_BUILD_DIR/pegasus-$RPM_PACKAGE_VERSION $RPM_BUILD_DIR/pegasus
export PEGASUS_DISPLAYCONSUMER_DIR=/use/%_lib/pegasus
%ifarch ia64
export PEGASUS_PLATFORM=LINUX_IA64_GNU
%else
export PEGASUS_PLATFORM=LINUX_IX86_GNU
%endif
export PEGASUS_HAS_SSL=yes
export PEGASUS_USE_RELEASE_DIRS=true
export ENABLE_PROVIDER_MANAGER2=true
export ENABLE_CMPI_PROVIDER_MANAGER=true
export PEGASUS_LOCAL_DOMAIN_SOCKET=true
export PEGASUS_PAM_AUTHENTICATION=true
export PEGASUS_USE_PAM_STANDALONE_PROC=true
export PEGASUS_USE_SYSLOGS=true
export PEGASUS_USE_RELEASE_DIRS=true
#export PEGASUS_USE_RELEASE_CONFIG_OPTIONS=true
export LD_RUN_PATH=/usr/%_lib/pegasus
make --directory=mak -f SDKMakefile  stageSDK 
make

%install
rm -rf $RPM_BUILD_ROOT
export PEGASUS_ROOT=$RPM_BUILD_DIR/pegasus-$RPM_PACKAGE_VERSION
export PEGASUS_HOME=$PEGASUS_ROOT/pegasus-build
#
# Make directories
mkdir -p $RPM_BUILD_ROOT/var/log/pegasus
mkdir -p $RPM_BUILD_ROOT/var/cache/pegasus/localauth
mkdir -p $RPM_BUILD_ROOT/var/cache/pegasus/providercache
mkdir -p $RPM_BUILD_ROOT%{_mandir}/{man1,man8}
mkdir -p $RPM_BUILD_ROOT/etc/pegasus/mof
#
# Programs
install -d $RPM_BUILD_ROOT/usr/sbin
install -d $RPM_BUILD_ROOT/usr/bin
#
# Init scripts
install -D -m 0755  %{S:1} $RPM_BUILD_ROOT/etc/init.d/pegasus-wbem
install -d $RPM_BUILD_ROOT/usr/sbin
install -d $RPM_BUILD_ROOT/usr/bin
install -m 0755  %{S:2} $RPM_BUILD_ROOT/usr/bin
install -m 0755  %{S:3} $RPM_BUILD_ROOT/usr/bin
install -d $RPM_BUILD_ROOT/usr/%_lib/pegasus
ln -sf ../../etc/init.d/pegasus-wbem $RPM_BUILD_ROOT/usr/sbin/rcpegasus-wbem
for f in cimserver cimservera cimauth  cimconfig; do
    install -m 0544 $PEGASUS_HOME/bin/$f $RPM_BUILD_ROOT/usr/sbin/
done
for f in cimmof cimmofl wbemexec tomof osinfo ipinfo cimprovider  ; do
   install  -m 0755 $PEGASUS_HOME/bin/$f $RPM_BUILD_ROOT/usr/bin/
done
install -m 0755 $PEGASUS_HOME/bin/CLI $RPM_BUILD_ROOT/usr/bin/WBEMCLI
cp  $PEGASUS_HOME/lib/* $RPM_BUILD_ROOT/usr/%_lib/pegasus
pushd $RPM_BUILD_ROOT/usr/%_lib/pegasus
ln -s . providers
for a in `ls -1 *.so.1 | sed s/\.so\.1/\.so/`
do
 ln -sf "$a.1" $a
done
popd
# SSL Files
#
install -D -m 0644  $PEGASUS_ROOT/src/Server/ssl.cnf    $RPM_BUILD_ROOT/etc/pegasus/ssl_orig
#
# cimserver config files
#
install -D -m 0644  $PEGASUS_ROOT/rpm/cimserver_planned.conf $RPM_BUILD_ROOT/etc/pegasus/cimserver_planned.conf
#install -D -m 0644  %{S:3} $RPM_BUILD_ROOT/etc/pegasus/cimserver_planned.conf
#
# WBEM pam authentication
#
install -D -m 0644  $PEGASUS_ROOT/rpm/wbem $RPM_BUILD_ROOT/etc/pam.d/wbem
#
# man pages
#
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1.Z/cimmof.1 $RPM_BUILD_ROOT%{_mandir}/man1/cimmof.1
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1.Z/cimprovider.1 $RPM_BUILD_ROOT%{_mandir}/man1/cimprovider.1
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1.Z/osinfo.1 $RPM_BUILD_ROOT%{_mandir}/man1/osinfo.1
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1.Z/wbemexec.1 $RPM_BUILD_ROOT%{_mandir}/man1/wbemexec.1
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimauth.1m $RPM_BUILD_ROOT%{_mandir}/man8/cimauth.8
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimconfig.1m $RPM_BUILD_ROOT%{_mandir}/man8/cimconfig.8
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimprovider.1m $RPM_BUILD_ROOT%{_mandir}/man8/cimprovider.8
install -D -m 0444  $PEGASUS_ROOT/rpm/manLinux/man1m.Z/cimserver.1m $RPM_BUILD_ROOT%{_mandir}/man8/cimserver.8
# Pegasus Schema
pushd Schemas/Pegasus
for dir in ManagedSystem Internal InterOp; do
    for mof in `find $dir -name \*mof`; do 
        cp $mof $RPM_BUILD_ROOT/etc/pegasus/mof/; 
    done
done
popd
# SDK
install -d $RPM_BUILD_ROOT/usr/include
cp -a $PEGASUS_HOME/stageSDK/include/* $RPM_BUILD_ROOT/usr/include/
# Compile mof files
export REPOSITORY_LOC=$RPM_BUILD_ROOT/var/lib/pegasus
mkdir -p $REPOSITORY_LOC
rm -rf $REPOSITORY_LOC/repository
export PG_SCHEMA_VER=20
export CIM_SCHEMA_VER=28
export MOF_PATH=$PEGASUS_ROOT/Schemas/CIM28
export PG_MOF_PATH=$RPM_BUILD_ROOT/etc/pegasus/mof
#
# The following are for the InterOp Namespace
#
INTEROPNS=root/PG_InterOp
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$RPM_BUILD_ROOT/usr/%_lib/pegasus
export LD_LIBRARY_PATH
$RPM_BUILD_ROOT/usr/bin/cimmofl -R $REPOSITORY_LOC -I$MOF_PATH -n$INTEROPNS $MOF_PATH/CIM_Core$CIM_SCHEMA_VER.mof
$RPM_BUILD_ROOT/usr/bin/cimmofl   -R $REPOSITORY_LOC -I$MOF_PATH -n$INTEROPNS $MOF_PATH/CIM_Event$CIM_SCHEMA_VER.mof
$RPM_BUILD_ROOT/usr/bin/cimmofl   -R $REPOSITORY_LOC -I$PG_MOF_PATH -n$INTEROPNS $PG_MOF_PATH/PG_InterOpSchema$PG_SCHEMA_VER.mof
#
# The following are for the Internal Namespace
#
INTERNALNS=root/PG_Internal
$RPM_BUILD_ROOT/usr/bin/cimmofl   -R $REPOSITORY_LOC -I$MOF_PATH -n$INTERNALNS $MOF_PATH/Core${CIM_SCHEMA_VER}_Qualifiers.mof
$RPM_BUILD_ROOT/usr/bin/cimmofl   -R $REPOSITORY_LOC -I$PG_MOF_PATH -n$INTERNALNS $PG_MOF_PATH/PG_InternalSchema$PG_SCHEMA_VER.mof
#
# The following are for the ManagedSystem Namespace
#
# ATTN:    MANAGEDSYSTEMNS=root/PG_ManagedSystem
MANAGEDSYSTEMNS=root/cimv2
$RPM_BUILD_ROOT/usr/bin/cimmofl   -R $REPOSITORY_LOC -I$MOF_PATH -n$MANAGEDSYSTEMNS -aE $MOF_PATH/CIM_Schema$CIM_SCHEMA_VER.mof
$RPM_BUILD_ROOT/usr/bin/cimmofl   -R $REPOSITORY_LOC -I$PG_MOF_PATH -n$MANAGEDSYSTEMNS -aE $PG_MOF_PATH/PG_ManagedSystemSchema$PG_SCHEMA_VER.mof
$RPM_BUILD_ROOT/usr/bin/cimmofl    -R $REPOSITORY_LOC -I$PG_MOF_PATH -n$INTEROPNS $PG_MOF_PATH/PG_ManagedSystemSchema${PG_SCHEMA_VER}R.mof

%post
%{fillup_and_insserv -f pegasus-wbem}
export INSTALL_LOG=/var/log/pegasus/install.log
echo " Generating SSL certificates... "
CN="Common Name"
EMAIL="test@email.address"
HOSTNAME=`uname -n`
sed -e "s/$CN/$HOSTNAME/"  \
    -e "s/$EMAIL/root@$HOSTNAME/" /etc/pegasus/ssl_orig \
    > /etc/pegasus/ssl.cnf
chmod 644 /etc/pegasus/ssl.cnf
chown bin /etc/pegasus/ssl.cnf
chgrp bin /etc/pegasus/ssl.cnf
# This random-enthropy-gathering is NOT random at all. Use the default one SSL has - (/dev/random).
# /bin/rpm -qa >/etc/pegasus/ssl.rnd 2>>$INSTALL_LOG
# /bin/netstat -a >>/etc/pegasus/ssl.rnd 2>>$INSTALL_LOG
#   -rand /etc/pegasus/ssl.rnd 
export RANDFILE=/etc/pegasus/cimserver.rnd
openssl req -x509 -days 365 -newkey rsa:2048 \
   -nodes -config /etc/pegasus/ssl.cnf   \
   -keyout /etc/pegasus/key.pem -out /etc/pegasus/cert.pem 2>>$INSTALL_LOG
cat /etc/pegasus/key.pem /etc/pegasus/cert.pem > /etc/pegasus/server_2048.pem
cat /etc/pegasus/cert.pem > /etc/pegasus/client_2048.pem
chmod 700 /etc/pegasus/*.pem
rm -f /etc/pegasus/key.pem /etc/pegasus/cert.pem
if [ -f /etc/pegasus/server.pem ] 
then
    echo " WARNING: /etc/pegasus/server.pem SSL Certificate file already exists."
else
    cp /etc/pegasus/server_2048.pem /etc/pegasus/server.pem
    chmod 400 /etc/pegasus/server.pem
fi
if [ -f /etc/pegasus/client.pem ]
then
    echo " WARNING: /etc/pegasus/client.pem SSL Certificate trust store already exists."
else
    cp /etc/pegasus/client_2048.pem /etc/pegasus/client.pem
    chmod 400 /etc/pegasus/client.pem
fi

%postun
%restart_on_update pegasus-wbem
%insserv_cleanup

%clean
#make clean
[ "$RPM_BUILD_ROOT" != "/" ] && [ -d $RPM_BUILD_ROOT ] && rm -rf $RPM_BUILD_ROOT;

%files
%defattr(-,root,root)
/etc/init.d/pegasus-wbem
/etc/pam.d/wbem
%dir  /etc/pegasus
%dir  /etc/pegasus/mof
/etc/pegasus/mof/*.mof
/var/lib/pegasus
/var/cache/pegasus
%dir  /var/log/pegasus
#%dir  /usr/%_lib/pegasus/providers
#/usr/%_lib/pegasus/providers/*.so
%{_mandir}/man1/cimmof.1.gz
%{_mandir}/man1/cimprovider.1.gz
%{_mandir}/man1/osinfo.1.gz
%{_mandir}/man1/wbemexec.1.gz
%{_mandir}/man8/cimauth.8.gz
%{_mandir}/man8/cimconfig.8.gz
%{_mandir}/man8/cimprovider.8.gz
%{_mandir}/man8/cimserver.8.gz
%config  /etc/pegasus/cimserver_planned.conf
/etc/pegasus/ssl_orig
/usr/bin/loadmof.sh
/usr/bin/rmmof.sh
/usr/bin/cimmof
/usr/bin/cimmofl
/usr/bin/osinfo
/usr/bin/ipinfo
/usr/bin/wbemexec
/usr/bin/WBEMCLI
/usr/bin/tomof
/usr/bin/cimprovider
/usr/sbin/rcpegasus-wbem
/usr/sbin/cimservera
/usr/sbin/cimserver
/usr/sbin/cimauth
/usr/sbin/cimconfig
/usr/%_lib/pegasus

%files devel
%defattr(-,root,root)
/usr/include/Pegasus
%defattr(-,root,root,0755)
%doc doc/*.txt doc/DOCREMARKS doc/HISTORY doc/NOTES doc/*.html doc/*.pdf pegasus-build/stageSDK/samples pegasus-build/stageSDK/html

%changelog -n pegasus-wbem
* Fri Jul 02 2004 - nashif@suse.de
- NPTL pthread libraries after a while get confused (#41217)
* Fri Jun 11 2004 - nashif@suse.de
- property systemName failed for provider  cmpi-base (#41327)
* Fri Jun 11 2004 - nashif@suse.de
- fixed provider path for lib64 systems
* Fri Jun 11 2004 - nashif@suse.de
- Completed fix of ##40617: installing and the de-installing
  sblim-providers leaves CIM classes in repository
* Thu Jun 03 2004 - nashif@suse.de
- Fixed bug with  mutex deadlock condition
  on thread specific data (#41418)
- a retry condition for sem_wait (in case EINTR is returned -
  which happens now since the threading library is more POSIX
  compliant)
- unloading of CMPI providers not cleaning up the internal list -
  making Pegasus think that the provider is already loaded.
* Fri May 28 2004 - nashif@suse.de
- Latest patch (pegasus-2.3.2) was missing some hooks after
  all small patches were merged into a cumulative patch.
* Thu May 27 2004 - nashif@suse.de
- CIMServer crashed when ela indication provider delivers
  an event (#41109)
- NPTL pthread libraries after a while get confused (#41217)
* Tue May 25 2004 - nashif@suse.de
- Fixed #40617: remove classes from repository when uninstalling
  providers
- Disable warning when using cimomfl
* Thu May 13 2004 - nashif@suse.de
- #40107: system calls in providers fail with return code -1
- Fixed bug that caused a crash using ELA indication provider
- Fixed rmmof.sh (removed double pipe)
* Fri May 07 2004 - nashif@suse.de
- #40020: removed cimuser from file list
- #39667: adapted rmmof.sh to use list of providers from registration
  mofs
* Fri Apr 30 2004 - nashif@suse.de
- Do not use hard-coded option, Instead use config file
- disable http connections by default
* Thu Apr 29 2004 - nashif@suse.de
- Update to 2.3.2 final
* Sun Apr 18 2004 - nashif@suse.de
- Fixed loadmof.sh script
* Fri Apr 16 2004 - nashif@suse.de
- #38805: pegasus-wbem start uses hard-coded configuration parameters
- #38803: PAM authentication uses the standalong PAM process
- disable http
* Wed Apr 07 2004 - nashif@suse.de
- Fixed script loadmof.sh when running in installed system
- Enable HTTP by default in cimser_planned.conf
* Tue Apr 06 2004 - nashif@suse.de
- Added script to compile MOFs during %%post of cmpi proivders
- Added Conflict to openwbem
- Added virtual Provide: cim-server
* Sat Apr 03 2004 - nashif@suse.de
- Update to latest CVS status (2.3.2)
- Fixed bugs:
  * #37600 LTC7144-pegasus-wbem missing 'osinfo' and 'ipinfo'  program
  * #37778 LTC7169-pegasus-wbem does not start. cimserver_planned.conf
  has invalid properties.
  * #37603 LTC7147-pegasus-wbem has the section 8 manpages in a wrong
  directory.
  * #37605 LTC7148-pegasus-wbem 'cimconfig'  manpage lists wrong
  directory and lacks some of the options
* Tue Feb 24 2004 - nashif@suse.de
- Fixed Requires
- Added PreReq for openssl
* Tue Feb 24 2004 - nashif@suse.de
- Compile mof files during rpm build
- Create ssl keys in %%post
- Fixed configuration files and removed obsolete options
* Thu Feb 19 2004 - nashif@suse.de
- Initial release: 2.3.2
