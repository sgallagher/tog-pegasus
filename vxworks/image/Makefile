.PHONY: repository
.PHONY: romfs

#ifndef PEGASUS_ROOT
#  $(error "PEGASUS_ROOT is not defined")
#endif
#
#ifndef PEGASUS_PLATFORM
#  $(error "PEGASUS_PLATFORM is not defined")
#endif

export PEGASUS_PLATFORM=VXWORKS_PENTIUM_DIAB
export PEGASUS_HOME=$(PEGASUS_ROOT)/$(PEGASUS_PLATFORM)
ROOT=../..
include $(ROOT)/options.mak
include $(ROOT)/mak/config.mak

CWD=$(shell pwd)

ROMFS=$(CWD)/romfs

all: romfs proj build

romfs:
	mkdir -p $(ROMFS)
	cp $(BIN_DIR)/cimserver $(ROMFS)

CFG="LINE_EDIT_MODE=emacs,LINE_LENGTH=256,STRING_FREE=manual,INTERPRETER=Cmd,VXE_PATH="

ifdef VXWORKS_PENTIUM_GNU
  TOOL=gnu
else
  TOOL=diab
endif

# ( cd proj; $(foreach i, $(COMPS), vxprj component add $(i) ;) )

define NL


endef

proj:
	mkdir -p proj
	( cd proj; vxprj create linux $(TOOL) vxsim.wpj )
	( cd proj; vxprj component remove INCLUDE_SHELL_BANNER )
	( cd proj; vxprj component remove INCLUDE_WDB_BANNER )
	( cd proj; vxprj component add INCLUDE_CPLUS_IOSTREAMS )
	( cd proj; vxprj component add INCLUDE_GETSERVBYNAME )
	( cd proj; vxprj component add INCLUDE_GETADDRINFO )
	( cd proj; vxprj component add INCLUDE_GETNAMEINFO )
	( cd proj; vxprj component add INCLUDE_GETNAMEINFO_SYSCTL )
	( cd proj; vxprj component add INCLUDE_GETNETBYNAME )
	( cd proj; vxprj component add INCLUDE_SSL )
	( cd proj; vxprj component add INCLUDE_SSL )
	( cd proj; vxprj parameter set LOCAL_MEM_SIZE 0x08000000 )
	( cd proj; vxprj parameter set JOB_TASK_STACK_SIZE 16000 )
	( cd proj; vxprj parameter setstring SHELL_DEFAULT_CONFIG $(CFG) )
	( cd proj; vxprj parameter set NUM_FILES 128 )

build:
	( cd proj; make TOOL=$(TOOL) ROMFS_DIR=$(ROMFS) )
	cp proj/vxWorks .
	cp proj/vxWorks.sym .

clean:
	rm -rf romfs
	rm -rf proj
	rm -rf counterFile
	rm -rf nvram.vxWorks0
	rm -rf vxWorks.sym
	rm -rf vxWorks

