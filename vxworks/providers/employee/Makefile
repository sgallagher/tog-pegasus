ROOT = ../../..
DIR = vxworks/providers/Exmployee
include $(ROOT)/mak/config.mak

LIBRARY = EmployeeProvider

SOURCES = \
    module.cpp \
    Resource.cpp \
    Employee.cpp \
    Employee_Provider.cpp \
    EmployeeLink.cpp \
    EmployeeLink_Provider.cpp \
    repository.cpp

LOCAL_DEFINES += -DCIMPLE_PEGASUS_MODULE

EXTRA_INCLUDES += -I$(PEGASUS_HOME)/include

LIBRARIES += cimplepegadap cimple pegprovider pegcommon

include $(ROOT)/mak/library.mak

regmod:

gen:
	genproj Static Employee EmployeeLink
