ROOT = ../../../../..

DIR = Pegasus/Config/tests/ConfigFileHandler

include $(ROOT)/mak/config.mak

include ../libraries.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestConfigFileHandler

SOURCES = ConfigFileHandler.cpp

include $(ROOT)/mak/program.mak

LOCAL_FILES_TO_CLEAN = \
	$(TMP_DIR)/cimserver_planned.conf \
	$(TMP_DIR)/cimserver_planned.conf.bak \
	$(TMP_DIR)/cimserver_current.conf \
	$(TMP_DIR)/cimserver_current.conf.bak

tests:
	$(COPY) ../../../../Server/cimserver_planned.conf $(TMP_DIR)/cimserver_planned.conf
	$(COPY) ../../../../Server/cimserver_planned.conf $(TMP_DIR)/cimserver_current.conf
	$(PROGRAM)
	$(RM) $(LOCAL_FILES_TO_CLEAN)

poststarttests: 
