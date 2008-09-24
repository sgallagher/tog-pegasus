.PHONY: romfs

CWD=$(shell pwd)
ROMFS=$(CWD)/romfs

all: romfs proj
	(cd proj; make ROMFS_DIR=$(ROMFS) RAM_LOW_ADRS=07200000 RAM_HIGH_ADRS=07600000)

ifndef TOOL
  TOOL=gnu
endif

proj:
	mkdir -p proj
	cp pegasus.cdf proj
	( cd proj; vxprj create iq8134x $(TOOL) vxsim.wpj )
	( cd proj; vxprj parameter set NUM_SYS_MBLKS 200 )
	( cd proj; vxprj parameter set NUM_SYS_CLBLKS 200 )
	( cd proj; vxprj parameter set NUM_SYS_512 500 )

clean:
	rm -rf proj file.pem server.pem romfs

share:
	cp proj/vxWorks /share/vxWorks

certs:
	openssl req -x509 -days 10000 -newkey rsa:2048 -nodes -config ssl.cnf -keyout file.pem -out server.pem
	@ echo "Created file.pem"
	@ echo "Created server.pem"

romfs: certs
	echo "ROMFS={$(ROMFS)}"
	mkdir -p $(ROMFS)/ssl/certs
	cp file.pem $(ROMFS)/ssl/certs
	cp server.pem $(ROMFS)/ssl/certs
