ROOT = ../../../..

DIR = Clients/cimconfig/tests

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

RESULTFILE =  $(TMP_DIR)/result
MASTERRESULTFILE = $(ROOT)/src/$(DIR)/result.master

tests:

install:

poststarttests:
	@$(RM) $(RESULTFILE)
	@$(MAKE) -i -s poststarttests_ignoreerror 
	$(COMPARERESULTS)
	@$(ECHO) +++ Test passed +++ 

poststarttests_ignoreerror:  
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "Valid cases:" >> $(RESULTFILE)
	@$(ECHO) "Testing all set options." >> $(RESULTFILE)
	@$(ECHO) "Set the current values:" >> $(RESULTFILE)
	@cimconfig -s traceLevel=1 -c  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -s traceLevel=2 -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -s traceLevel=3 -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -s traceLevel=4 -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -s traceComponents=ALL -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -s traceComponents=Config -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -s traceComponents=Config,XmlParser -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -s traceFilePath=/tmp/test.trace -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -s traceFilePath=/tmp/test1.trace -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -s traceFilePath=/tmp/test.trace -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "Testing all get options." >> $(RESULTFILE)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -g traceLevel >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -g traceComponents >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -g traceFilePath >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "Get the current values:" >> $(RESULTFILE)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -g  traceLevel -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -g  traceComponents -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -g  traceFilePath -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "Get the planned values:" >> $(RESULTFILE)
	@cimconfig -g traceLevel -p >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -g traceComponents -p >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "Set the planned values:" >> $(RESULTFILE)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -s traceComponents=ALL -p >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -s traceComponents=Config -p >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -s traceComponents=Config,XmlParser -p >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -s traceComponents= -p >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "Testing all unset options." >> $(RESULTFILE)
	@$(ECHO) "Unset the current values: " >> $(RESULTFILE)
	@cimconfig -u traceLevel -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@cimconfig -u traceComponents -c >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)
	@$(ECHO) "Unset the planned values:" >> $(RESULTFILE)
	@cimconfig -u traceComponents -p >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) " " >> $(RESULTFILE)

clean:

depend:
