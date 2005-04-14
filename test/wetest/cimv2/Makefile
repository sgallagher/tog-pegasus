# Wbemexec test associated w/cimv2 schema
# Perform clean build
#
ROOT=../../..

include $(ROOT)/mak/config.mak
include $(ROOT)/test/config.mak

DIRS = \
	Load\
	AssociatorNames\
	Associators\
	GetClass\
	CreateClass\
	ModifyClass\
	DeleteClass\
	EnumerateInstances \
	EnumerateInstanceNames\
	EnumerateQualifiers\
	GetQualifier\
	SetQualifier\
	DeleteQualifier\
	References \
	ReferenceNames

ifndef PEGASUS_XML_ORDERING_DEFECT_ENCOUNTERED
   DIRS += \
        EnumerateClasses\
        EnumerateClassNames
endif

include $(ROOT)/mak/recurse.mak

