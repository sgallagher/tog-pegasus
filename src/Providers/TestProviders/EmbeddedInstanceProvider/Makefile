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

tests:

poststarttests:
