ROOT=../../../..

include $(ROOT)/mak/config.mak

DIRS = \
    Parser \
    ParserCLI

ifdef PEGASUS_ENABLE_EXECQUERY
    DIRS += \
       ExecQuery
endif

include $(ROOT)/mak/recurse.mak
