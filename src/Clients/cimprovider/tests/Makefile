ROOT = ../../../..

DIR = Clients/cimprovider/tests

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

RESULTFILE =  $(TMP_DIR)/result
MASTERRESULTFILE = $(ROOT)/src/$(DIR)/result.master

tests:

install:

clean:
	@$(RM) $(RESULTFILE)

poststarttests:
	@$(RM) $(RESULTFILE)
	@$(MAKE) -i -s poststarttests_ignoreerror
	@$(STRIPCRS)
	$(COMPARERESULTS)
	@$(ECHO) +++ Test passed +++
	@$(RM) $(RESULTFILE)

poststarttests_ignoreerror:
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@cimprovider >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@cimprovider -d  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@cimprovider -d -m  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@cimprovider -e  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@cimprovider -e -m  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@cimprovider -r >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@cimprovider -r -m >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Duplicate -d parameter" >> $(RESULTFILE)
	@cimprovider -d -m test -d -m test2  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Duplicate -e parameter" >> $(RESULTFILE)
	@cimprovider -e -m test -e -m test2  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Duplicate -r parameter" >> $(RESULTFILE)
	@cimprovider -r -m test -r -m test2  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Duplicate -l parameter" >> $(RESULTFILE)
	@cimprovider -l -l >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Duplicate -s parameter" >> $(RESULTFILE)
	@cimprovider -l -s -s >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Unexpected option" >> $(RESULTFILE)
	@cimprovider -d -l -m test >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Unexpected option" >> $(RESULTFILE)
	@cimprovider -e -l -m test >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Unexpected option" >> $(RESULTFILE)
	@cimprovider -r -l -m test >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Unexpected option" >> $(RESULTFILE)
	@cimprovider -l -r >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Unexpected option" >> $(RESULTFILE)
	@cimprovider -d -p test>> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Unexpected option" >> $(RESULTFILE)
	@cimprovider -l -s -m test>> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "argument was unexpected" >> $(RESULTFILE)
	@cimprovider -l test>> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "argument was unexpected" >> $(RESULTFILE)
	@cimprovider -l -s test>> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Disable provider module" >> $(RESULTFILE)
	@cimprovider -d -m ComputerSystemProvider>> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	@$(ECHO) "Disable the provider module again" >> $(RESULTFILE)
	@cimprovider -d -m ComputerSystemProvider>> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	@$(ECHO) "Enable provider module" >> $(RESULTFILE)
	@cimprovider -e -m ComputerSystemProvider>> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)
	@$(ECHO) "Enable the provider module again" >> $(RESULTFILE)
	@cimprovider -e -m ComputerSystemProvider>> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) >> $(RESULTFILE)

depend:

general:

strip-license:

prepend-license:
