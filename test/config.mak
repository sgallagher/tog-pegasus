include $(ROOT)/mak/test.mak

HOSTNAME =
PORT =
HTTPMETHOD = -m M-POST
HTTPVERSION = -v 1.1
USER = -u guest
PASSWORD = -w guest
SSL =

PEGASUS_XML_ORDERING_DEFECT_ENCOUNTERED=on

XMLREQUESTS = $(foreach i, $(XMLSCRIPTS), $i.xml)
XMLRESPONSES = $(XMLREQUESTS:.xml=.rsp)

XMLREQUESTS_DS = $(foreach i, $(XMLSCRIPTS_DS), $i.xml)
XMLRESPONSES_DS = $(XMLREQUESTS_DS:.xml=.rsp_ds)


WBEMEXECOPTIONS = $(HOSTNAME) $(PORT) $(HTTPMETHOD) $(HTTPVERSION) $(USER) $(PASSWORD) $(SSL) 

%.rsp: %.xml
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > $(TMP_DIR)/$*.rsp | cd .
	@ $(DIFF) $*rspgood.xml $(TMP_DIR)/$*.rsp
	@ $(RM) $(TMP_DIR)/$*.rsp
	@ $(ECHO) +++ $* passed successfully +++

##	@ $(call DIFFSORT,file_unsorted,file_sorted)

%.rsp_ds: %.xml
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > $(TMP_DIR)/$*.rsp_ds | cd .
	@ $(call DIFFSORT,$*rspgood.xml,$(TMP_DIR)/$*.rsp_ds)
	@ $(RM) $(TMP_DIR)/$*.rsp_ds
	@ $(ECHO) +++ $* passed successfully +++


