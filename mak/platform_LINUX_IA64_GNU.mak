include $(ROOT)/mak/config-unix.mak

MAJOR_VERSION_NUMBER = 1

OS = linux

ARCHITECTURE = IA64

COMPILER = gnu

SYS_INCLUDES = -I/usr/include/g++-3

PLATFORM_VERSION_SUPPORTED = yes

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

#
## The following flags need to be set or unset
## to compile-in the code required for PAM authentication
## and compile-out the code that uses the password file.
##

ifdef PEGASUS_PAM_AUTHENTICATION
 DEFINES += -DPEGASUS_PAM_AUTHENTICATION -DPEGASUS_NO_PASSWORDFILE
endif

##
## The following flag needs to be set to compile in code for
## not listing the trace configuration properties. This will cause
## the trace configuration properties, traceComponents and traceLevel
## not to be listed when using the cimconfig -l command.
##
DEFINES += -DPEGASUS_DONOT_LIST_TRACE_PROPERTIES

ifdef PEGASUS_DEBUG
FLAGS = -g -W -Wall -Wno-unused  -fPIC -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT
else
FLAGS = -O2 -W -Wall -Wno-unused -fPIC -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT
endif

FLAGS += -DPEGASUS_PLATFORM_LINUX_GENERIC_GNU
FLAGS += -DPEGASUS_NO_SLP
NO_SLP = true

#FLAGS += -DPEGASUS_HAS_SSL
#SYS_LIBS = -ldl -lpthread -lcrypt -lssl
SYS_LIBS = -ldl -lpthread -lcrypt

# Build using fixed release settings
#
ifdef PEGASUS_USE_RELEASE_CONFIG_OPTIONS
FLAGS += -DPEGASUS_USE_RELEASE_CONFIG_OPTIONS
endif
ifdef PEGASUS_USE_RELEASE_DIRS
FLAGS += -DPEGASUS_USE_RELEASE_DIRS
endif

# PAM support
ifdef PEGASUS_PAM_AUTHENTICATION
   ifeq ($(HPUX_IA64_VERSION), yes)
      SYS_LIBS += -L$(PAMLIB_HOME) -lpam
   else
      SYS_LIBS += -lpam
   endif
endif

CXX = g++

SH = sh

YACC = yacc

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes

LIB_SUFFIX = .$(MAJOR_VERSION_NUMBER)

