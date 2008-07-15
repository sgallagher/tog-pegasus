.PHONY: romfs

CWD=$(shell pwd)
ROMFS=$(CWD)/romfs

all: proj
	(cd proj; make ROMFS_DIR=$(ROMFS) RAM_LOW_ADRS=07200000 RAM_HIGH_ADRS=07600000)

ifndef TOOL
  TOOL=gnu
endif

proj:
	mkdir -p proj
	cp pegasus.cdf proj
	( cd proj; vxprj create iq8134x $(TOOL) vxsim.wpj )

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
