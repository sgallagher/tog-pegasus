MAJOR_VERSION_NUMBER = 0

OS = HPUX

ifdef ACC_COMPILER_COMMAND
   CXX = $(ACC_COMPILER_COMMAND)
else
   CXX = aCC
endif

COMPILER = acc

PLATFORM_VERSION_SUPPORTED = yes

SYS_INCLUDES = 

ifdef PEGASUS_CCOVER
 SYS_INCLUDES += -I/opt/ccover11/include
endif

ifdef PEGASUS_PURIFY
 SYS_INCLUDES += -I$(PURIFY_HOME)
endif

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -DPEGASUS_PLATFORM_HPUX_ACC

ifdef PEGASUS_AGENT
 DEFINES += -DHPUX_EMANATE
endif

ifdef PEGASUS_CCOVER
 DEFINES += -DPEGASUS_CCOVER
endif

ifdef PEGASUS_PURIFY
 DEFINES += -DPEGASUS_PURIFY
endif

ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
 DEFINES += -DPEGASUS_LOCAL_DOMAIN_SOCKET
endif

ifdef USE_CONNECTLOCAL
 DEFINES += -DUSE_CONNECTLOCAL
endif

ifdef PEGASUS_NOASSERTS
 DEFINES += -DNDEBUG
endif

##
## The following flags need to be set or unset 
## to compile-in the code required for PAM authentication
## and compile-out the code that uses the password file.
##

ifdef PEGASUS_PAM_AUTHENTICATION
 DEFINES += -DPEGASUS_PAM_AUTHENTICATION -DPEGASUS_NO_PASSWORDFILE
endif


DEPEND_INCLUDES =


## Flags:
##     +Z - produces position independent code (PIC).
##     +DAportable generates code for any HP9000 architecture
##     -Wl, passes the following option to the linker
##       +s causes the linked image or shared lib to be able to
##          search for any referenced shared libs dynamically in
##          SHLIB_PATH (LD_LIBRARY_PATH on 64-bit HP9000)
##       +b enables dynamic search in the specified directory(ies)
##

FLAGS = 

ifeq ($(PEGASUS_SUPPORTS_DYNLIB),yes)
  FLAGS += -Wl,+s -Wl,+b/opt/wbem/lib
endif

ifdef PEGASUS_DEBUG
  FLAGS += -g
endif

SYS_LIBS = -lpthread -lrt

SH = sh

YACC = bison

COPY = cp

MOVE = mv

LIB_SUFFIX = .$(MAJOR_VERSION_NUMBER)
