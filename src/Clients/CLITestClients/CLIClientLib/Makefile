ROOT = $(PEGASUS_ROOT)

DIR = Clients/CLITestClients/CLIClientLib

include $(ROOT)/mak/config.mak

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DPEGASUS_CLI_INTERNAL -DPEGASUS_INTERNALONLY

LIBRARY = pegCLIClientLib

LIBRARIES = \
    pegcommon \
    pegclient \
    pegprm 

SOURCES = \
    CLIClientLib.cpp    

OBJECTS = $(SOURCES:.cpp=$(OBJ))

include $(ROOT)/mak/library.mak
