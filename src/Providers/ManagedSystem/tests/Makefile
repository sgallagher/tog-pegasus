# wbemexec test associated ManagedSystem

ROOT = ../../../..
include $(ROOT)/mak/config.mak

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
	@ wbemexec $(WBEMEXECOPTIONS) $*.xml > $*.rsp | cd .

XMLSCRIPTS = \
   PG_ComputerSystem00001 \
   PG_ComputerSystem00002 \
   PG_ComputerSystem00003 \
   PG_ComputerSystem00004 \
   PG_ComputerSystem00005 \
   PG_ComputerSystem00006 \
   PG_OperatingSystem25001 \
   PG_OperatingSystem25002


clean:
	$(RM) $(XMLRESPONSES)
	

tests: $(XMLRESPONSES)

poststarttests:

