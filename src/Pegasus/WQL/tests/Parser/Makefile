ROOT = ../../../../..

DIR = Pegasus/WQL/tests/Parser

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegwql$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX)

include $(ROOT)/mak/config.mak

PROGRAM = TestParser
SOURCES = Parser.cpp

include $(ROOT)/mak/program.mak

tests:

test1:
	TestParser "select * from MyClass"

test2:
	TestParser \
	    "SELECT min,max,count,flag" \
	    "FROM MyClass" \
	    "WHERE min > 10 AND max < 90 OR count > 10 AND flag = TRUE"

test3:
	TestParser \
	    "SELECT *" \
	    "FROM MyClass" \
	    "WHERE min > 10 AND max < 90 OR count IS NULL"

test3:
	TestParser \
	    "SELECT *" \
	    "FROM MyClass" \
	    "WHERE x >= 10 OR y > 90 AND z = 500"

test4:
	TestParser \
	    "SELECT *" \
	    "FROM MyClass" \
	    "WHERE (x >= 10 OR y > 90) AND z = 500"
