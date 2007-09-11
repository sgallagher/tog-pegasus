all: romfs image build

ROMFS=$(PEGASUS_HOME)/romfs

romfs:
	mkdir -p $(ROMFS)/lib
	cp $(WIND_BASE)/target/usr/root/SIMPENTIUMgnu/bin/libc.so.1 $(ROMFS)/lib

image:
	mkdir -p image
	( cd image; vxprj create linux gnu vxsim.wpj )
	( cd image; vxprj component add INCLUDE_POSIX_PTHREAD_SCHEDULER )
	( cd image; vxprj parameter set LOCAL_MEM_SIZE 0x04000000 )

build:
	( cd image; make TOOL=gnu ROMFS_DIR=$(ROMFS) )

clean:
	rm -rf image
	rm -rf $(ROMFS)/lib/libc.so.1
