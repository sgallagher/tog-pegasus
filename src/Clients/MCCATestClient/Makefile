ROOT = $(PEGASUS_ROOT)

include $(ROOT)/mak/config.mak
include $(ROOT)/mak/configschema.mak

PEGASUS_ZOS_PROGRAM_OBJECT = yes

DIR = Clients/MCCATestClient
include $(ROOT)/mak/config.mak

ifdef PEGASUS_HAS_SSL
   FLAGS += -DPEGASUS_HAS_SSL -DPEGASUS_SSL_RANDOMFILE
   SYS_INCLUDES += -I$(OPENSSL_HOME)/include
endif

LIBRARIES = \
    pegcommon \
    pegmanagedclient

PROGRAM = MCCATestClient
SOURCES = MCCATestClient.cpp

ifneq ($(PEGASUS_PLATFORM),ZOS_ZSERIES_IBM)
  ifneq ($(OS),HPUX)
    ifdef PEGASUS_ENABLE_SLP
       LIBRARIES += slp
    endif
  endif
endif

include $(ROOT)/mak/program.mak

tests:

poststarttests:
	
	$(PROGRAM)

# Load the sample MOF with the local compiler. Cleans the existing repository
# before reloading.
repository:
	@ echo +++++ Removing existing repository named root/test/A
	@$(RMREPOSITORY) root/test/A
	@ echo +++++ Removing existing repository named root/test/B
	@$(RMREPOSITORY) root/test/B

# Loading Core mof into the two new test namespaces	
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into root/test/A namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n/root/test/A $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	
	@ echo +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into root/test/B namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -I$(CIM_SCHEMA_DIR) -n/root/test/B $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof

# Loading MCCA_TestClass into the two namespaces
	@ echo +++++ Loading MCCA_TestClass into root/test/A namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -nroot/test/A MCCA_TestClass.mof
	@ echo +++++ Loading MCCA_TestClass into root/test/B namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -nroot/test/B MCCA_TestClass.mof
# Loading Association Schema definition into both namespaces		
	@ echo +++++ Loading MCCA_TestAssocClass into root/test/A namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -nroot/test/A $(ROOT)/src/Providers/TestProviders/MCCA_TestAssocProvider/MCCA_TestAssocPrvSchema.mof
	@ echo +++++ Loading MCCA_TestAssocClass into root/test/B namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -nroot/test/B $(ROOT)/src/Providers/TestProviders/MCCA_TestAssocProvider/MCCA_TestAssocPrvSchema.mof
# Registering Provider in PG_InterOp
	@ echo +++++ Loading  MCCA_TestAssocClass provider definitions into PG_InterOp namespace ...
	@ cimmofl -R$(REPOSITORY_DIR) -n$(INTEROPNS) $(ROOT)/src/Providers/TestProviders/MCCA_TestAssocProvider/MCCA_TestAssocPrvReg.mof
	
unregister:
	$(MAKE) -i unregisterproviders

unregisterproviders:
	cimprovider -r -m MCCATestAssocProviderModule	
