ROOT = ../../../..

DIR = Clients/cimuser/tests

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

RESULTFILE =  $(ROOT)/src/$(DIR)/result
MASTERRESULTFILE = $(ROOT)/src/$(DIR)/result.master

tests:

install:

clean:
	@$(RM) $(RESULTFILE)

poststarttests:
	@$(RM) $(RESULTFILE)
	@$(ECHO) "List Users" >> $(RESULTFILE)
	@cimuser -l >> $(RESULTFILE) $(REDIRECTERROR)
	@$(MAKE) -i -s poststarttests_ignoreerror
	$(COMPARERESULTS)
	@$(ECHO) +++ Test passed +++

poststarttests_ignoreerror:
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@cimuser >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@cimuser -a -m  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@cimuser -a -r  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Remove user lp" >> $(RESULTFILE)
	@cimuser -r -u lp  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Add user lp" >> $(RESULTFILE)
	@cimuser -a -u lp -w test  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Modify correct password" >> $(RESULTFILE)
	@cimuser -m  -u lp -w test -n test1 >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Add user lp again - error" >> $(RESULTFILE)
	@cimuser -a -u lp -w test  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Add a user not existing on system - error" >> $(RESULTFILE)
	@cimuser -a -u lp -w test  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@cimuser -a  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@cimuser -m  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Modify wrong user name - error" >> $(RESULTFILE)
	@cimuser -m  -u test1 -w test -n test1 >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@cimuser -r  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@cimuser -r -u  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Remove user lp" >> $(RESULTFILE)
	@cimuser -r -u lp >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Remove user test1 - error"  >> $(RESULTFILE)
	@cimuser -r -u test1 >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Remove user lp again" >> $(RESULTFILE)
	@cimuser -r -u lp >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Duplicate -a parameter" >> $(RESULTFILE)
	@cimuser -a -u lp -w lp -a -u lp -w lp >> $(RESULTFILE) $(REDIRECTERROR)

depend:
