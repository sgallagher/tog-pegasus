# Makefile builds the Pegasus Repository
# ATTN: Today this makefile is implementation dependent.
#       It assumes that the repository is a set of directories
#       that can be removed by removing the directories.
#       This must be updated to an implementation independent repository
#       remove function.

ROOT = $(PEGASUS_ROOT)
include $(ROOT)/mak/config.mak

# The following two variables determine which CIM Schema is compiled
# from the CIMSchemas Directory.
# To Change the CIM Schema used, change the following two environment
# variables.

SCHEMA_VER=25
SCHEMA_DIR=CIM$(SCHEMA_VER)

#Special because Karl goofed and put the prelim on after the version number.
#SCHEMA_VER=26
#SCHEMA_DIR=CIM26Prelim

# Define Namespace in which the CIM Schemas are to be Loaded.
# Note that by default, the CORE is also loaded into root.
NAMESPACE=root/cimv2

MOF_PATH = $(ROOT)/Schemas/$(SCHEMA_DIR)

all:

# Delete and rebuild the repository using the MOF compiler.

repository:
	@ echo +++++ Removing existing repository named $(REPOSITORY_ROOT)
	@ $(RMREPOSITORY) $(REPOSITORY_ROOT)
	@ echo +++++ Populating CIM repository - this will take a while
	@ echo +++++ Loading CIM_Core$(SCHEMA_VER).mof into root namespace...
	@ cimmofl -I$(MOF_PATH) -nroot $(MOF_PATH)/CIM_Core$(SCHEMA_VER).mof
	@ echo +++++ Loading CIM_Schema$(SCHEMA_VER).mof into $(NAMESPACE) namespace...
	@ cimmofl -I$(MOF_PATH) -n$(NAMESPACE) $(MOF_PATH)/CIM_Schema$(SCHEMA_VER).mof

# The following is a hangover from previous testing and simply loads the
# core into the namespace defined by NAMESPACE.  It should probably be 
# deleted since there is no use for it any more.
short:
	@ echo +++++ Populating CIM repository - this will take a minute
	$(RMREPOSITORY) $(REPOSITORY_ROOT)
	echo +++++ Loading CIM_Core.mof into root namespace...
	cimmofl -I$(MOF_PATH) -n$(NAMESPACE) $(MOF_PATH)/CIM_Core$(SCHEMA_VER).mof

clean:
	@ echo +++++ Removing existing CIM Repository named $(REPOSITORY_ROOT)
	@ $(RMREPOSITORY) $(REPOSITORY_ROOT)


depend:

sub:

misc:

tests:

poststarttests:

general:

xml:
	@ echo +++++ Loading CIM_Schema25.mof into root/cimv2 namespace...
	@ cimmof --xml -I$(MOF_PATH) -nroot/cimv2 $(MOF_PATH)/CIM_Schema25.mof
