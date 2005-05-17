#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
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
# wbemexec test associated ManagedSystem

ROOT = ../../../../..
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

HOSTNAME =
PORT = -p 5988
HTTPMETHOD = -m M-POST
HTTPVERSION = -v 1.1
USER = -u guest
PASSWORD = -w guest

XMLREQUESTS = $(foreach i, $(XMLSCRIPTS), $i.xml)
XMLRESPONSES = $(XMLREQUESTS:.xml=.rsp)

WBEMEXECOPTIONS = $(HOSTNAME) $(PORT) $(HTTPMETHOD) $(HTTPVERSION) $(USER) $(PASSWORD)

%.rsp: %.xml

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
	@chtag -tc IBM-1047 ./PG_OS_Fail*.xml
endif
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > $(TMP_DIR)/$*.rsp || cd .
	@ $(DIFF) $*rspgood.xml $(TMP_DIR)/$*.rsp
	@ $(RM) $(TMP_DIR)/$*.rsp
	@ $(ECHO) +++ $* passed successfully +++

ifeq ($(OS), HPUX)
XMLSCRIPTS = \
   PG_OS_Fail1_CreateInstance \
   PG_OS_Fail2_DeleteInstance  \
   PG_OS_Fail3_InvokeMethod \
   PG_OS_Fail4_ModifyInstance  \
   PG_OS_Fail5_GetInstance_ShortKeys \
   PG_OS_Fail6_GetInstance_ExtraKeys \
   PG_OS_Fail7_GetInstance_DupKeys
#   PG_OS_Fail8_EnumNamesCIM \
#   PG_OS_Fail9_EnumCIM
else
ifeq ($(PEGASUS_PLATFORM), LINUX_GENERIC_GNU)
XMLSCRIPTS = \
   PG_OS_Fail1_CreateInstance \
   PG_OS_Fail2_DeleteInstance  \
   PG_OS_Fail3_InvokeMethod \
   PG_OS_Fail4_ModifyInstance  \
   PG_OS_Fail5_GetInstance_ShortKeys \
   PG_OS_Fail6_GetInstance_ExtraKeys \
   PG_OS_Fail7_GetInstance_DupKeys
#   PG_OS_Fail8_EnumNamesCIM \
#   PG_OS_Fail9_EnumCIM
else
ifeq ($(PEGASUS_PLATFORM), LINUX_IA64_GNU)
XMLSCRIPTS = \
   PG_OS_Fail1_CreateInstance \
   PG_OS_Fail2_DeleteInstance  \
   PG_OS_Fail3_InvokeMethod \
   PG_OS_Fail4_ModifyInstance  \
   PG_OS_Fail5_GetInstance_ShortKeys \
   PG_OS_Fail6_GetInstance_ExtraKeys \
   PG_OS_Fail7_GetInstance_DupKeys
#   PG_OS_Fail8_EnumNamesCIM \
#   PG_OS_Fail9_EnumCIM
else
XMLSCRIPTS = \
   PG_OS_Fail1_CreateInstance \
   PG_OS_Fail2_DeleteInstance  \
   PG_OS_Fail3_InvokeMethod
#   PG_OS_Fail8_EnumNamesCIM \
#   PG_OS_Fail9_EnumCIM
endif
endif
endif

clean:
	$(RM) $(XMLRESPONSES)

tests:

messages:

poststarttests: $(XMLRESPONSES)

depend:

general:

strip-license:

prepend-license:

