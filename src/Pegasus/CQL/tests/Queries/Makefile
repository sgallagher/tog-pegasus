
ROOT = ../../../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/test.mak

ifeq ($(PEGASUS_PLATFORM), WIN32_IX86_MSVC)
    DIFF = mu compare
else
    DIFF = diff
endif

# Steps to add a testcase:
# 1) Add your .query and .query.resgood file to this directory
# 2) Add a rule to build your .query.res file from a .query file 
# 3) Add the .query.res file to the tests target  BUT NOT YET!!!!!
# 4) Add the .query.res file to the clean target  BUT NOT YET!!!!!

Like.query.res: Like.query
	@ cqlcli Like.query -test 1 -className CQL_TestPropertyTypes > $(TMP_DIR)/Like.query.res | cd .
	@ $(DIFF) Like.query.resgood $(TMP_DIR)/Like.query.res
	@ $(RM) $(TMP_DIR)/Like.query.res
	@ $(ECHO) +++ Like.query passed successfully +++

clean:
	$(RM) Like.query.res

depend:

tests:

poststarttests:

general:

install:
