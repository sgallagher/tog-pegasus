# Makefile builds the Pegasus Repository
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = ../../../..
include $(ROOT)/mak/config.mak

MOF_PATH = $(ROOT)/Schemas/CIM25


all:

# Delete and rebuild the repository using the MOF compiler.

repository:
	@ echo +++++ Populating CIM repository - this will take a minute
	@ $(RMDIRHIER) $(REPOSITORY_ROOT)
	@ echo +++++ Loading CIM_Core.mof into root namespace...
	@ cimmof -I$(MOF_PATH) -nroot $(MOF_PATH)/CIM_Core25.mof
	@ echo +++++ Loading CIM_Schema25.mof into root/cimv2 namespace...
	@ cimmof -I$(MOF_PATH) -nroot/cimv2 $(MOF_PATH)/CIM_Schema25.mof

clean:

depend:

sub:

misc:

tests:
