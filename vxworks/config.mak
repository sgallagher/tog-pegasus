export PEGASUS_PLATFORM=VXWORKS_PENTIUM_GNU
export PEGASUS_ROOT=$(HOME)/pegasus
export PEGASUS_HOME=$(HOME)/pegasus/$(PEGASUS_PLATFORM)
export PEGASUS_DISABLE_PROV_USERCTXT=1
export PEGASUS_DEFAULT_ENABLE_OOP=false
export CC=ccache ccpentium
export CXX=ccache c++pentium
