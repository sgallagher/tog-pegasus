# Makefile builds the required Pegasus Namespaces 
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak

all:

# Delete and rebuild the repository using the MOF compiler.

repository:
	$(MAKE) --directory=Internal repository
	$(MAKE) --directory=InterOp repository
	$(MAKE) --directory=ManagedSystem repository

repositoryServer:
	$(MAKE) --directory=Internal repositoryServer
	$(MAKE) --directory=InterOp repositoryServer
	$(MAKE) --directory=ManagedSystem repositoryServer

clean:
	$(MAKE) --directory=Internal clean
	$(MAKE) --directory=InterOp clean
	$(MAKE) --directory=ManagedSystem clean


depend:

sub:

misc:

tests:

poststarttests:

general:

xml:
	@ echo +++++ Loading CIM_Schema25.mof into root/cimv2 namespace...
	@ cimmof --xml -I$(MOF_PATH) -nroot/cimv2 $(MOF_PATH)/CIM_Schema25.mof
