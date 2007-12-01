ROOT = ../../..
DIR = vxworks/providers/Exmployee
include $(ROOT)/mak/config.mak

LIBRARY = EmployeeProvider

SOURCES = \
    module.cpp \
    Resource.cpp \
    Employee.cpp \
    Employee_Provider.cpp \
    repository.cpp

LOCAL_DEFINES += -DCIMPLE_PEGASUS_MODULE

##
## Change provider entry point from "PegasusCreateProvider" to 
## "CreateEmployeeProvider".
##
LOCAL_DEFINES += -DPegasusCreateProvider=PegasusCreateProvider_Employee

EXTRA_INCLUDES += -I$(PEGASUS_HOME)/include

LIBRARIES += cimplepegadap cimple pegprovider pegcommon

include $(ROOT)/mak/library.mak

regmod:
