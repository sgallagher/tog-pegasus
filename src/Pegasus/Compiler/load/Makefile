# Makefile builds the Pegasus Repository
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = ../../../..
include $(ROOT)/mak/config.mak

# The following two variables determine which CIM Schema is compiled
# from the Schemas Directory

SCHEMA_VER=26
SCHEMA_DIR=CIM26prelim

MOF_PATH = $(ROOT)/Schemas/$(SCHEMA_DIR)

all:

# Delete and rebuild the repository using the MOF compiler.

repository:
	@ echo +++++ Populating CIM repository - this will take a minute
	@ $(RMDIRHIER) $(REPOSITORY_ROOT)
	@ echo +++++ Loading CIM_Core$(SCHEMA_VER).mof into root namespace...
	@ cimmof -I$(MOF_PATH) -nroot $(MOF_PATH)/CIM_Core$(SCHEMA_VER).mof
	@ echo +++++ Loading CIM_Schema$(SCHEMA_VER).mof into root/cimv2 namespace...
	@ cimmof -I$(MOF_PATH) -nroot/cimv2 $(MOF_PATH)/CIM_Schema$(SCHEMA_VER).mof

short:
	@ echo +++++ Populating CIM repository - this will take a minute
	$(RMDIRHIER) $(REPOSITORY_ROOT)
	echo +++++ Loading CIM_Core.mof into root namespace...
	cimmof -I$(MOF_PATH) -nroot/cimv2 $(MOF_PATH)/CIM_Core$(SCHEMA_VER).mof

clean:
	@ echo +++++ Deleting CIM Repository at $(REPOSITORY_ROOT)
	@ $(RMDIRHIER) $(REPOSITORY_ROOT)


depend:

sub:

misc:

tests:

xml:
	@ echo +++++ Loading CIM_Schema25.mof into root/cimv2 namespace...
	@ cimmof --xml -I$(MOF_PATH) -nroot/cimv2 $(MOF_PATH)/CIM_Schema25.mof
