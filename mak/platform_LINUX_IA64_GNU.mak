# Platform Configuration for Linux on IA-64
# If you have generic Linux config options, please place them in config-linux.mak

include $(ROOT)/mak/config-linux.mak

ARCHITECTURE = IA64

DEFINES += -DPEGASUS_DISABLE_KILLING_HUNG_THREADS
DEFINES += -DPEGASUS_USE_SYSLOGS

##
## The following flag needs to be set to compile in code for
## not listing the trace configuration properties. This will cause
## the trace configuration properties, traceComponents and traceLevel
## not to be listed when using the cimconfig -l command.
##
DEFINES += -DPEGASUS_DONOT_LIST_TRACE_PROPERTIES
