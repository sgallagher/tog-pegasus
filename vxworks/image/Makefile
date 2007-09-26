.PHONY: repository

export HOST_PEGASUS_HOME:=$(PEGASUS_HOME)
include ../config.mak
ROOT = ../..
include $(ROOT)/mak/config.mak

all: romfs proj build

ROMFS=$(PEGASUS_HOME)/romfs

romfs:
	mkdir -p $(ROMFS)/lib
	cp $(WIND_BASE)/target/usr/root/SIMPENTIUMgnu/bin/libc.so.1 $(ROMFS)/lib

CFG="LINE_EDIT_MODE=emacs,LINE_LENGTH=256,STRING_FREE=manual,INTERPRETER=Cmd,VXE_PATH=$(PEGASUS_HOME)/bin"

VFS=$(HOME)/pegasus.vfs

HOSTNAME=$(shell hostname)

PEGASUS_PROVIDER_DIR="$(HOSTNAME):$(PEGASUS_HOME)/lib"

proj:
	mkdir -p proj
	cp pegasus.cdf proj
	( cd proj; vxprj create linux gnu vxsim.wpj )
	( cd proj; vxprj component remove INCLUDE_SHELL_BANNER )
	( cd proj; vxprj component remove INCLUDE_WDB_BANNER )
	( cd proj; vxprj parameter set LOCAL_MEM_SIZE 0x08000000 )
	( cd proj; vxprj parameter setstring PEGASUS_VFS_NAME $(VFS))
	( cd proj; vxprj parameter setstring SHELL_DEFAULT_CONFIG $(CFG) )
	( cd proj; vxprj parameter set HRFS_DEFAULT_MAX_FILES 128 )
	( cd proj; vxprj parameter set HRFS_DEFAULT_MAX_BUFFERS 128 )
	( cd proj; vxprj parameter set NUM_FILES 128 )

obsolete:
	( cd proj; vxprj component add INCLUDE_POSIX_PTHREAD_SCHEDULER )
	( cd proj; vxprj component add INCLUDE_HRFS )
	( cd proj; vxprj component add INCLUDE_HRFS_FORMAT )
	( cd proj; vxprj component add INCLUDE_PEGASUS )
	( cd proj; vxprj component add INCLUDE_DOSFS )
	( cd proj; vxprj component add INCLUDE_RAM_DISK )

build:
	( cd proj; make TOOL=gnu ROMFS_DIR=$(ROMFS) )
	cp proj/vxWorks .
	cp proj/vxWorks.sym .

clean:
	rm -rf proj
	rm -rf $(ROMFS)/lib/libc.so.1
	rm -rf counterFile
	rm -rf nvram.vxWorks0
	rm -rf vxWorks.sym
	rm -rf vxWorks

DEV=/pegasus:0
TMP=Makefile.tmp

##
## repository:
##
SRCDIR=$(HOST_PEGASUS_HOME)/repository
DESTDIR=$(DEV)/repository
INTEROP=$(DESTDIR)/root\#PG_InterOp/instances

repository:
	@ rm -rf $(TMP)
	@ echo "rm -r $(DESTDIR)" >> $(TMP)
	@ echo "mkdir $(DESTDIR)" >> $(TMP)
	@ echo "cp -r $(SRCDIR) $(DESTDIR)" >> $(TMP)
	@ echo "rm $(INTEROP)/PG_ProviderModule.instances" >> $(TMP)
	@ echo "rm $(INTEROP)/PG_ProviderModule.idx" >> $(TMP)
	@ echo "rm $(INTEROP)/PG_Provider.instances" >> $(TMP)
	@ echo "rm $(INTEROP)/PG_Provider.idx" >> $(TMP)
	@ echo "rm $(INTEROP)/PG_ProviderCapabilities.instances" >> $(TMP)
	@ echo "rm $(INTEROP)/PG_ProviderCapabilities.idx" >> $(TMP)
	@ echo "C reboot(1)" >> $(TMP)
	@ vxsim -exitOnError -s $(TMP)
	@ echo ""
	@ rm -rf $(TMP)

##
## lib:
##
SRCDIR2=$(PEGASUS_HOME)/lib
DESTDIR2=$(DEV)/lib

lib:
	@ rm -rf $(TMP)
	@ echo "rm -r $(DESTDIR2)" >> $(TMP)
	@ echo "mkdir $(DESTDIR2)" >> $(TMP)
	@ echo "cp -r $(SRCDIR2) $(DESTDIR2)" >> $(TMP)
	@ echo "C reboot(1)" >> $(TMP)
	@ vxsim -exitOnError -s $(TMP)
	@ echo ""
	@ rm -rf $(TMP)
