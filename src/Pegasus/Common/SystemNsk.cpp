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

String System::getPassword(const char* prompt)
{
    //
    // This function needs to be implemented
    // Return a dummy string for now
    //
    String password("dummy");

    return password;
}

PEGASUS_NAMESPACE_END
