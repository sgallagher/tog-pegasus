ROOT = ../../../..

OLD_REPOSITORY_PATH = ./
NEW_REPOSITORY_PATH = ./newRepository
RESULTDIR           = ./newRepository/test\#repupgrade
TESTNAMESPACE       = test/repupgrade
RESULTFILE 	    =  $(TMP_DIR)/result
MASTERRESULTFILE    = $(ROOT)/src/$(DIR)/result.master

DIR = Clients/repupgrade/tests

include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon \
    pegrepository 

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = upgradetest

SOURCES = UpgradeTest.cpp

include $(ROOT)/mak/program.mak
include $(ROOT)/mak/test.mak

tests:
ifndef PEGASUS_USE_RELEASE_DIRS
  ifndef PEGASUS_OS_HPUX	
	@$(RMREPOSITORY) $(OLD_REPOSITORY_PATH)repository/test#repupgrade
	@$(RMREPOSITORY) ./newRepository
	@ cimmofl "-R$(OLD_REPOSITORY_PATH)" "-I$(CIM_SCHEMA_DIR)" "-n$(TESTNAMESPACE)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/Core$(CIM_SCHEMA_VER)_Qualifiers.mof
	@ cimmofl "-R$(OLD_REPOSITORY_PATH)" "-n$(TESTNAMESPACE)" $(ROOT)/src/Providers/sample/Load/TST_Family.mof
	@ $(MKDIRHIER) $(NEW_REPOSITORY_PATH)
	@repupgrade -o $(OLD_REPOSITORY_PATH)repository -n $(NEW_REPOSITORY_PATH)
	$(PROGRAM) $(OLD_REPOSITORY_PATH)repository $(NEW_REPOSITORY_PATH)
	@$(RM) $(RESULTFILE)
	@$(MAKE) -i -s tests_ignoreerror
	@$(STRIPCRS)
	@$(COMPARERESULTS)
	@$(ECHO) +++ Test passed +++
  endif
endif

tests_ignoreerror:
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Display usage - error" >> $(RESULTFILE)
	@repupgrade >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@repupgrade -o  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Display Usage - error" >> $(RESULTFILE)
	@repupgrade -n  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Display help message" >> $(RESULTFILE)
	@repupgrade -h  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Display help message" >> $(RESULTFILE)
	@repupgrade --help  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Non-existent old repository - error case" >> $(RESULTFILE)
	@repupgrade -o non-existent-dir  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Non-existent new repository - error case" >> $(RESULTFILE)
	@repupgrade -n non-existent-dir  >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Combining options - error case" >> $(RESULTFILE)
	@repupgrade -h -o >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Combining options - error case" >> $(RESULTFILE)
	@repupgrade -h -r >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Combining options - error case" >> $(RESULTFILE)
	@repupgrade -h -n >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Combining options - error case" >> $(RESULTFILE)
	@repupgrade -o -h >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Combining options - error case" >> $(RESULTFILE)
	@repupgrade -n -h >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Combining options - error case" >> $(RESULTFILE)
	@repupgrade -o -o >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Combining options - error case" >> $(RESULTFILE)
	@repupgrade -n -n >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Combining options - error case" >> $(RESULTFILE)
	@repupgrade -h -h >> $(RESULTFILE) $(REDIRECTERROR)
	@$(ECHO) "  " >> $(RESULTFILE)
	@$(ECHO) "Combining options - error case" >> $(RESULTFILE)
	@repupgrade -v -v >> $(RESULTFILE) $(REDIRECTERROR)

poststarttests:

install:

