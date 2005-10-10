#ifndef _Pegasus_Common_Atomic_h
#define _Pegasus_Common_Atomic_h

#include <Pegasus/Common/Config.h>
#include <new>

#if defined(PEGASUS_PLATFORM_LINUX_IX86_GNU)
# include "Atomic_LINUX_IX86_GNU.h"
#elif defined(PEGASUS_PLATFORM_LINUX_PPC_GNU)
# include "Atomic_LINUX_PPC_GNU.h"
#elif defined(PEGASUS_PLATFORM_WIN32_IX86_MSVC)
# include "Atomic_WIN32_IX86_MSVC.h"
#else
# include "Atomic_Generic.h"
#endif

#endif /* _Pegasus_Common_Atomic_h */
