ROOT=../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

DIRS = \
	Load\
	Headers\
	CreateInstance\
	GetInstance\
	EnumerateInstanceNames\
	EnumerateInstances\
	ErrorXml\
	ModifyInstance\
	DeleteInstance\
        Property

ifdef PEGASUS_ENABLE_EXECQUERY
DIRS += \
        ExecQuery
endif

# These tests are not re-runnable
#	ModifyClass\
#	DeleteClass
	
include $(ROOT)/mak/recurse.mak

