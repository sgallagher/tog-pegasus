#//%LICENSE////////////////////////////////////////////////////////////////
#//
#// Licensed to The Open Group (TOG) under one or more contributor license
#// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
#// this work for additional information regarding copyright ownership.
#// Each contributor licenses this file to you under the OpenPegasus Open
#// Source License; you may not use this file except in compliance with the
#// License.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a
#// copy of this software and associated documentation files (the "Software"),
#// to deal in the Software without restriction, including without limitation
#// the rights to use, copy, modify, merge, publish, distribute, sublicense,
#// and/or sell copies of the Software, and to permit persons to whom the
#// Software is furnished to do so, subject to the following conditions:
#//
#// The above copyright notice and this permission notice shall be included
#// in all copies or substantial portions of the Software.
#//
#// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
#// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
#// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
#// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
#// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
#// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
#// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//////////////////////////////////////////////////////////////////////////
# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-preun.spec
#           install   remove   upgrade  reinstall
#  preun       -        0         1         -
if [ $1 -eq 0 ]; then
   # Check if the cimserver is running
   isRunning=`ps -el | grep cimserver |  grep -v "grep cimserver"`
   if [ "$isRunning" ]; then
      %PEGASUS_SBIN_DIR/cimserver -s
   fi 
   /sbin/chkconfig --del tog-pegasus;
   rm -f %PEGASUS_VARDATA_DIR/cimserver_current.conf;
   [ -d %PEGASUS_REPOSITORY_DIR ]  && rm -rf %PEGASUS_REPOSITORY_DIR;
   [ -d %PEGASUS_VARDATA_CACHE_DIR ]  && rm -rf %PEGASUS_VARDATA_CACHE_DIR;
   rm -f %PEGASUS_LOCAL_DOMAIN_SOCKET_PATH;
   rm -f %PEGASUS_CIMSERVER_START_FILE;
   rm -f %PEGASUS_CIMSERVER_START_LOCK_FILE;
fi
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-preun.spec
