#include "System.h"

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "SystemWindows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "SystemUnix.cpp"
#else
# error "Unsupported platform"
#endif
