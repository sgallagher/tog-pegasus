include $(ROOT)/mak/config-vms.mak

OS = VMS

ARCHITECTURE = ALPHA

COMPILER = DECCXX

SYS_VMSINCLUDES = -I$(ROOT)/src/stdcxx/cwrappers

DEFINES = -DPEGASUS_OS_VMS -DPEGASUS_PLATFORM_$(PEGASUS_PLATFORM)

DEPEND_INCLUDES =

OBJ_DIR = $(HOME_DIR)/P_obj/$(DIR)
BIN_DIR = $(HOME_DIR)/P_bin
LIB_DIR = $(HOME_DIR)/P_lib
OPT_DIR = $(HOME_DIR)/P_opt

CFLAGS = /repos=$(CXXREPOSITORY_VMSROOT)/template_def=time
CCFLAGS = /OPT=INLINE=ALL/nowarn
#CFLAGS = /repos=$(CXXREPOSITORY_VMSROOT)/template_def=time/preprocess_only/implicit_include
ifdef PEGASUS_DEBUG
#CFLAGS += /debug/noopt/show=include/machi/lis
CFLAGS += /debug/noopt/show=include/lis=$(OBJ_VMSDIRA)]
CCFLAGS = /debug/noopt/nowarn/show=include/lis=$(OBJ_VMSDIRA)]
#CFLAGS += /debug/noopt/show=include/lis=$(OBJ_VMSDIRA)]/preprocess_only/implicit_include
LFLAGS = /debug/map=$(BIN_VMSDIRA)]$(PROGRAM)
endif

SYS_LIBS =+sys$share:sys$lib_c/lib

# SSL support
OPENSSL_VMSHOME =/Pegasus_Blddsk/OpenSSL
OPENSSL_HOME = $(OPENSSL_VMSHOME)

PEGASUS_HAS_SSL = yes
PEGASUS_DISABLE_PERFINST = yes
#PEGASUS_ENABLE_CMPI_PROVIDER_MANAGER = yes

CXX = cxx

SH = sh

YACC = bison

RM = mu rm

DIFF = mu diff

SORT = mu sort

COPY = mu copy

MOVE = mu move

PEGASUS_SUPPORTS_DYNLIB = yes

LIB_SUFFIX =.olb

# The Provider User Context feature (PEP 197) is not yet supported on OpenVMS
PEGASUS_DISABLE_PROV_USERCTXT=1
