ROOT = ../../../../..

DIR = Pegasus/Config/tests/ConfigFileHandler

include $(ROOT)/mak/config.mak

include ../libraries.mak

PROGRAM = TestConfigFileHandler

SOURCES = ConfigFileHandler.cpp

include $(ROOT)/mak/program.mak

FILES_TO_CLEAN = \
	./cimserver_planned.conf \
	./cimserver_planned.conf.bak \
	./cimserver_current.conf \
	./cimserver_current.conf.bak

tests:
	$(COPY) ../../../../Server/cimserver_planned.conf ./cimserver_planned.conf
	$(COPY) ../../../../Server/cimserver_planned.conf ./cimserver_current.conf
	$(PROGRAM)
	$(RM) $(FILES_TO_CLEAN)
