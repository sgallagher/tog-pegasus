ROOT = ../../../../..

DIR = Pegasus/WQL/tests/ParserCLI

LIBRARIES = pegwql pegcommon

include $(ROOT)/mak/config.mak

LOCAL_DEFINES = -DPEGASUS_INTERNALONLY

PROGRAM = TestParserCLI
SOURCES = ParserCLI.cpp

include $(ROOT)/mak/program.mak

# Warning: These tests are based on s specific set of paramaters and their
# values in ParserCLI. the values of w,x,y,z are preset and the
# end result must be true.   Thus, for example w is always TRUE

tests: test1 test2 test2a test3 test4 test5 test5a test6 test7

ifdef PEGASUS_SNIA_INTEROP_TEST
tests += test8
endif

test1:
	@ $(PROGRAM) "SELECT w,x,y,z FROM ClassName"

test2:
	@ $(PROGRAM) "SELECT w,x,y,z FROM ClassName WHERE w = TRUE OR w = FALSE"

test2a:
	@ $(PROGRAM) "SELECT w,x,y,z FROM ClassName WHERE w = TRUE AND w != FALSE"	

test3:
	@ $(PROGRAM) "SELECT w,x,y,z FROM ClassName WHERE w = TRUE AND x >= 10 AND y <= 13.10 AND z = \"Ten\"" 

test4:
	@ $(PROGRAM) "SELECT * FROM ClassName WHERE (w = TRUE AND x >= 10 AND y <= 13.10 AND z = \"Ten\") AND NOT w = TRUE IS NOT TRUE"

test5:
	@ $(PROGRAM) "SELECT * FROM ClassName WHERE NOT NOT NOT x < 5"

test5a:
	@ $(PROGRAM) "SELECT w,x,y,z FROM ClassName WHERE w <> FALSE"

test6:
	@ $(PROGRAM) "SELECT * FROM ClassName WHERE v IS NULL"

test7:
	@ $(PROGRAM) "SELECT * FROM CIM_InstCreation WHERE SourceInstance ISA CIM_ComputerSystem"

poststarttests:

