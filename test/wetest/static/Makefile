ROOT=../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

DIRS = \
	Load\
	Headers

BROKEN = \
	CreateInstance\
	GetInstance\
	EnumerateInstanceNames\
	EnumerateInstances\
	ErrorXml\
	ModifyInstance\
	DeleteInstance\
        Property

# These tests are not re-runnable
#	ModifyClass\
#	DeleteClass
	
include $(ROOT)/mak/recurse.mak

