//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM,
// Compaq Computer Corporation
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
//   FILE       SystemNsk.cpp
//
//   PURPOSE    System NSK dummy class to satisfy the PEGASUS Code.
//
//   REVISION HISTORY
//
//   Date      Release   Revisor           Revision
//   04NOV01   1         Rudy Schuet       Initial creation
//   16NOV01   1         Nag Boranna       Added get login name function
//
// ***************************************************************************
//                                                                             

#include <cextdecs.h(NODENUMBER_TO_NODENAME_)>

PEGASUS_NAMESPACE_BEGIN

void System::getCurrentTime(Uint32& seconds, Uint32& milliseconds) {}
String System::getCurrentASCIITime() { return String(); }
void System::sleep(Uint32 seconds) {}
Boolean System::exists(const char* path) { return false; }
Boolean System::canRead(const char* path) { return false; }
Boolean System::getCurrentDirectory(char* path, Uint32 size) { return false; }
Boolean System::isDirectory(const char* path) { return false; }
Boolean System::changeDirectory(const char* path) { return false; }
Boolean System::makeDirectory(const char* path) { return false; }
Boolean System::getFileSize(const char* path, Uint32& size) { return false; }
Boolean System::removeDirectory(const char* path) { return false; }
Boolean System::removeFile(const char* path) { return false; }
Boolean System::renameFile(const char* oldPath, const char* newPath) 
    { return false; }
DynamicLibraryHandle System::loadDynamicLibrary(const char* fileName) 
    { return DynamicLibraryHandle (); }
void System::unloadDynamicLibrary(DynamicLibraryHandle libraryHandle) {}
String System::dynamicLoadError() { return String(); }

DynamicSymbolHandle System::loadDynamicSymbol(
     DynamicLibraryHandle libraryHandle,
     const char* symbolName)
  { return DynamicSymbolHandle(); }

String System::getHostName() 
{
  char node[10];
  short nodelen, ret;
  ret = NODENUMBER_TO_NODENAME_(,node,8,&nodelen);
  if (!ret) {
    node[nodelen] = 0;
    return String (&node[1]); }   // skip back slash
  else return String();                           
} 

Uint32 System::lookupPort(
    const char * serviceName,
    Uint32 defaultPort)
{
    return defaultPort;
}

String System::getPassword(const char* prompt)
{
    //ATTN: Implement this method to get password from User with no echo
    //      This is used in cimuser CLI
    String password("dummy");

    return password;
}

String System::getCurrentLoginName() { return String(); }

String System::encryptPassword(const char* password, const char* salt)
{
    // ATTN: Implement this method to encrypt the password
    //       This is used in User Manager
    return (String("dummy"));
}

Boolean System::isSystemUser(char* userName)
{
    // ATTN: Implement this method to verify if user is vaild 
    //       on the local system
    //       This is used in User Manager
    return true;
}

Boolean System::isPrivilegedUser(const String userName)
{
    // ATTN: Implement this method to verify if user executing the current
    //       command is a priviliged user, when user name is not passed as
    //       as argument. If user name is passed the function checks
    //       whether the given user is a priviliged user.
    //       This is used in cimuser CLI and CIMOperationRequestAuthorizer
    return true;
}

String System::getPrivilegedUserName()
{
    // ATTN-NB-03-20000304: Implement code to get the privileged
    // user on the system.

    return (String(""));
}

Uint32 System::getPID()
{
    // ATTN: Implement this to get the process ID of the current process
    //       This is used by the Tracer
    return 0;
}
    
PEGASUS_NAMESPACE_END
