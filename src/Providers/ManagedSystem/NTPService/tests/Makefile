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
# wbemexec tests for ManagedSystem NTP provider

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
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > $(TMP_DIR)/$*.rsp || cd .
	@ $(DIFF) $*rspgood.xml $(TMP_DIR)/$*.rsp
	@ $(RM) $(TMP_DIR)/$*.rsp
	@ $(ECHO) +++ $* passed successfully +++

XMLCOVREQUESTS = $(foreach i, $(XMLCOVSCRIPTS), $i.xml)
XMLCOVRESPONSES = $(XMLCOVREQUESTS:.xml=.cov)

%.cov: %.xml
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > $(TMP_DIR)/$*.cov || cd .
	@ $(RM) $(TMP_DIR)/$*.cov
	@ $(ECHO) +++ $* code coverage exercise complete +++

ifeq ($(OS), HPUX)
XMLSCRIPTS = \
	PG_NTP_Fail1_CreateInstance \
	PG_NTP_Fail2_DeleteInstance \
	PG_NTP_Fail3_ModifyInstance \
	PG_NTP_Fail4_GetInstance_ShortKeys \
	PG_NTP_Fail5_GetInstance_ExtraKeys \
	PG_NTP_Fail6_GetInstance_DupKeys
XMLCOVSCRIPTS = \
	PG_NTP_EnumInst \
	PG_NTP_EnumInstNames
else
ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
XMLSCRIPTS =
XMLCOVSCRIPTS =
else
ifeq ($(PEGASUS_PLATFORM), LINUX_IA64_GNU)
XMLSCRIPTS =
XMLCOVSCRIPTS =
else
ifeq ($(OS), VMS)
XMLSCRIPTS =
XMLCOVSCRIPTS =
else
XMLSCRIPTS =
XMLCOVSCRIPTS =
endif
endif
endif
endif

clean:
	$(RM) $(XMLRESPONSES)

tests:

poststarttests: $(XMLRESPONSES) $(XMLCOVRESPONSES)

depend:

general:

messages:

strip-license:

prepend-license:

