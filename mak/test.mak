ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
  CIMSERVER_START_SERVICE = 
  CIMSERVER_STOP_SERVICE = 
  SLEEP =
  STRIPCRS = stripcrs $(RESULTFILE) | stripcrs $(MASTERRESULTFILE)
  DIFF = mu compare
  REDIRECTERROR = 2>&1
else
  CIMSERVER_START_SERVICE = cimserver
  CIMSERVER_STOP_SERVICE = /usr/bin/ps -ef | /usr/bin/grep cimserver | /usr/bin/grep -v grep | /usr/bin/awk '{print "kill -9 "$$2 |"/usr/bin/sh"}'
  SLEEP = sleep 5
  STRIPCRS =
  DIFF = diff
  REDIRECTERROR = 2>&1
endif

ifdef DIFF
  COMPARERESULTS = @$(DIFF) $(MASTERRESULTFILE) $(RESULTFILE)
else
  COMPARERESULTS = @$(ECHO) "Results not compared."
endif

ifdef PEGASUS_TMP
  TMP_DIR = $(subst \,/,$(PEGASUS_TMP))
else
  TMP_DIR = .
endif

default:


cimstop:
	$(CIMSERVER_STOP_SERVICE)
	$(SLEEP)

cimstart:
	$(CIMSERVER_START_SERVICE)
	$(SLEEP)

