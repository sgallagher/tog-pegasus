#//%2005////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#// 
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
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
BIN_VMSDIR = $(HOME_VMSDIR)[bin
LIB_VMSDIR = $(HOME_VMSDIR)[lib
OPT_VMSDIR = $(HOME_VMSDIR)[opt
PLATFORM_VMSDIR = $(PEGASUS_VMSROOT)[src.platforms.vms

REP_DIR = $(HOME_DIR)/cxx_repository

OBJ_VMSDIRA = $(HOME_VMSDIRA)obj$(VMSDIR)
BIN_VMSDIRA = $(HOME_VMSDIRA)bin
LIB_VMSDIRA = $(HOME_VMSDIRA)lib
OPT_VMSDIRA = $(HOME_VMSDIRA)opt
PLATFORM_VMSDIRA = $(PEGASUS_VMSROOTA)src.platforms.vms

OBJ_VMSDIRB =obj
BIN_VMSDIRB =bin
LIB_VMSDIRB =lib
OPT_VMSDIRB =opt

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

RM = $(MU) rm

DIFF = $(MU) diff

SORT = $(MU) sort

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

