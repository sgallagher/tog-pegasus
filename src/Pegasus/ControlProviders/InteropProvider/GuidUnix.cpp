//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Tony Fiorentino (fiorentino_tony@emc.com)
//
PEGASUS_USING_STD;

PEGASUS_NAMESPACE_BEGIN

String Guid::getGuid(const String &prefix)
{
  Uint32 seconds(0), milliSeconds(0);
  System::getCurrentTime(seconds, milliSeconds);
  CIMValue secondsValue(seconds);
  CIMValue milliSecondsValue(milliSeconds);
  String ipAddress("127.0.0.1");
  String hostName(System::getHostName());

  struct hostent * phostent;
  struct in_addr   inaddr;
  
  if ((phostent = ::gethostbyname((const char *)hostName.getCString())) != NULL)
    {
      ::memcpy( &inaddr, phostent->h_addr,4);
      ipAddress = ::inet_ntoa( inaddr );
    }

  // change the dots to dashes
  for (Uint32 i=0; i<ipAddress.size(); i++)
    {
      if (ipAddress[i] == Char16('.'))
        ipAddress[i] = Char16('-');
    }

  return (secondsValue.toString() + milliSecondsValue.toString() + "-" + ipAddress);
}

PEGASUS_NAMESPACE_END
// END_OF_FILE
