include $(ROOT)/mak/config-unix.mak

OS = linux

ARCHITECTURE = IA64

COMPILER = gnu

SYS_INCLUDES = -I/usr/include/g++-3

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

#
## The following flags need to be set or unset
## to compile-in the code required for PAM authentication
## and compile-out the code that uses the password file.
##

ifdef PEGASUS_PAM_AUTHENTICATION
 DEFINES += -DPEGASUS_PAM_AUTHENTICATION -DPEGASUS_NO_PASSWORDFILE
endif

ifdef PEGASUS_DEBUG
FLAGS = -g -W -Wall -Wno-unused  -fPIC -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT
else
FLAGS = -O2 -W -Wall -Wno-unused -fPIC -D_GNU_SOURCE -DTHREAD_SAFE -D_REENTRANT
endif

FLAGS += -DPEGASUS_PLATFORM_LINUX_GENERIC_GNU

#FLAGS += -DPEGASUS_HAS_SSL
#SYS_LIBS = -ldl -lpthread -lcrypt -lssl
SYS_LIBS = -ldl -lpthread -lcrypt

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

