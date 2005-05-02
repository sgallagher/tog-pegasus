ROOT = ../../../..
DIR = Unsupported/LifecycleIndicationProviders/testclient
include $(ROOT)/mak/config.mak

PEGASUS_ZOS_PROGRAM_OBJECT = yes
LIBRARIES = \
     pegclient \
     pegcommon

EXTRA_INCLUDES = $(SYS_INCLUDES)

LOCAL_DEFINES = -DINDICATION_DIR=\"$(DISPLAYCONSUMER_DIR)\"

PROGRAM = TestLifecycleIndication

SOURCES = LifecycleIndication.cpp

SAMPLEPROVIDERNS="root/SampleProvider"
SAMPLECLASS="Sample_LifecycleIndicationProviderClass"
	  
include $(ROOT)/mak/program.mak

tests:

poststarttests:
	@$(ECHO) "TestLifecycleIndication setup wql"
	@$(PROGRAM) setup wql
	@$(ECHO) "TestLifecycleIndication createInstance"
	@$(PROGRAM) createInstance
	@$(ECHO) "TestLifecycleIndication createInstance"
	@$(PROGRAM) createInstance
	@$(ECHO) "TestLifecycleIndication createInstance"
	@$(PROGRAM) createInstance
	@$(ECHO) "How many instances exist? Lets enumerate them..."
	-CLI ei -n $(SAMPLEPROVIDERNS) $(SAMPLECLASS)
	@$(ECHO) "TestLifecycleIndication deleteInstance"
	@$(PROGRAM) deleteInstance
	@$(ECHO) "TestLifecycleIndication deleteInstance"
	@$(PROGRAM) deleteInstance
	@$(ECHO) "We've deleted two instances, should just be one left..."
	-CLI ei -n $(SAMPLEPROVIDERNS) $(SAMPLECLASS)
	@$(ECHO) "TestLifecycleIndication deleteInstance"
	@$(PROGRAM) deleteInstance
	@$(ECHO) "TestLifecycleIndication cleanup"
	@$(PROGRAM) cleanup
	@$(ECHO) "TestLifecycleIndication setup cim:cql"
	@$(PROGRAM) setup cim:cql
	@$(ECHO) "TestLifecycleIndication createInstance"
	@$(PROGRAM) createInstance
	@$(ECHO) "TestLifecycleIndication createInstance"
	@$(PROGRAM) createInstance
	@$(ECHO) "TestLifecycleIndication createInstance"
	@$(PROGRAM) createInstance
	@$(ECHO) "How many instances exist? Lets enumerate them..."
	-CLI ei -n $(SAMPLEPROVIDERNS) $(SAMPLECLASS)
	@$(ECHO) "TestLifecycleIndication deleteInstance"
	@$(PROGRAM) deleteInstance
	@$(ECHO) "TestLifecycleIndication deleteInstance"
	@$(PROGRAM) deleteInstance
	@$(ECHO) "We've deleted two instances, should just be one left..."
	-CLI ei -n $(SAMPLEPROVIDERNS) $(SAMPLECLASS)
	@$(ECHO) "TestLifecycleIndication deleteInstance"
	@$(PROGRAM) deleteInstance
	@$(ECHO) "TestLifecycleIndication cleanup"
	@$(PROGRAM) cleanup
	@$(ECHO) "+++++ TestLifecycleIndication passed all tests"
