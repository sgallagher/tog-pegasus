ROOT = ../..
DIR = Server
include $(ROOT)/mak/config.mak

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegconfig$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegserver$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegclient$(LIB_SUFFIX) 

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
    LIBRARIES += $(LIB_DIR)/$(LIB_PREFIX)slp$(LIB_SUFFIX)		
endif

ifeq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
DYNAMIC_LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegconfig.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegrepository.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegserver.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegprovider.x \
    $(LIB_DIR)/$(LIB_PREFIX)pegclient.x
else
DYNAMIC_LIBRARIES = \
    -lpegcommon \
    -lpegconfig \
    -lpegrepository \
    -lpegserver \
    -lpegprovider \
    -lpegclient \
    -lslp
endif

EXTRA_INCLUDES = $(SYS_INCLUDES)

PROGRAM = cimserver

SOURCES = cimserver.cpp

include $(ROOT)/mak/program.mak

ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
  SYS_LIBS = ws2_32.lib advapi32.lib	
endif


tests:

install:
	$(COPY) cimserver_planned.conf $(PEGASUS_HOME)
	$(COPY) cimserver_current.conf $(PEGASUS_HOME)

run: install
ifeq ($(PEGASUS_PLATFORM),WIN32_IX86_MSVC)
	cimserver -l
else
	cimserver
endif

