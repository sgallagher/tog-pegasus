misc:
	@ - bash $(ROOT)/mak/misc.sh

strip-license:
	mu strip //% //= *.h *.cpp

prepend-license:
	mu prepend $(ROOT)/doc/license.txt *.h *.cpp

ifeq ($(PEGASUS_PLATFORM),HPUX_PARISC_ACC)

ifeq ($(OPTION),1)
general:
	muplus sourcelist -Dpegasus/src/$(DIR) *.cpp
endif

ifeq ($(OPTION),2)
general:
	@$(foreach i, $(SOURCES),  printf "%s " "pegasus/src/$(DIR)/$(i)" >> $(PEGASUS_HOME)/Sources;)
endif

ifeq ($(OPTION),3)
general:
	@$(foreach i, $(SOURCES),  echo pegasus/src/$(DIR)/$(i); grep -n -eATTN -eREVIEW -e" FIX" -eFIXME $(i);)
endif

endif
