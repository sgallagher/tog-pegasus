export HOST_PEGASUS_HOME:=$(PEGASUS_HOME)
include ../config.mak
ROOT = ../..
include $(ROOT)/mak/config.mak

all: romfs proj build

ROMFS=$(PEGASUS_HOME)/romfs

romfs:
	mkdir -p $(ROMFS)/lib
	cp $(WIND_BASE)/target/usr/root/SIMPENTIUMgnu/bin/libc.so.1 $(ROMFS)/lib

SHELL_DEFAULT_CONFIG="LINE_EDIT_MODE=emacs,LINE_LENGTH=256,STRING_FREE=manual,INTERPRETER=Cmd,VXE_PATH=$(PEGASUS_HOME)/bin"

proj:
	mkdir -p proj
	( cd proj; vxprj create linux gnu vxsim.wpj )
	( cd proj; vxprj component add INCLUDE_POSIX_PTHREAD_SCHEDULER )
	( cd proj; vxprj component add INCLUDE_HRFS )
	( cd proj; vxprj component add INCLUDE_HRFS_FORMAT )
	( cd proj; vxprj component add INCLUDE_PEGASUS )
	( cd proj; vxprj component remove INCLUDE_SHELL_BANNER )
	( cd proj; vxprj component remove INCLUDE_WDB_BANNER )
	( cd proj; vxprj parameter set LOCAL_MEM_SIZE 0x08000000 )
	( cd proj; vxprj parameter set PASSFS_CACHE FALSE )
	( cd proj; vxprj parameter setstring SHELL_DEFAULT_CONFIG $(SHELL_DEFAULT_CONFIG) )

obsolete:
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

SRCDIR=$(HOST_PEGASUS_HOME)/repository
DESTDIR=/pegasus:0/repository
TMP=repository.tmp

repository:
	@ rm -rf $(TMP)
	@ echo "rm -r $(DESTDIR)" >> $(TMP)
	@ echo "mkdir $(DESTDIR)" >> $(TMP)
	@ echo "cp -r $(SRCDIR) $(DESTDIR)" >> $(TMP)
	@ echo "C reboot(1)" >> $(TMP)
	@ vxsim -exitOnError -s $(TMP)
	@ echo ""
	@ rm -rf $(TMP)
