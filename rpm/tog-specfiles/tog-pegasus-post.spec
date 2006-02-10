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
# Start of section pegasus/rpm/tog-specfiles/tog-pegasus-post.spec
#
#           install   remove   upgrade  reinstall
# %post        1        -         2         2
#
   export PEGASUS_ARCH_LIB=%PEGASUS_ARCH_LIB

   if [ -d %PEGASUS_PREV_REPOSITORY_DIR ]; then
       # Running Repository Upgrade utility
       %PEGASUS_SBIN_DIR/repupgrade %PEGASUS_PREV_REPOSITORY_DIR \
           %PEGASUS_REPOSITORY_DIR 2>>%PEGASUS_INSTALL_LOG
       /bin/tar -C %PEGASUS_REPOSITORY_PARENT_DIR -cf \
           %PEGASUS_PREV_REPOSITORY_DIR`date '+%Y-%m-%d-%s.%N'`.tar \
           %PEGASUS_PREV_REPOSITORY_DIR_NAME
       rm -rf %PEGASUS_PREV_REPOSITORY_DIR
   fi
   if [ $1 -eq 1 ]; then
%if %{AUTOSTART}
       /sbin/chkconfig --add tog-pegasus
%endif
   :;
   elif [ $1 -gt 0 ]; then
       /etc/init.d/tog-pegasus condrestart
   :;
   fi
#
# End of section pegasus/rpm/tog-specfiles/tog-pegasus-post.spec
