PEGASUS_PLATFORM = HPUX_IA64_ACC

ifndef PEGASUS_LIB_DIR
  PEGASUS_LIB_DIR = /opt/wbem/lib
endif

ifndef PEGASUS_PROVIDER_LIB_DIR
  PEGASUS_PROVIDER_LIB_DIR = /opt/wbem/lib/providers
endif

ifndef CIM_MOF_PATH
  CIM_MOF_PATH = /etc/opt/wbem/mof
endif

ifndef PEGASUS_INCLUDES
  PEGASUS_INCLUDES = -I /opt/wbem/include
endif

ifndef SAMPLES_DIR
  SAMPLES_DIR = /opt/wbem/samples
endif

SYM_LINK_LIB = $(PEGASUS_PROVIDER_LIB_DIR)/lib$(LIBRARY)

COMPILE_COMMAND = aCC

PLATFORM_SUFFIX = so

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -DHPUX_IA64_NATIVE_COMPILER -DPEGASUS_TEMP_HARDCODED_IND_DELIVERY -DINDICATION_DIR=\"/var/opt/wbem\"

## Options:
##     +DD64  uses the LP64 data model
##     -AA    turns on newly supported ANSI C++ Standard features
##     -mt    sets various -D flags to enable multi-threading
##     -b     creates a shared library
##     -Wl,   passes the following option to the linker
##       +s   causes the linked image or shared lib to be able to
##            search for any referenced shared libs dynamically in
##            SHLIB_PATH (LD_LIBRARY_PATH on 64-bit HP9000)
##       +b   enables dynamic search in the specified directory(ies)
##     +Z     produces position independent code (PIC)
##

PROGRAM_COMPILE_OPTIONS = +DD64 -AA -mt

LIBRARY_COMPILE_OPTIONS = $(PROGRAM_COMPILE_OPTIONS) +Z

LIBRARY_LINK_OPTIONS = $(LIBRARY_COMPILE_OPTIONS) -b -Wl,+s -Wl,+b$(PEGASUS_LIB_DIR) -L$(PEGAGUS_LIB_DIR)

PROGRAM_LINK_OPTIONS = $(PROGRAM_COMPILE_OPTIONS) -L$(PEGASUS_LIB_DIR)

SYS_LIBS = -lpthread -lrt

LIBRARY_LINK_COMMAND = $(COMPILE_COMMAND)

PROGRAM_LINK_COMMAND = $(COMPILE_COMMAND)

LINK_OUT = -o

