include $(ROOT)/mak/config-unix.mak

OS = zos

ARCHITECTURE = zseries

COMPILER = ibm

#SYS_INCLUDES = -I/usr/lpp/tcpip/include -I/usr/lpp/ioclib/include -I$(ROOT)/src/StandardIncludes/zOS
SYS_INCLUDES = -I/usr/lpp/tcpip/include -I/usr/lpp/ioclib/include
DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -D_OPEN_SOURCE=3

ifdef PEGASUS_KERBEROS_AUTHENTICATION
  DEFINES += -DPEGASUS_KERBEROS_AUTHENTICATION
endif


DEPEND_DEFINES = -D__IBMCPP__=400

ifdef PEGASUS_DEBUG
FLAGS = -g -W "c,ASCII,XPLINK,dll,expo,rtti(dynamiccast),langlvl(extended),float(ieee)"
PR_FLAGS = -g -W "c,ASCII,XPLINK,dll,expo,langlvl(extended),FLOAT(IEEE)" -W "l,XPLINK,dll"
else
FLAGS = -O2 -W "c,ASCII,XPLINK,dll,expo,rtti(dynamiccast),langlvl(extended),float(ieee)"
PR_FLAGS = -O2 -W "c,ASCII,XPLINK,dll,expo,langlvl(extended),FLOAT(IEEE)" -W "l,XPLINK,dll"
endif

ifdef PEGASUS_ZOS_SECURITY
  DEFINES += -DPEGASUS_ZOS_SECURITY
endif

ifdef PEGASUS_HAS_MY_KERBEROS
  FLAGS+= -L/usr/local/lib
  PR_FLAGS+= -L/usr/local/lib
endif

ifdef PEGASUS_ZOS_THREADLEVEL_SECURITY
  DEFINES += -DPEGASUS_ZOS_THREADLEVEL_SECURITY
endif

SYS_LIBS =

CXX = c++ -+

CC = cc

SH = sh

YACC = bison

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes

PEGASUS_HAS_MAKEDEPEND = yes
