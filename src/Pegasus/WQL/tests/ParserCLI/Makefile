ROOT = ../../../../..

DIR = Pegasus/WQL/tests/ParserCLI

LIBRARIES = \
    $(LIB_DIR)/$(LIB_PREFIX)pegwql$(LIB_SUFFIX) \
    $(LIB_DIR)/$(LIB_PREFIX)pegcommon$(LIB_SUFFIX)

include $(ROOT)/mak/config.mak

PROGRAM = ParserCLI
SOURCES = ParserCLI.cpp

include $(ROOT)/mak/program.mak

tests:

test1:
	ParserCLI "select * from MyClass"

test2:
	ParserCLI \
	    "SELECT min,max,count,flag" \
	    "FROM MyClass" \
	    "WHERE min > 10 AND max < 90 OR count > 10 AND flag = TRUE"

test3:
	ParserCLI \
	    "SELECT *" \
	    "FROM MyClass" \
	    "WHERE min > 10 AND max < 90 OR count IS NULL"

test4:
	ParserCLI \
	    "SELECT *" \
	    "FROM MyClass" \
	    "WHERE x >= 10 OR y > 90 AND z = 500"

test5:
	ParserCLI \
	    "SELECT *" \
	    "FROM MyClass" \
	    "WHERE (x >= 10 OR y > 90) AND z = 500"
