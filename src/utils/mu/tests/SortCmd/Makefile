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

tests: testsort testdiffsort
	@ $(ECHO) +++++ all test passed	

testsort:
	 $(SORT) file1_unsorted > file.tmp
	 $(DIFF) file.tmp file1_sorted
	 $(RM) file.tmp
	@ $(ECHO) +++++ mu sort test passed	

testdiffsort:
	 $(call DIFFSORT,file1_unsorted,file1_reverse_sorted)
	@ $(ECHO) +++++ diffsort passed	
     

poststarttests:

