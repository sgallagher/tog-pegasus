# wbemexec test associated ManagedSystem

.SUFFIXES: .xml .rsp

ROOT = $(PEGASUS_ROOT)
PEGASUS_PLATFORM=HPUX_PARISC_ACC

include $(ROOT)/mak/config.mak

HOSTNAME =
PORT = -p 5988
HTTPMETHOD = -m M-POST
HTTPVERSION = -v 1.1
USER = -u guest
PASSWORD = -w guest

DIFF = diff

XMLREQUESTS = $(foreach i, $(XMLSCRIPTS), $i.xml)
XMLRESPONSES = $(XMLREQUESTS:.xml=.rsp)

WBEMEXECOPTIONS = $(HOSTNAME) $(PORT) $(HTTPMETHOD) $(HTTPVERSION) $(USER) $(PASSWORD)

.xml.rsp:
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > /tmp/$*.rsp | cd .
	@ $(DIFF) $*rspgood.xml /tmp/$*.rsp
	@ $(RM) /tmp/$*.rsp
	@ echo +++ $* passed successfully +++

XMLSCRIPTS = \
	PG_NTP_Fail1_CreateInstance.xml \
	PG_NTP_Fail1_CreateInstancerspgood.xml \
	PG_NTP_Fail2_DeleteInstance.xml \
	PG_NTP_Fail2_DeleteInstancerspgood.xml \
	PG_NTP_Fail3_ModifyInstance.xml \
	PG_NTP_Fail3_ModifyInstancerspgood.xml \
	PG_NTP_Fail4_GetInstance_ShortKeys.xml \
	PG_NTP_Fail4_GetInstance_ShortKeysrspgood.xml \
	PG_NTP_Fail5_GetInstance_ExtraKeys.xml \
	PG_NTP_Fail5_GetInstance_ExtraKeysrspgood.xml \
	PG_NTP_Fail6_GetInstance_DupKeys.xml \
	PG_NTP_Fail6_GetInstance_DupKeysrspgood.xml \
	PG_NTP_Fail7_EnumNamesCIM.xml \
	PG_NTP_Fail7_EnumNamesCIMrspgood.xml \
	PG_NTP_Fail8_EnumCIM.xml \
	PG_NTP_Fail8_EnumCIMrspgood.xml

clean:
	$(RM) $(XMLRESPONSES)
	

tests: 

poststarttests: $(XMLRESPONSES)

depend:

general:

strip-license:

prepend-license:
