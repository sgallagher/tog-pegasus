ROOT = $(PEGASUS_ROOT)

DIR = Providers/TestProviders/EmbeddedInstanceProvider
SCHEMA_DIR = $(PEGASUS_ROOT)/Schemas/CIMPrelim291
DYNAMIC_TEST_NAMESPACE = test/EmbeddedInstance/Dynamic
STATIC_TEST_NAMESPACE = test/EmbeddedInstance/Static

include $(ROOT)/mak/config.mak

LIBRARY = EmbeddedInstanceProvider

LIBRARIES = \
	pegcommon \
	pegprovider
	
SOURCES = \
	EmbeddedInstanceProviderMain.cpp \
	EmbeddedInstanceProvider.cpp

include $(ROOT)/mak/library.mak

repository:
	cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" -n$(DYNAMIC_TEST_NAMESPACE) $(SCHEMA_DIR)/Core_Qualifiers.mof
	cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(SCHEMA_DIR)" "-n$(DYNAMIC_TEST_NAMESPACE)" -aE CIMError.mof
	cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(SCHEMA_DIR)" "-n$(DYNAMIC_TEST_NAMESPACE)" -aE $(SCHEMA_DIR)/CIM_Core.mof
	cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(SCHEMA_DIR)" "-n$(DYNAMIC_TEST_NAMESPACE)" -aE $(SCHEMA_DIR)/CIM_Event.mof
	cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(SCHEMA_DIR)" "-n$(DYNAMIC_TEST_NAMESPACE)" -aE $(SCHEMA_DIR)/CIM_Interop.mof
	cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(SCHEMA_DIR)" "-n$(DYNAMIC_TEST_NAMESPACE)" -aE EmbeddedInstanceProviderSchema.mof

	cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" -n$(STATIC_TEST_NAMESPACE) $(SCHEMA_DIR)/Core_Qualifiers.mof
	cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(SCHEMA_DIR)" "-n$(STATIC_TEST_NAMESPACE)" -aE CIMError.mof
	cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(SCHEMA_DIR)" "-n$(STATIC_TEST_NAMESPACE)" -aE $(SCHEMA_DIR)/CIM_Core.mof
	cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(SCHEMA_DIR)" "-n$(STATIC_TEST_NAMESPACE)" -aE $(SCHEMA_DIR)/CIM_Event.mof
	cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(SCHEMA_DIR)" "-n$(STATIC_TEST_NAMESPACE)" -aE $(SCHEMA_DIR)/CIM_Interop.mof
	cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(SCHEMA_DIR)" "-n$(STATIC_TEST_NAMESPACE)" -aE EmbeddedInstanceProviderSchema.mof

	cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-nroot/PG_InterOp" EmbeddedInstanceProviderR.mof

tests:

poststarttests:
