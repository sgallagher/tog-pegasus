ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak
include $(ROOT)/mak/commands.mak

all:

tests:

CIMSERVER_OPTIONS = \
    daemon=true \
    enableAuthentication=true \
    logLevel=TRACE \
    traceLevel=4 \
    traceComponents=all

SECONDS=2

poststarttests:
	$(MAKE) stop
	$(MAKE) start
	$(MAKE) stop

stop:
	@ $(ECHO) ""
	@ $(ECHO) "########## Stopping cimserver..."
	-@ cimserver -s
	@ $(SLEEP) $(SECONDS)

start:
	@ $(ECHO) ""
	@ $(ECHO) "########## Starting cimserver..."
	@ cimserver daemon=true enableAuthentication=true

depend:
