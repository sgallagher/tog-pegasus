ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
  CIMSERVER_START_SERVICE = 
  CIMSERVER_STOP_SERVICE = 
  SLEEP =
  DIFF = mu compare
  REDIRECTERROR = 2>&1
else
  CIMSERVER_START_SERVICE = cimserver -d
  CIMSERVER_STOP_SERVICE = /usr/bin/ps -ef | /usr/bin/grep cimserver | /usr/bin/grep -v grep | /usr/bin/awk '{print "kill -9 "$$2 |"/usr/bin/sh"}'
  SLEEP = sleep 5
  DIFF = diff
  REDIRECTERROR = 2>&1
endif

ifdef DIFF
  COMPARERESULTS = @$(DIFF) $(RESULTFILE) $(MASTERRESULTFILE)
else
  COMPARERESULTS = @$(ECHO) "Results not compared."
endif

default:


cimstop:
	$(CIMSERVER_STOP_SERVICE)
	$(SLEEP)

cimstart:
	$(CIMSERVER_START_SERVICE)
	$(SLEEP)

