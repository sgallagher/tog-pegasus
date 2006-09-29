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
	$(PEGASUS_JAVA_COMPILER) -classpath $(PEGASUS_HOME)/lib/JMPIImpl.jar $*.java

NAMESPACE          = root/SampleProvider
PACKAGEDIR         = Associations/
JAR1               = JMPIExpAssociationProvider.jar
JAR1CLASSES        = \
	JMPIExpAssociationProvider.class
EXTRAJAR1CLASSES   =
JAR2               = JMPIAssociationProvider.jar
JAR2CLASSES        = \
	JMPIAssociationProvider.class
EXTRAJAR2CLASSES   =
JARS               = $(JAR1) $(JAR2)

LOCAL_CLEAN_TARGET = \
	$(JAR1CLASSES) $(EXTRAJAR1CLASSES) $(addprefix $(PEGASUS_HOME)/lib/,$(JAR1)) \
	$(JAR2CLASSES) $(EXTRAJAR2CLASSES) $(addprefix $(PEGASUS_HOME)/lib/,$(JAR2))

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

tests:

poststarttests:			$(addprefix $(PEGASUS_HOME)/lib/,$(JARS))
	@CLI a  --sum -count 1 -n $(NAMESPACE) JMPIExpAssociation_Teacher
	@CLI an --sum -count 1 -n $(NAMESPACE) JMPIExpAssociation_Teacher
	@CLI r  --sum -count 2 -n $(NAMESPACE) JMPIExpAssociation_Teacher
	@CLI rn --sum -count 2 -n $(NAMESPACE) JMPIExpAssociation_Teacher
	@CLI a  --sum -count 3 -n $(NAMESPACE) -r Teaches 'JMPIExpAssociation_Teacher.Name="Teacher1",Identifier=1'
	@CLI an --sum -count 3 -n $(NAMESPACE) -r Teaches 'JMPIExpAssociation_Teacher.Name="Teacher1",Identifier=1'
	@CLI r  --sum -count 1 -n $(NAMESPACE) -r Teaches JMPIExpAssociation_Teacher
	@CLI rn --sum -count 3 -n $(NAMESPACE) -r Teaches 'JMPIExpAssociation_Teacher.Name="Teacher1",Identifier=1'

	@CLI a  --sum -count 1 -n $(NAMESPACE) JMPIExpAssociation_Student
	@CLI an --sum -count 1 -n $(NAMESPACE) JMPIExpAssociation_Student
	@CLI r  --sum -count 2 -n $(NAMESPACE) JMPIExpAssociation_Student
	@CLI rn --sum -count 2 -n $(NAMESPACE) JMPIExpAssociation_Student
	@CLI a  --sum -count 3 -n $(NAMESPACE) -r TaughtBy 'JMPIExpAssociation_Student.Name="Student1",Identifier=1'
	@CLI an --sum -count 3 -n $(NAMESPACE) -r TaughtBy 'JMPIExpAssociation_Student.Name="Student1",Identifier=1'
	@CLI r  --sum -count 1 -n $(NAMESPACE) -r TaughtBy JMPIExpAssociation_Student
	@CLI rn --sum -count 3 -n $(NAMESPACE) -r TaughtBy 'JMPIExpAssociation_Student.Name="Student1",Identifier=1'

	@CLI a  --sum -count 1 -n $(NAMESPACE) JMPIAssociation_Teacher
	@CLI an --sum -count 1 -n $(NAMESPACE) JMPIAssociation_Teacher
	@CLI r  --sum -count 2 -n $(NAMESPACE) JMPIAssociation_Teacher
	@CLI rn --sum -count 2 -n $(NAMESPACE) JMPIAssociation_Teacher
	@CLI a  --sum -count 3 -n $(NAMESPACE) -r Teaches 'JMPIAssociation_Teacher.Name="Teacher1",Identifier=1'
	@CLI an --sum -count 3 -n $(NAMESPACE) -r Teaches 'JMPIAssociation_Teacher.Name="Teacher1",Identifier=1'
	@CLI r  --sum -count 1 -n $(NAMESPACE) -r Teaches JMPIAssociation_Teacher
	@CLI rn --sum -count 3 -n $(NAMESPACE) -r Teaches 'JMPIAssociation_Teacher.Name="Teacher1",Identifier=1'

	@CLI a  --sum -count 1 -n $(NAMESPACE) JMPIAssociation_Student
	@CLI an --sum -count 1 -n $(NAMESPACE) JMPIAssociation_Student
	@CLI r  --sum -count 2 -n $(NAMESPACE) JMPIAssociation_Student
	@CLI rn --sum -count 2 -n $(NAMESPACE) JMPIAssociation_Student
	@CLI a  --sum -count 3 -n $(NAMESPACE) -r TaughtBy 'JMPIAssociation_Student.Name="Student1",Identifier=1'
	@CLI an --sum -count 3 -n $(NAMESPACE) -r TaughtBy 'JMPIAssociation_Student.Name="Student1",Identifier=1'
	@CLI r  --sum -count 1 -n $(NAMESPACE) -r TaughtBy JMPIAssociation_Student
	@CLI rn --sum -count 3 -n $(NAMESPACE) -r TaughtBy 'JMPIAssociation_Student.Name="Student1",Identifier=1'

$(PEGASUS_HOME)/lib/$(JAR1):	$(JAR1CLASSES)
	$(PEGASUS_JAVA_JAR) -cf $@ $(addprefix -C .. $(PACKAGEDIR),$^) $(addprefix -C .. $(PACKAGEDIR),$(EXTRAJAR1CLASSES))

$(PEGASUS_HOME)/lib/$(JAR2):	$(JAR2CLASSES)
	$(PEGASUS_JAVA_JAR) -cf $@ $(addprefix -C .. $(PACKAGEDIR),$^) $(addprefix -C .. $(PACKAGEDIR),$(EXTRAJAR2CLASSES))

#include $(ROOT)/test/configend.mak
