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
	TestParser
