ROOT = ../../../../..
## DIR = pegasus/src/utils/mu/tests/SortCmd

include $(ROOT)/mak/config.mak


LOCAL_DEFINES = -DPEGASUS_INTERNALONLY
LOCAL_CLEAN_TARGET=local-clean

## PROGRAM = 
## SOURCES =

include $(ROOT)/mak/program.mak

.PHONY: FORCE local-clean tests poststarttests

FORCE: 

local-clean:
	$(RM) *.tmp

tests: testdiffsort testsort1 testsort2
	@ $(ECHO) $(SORT) +++++ all test passed	

##
## test that charecters are not added by sorting
##
testsort1:	
	@ $(SORT) file1_unsorted.tmp > file11.tmp
	@ $(SORT) file11.tmp > file12.tmp
	@ $(SORT) file12.tmp > file13.tmp
	@ $(SORT) file13.tmp > file14.tmp
	@ $(DIFF) file11.tmp file14.tmp
	@ $(RM) *.tmp
	@ $(ECHO) $(SORT) +++++  test1 passed	
##
## test that sort order is correct by comparing to saved output file
##
testsort2:
	@ $(SORT) file1_unsorted > file11.tmp
	@ $(DIFF) file11.tmp file1_sorted
	@ $(RM) *.tmp
	@ $(ECHO) $(SORT) +++++  test2 passed

##
## test the diffsort macro
## 
testdiffsort:
	@ $(call DIFFSORT,file1_unsorted,file1_reverse_sorted)
	@ $(ECHO) diffsort +++++ passed	
     

poststarttests:

