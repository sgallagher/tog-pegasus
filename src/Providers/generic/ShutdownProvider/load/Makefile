# Makefile loads the Pegasus CIM Object Manager provider mof

ROOT = ../../../../..
include $(ROOT)/mak/config.mak

MOF_PATH = ../


all:

# Delete and rebuild the repository using the MOF compiler.

shutdownService:
	@ echo +++++ adding shutdown provider mof ++++++
	@ echo +++++ Loading PG_ShutdownService.mof into root/cimv2 namespace...
	@ cimmof -I$(MOF_PATH) -nroot/cimv2 ../PG_ShutdownService.mof

clean:

depend:

sub:

misc:

tests:
