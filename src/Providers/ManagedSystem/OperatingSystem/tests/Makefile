# wbemexec test associated ManagedSystem

ROOT = ../../../../..
include $(ROOT)/mak/config.mak

HOSTNAME =
PORT = -p 5988
HTTPMETHOD = -m M-POST
HTTPVERSION = -v 1.1
USER = -u guest
PASSWORD = -w guest

ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
   DIFF = mu compare
else
   DIFF = diff
endif

XMLREQUESTS = $(foreach i, $(XMLSCRIPTS), $i.xml)
XMLRESPONSES = $(XMLREQUESTS:.xml=.rsp)

WBEMEXECOPTIONS = $(HOSTNAME) $(PORT) $(HTTPMETHOD) $(HTTPVERSION) $(USER) $(PASSWORD)

%.rsp: %.xml
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > $(TMP_DIR)/$*.rsp | cd .
	@ $(DIFF) $*rspgood.xml $(TMP_DIR)/$*.rsp
	@ $(RM) $(TMP_DIR)/$*.rsp
	@ echo +++ $* passed successfully +++

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

poststarttests: $(XMLRESPONSES)

depend:

general:

strip-license:

prepend-license:
