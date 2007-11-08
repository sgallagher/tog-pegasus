all: proj
	(cd proj; make RAM_LOW_ADRS=07200000 RAM_HIGH_ADRS=07600000)

ifndef TOOL
  TOOL=gnu
endif

proj:
	mkdir -p proj
	cp pegasus.cdf proj
	( cd proj; vxprj create iq8134x $(TOOL) vxsim.wpj )

clean:
	rm -rf proj

share:
	cp proj/vxWorks /share/vxWorks
