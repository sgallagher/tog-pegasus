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
# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-pre.spec
#           install   remove   upgrade  reinstall
# pre          1        -         2         2
#
# If it exists, the active OpenPegasus Repository
#    directory, %PEGASUS_REPOSITORY_DIR is renamed
#    to %PEGASUS_PREV_REPOSITORY_DIR.  If
#    %PEGASUS_PREV_REPOSITORY_DIR already exists,
#    it is saved as an archive file.

if [ -d %PEGASUS_REPOSITORY_DIR"/root#PG_Internal" ]; then
   #
   # Save the current repository to prev_repository
   #
   if [ -d %PEGASUS_PREV_REPOSITORY_DIR ]; then
        /bin/tar -C %PEGASUS_REPOSITORY_PARENT_DIR -cf \
            %PEGASUS_PREV_REPOSITORY_DIR`date '+%Y-%m-%d-%s.%N'`.tar \
            %PEGASUS_PREV_REPOSITORY_DIR_NAME
        rm -rf %PEGASUS_PREV_REPOSITORY_DIR
   fi
   /bin/mv %PEGASUS_REPOSITORY_DIR %PEGASUS_PREV_REPOSITORY_DIR
fi
if [ $1 -gt 0 ]; then
   #  Create the 'pegasus' user and group:
   /usr/sbin/groupadd pegasus > /dev/null 2>&1 || :;
   /usr/sbin/useradd -c "tog-pegasus OpenPegasus WBEM/CIM services" \
        -g pegasus -s /sbin/nologin -r -d %PEGASUS_VARDATA_DIR pegasus \
         > /dev/null 2>&1 || :;
fi
if [ $1 -eq 2 ]; then
    rm -f %PEGASUS_LOCAL_DOMAIN_SOCKET_PATH;
    rm -f %PEGASUS_CIMSERVER_START_FILE;
    rm -f %PEGASUS_CIMSERVER_START_LOCK_FILE;
fi
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-pre.spec
