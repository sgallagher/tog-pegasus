#include "Dir.h"

#if defined(PEGASUS_OS_TYPE_WINDOWS)
# include "DirWindows.cpp"
#elif defined(PEGASUS_OS_TYPE_UNIX)
# include "DirUnix.cpp"
#else
# error "Unsupported platform"
#endif
