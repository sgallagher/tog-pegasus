#ifndef Pegasus_ConfigPlatform_h
#define Pegasus_ConfigPlatform_h

#include <Pegasus/Common/ConfigWindows.h>

#define PEGASUS_OS_TYPE_WINDOWS

#define PEGASUS_OS_WIN32

#define PEGASUS_ARCHITECTURE_IX86

#define PEGASUS_COMPILER_MSVC

// Suppress this warning: "identifier was truncated to '255' characters in the 
// debug information"

#pragma warning( disable : 4786 )

#endif /* Pegasus_ConfigPlatform_h */
