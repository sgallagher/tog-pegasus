SYM_LINK_LIB = $(PEGASUS_PROVIDER_LIB_DIR)/lib$(LIBRARY)

COMPILE_COMMAND = g++

COMPILE_C_COMMAND = gcc

PLATFORM_SUFFIX = so

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

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

PROGRAM_LINK_OPTIONS = -Xlinker -rpath -Xlinker $(PEGASUS_DEST_LIB_DIR)

LIBRARY_COMPILE_OPTIONS = $(PROGRAM_COMPILE_OPTIONS)

LIBRARY_LINK_OPTIONS = -Xlinker -rpath -Xlinker $(PEGASUS_DEST_LIB_DIR)

SYS_LIBS = -ldl -lpthread -lcrypt

LIBRARY_LINK_COMMAND = $(COMPILE_COMMAND) -shared

PROGRAM_LINK_COMMAND = $(COMPILE_COMMAND)

LINK_OUT = -o

          
