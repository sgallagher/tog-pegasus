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

# Local domain sockets, or an equivalent, is not currently supported on z/OS. Bug 2147
PEGASUS_DISABLE_LOCAL_DOMAIN_SOCKET=1

SYS_LIBS =

CXX = c++ -+

CC = cc

SH = sh

YACC = bison

RM = rm -f

DIFF = diff

SORT = sort

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

PEGASUS_SUPPORTS_DYNLIB = yes

PEGASUS_HAS_MAKEDEPEND = yes
# l10n
ifdef PEGASUS_HAS_MESSAGES
  DEFINES += -DPEGASUS_HAS_MESSAGES
  ifdef ICU_ROOT
        ifdef ICU_INSTALL
          MSG_COMPILE = ${ICU_INSTALL}/bin/genrb
        else
          MSG_COMPILE = ${ICU_ROOT}/bin/genrb
        endif
        MSG_FLAGS =
        MSG_SOURCE_EXT = .txt
        MSG_COMPILE_EXT = .res
        CNV_ROOT_CMD = cnv2rootbundle

##################################
##
## ICU_NO_UPPERCASE_ROOT if set, specifies NOT to uppercase the root
## resource bundle, default is to uppercase the root resource bundle
##
##################################

        ifdef ICU_NO_UPPERCASE_ROOT
          CNV_ROOT_FLAGS =
        else
          CNV_ROOT_FLAGS = -u
        endif

####################################
##
## ICU_ROOT_BUNDLE_LANG if set, specifies the language that the root resource
## bundle will be generated from defaults to _en if not set.  if set, for
## any directory containing resource bundles, there must exist a file name:
## package(the value of ICU_ROOT_BUNDLE_LANG).txt or the make messages
## target will fail
##
####################################

        ifdef ICU_ROOT_BUNDLE_LANG
          MSG_ROOT_SOURCE = $(ICU_ROOT_BUNDLE_LANG)
        else
          MSG_ROOT_SOURCE = _en
        endif
        SYS_INCLUDES += -I${ICU_INSTALL}/include
        DEFINES += -DPEGASUS_HAS_ICU
        ifdef ICU_INSTALL
	  FLAGS+ = -L${ICU_INSTALL}/lib
	  PR_FLAGS += -L${ICU_INSTALL}/lib
          EXTRA_LIBRARIES += ${ICU_INSTALL}/lib/libicui18n.x ${ICU_INSTALL}/lib/libicuuc.x
        endif
  endif
endif
