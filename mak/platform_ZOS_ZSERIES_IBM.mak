include $(ROOT)/mak/config-unix.mak

OS = zos

ARCHITECTURE = zseries

COMPILER = ibm

#SYS_INCLUDES = -I/usr/lpp/tcpip/include -I/usr/lpp/ioclib/include -I$(ROOT)/src/StandardIncludes/zOS
SYS_INCLUDES = -I/usr/lpp/tcpip/include -I/usr/lpp/ioclib/include

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_DEFINES = -D__IBMCPP__=400

ifdef PEGASUS_DEBUG
FLAGS = -g -W "c,dll,expo,rtti(dynamiccast),langlvl(extended)"
PR_FLAGS = -g -W "c,XPLINK(back,stor)" -W "l,XPLINK"
#FLAGS = -g -W "c,dll,expo,langlvl(extended),tempinc($(ROOT))"
#PR_FLAGS = -g -W "c,XPLINK(back,stor),langlvl(extended),tempinc($(ROOT))" \
# -W "l,XPLINK"
else
FLAGS = -O2 -W "c,dll,expo,rtti(dynamiccast),langlvl(extended)"
PR_FLAGS = -O2 -W "c,XPLINK" -W "l,XPLINK"
#FLAGS = -O2 -W "c,dll,expo,langlvl(extended),tempinc($(ROOT))"
#PR_FLAGS = -O2 -W "c,XPLINK,langlvl(extended),tempinc($(ROOT))" -W "l,XPLINK"
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
