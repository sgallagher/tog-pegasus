PEGASUS_PLATFORM = LINUX_IX86_GNU

ifndef PEGASUS_LIB_DIR
  PEGASUS_LIB_DIR = /usr/lib/pegasus
endif

ifndef PEGASUS_PROVIDER_LIB_DIR
  PEGASUS_PROVIDER_LIB_DIR = /usr/lib/pegasus/providers
endif

ifndef CIM_MOF_PATH
  CIM_MOF_PATH = /usr/share/cim-schema/cim27
endif

ifndef PEGASUS_INCLUDES
  PEGASUS_INCLUDES = -I /usr/include
endif

ifndef SAMPLES_DIR
  SAMPLES_DIR = /usr/share/doc/packages/pegasus-wbem-devel/samples
endif

SYM_LINK_LIB = $(PEGASUS_PROVIDER_LIB_DIR)/lib$(LIBRARY)

COMPILE_COMMAND = g++

PLATFORM_SUFFIX = so

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -DINDICATION_DIR=\"/var/cache/pegasus\"

## Options:
##     -O2          turns on level 2 optimization
##     -W           prints extra warning messages
##     -Wall        enables all the warnings
##     -Wno-unused  does not print unused variable warnings
##     -Wl,         passes the following option to the linker
##       -h         causes the dynamic linker to attempt to load the shared
##                  object name specified
##     -Xlinker     passes an option to the linker
##       -rpath     adds a directory to the runtime library search path
##

PROGRAM_COMPILE_OPTIONS = -O2 -W -Wall -Wno-unused -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT


LIBRARY_COMPILE_OPTIONS = $(PROGRAM_COMPILE_OPTIONS)

LIBRARY_LINK_OPTIONS = -Xlinker -rpath -Xlinker $(PEGASUSLIB_DIR)

SYS_LIBS = -ldl -lpthread -lcrypt

LIBRARY_LINK_COMMAND = $(COMPILE_COMMAND) -shared

PROGRAM_LINK_COMMAND = $(COMPILE_COMMAND)

LINK_OUT = -o

          
