include $(ROOT)/mak/config-unix.mak

OS = zos

ARCHITECTURE = zseries

COMPILER = ibm

#SYS_INCLUDES = -I/usr/lpp/tcpip/include -I/usr/lpp/ioclib/include -I$(ROOT)/src/StandardIncludes/zOS
SYS_INCLUDES = -I/usr/lpp/tcpip/include -I/usr/lpp/ioclib/include
DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM) -D_OPEN_SOURCE=3

DEFINES += -DPEGASUS_DISABLE_KILLING_HUNG_THREADS

DEPEND_DEFINES = -D__IBMCPP__=400

ifdef PEGASUS_DEBUG
FLAGS = -+ -g -W "c,ASCII,XPLINK,dll,expo,rtti(dynamiccast),langlvl(extended),float(ieee)"
PR_FLAGS = -+ -g -W "c,ASCII,XPLINK,dll,expo,langlvl(extended),FLOAT(IEEE)" -W "l,XPLINK,dll"
else
FLAGS = -+ -O2 -W "c,ASCII,XPLINK,dll,expo,rtti(dynamiccast),langlvl(extended),float(ieee)"
PR_FLAGS = -+ -O2 -W "c,ASCII,XPLINK,dll,expo,langlvl(extended),FLOAT(IEEE)" -W "l,XPLINK,dll"
endif

SYS_LIBS =

CXX = c++

SH = sh

YACC = bison

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes

PEGASUS_HAS_MAKEDEPEND = yes
