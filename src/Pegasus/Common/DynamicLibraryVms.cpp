//%2005////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Sean Keenan, Hewlett-Packard Company <sean.keenan@hp.com>
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Config/ConfigManager.h>
#include "DynamicLibrary.h"
#include "HashTable.h"

#include <chfdef.h>
#include <dlfcn.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdio>
#include <time.h>
#include <lib$routines.h>
#include <sys/time.h>
#include <netdb.h>
#include <prvdef.h>
#include <descrip.h>
#include <stsdef.h>
#include <libdef.h>
#include <ssdef.h>
#include <starlet.h>

#include <Pegasus/Config/ConfigManager.h>

PEGASUS_NAMESPACE_BEGIN

Boolean DynamicLibrary::load(void)
{
    // ensure the module is not already loaded
    PEGASUS_ASSERT(isLoaded() == false);

    CString cstr = _fileName.getCString();

    _handle = dlopen(cstr, RTLD_NOW);
    return(isLoaded());
}

Boolean DynamicLibrary::unload(void)
{
    // ensure the module is loaded
    PEGASUS_ASSERT(isLoaded() == true);

    dlclose(_handle);
    _handle = 0;

    return(isLoaded());
}

Boolean DynamicLibrary::isLoaded(void) const
{
    return(_handle != 0);
}

DynamicLibrary::LIBRARY_SYMBOL DynamicLibrary::getSymbol(const String & symbolName)
{

  char* Errorout;
  void* Dsh;

  if(isLoaded())
  {
    CString cstr = symbolName.getCString();

    if ((Dsh = dlsym(_handle, cstr)) == 0)
    {
      Errorout = dlerror();
    }
    return (LIBRARY_SYMBOL)Dsh;
  }
  return (0);
}

PEGASUS_NAMESPACE_END
