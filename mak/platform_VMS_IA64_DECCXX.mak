include $(ROOT)/mak/config-vms.mak

OS = VMS

ARCHITECTURE = ia64

COMPILER = deccxx

SYS_VMSINCLUDES = -I$(ROOT)/src/stdcxx/cwrappers

DEFINES = -DPEGASUS_OS_VMS -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_INCLUDES =

OPT_DIR = $(HOME_DIR)/opt

CFLAGS = /repos=$(CXXREPOSITORY_VMSROOT)/template_def=time
CCFLAGS = /OPT=INLINE=ALL/nowarn
#CFLAGS = /repos=$(CXXREPOSITORY_VMSROOT)/template_def=time/names=as_is
#CFLAGS = /repos=$(CXXREPOSITORY_VMSROOT)/template_def=time/preprocess_only/implicit_include
ifdef PEGASUS_DEBUG
CFLAGS += /debug/noopt/show=include/lis=$(OBJ_VMSDIRA)]
CCFLAGS = /debug/noopt/nowarn/show=include/lis=$(OBJ_VMSDIRA)]
#CFLAGS += /debug/noopt/show=include/lis=$(OBJ_VMSDIRA)]/be_dump="-mGLOB_show_limit_info"

LFLAGS = /debug/map=$(BIN_VMSDIRA)]$(PROGRAM)
endif

INSTALL_LIBRARY = $(ECHO) Not supported.
INSTALL_PROGRAM = $(ECHO) Not supported.

SYS_LIBS =+sys$share:sys$lib_c/lib

# SSL support
OPENSSL_VMSHOME =/Pegasus_Blddsk/OpenSSL
OPENSSL_HOME = $(OPENSSL_VMSHOME)
PEGASUS_HAS_SSL = yes
#PEGASUS_HAS_PERFINST = yes
#PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER = yes

CXX = cxx

SH = sh

YACC = bison

COPY = mu copy

MOVE = mu move

PEGASUS_SUPPORTS_DYNLIB = yes

LIB_SUFFIX =.olb
