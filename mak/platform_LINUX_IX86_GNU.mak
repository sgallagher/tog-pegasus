include $(ROOT)/mak/config-unix.mak

MAJOR_VERSION_NUMBER = 1

OS = linux

ARCHITECTURE = iX86

COMPILER = gnu

ifdef PEGASUS_USES_GCCV3
  SYS_INCLUDES = -I/usr/include/g++-3
else
  SYS_INCLUDES = -I/usr/include/g++
endif

PLATFORM_VERSION_SUPPORTED = yes

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEFINES += -DPEGASUS_HAS_SIGNALS

ifdef PEGASUS_DEBUG
FLAGS = -g -W -Wall -Wno-unused  -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT -DPEGASUS_DEBUG
else
FLAGS = -O2 -W -Wall -Wno-unused -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT
endif

#
## The following flags need to be set or unset
## to compile-in the code required for PAM authentication
## and compile-out the code that uses the password file.
##

ifdef PEGASUS_PAM_AUTHENTICATION
 DEFINES += -DPEGASUS_PAM_AUTHENTICATION -DPEGASUS_NO_PASSWORDFILE
endif

SYS_LIBS = -ldl -lpthread -lcrypt

ifdef PEGASUS_HAS_PERFINST
  FLAGS += -DPEGASUS_HAS_PERFINST
endif

ifdef PEGASUS_HAS_SSL
  FLAGS += -DPEGASUS_HAS_SSL
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

ifdef PEGASUS_NO_SLP
  FLAGS += -DPEGASUS_NO_SLP
  NO_SLP = true
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

ifndef PEGASUS_USE_MU_DEPEND
PEGASUS_HAS_MAKEDEPEND = yes
endif

LIB_SUFFIX = .so.$(MAJOR_VERSION_NUMBER)

ifdef PEGASUS_LOCAL_DOMAIN_SOCKET
 FLAGS += -DPEGASUS_LOCAL_DOMAIN_SOCKET
endif

ifdef USE_CONNECTLOCAL
 FLAGS += -DUSE_CONNECTLOCAL
endif
