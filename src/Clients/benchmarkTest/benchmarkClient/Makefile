ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/benchmarkTest/benchmarkClient
include $(ROOT)/mak/config.mak

LIBRARIES = \
    pegcommon \
    peggetoopt \
    pegclient \
    pegcliutils \
    benchmarkDefinition

PROGRAM = benchmarkTest
SOURCES = benchmarkTest.cpp benchmarkTestException.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:

