#%/////////////////////////////////////////////////////////////////////////////
#
# Copyright (c) 2004 BMC Software, Hewlett-Packard Company, IBM,
# The Open Group, Tivoli Systems
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
# ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
# "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
# LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
# PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
# ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#
#==============================================================================
#
# Author: Konrad Rzeszutek <konradr@us.ibm.com>
#
#%/////////////////////////////////////////////////////////////////////////////
#
# Package spec for OpenPegasus 2.4
#
Summary: WBEM Services for Linux
Name: lsb-pegasus
Version: 2.4
Release: 1
Group: Systems Management/Base
Vendor: OpenSource Org
Copyright: Open Group Pegasus Open Source
BuildRoot: /home/konrad/MAIN/install
Requires: openssl-devel >= 0.9.6 lsb >= 1.3
Provides: cimserver lsb-pegasus

%description
WBEM Services for Red Hat Linux enables management solutions that deliver
increased control of enterprise resources. WBEM is a platform and resource
independent DMTF standard that defines a common information model and
communication protocol for monitoring and controlling resources from diverse
sources.


%pre

%install

%post
chmod 755 /etc/init.d/lsb-pegasus
/usr/lib/lsb/install_initd /etc/init.d/lsb-pegasus

CN="Common Name"
EMAIL="test@email.address"
HOSTNAME=`uname -n`
sed -e "s/$CN/$HOSTNAME/"  \
    -e "s/$EMAIL/root@$HOSTNAME/" /var/opt/lsb-pegasus/ssl.orig \
    > /var/opt/lsb-pegasus/ssl.cnf
chmod 644 /var/opt/lsb-pegasus/ssl.cnf
chown bin /var/opt/lsb-pegasus/ssl.cnf
chgrp bin /var/opt/lsb-pegasus/ssl.cnf

openssl req -x509 -days 365 -newkey rsa:2048 \
   -nodes -config /var/opt/lsb-pegasus/ssl.cnf   \
   -keyout /etc/opt/lsb-pegasus/key.pem -out /etc/opt/lsb-pegasus/cert.pem 

cat /etc/opt/lsb-pegasus/key.pem > /etc/opt/lsb-pegasus/file_2048.pem
cat /etc/opt/lsb-pegasus/cert.pem > /etc/opt/lsb-pegasus/server_2048.pem
cat /etc/opt/lsb-pegasus/cert.pem > /etc/opt/lsb-pegasus/client_2048.pem
chmod 700 /etc/opt/lsb-pegasus/*.pem

rm -f /etc/opt/lsb-pegasus/key.pem /etc/opt/lsb-pegasus/cert.pem

if [ -f /etc/opt/lsb-pegasus/server.pem ] 
then
    echo "WARNING: /etc/opt/lsb-pegasus/server.pem SSL Certificate file already exists."
else
    cp /etc/opt/lsb-pegasus/server_2048.pem /etc/opt/lsb-pegasus/server.pem
    cp /etc/opt/lsb-pegasus/file_2048.pem /etc/opt/lsb-pegasus/file.pem
    chmod 400 /etc/opt/lsb-pegasus/server.pem /etc/opt/lsb-pegasus/file.pem
fi

if [ -f /etc/opt/lsb-pegasus/client.pem ]
then
    echo "WARNING: /etc/opt/lsb-pegasus/client.pem SSL Certificate trust store already exists."
else
    cp /etc/opt/lsb-pegasus/client_2048.pem /etc/opt/lsb-pegasus/client.pem
    chmod 400 /etc/opt/lsb-pegasus/client.pem
fi

%postun
/usr/lib/lsb/remove_initd /etc/init.d/lsb-pegasus

%files

%attr ( - bin bin) /opt/lsb-pegasus
%attr ( - bin bin) /etc/opt/lsb-pegasus
%attr ( - bin bin) /etc/init.d/lsb-pegasus
%attr ( - bin bin) /etc/pam.d/wbem
%attr ( - bin bin) /var/opt/lsb-pegasus/
