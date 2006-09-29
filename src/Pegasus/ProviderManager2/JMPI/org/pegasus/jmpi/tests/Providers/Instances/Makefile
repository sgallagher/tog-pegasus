#//%2006////////////////////////////////////////////////////////////////////////
#//
#// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
#// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
#// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation, The Open Group.
#// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
#// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; VERITAS Software Corporation; The Open Group.
#// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
#// EMC Corporation; Symantec Corporation; The Open Group.
#//
#// Permission is hereby granted, free of charge, to any person obtaining a copy
#// of this software and associated documentation files (the "Software"), to
#// deal in the Software without restriction, including without limitation the
#// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
#// sell copies of the Software, and to permit persons to whom the Software is
#// furnished to do so, subject to the following conditions:
#//
#// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
#// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
#// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
#// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
#// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
#// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
#// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#//
#//==============================================================================
ifndef PEGASUS_ROOT
    $(error PEGASUS_ROOT environment variable undefined)
endif
ifndef PEGASUS_HOME
    $(error PEGASUS_HOME environment variable undefined)
endif

include $(PEGASUS_ROOT)/mak/config.mak
include $(PEGASUS_ROOT)/mak/configschema.mak
include $(PEGASUS_ROOT)/test/config.mak

.SUFFIXES: .java .class .jar

.java.class:
# Note: .. included in path to compile package helper classes if necessary
	$(PEGASUS_JAVA_COMPILER) -classpath $(PEGASUS_HOME)/lib/JMPIImpl.jar$(PEGASUS_JAVA_CLASSPATH_DELIMITER).. $*.java

NAMESPACE          = root/SampleProvider
PACKAGEDIR         = Instances/
JAR1               = JMPIExpInstanceProvider.jar
JAR1CLASSES        = \
	JMPIExpInstanceProvider.class
EXTRAJAR1CLASSES   = \
	CommonInstanceProvider.class \
	CommonInstanceProvider\$$CIMInstanceComparer.class \
	CommonInstanceProvider\$$CIMObjectPathComparer.class \
	CommonInstanceProvider\$$Compare.class \
        CommonInstanceProvider\$$SortableVector.class
ifneq ($(PEGASUS_JVM),gcj)
EXTRAJAR1CLASSES  += \
	CommonInstanceProvider\$$1.class
endif
JAR2               = JMPIInstanceProvider.jar
JAR2CLASSES        = \
	JMPIInstanceProvider.class
EXTRAJAR2CLASSES   = \
	CommonInstanceProvider.class \
	CommonInstanceProvider\$$CIMInstanceComparer.class \
	CommonInstanceProvider\$$CIMObjectPathComparer.class \
	CommonInstanceProvider\$$Compare.class \
        CommonInstanceProvider\$$SortableVector.class
ifneq ($(PEGASUS_JVM),gcj)
EXTRAJAR2CLASSES  += \
	CommonInstanceProvider\$$1.class
endif
JAR3               = JMPICIMInstanceProvider.jar
JAR3CLASSES        = \
	JMPICIMInstanceProvider.class
EXTRAJAR3CLASSES   = \
	CommonInstanceProvider.class \
	CommonInstanceProvider\$$CIMInstanceComparer.class \
	CommonInstanceProvider\$$CIMObjectPathComparer.class \
	CommonInstanceProvider\$$Compare.class \
        CommonInstanceProvider\$$SortableVector.class
ifneq ($(PEGASUS_JVM),gcj)
EXTRAJAR3CLASSES  += \
	CommonInstanceProvider\$$1.class
endif
JAR4               = JMPICIMInstance2Provider.jar
JAR4CLASSES        = \
	JMPICIMInstance2Provider.class
EXTRAJAR4CLASSES   = \
	CommonInstanceProvider.class \
	CommonInstanceProvider\$$CIMInstanceComparer.class \
	CommonInstanceProvider\$$CIMObjectPathComparer.class \
	CommonInstanceProvider\$$Compare.class \
        CommonInstanceProvider\$$SortableVector.class
ifneq ($(PEGASUS_JVM),gcj)
EXTRAJAR4CLASSES  += \
	CommonInstanceProvider\$$1.class
endif
JARS               = $(JAR1) $(JAR2) $(JAR3) $(JAR4)

LOCAL_CLEAN_TARGET = \
	$(JAR1CLASSES) $(EXTRAJAR1CLASSES) $(addprefix $(PEGASUS_HOME)/lib/,$(JAR1)) \
	$(JAR2CLASSES) $(EXTRAJAR2CLASSES) $(addprefix $(PEGASUS_HOME)/lib/,$(JAR2)) \
	$(JAR3CLASSES) $(EXTRAJAR3CLASSES) $(addprefix $(PEGASUS_HOME)/lib/,$(JAR3)) \
	$(JAR4CLASSES) $(EXTRAJAR4CLASSES) $(addprefix $(PEGASUS_HOME)/lib/,$(JAR4))

all build: 			$(addprefix $(PEGASUS_HOME)/lib/,$(JARS))

depend:

clean:
	$(RM) $(LOCAL_CLEAN_TARGET)

repository:			cimmofl_repository

cimmofl_repository:
	@$(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(NAMESPACE) namespace ...
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" "-n$(NAMESPACE)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@$(ECHO) +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(NAMESPACE) namespace ...
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" "-I$(CIM_SCHEMA_DIR)" "-n$(NAMESPACE)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof
	@$(ECHO) +++++ Loading $(subst .jar,,$(JAR1)) into $(NAMESPACE) namespace ...
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" -I. "-n$(NAMESPACE)" $(subst .jar,,$(JAR1)).mof
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" -I. "-n$(INTEROPNS)" $(subst .jar,,$(JAR1))R.mof
	@$(ECHO) +++++ Loading $(subst .jar,,$(JAR2)) into $(NAMESPACE) namespace ...
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" -I. "-n$(NAMESPACE)" $(subst .jar,,$(JAR2)).mof
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" -I. "-n$(INTEROPNS)" $(subst .jar,,$(JAR2))R.mof
	@$(ECHO) +++++ Loading $(subst .jar,,$(JAR3)) into $(NAMESPACE) namespace ...
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" -I. "-n$(NAMESPACE)" $(subst .jar,,$(JAR3)).mof
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" -I. "-n$(INTEROPNS)" $(subst .jar,,$(JAR3))R.mof
	@$(ECHO) +++++ Loading $(subst .jar,,$(JAR4)) into $(NAMESPACE) namespace ...
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" -I. "-n$(NAMESPACE)" $(subst .jar,,$(JAR4)).mof
	@cimmofl "-R$(REPOSITORY_DIR)" "-N$(REPOSITORY_NAME)" "-M$(REPOSITORY_MODE)" -I. "-n$(INTEROPNS)" $(subst .jar,,$(JAR4))R.mof

cimmof_repository:
	@$(ECHO) +++++ Loading CIM_Core$(CIM_SCHEMA_VER) into $(NAMESPACE) namespace ...
	@cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(NAMESPACE)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Core$(CIM_SCHEMA_VER).mof
	@$(ECHO) +++++ Loading CIM_Event$(CIM_SCHEMA_VER) into $(NAMESPACE) namespace ...
	@cimmof "-I$(CIM_SCHEMA_DIR)" "-n$(NAMESPACE)" $(ALLOW_EXPERIMENTAL) $(CIM_SCHEMA_DIR)/CIM_Event$(CIM_SCHEMA_VER).mof
	@$(ECHO) +++++ Loading $(subst .jar,,$(JAR1)) into $(NAMESPACE) namespace ...
	@cimmof -I. "-n$(NAMESPACE)" $(subst .jar,,$(JAR1)).mof
	@cimmof -I. "-n$(INTEROPNS)" $(subst .jar,,$(JAR1))R.mof
	@$(ECHO) +++++ Loading $(subst .jar,,$(JAR2)) into $(NAMESPACE) namespace ...
	@cimmof -I. "-n$(NAMESPACE)" $(subst .jar,,$(JAR2)).mof
	@cimmof -I. "-n$(INTEROPNS)" $(subst .jar,,$(JAR2))R.mof
	@$(ECHO) +++++ Loading $(subst .jar,,$(JAR3)) into $(NAMESPACE) namespace ...
	@cimmof -I. "-n$(NAMESPACE)" $(subst .jar,,$(JAR3)).mof
	@cimmof -I. "-n$(INTEROPNS)" $(subst .jar,,$(JAR3))R.mof
	@$(ECHO) +++++ Loading $(subst .jar,,$(JAR4)) into $(NAMESPACE) namespace ...
	@cimmof -I. "-n$(NAMESPACE)" $(subst .jar,,$(JAR4)).mof
	@cimmof -I. "-n$(INTEROPNS)" $(subst .jar,,$(JAR4))R.mof

tests:

poststarttests:			$(addprefix $(PEGASUS_HOME)/lib/,$(JARS))
	CLI im -n $(NAMESPACE) 'JMPIExpInstance_TestPropertyTypes' 'resetProvider'
	CLI gi --sum -count 1 -n $(NAMESPACE) 'JMPIExpInstance_TestPropertyTypes.CreationClassName="JMPIExpInstance_TestPropertyTypes",InstanceId=1'
	CLI gi --sum -count 1 -n $(NAMESPACE) 'JMPIExpInstance_TestPropertyTypes.CreationClassName="JMPIExpInstance_TestPropertyTypes",InstanceId=2'
	CLI ni --sum -count 2 -n $(NAMESPACE) 'JMPIExpInstance_TestPropertyTypes'
	CLI ei --sum -count 2 -n $(NAMESPACE) 'JMPIExpInstance_TestPropertyTypes'
	CLI im -n $(NAMESPACE) 'JMPIExpInstance_TestPropertyTypes' 'enableModifications' 'fState=true'

	CLI im -n $(NAMESPACE) 'JMPIInstance_TestPropertyTypes' 'resetProvider'
	CLI gi --sum -count 1 -n $(NAMESPACE) 'JMPIInstance_TestPropertyTypes.CreationClassName="JMPIInstance_TestPropertyTypes",InstanceId=1'
	CLI gi --sum -count 1 -n $(NAMESPACE) 'JMPIInstance_TestPropertyTypes.CreationClassName="JMPIInstance_TestPropertyTypes",InstanceId=2'
	CLI ni --sum -count 2 -n $(NAMESPACE) 'JMPIInstance_TestPropertyTypes'
	CLI ei --sum -count 2 -n $(NAMESPACE) 'JMPIInstance_TestPropertyTypes'
	CLI im -n $(NAMESPACE) 'JMPIInstance_TestPropertyTypes' 'enableModifications' 'fState=true'

	CLI im -n $(NAMESPACE) 'JMPICIMInstance_TestPropertyTypes' 'resetProvider'
	CLI gi --sum -count 1 -n $(NAMESPACE) 'JMPICIMInstance_TestPropertyTypes.CreationClassName="JMPICIMInstance_TestPropertyTypes",InstanceId=1'
	CLI gi --sum -count 1 -n $(NAMESPACE) 'JMPICIMInstance_TestPropertyTypes.CreationClassName="JMPICIMInstance_TestPropertyTypes",InstanceId=2'
	CLI ni --sum -count 2 -n $(NAMESPACE) 'JMPICIMInstance_TestPropertyTypes'
	CLI ei --sum -count 2 -n $(NAMESPACE) 'JMPICIMInstance_TestPropertyTypes'
	CLI im -n $(NAMESPACE) 'JMPICIMInstance_TestPropertyTypes' 'enableModifications' 'fState=true'

	CLI im -n $(NAMESPACE) 'JMPICIMInstance2_TestPropertyTypes' 'resetProvider'
	CLI gi --sum -count 1 -n $(NAMESPACE) 'JMPICIMInstance2_TestPropertyTypes.CreationClassName="JMPICIMInstance2_TestPropertyTypes",InstanceId=1'
	CLI gi --sum -count 1 -n $(NAMESPACE) 'JMPICIMInstance2_TestPropertyTypes.CreationClassName="JMPICIMInstance2_TestPropertyTypes",InstanceId=2'
	CLI ni --sum -count 2 -n $(NAMESPACE) 'JMPICIMInstance2_TestPropertyTypes'
	CLI ei --sum -count 2 -n $(NAMESPACE) 'JMPICIMInstance2_TestPropertyTypes'
	CLI im -n $(NAMESPACE) 'JMPICIMInstance2_TestPropertyTypes' 'enableModifications' 'fState=true'

poststarttests2:
	CLI xq -n $(NAMESPACE) 'SELECT InstanceId FROM JMPIExpInstance_TestPropertyTypes WHERE InstanceId = 1' 'WQL'

$(PEGASUS_HOME)/lib/$(JAR1):	$(JAR1CLASSES) CommonInstanceProvider.class
	$(PEGASUS_JAVA_JAR) -cf $@ $(addprefix -C .. $(PACKAGEDIR),$^) $(addprefix -C .. $(PACKAGEDIR),$(EXTRAJAR1CLASSES))

$(PEGASUS_HOME)/lib/$(JAR2):	$(JAR2CLASSES) CommonInstanceProvider.class
	$(PEGASUS_JAVA_JAR) -cf $@ $(addprefix -C .. $(PACKAGEDIR),$^) $(addprefix -C .. $(PACKAGEDIR),$(EXTRAJAR2CLASSES))

$(PEGASUS_HOME)/lib/$(JAR3):	$(JAR3CLASSES) CommonInstanceProvider.class
	$(PEGASUS_JAVA_JAR) -cf $@ $(addprefix -C .. $(PACKAGEDIR),$^) $(addprefix -C .. $(PACKAGEDIR),$(EXTRAJAR3CLASSES))

$(PEGASUS_HOME)/lib/$(JAR4):	$(JAR4CLASSES) CommonInstanceProvider.class
	$(PEGASUS_JAVA_JAR) -cf $@ $(addprefix -C .. $(PACKAGEDIR),$^) $(addprefix -C .. $(PACKAGEDIR),$(EXTRAJAR4CLASSES))

#include $(ROOT)/test/configend.mak
