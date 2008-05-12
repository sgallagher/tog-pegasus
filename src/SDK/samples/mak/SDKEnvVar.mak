# The variables defined in this file are intended for use only
# within the OpenPegasus development and test environment. Prior 
# to including this file in an SDK product release, these values should
# be modified to specify the desired platform or vendor directory
# locations. 

ifndef PEGASUS_BIN_DIR
    PEGASUS_BIN_DIR=$(PEGASUS_HOME)/bin
endif
ifndef PEGASUS_DEST_LIB_DIR
    PEGASUS_DEST_LIB_DIR=$(PEGASUS_HOME)/lib
endif
ifndef PEGASUS_PROVIDER_LIB_DIR
    PEGASUS_PROVIDER_LIB_DIR=$(PEGASUS_HOME)/lib
endif
ifndef PEGASUS_SAMPLES_DIR
    PEGASUS_SAMPLES_DIR=$(PEGASUS_ROOT)/src/SDK/samples
endif
ifndef PEGASUS_SAMPLE_OBJ_DIR
    PEGASUS_SAMPLE_OBJ_DIR=$(PEGASUS_HOME)/obj/sdk
endif
ifndef PEGASUS_SAMPLE_LIB_DIR
    PEGASUS_SAMPLE_LIB_DIR=$(PEGASUS_HOME)/lib/sdk
endif
ifndef PEGASUS_SAMPLE_BIN_DIR
    PEGASUS_SAMPLE_BIN_DIR=$(PEGASUS_HOME)/bin/sdk
endif
ifndef PEGASUS_INDICATION_CONSUMER_LOG_DIR
    PEGASUS_INDICATION_CONSUMER_LOG_DIR=$(PEGASUS_HOME)
endif
ifndef PEGASUS_INCLUDE_DIR
    PEGASUS_INCLUDE_DIR=$(PEGASUS_ROOT)/src
endif
ifndef PEGASUS_CIM_SCHEMA_DIR
    PEGASUS_CIM_SCHEMA_DIR=$(PEGASUS_ROOT)/Schemas/CIM217
    CIM_SCHEMA_VER=
endif
