#############################################################

ifdef PEGASUS_VMSHOME
  HOME_VMSDIR = $(PEGASUS_VMSHOME)
  HOME_VMSDIRA = $(PEGASUS_VMSHOMEA)
else
  ERROR = pegasus_vmshome_undefined
pegasus_vmshome_undefined:
	@ echo PEGASUS_VMSHOME environment variable undefined
	@ exit 1
endif

VMSROOT =  $(PEGASUS_VMSROOT)
OBJ_VMSDIR = $(HOME_VMSDIR)[obj$(VMSDIR)
OBJ_VMSDIRA = $(HOME_VMSDIRA)obj$(VMSDIR)
BIN_VMSDIR = $(HOME_VMSDIR)[bin
BIN_VMSDIRA = $(HOME_VMSDIRA)bin
LIB_VMSDIR = $(HOME_VMSDIR)[lib
LIB_VMSDIRA = $(HOME_VMSDIRA)lib
OPT_VMSDIR = $(HOME_VMSDIR)[opt
OPT_VMSDIRA = $(HOME_VMSDIRA)opt
PLATFORM_VMSDIR = $(PEGASUS_VMSROOT)[src.platforms.vms
PLATFORM_VMSDIRA = $(PEGASUS_VMSROOTA)src.platforms.vms
REP_DIR = $(HOME_DIR)/cxx_repository

MU = mu
#MU = mudebug

# define the location for the CXX repository

CXXREPOSITORY_VMSROOT = $(HOME_VMSDIRA)cxx_repository]

# define the location for the repository

#REPOSITORY_VMSDIR = $(HOME_VMSDIR)
#REPOSITORY_VMSROOT = $(REPOSITORY_VMSDIR)[repository]

REPOSITORY_DIR = /var/opt/wbem
REPOSITORY_VMSROOT = $(REPOSITORY_VMSDIR)[.repository]

######################################################

OS_TYPE = vms

RM = $(MU) rm

RMDIRHIER = $(MU) rmdirhier

MKDIRHIER = $(MU) mkdirhier

MUDEPEND = $(MU) depend

EXE_OUT = 

OBJ = .obj

OBJ_OUT = 

EXE = .exe

LIB_PREFIX = lib

COPY = $(MU) copy

TOUCH = $(MU) touch

ECHO = $(MU) echo

MYCOMMA = ,
MYEMPTY =
MYQUOTE ="
MYSPACE = $(MYEMPTY) $(MYEMPTY)
#T_FLAGS = $(subst -D,$(MYCOMMA),$(FLAGS))
#TMP_FLAGS = $(subst $(MYSPACE),$(MYEMPTY),$(T_FLAGS))
TMP_FLAGS = $(subst $(MYSPACE),$(MYEMPTY),$(subst -D,$(MYCOMMA),$(FLAGS)))

#T_DEFINES = $(subst -D,$(MYCOMMA),$(DEFINES))
#TMP_DEFINES = $(subst $(MYSPACE),$(MYEMPTY),$(T_DEFINES))
TMP_DEFINES = $(subst $(MYSPACE),$(MYEMPTY),$(subst -D,$(MYCOMMA),$(DEFINES)))

#T_LDEFINES = $(subst -D,$(MYCOMMA),$(LOCAL_DEFINES))
#TMP_LDEFINES = $(subst $(MYSPACE),$(MYEMPTY),$(T_LDEFINES))
TMP_LDEFINES = $(subst $(MYSPACE),$(MYEMPTY),$(subst -D,$(MYCOMMA),$(LOCAL_DEFINES)))

#T_SINCLUDES = $(subst -I,$(MYCOMMA),$(LOCAL_DEFINES))
TTMP_SINCLUDES = $(subst $(MYSPACE),$(MYEMPTY),$(subst -I,$(MYCOMMA)$(MYQUOTE),$(SYS_VMSINCLUDES)))
TMP_SINCLUDES = $(subst $(MYCOMMA),$(MYQUOTE)$(MYCOMMA),$(TTMP_SINCLUDES))

##SYS_INCLUDES =,"$(OPENSSL_VMSHOME)"

EXTRA_VMSINCLUDES =$(subst -I,$(MYQUOTE)$(MYCOMMA)$(MYQUOTE),$(EXTRA_INCLUDES))

VMSDIR = $(subst /,.,$(addprefix .,$(DIR)))

