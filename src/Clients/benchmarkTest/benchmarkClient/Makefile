ROOT = $(PEGASUS_ROOT)

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/benchmarkTest/benchmarkClient
include $(ROOT)/mak/config.mak

LIBRARIES = \
    benchmarkDefinition \
    pegcliutils \
    pegclient \
    peggetoopt \
    pegcommon

PROGRAM = benchmarkTest
SOURCES = benchmarkTest.cpp benchmarkTestException.cpp

include $(ROOT)/mak/program.mak

tests:

poststarttests:
