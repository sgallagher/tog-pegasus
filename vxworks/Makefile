all: romfs image build

ROMFS=$(PEGASUS_HOME)/romfs

romfs:
	mkdir -p $(ROMFS)/lib
	cp $(WIND_BASE)/target/usr/root/SIMPENTIUMgnu/bin/libc.so.1 $(ROMFS)/lib

SHELL_DEFAULT_CONFIG="LINE_EDIT_MODE=emacs,LINE_LENGTH=256,STRING_FREE=manual,INTERPRETER=Cmd,VXE_PATH=$(PEGASUS_HOME)/bin"

image:
	mkdir -p image
	( cd image; vxprj create linux gnu vxsim.wpj )
	( cd image; vxprj component add INCLUDE_POSIX_PTHREAD_SCHEDULER )
	( cd image; vxprj component remove INCLUDE_SHELL_BANNER )
	( cd image; vxprj component remove INCLUDE_WDB_BANNER )
	( cd image; vxprj parameter set LOCAL_MEM_SIZE 0x04000000 )
	( cd image; vxprj parameter setstring SHELL_DEFAULT_CONFIG $(SHELL_DEFAULT_CONFIG) )


build:
	( cd image; make TOOL=gnu ROMFS_DIR=$(ROMFS) )
	cp image/vxWorks .
	cp image/vxWorks.sym .

clean:
	rm -rf image
	rm -rf $(ROMFS)/lib/libc.so.1
	rm -rf counterFile
	rm -rf nvram.vxWorks0
	rm -rf vxWorks.sym
	rm -rf vxWorks
