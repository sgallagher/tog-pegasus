# wbemexec tests for ManagedSystem IP provider

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

XMLCOVREQUESTS = $(foreach i, $(XMLCOVSCRIPTS), $i.xml)
XMLCOVRESPONSES = $(XMLCOVREQUESTS:.xml=.cov)

%.cov: %.xml
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > $(TMP_DIR)/$*.cov | cd .
	@ $(RM) $(TMP_DIR)/$*.cov
	@ echo +++ $* code coverage exercise complete +++

ifeq ($(OS), HPUX)
XMLSCRIPTS = \
  f01_ci_BIPTLEp \
  f02_di_BIPTLEp \
  f04_mi_BIPTLEp \
  f05_giShortKeys_BIPTLEp \
  f06_giExtraKeys_BIPTLEp \
  f07_giDupKeys_BIPTLEp \
  f08_giBadSCCN_BIPTLEp \
  f09_giBadInst_BIPTLEp \
  f10_giShortRef_BIPTLEp \
  f11_giExtraRef_BIPTLEp \
  f12_giDupRef_BIPTLEp \
  f01_ci_IPPEp \
  f02_di_IPPEp \
  f04_mi_IPPEp \
  f05_giShortKeys_IPPEp \
  f06_giExtraKeys_IPPEp \
  f07_giDupKeys_IPPEp \
  f08_giBadSCCN_IPPEp \
  f09_giBadInst_IPPEp \
  f01_ci_IPR \
  f02_di_IPR \
  f04_mi_IPR \
  f05_giShortKeys_IPR \
  f06_giExtraKeys_IPR \
  f07_giDupKeys_IPR \
  f08_giBadSCCN_IPR \
  f09_giBadInst_IPR
XMLCOVSCRIPTS = \
  c01_ei_BIPTLEp \
  c01_ei_IPPEp \
  c01_ei_IPR
else
ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
XMLSCRIPTS = \
  f01_ci_BIPTLEp \
  f02_di_BIPTLEp \
  f04_mi_BIPTLEp \
  f05_giShortKeys_BIPTLEp \
  f06_giExtraKeys_BIPTLEp \
  f07_giDupKeys_BIPTLEp \
  f08_giBadSCCN_BIPTLEp \
  f09_giBadInst_BIPTLEp \
  f10_giShortRef_BIPTLEp \
  f11_giExtraRef_BIPTLEp \
  f12_giDupRef_BIPTLEp \
  f01_ci_IPPEp \
  f02_di_IPPEp \
  f04_mi_IPPEp \
  f05_giShortKeys_IPPEp \
  f06_giExtraKeys_IPPEp \
  f07_giDupKeys_IPPEp \
  f08_giBadSCCN_IPPEp \
  f09_giBadInst_IPPEp \
  f01_ci_IPR \
  f02_di_IPR \
  f04_mi_IPR \
  f05_giShortKeys_IPR \
  f06_giExtraKeys_IPR \
  f07_giDupKeys_IPR \
  f08_giBadSCCN_IPR \
  f09_giBadInst_IPR
XMLCOVSCRIPTS = 
else
ifeq ($(PEGASUS_PLATFORM), LINUX_IA64_GNU) 
XMLSCRIPTS = 
XMLCOVSCRIPTS = 
else
XMLSCRIPTS = 
XMLCOVSCRIPTS = 
endif
endif
endif

clean:
	$(RM) $(XMLRESPONSES)
	

tests: 

messages:

poststarttests: $(XMLRESPONSES) $(XMLCOVRESPONSES)

depend:

general:

strip-license:

prepend-license:
