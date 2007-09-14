CONFIGURATION=VXWORKS_PENTIUM_GNU
#CONFIGURATION=LINUX_IX86_GNU

ifeq ($(CONFIGURATION),VXWORKS_PENTIUM_GNU)
    export PEGASUS_ROOT ::= $(shell dirname $(shell pwd))
    export PEGASUS_HOME=$(PEGASUS_ROOT)/$(PEGASUS_PLATFORM)
    export PEGASUS_PLATFORM=VXWORKS_PENTIUM_GNU
    export PEGASUS_DISABLE_PROV_USERCTXT=1
    export PEGASUS_DEFAULT_ENABLE_OOP=false
    export CC=ccache ccpentium
    export CXX=ccache c++pentium
endif


ifeq ($(CONFIGURATION),LINUX_IX86_GNU)
    export PEGASUS_ROOT ::= $(shell dirname $(shell pwd))
    export PEGASUS_HOME=$(PEGASUS_ROOT)/$(PEGASUS_PLATFORM)
    export PEGASUS_PLATFORM=LINUX_IX86_GNU
    export PEGASUS_DISABLE_PROV_USERCTXT=1
    export PEGASUS_DEFAULT_ENABLE_OOP=false
    export CC=ccache ccpentium
    export CXX=ccache c++pentium
endif
