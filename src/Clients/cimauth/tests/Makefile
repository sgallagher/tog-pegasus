ROOT = ../../../..

DIR = Clients/cimauth/tests

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

poststarttests_ignoreerror:
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@cimauth >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@cimauth -a  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@cimauth -m  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@cimauth -r  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@cimauth -a -m  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@cimauth -a -r  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Duplicate -a parameter" >> $(RESULTFILE)
	@cimauth -a -u www -n root/cimv2 -a -u xp  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Add auth for user not existing on system - error" >> $(RESULTFILE)
	@cimauth -a -u xp -n test -R  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Add CIM user www" >> $(RESULTFILE)
	@cimuser -a -u www -w www  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Add auth for user www on root/cimxyz2 - error" >> $(RESULTFILE)
	@cimauth -a -u www -n root/cimxyz2 -R  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Add auth for user www on root/cimv2" >> $(RESULTFILE)
	@cimauth -a -u www -n root/cimv2 -R  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Modify auth for user www on root/cimv2" >> $(RESULTFILE)
	@cimauth -m -u www -n root/cimv2 -R -W  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Add auth again for user www on root/cimv2 - error" >> $(RESULTFILE)
	@cimauth -a -u www -n root/cimv2 -R  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Modify wrong user name - error" >> $(RESULTFILE)
	@cimauth -m -u xuztest1 -n root/cimv2 -R >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Remove user xp - error" >> $(RESULTFILE)
	@cimauth -r -u xp -n root/cimv2 >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Remove auth for user www on root/cimv2" >> $(RESULTFILE)
	@cimauth -r -u www -n root/cimv2  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Remove all auth for user www" >> $(RESULTFILE)
	@cimauth -r -u www  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "Remove CIM user www" >> $(RESULTFILE)
	@cimuser -r -u www  >> $(RESULTFILE) $(REDIRECTERROR)

depend:
