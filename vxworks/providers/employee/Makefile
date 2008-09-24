ROOT = ../../..
DIR = vxworks/providers/Exmployee
include $(ROOT)/mak/config.mak

LIBRARY = EmployeeProvider

SOURCES = \
    Alert.cpp \
    Alert_Provider.cpp \
    CIM_Indication.cpp \
    Employee.cpp \
    EmployeeLink.cpp \
    EmployeeLink_Provider.cpp \
    Employee_Provider.cpp \
    module.cpp \
    repository.cpp \
    Resource.cpp

LOCAL_DEFINES += -DCIMPLE_PEGASUS_MODULE

EXTRA_INCLUDES += -I$(PEGASUS_HOME)/include

LIBRARIES += cimplepegadap cimple pegprovider pegcommon

include $(ROOT)/mak/library.mak

regmod:

gen:
	genproj Static Alert Employee EmployeeLink
