include $(ROOT)/mak/config-unix.mak

OS = aix

ARCHITECTURE = rs

COMPILER = xlc

PLATFORM_VERSION_SUPPORTED = yes

SYS_INCLUDES = -I/usr/vacpp/include

DEFINES = -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_INCLUDES =

DEPEND_DEFINES = -D__IBMCPP__=500

ifdef PEGASUS_DEBUG
FLAGS = -g
else
FLAGS = -O2
endif

FLAGS += -qrtti=dyna 

SYS_LIBS = -ldl 

# SSL support
ifdef PEGASUS_HAS_SSL
OPENSSL_HOME = /usr/linux
endif

CXX = xlC_r

SH = sh

YACC = bison

COPY = cp

MOVE = mv

LIB_SUFFIX = .so

AIX_LIB_PRIORITY = 0
AIX_LD_HALT = 8

ifndef PEGASUS_USE_MU_DEPEND
PEGASUS_HAS_MAKEDEPEND = yes
endif

PEGASUS_SUPPORTS_DYNLIB = yes

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
        #SYS_INCLUDES += -I${ICU_ROOT}/source/common
        #SYS_INCLUDES += -I${ICU_ROOT}/source/i18n
        SYS_INCLUDES += -I${ICU_ROOT}/include
        DEFINES += -DPEGASUS_HAS_ICU
        ifdef ICU_INSTALL
          EXTRA_LIBRARIES += -L${ICU_INSTALL}/lib -licui18n -licuuc
          #SYS_LIBS += -L${ICU_INSTALL}/lib -licui18n -licuuc
        endif
  endif
endif

ifdef PEGASUS_USE_RELEASE_DIRS
  PEGASUS_DEST_LIB_DIR =/usr/linux/lib
endif

