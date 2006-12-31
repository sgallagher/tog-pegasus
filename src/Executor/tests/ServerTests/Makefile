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

ifndef PASSWORD
  $(error "Usage: make PASSWORD=<password>")
endif

CIMSERVER_OPTIONS=enableAuthentication=true logLevel=TRACE

poststarttests:
	$(MAKE) stop
	$(MAKE) start
	$(MAKE) test1
	$(MAKE) stop

stop:
	@ $(ECHO) ""
	@ $(ECHO) "########## Stopping cimserver..."
	-@ cimserver -s
	@ $(SLEEP) $(SECONDS)

start:
	@ $(ECHO) ""
	@ $(ECHO) "########## Starting cimserver..."
	cimserver $(CIMSERVER_OPTIONS)


test1:
	@ $(SLEEP) $(SECONDS)
	cimcli -l 192.168.1.20 -u pegasus -p $(PASSWORD) ei CIM_ManagedElement
	
depend:

