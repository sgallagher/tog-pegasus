# wbemexec test associated ManagedSystem

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak

#HOSTNAME =
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
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > /tmp/$*.rsp | cd .
	@ $(DIFF) $*rspgood.xml /tmp/$*.rsp
	@ $(RM) /tmp/$*.rsp
	@ echo +++ $* passed successfully +++

ifeq ($(PEGASUS_PLATFORM), HPUX_PARISC_ACC)
XMLSCRIPTS = \
	PG_DNS_Fail1_CreateInstance \
	PG_DNS_Fail1_CreateInstancerspgood \
	PG_DNS_Fail2_DeleteInstance \
	PG_DNS_Fail2_DeleteInstancerspgood \
	PG_DNS_Fail3_ModifyInstance \
	PG_DNS_Fail3_ModifyInstancerspgood \
	PG_DNS_Fail4_GetInstance_ShortKeys \
	PG_DNS_Fail4_GetInstance_ShortKeysrspgood \
	PG_DNS_Fail5_GetInstance_ExtraKeys \
	PG_DNS_Fail5_GetInstance_ExtraKeysrspgood \
	PG_DNS_Fail6_GetInstance_DupKeys \
	PG_DNS_Fail6_GetInstance_DupKeysrspgood \
	PG_DNS_Fail7_EnumNamesCIM \
	PG_DNS_Fail7_EnumNamesCIMrspgood \
	PG_DNS_Fail8_EnumCIM \
	PG_DNS_Fail8_EnumCIMrspgood
endif

clean:
	$(RM) $(XMLRESPONSES)

tests: 

poststarttests: $(XMLRESPONSES)

depend:

general:

strip-license:

prepend-license:
