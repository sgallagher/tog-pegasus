//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Author: Susan Campbell, Hewlett-Packard Company (scampbell@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef _OSINFO_H
#define _OSINFO_H

#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/Common/CIMDateTime.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

class OSInfo
{
  public:
     OSInfo(void);
     ~OSInfo(void);

     void getOSInfo(const int argc, const char** argv);

     // utility methods for common functions
     void errorExit(const String &message);
     void gatherProperties(CIMInstance &inst, Boolean cimFormat);
     void displayProperties();

  private:
     String osCSName;       // the hostname
     String osName;         // the name of the operating system
     String osVersion;
     String osOtherInfo;    // for Linux, the distribution info
     String osBootUpTime;
     String osCapability;
     String osLicensedUsers;
     String osLocalDateTime;
     String osSystemUpTime;
     void _usage();
};

#endif
