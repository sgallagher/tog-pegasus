# Configuration options for Pegasus on all architectures running Linux 

include $(ROOT)/mak/config-unix.mak

PEGASUS_PLATFORM_LINUX_GENERIC_GNU = 1
DEFINES += -DPEGASUS_PLATFORM_LINUX_GENERIC_GNU
DEFINES += -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

OS = linux

COMPILER = gnu

PLATFORM_VERSION_SUPPORTED = yes

CXX = g++

SH = sh

YACC = yacc

COPY = cp

MOVE = mv

PEGASUS_SUPPORTS_DYNLIB = yes

MAJOR_VERSION_NUMBER = 1

LIB_SUFFIX = .so.$(MAJOR_VERSION_NUMBER)

DEFINES += -DPEGASUS_HAS_SIGNALS

SYS_LIBS = -ldl -lpthread -lcrypt

ifdef PEGASUS_HAS_SSL
  SYS_LIBS += -lssl
endif

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

## The following flags need to be set or unset
## to compile-in the code required for PAM authentication
## and compile-out the code that uses the password file.

 DEFINES += -DPEGASUS_PAM_AUTHENTICATION -DPEGASUS_NO_PASSWORDFILE

##
## The following flag sets the Authentication to be PAM based authentication.
##
  ifdef PEGASUS_ALWAYS_USE_PAM
   DEFINES += -DPEGASUS_ALWAYS_USE_PAM
  endif
endif


ifdef PEGASUS_DEBUG
FLAGS += -g -fPIC -W -Wall -Wno-unused  -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT -DPEGASUS_DEBUG
else
FLAGS += -O2 -fPIC -W -Wall -Wno-unused -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT
endif

ifdef PEGASUS_NO_SLP
  FLAGS += -DPEGASUS_NO_SLP
  NO_SLP = true
endif

ifdef PEGASUS_HAS_PERFINST
  FLAGS += -DPEGASUS_HAS_PERFINST
endif

ifndef PEGASUS_USE_MU_DEPEND
PEGASUS_HAS_MAKEDEPEND = yes
endif

ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
 FLAGS += -DPEGASUS_LOCAL_DOMAIN_SOCKET
endif

ifdef USE_CONNECTLOCAL
 FLAGS += -DUSE_CONNECTLOCAL
endif
