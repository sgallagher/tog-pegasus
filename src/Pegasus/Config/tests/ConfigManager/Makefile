ROOT = ../../../../..

DIR = Pegasus/Config/tests/ConfigManager

include $(ROOT)/mak/config.mak

include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestConfigManager

SOURCES = ConfigManager.cpp

include $(ROOT)/mak/program.mak

LOCAL_FILES_TO_CLEAN = \
	./cimserver_planned.conf \
	./cimserver_planned.conf.bak \
	./cimserver_current.conf \
	./cimserver_current.conf.bak

tests:
	$(COPY) ../../../../Server/cimserver_planned.conf ./cimserver_planned.conf
	$(COPY) ../../../../Server/cimserver_planned.conf ./cimserver_current.conf
	$(PROGRAM)
	$(RM) $(LOCAL_FILES_TO_CLEAN)

poststarttests: 
