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
// Author: Bob Blair (bblair@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


//
// Header for a class to generate CIMValue objects from String values
//
//
//
// Make an object reference available in its parts.
//
// An object name has 3 parts:  namespaceType, namespaceHandle and modelPath
// For example, in the object name
//       http://somehost:1234/a/b/c:myobj.first="top", last="cat"
// the parts are
//       namespaceType -- http:
//       namespaceHandle -- /somehost:a/b/c
//       modelPath -- myobj.first="top", last="cat"
//
// The namespaceHandle has two components:  host and path.
// In the example above, this divides as
//       host -- somehost
//       path -- /a/b/c
//
// The host may have two components:  a host name and a port.
//       host -- hostname:1234
//
// The modelPath has two components as well:  className and CIMKeyBindings.

// In the example,
//       className -- myobj
//       CIMKeyBindings -- first="top", last="cat"
//
// This class allows you to convert among various forms of the object name:
//     -- the String (as above)
//     -- the decomposed pieces
//     -- an instance object
//     -- a reference object
//
// This class uses components from pegasus/Common

#ifndef _PEGASUS_COMPILER_OBJNAME_H_
#define _PEGASUS_COMPILER_OBJNAME_H_


#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Compiler/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

class PEGASUS_COMPILER_LINKAGE namespaceHandle {
 private:
  String _Stringrep;
  String _host;
  String _path;
  void namespaceHandleRepToComponents(const String &rep);
  const String &namespaceHandleComponentsToRep();
 public:
  namespaceHandle(const String &Stringrep);
  namespaceHandle(const String &host, const String &path);
  ~namespaceHandle() {;}
  const String &Stringrep() { return _Stringrep == "" ? 
				namespaceHandleComponentsToRep() : 
    _Stringrep; }
  const String &host() { return _host; }
  const String &path() { return _path; }
};

class PEGASUS_COMPILER_LINKAGE modelPath {
 private:
  String _Stringrep;
  String _className;
  String _keyString;
  Array<CIMKeyBinding> _KeyBindings;
  void modelPathRepToComponents(const String &rep);
  const String &modelPathComponentsToRep();
 public:
  modelPath(const String &Stringrep);
  modelPath(const String &classname, const String &keyString);
  modelPath(const String &classname, const Array<CIMKeyBinding>&bindings);
  ~modelPath();
  static CIMKeyBinding::Type KeyBindingTypeOf(const String &s);
  const String &Stringrep() { return modelPathComponentsToRep(); }
  const String &className() { return _className; }
  const String &keyString() { return KeyBindingsToKeyString(); }
  const String &KeyBindingsToKeyString();
  const Array<CIMKeyBinding>& KeyBindings() { return _KeyBindings; }
};

class PEGASUS_COMPILER_LINKAGE objectName {
 private:
  String _Stringrep;
  String _namespaceType;
  namespaceHandle *_namespaceHandle;
  modelPath *_modelPath;
  CIMObjectPath *_reference;
  CIMInstance *_instance;
  Array<CIMKeyBinding> _empty;

 public:
  objectName();
  objectName(const String &Stringrep);
  objectName(const String &namespaceType, const String &namespaceHandle,
      const String &modelPath);
  objectName(const String &namespaceType, const String &host,
      const String &path, const String &modelpath);
  objectName(const String &namespaceType, const String &host,
      const String &path, const String &classname,
      const String &keyString);
  objectName(const CIMInstance &instance);
  ~objectName();
  void set(const String &rep);
  void set(const String &namespaceType, 
		       const String &namespaceHandle,
		       const String &modelPath);
  const String &Stringrep() { return _Stringrep; }
  const String &namespaceType() { return _namespaceType; }
  const String &handle() const {
    return _namespaceHandle ? _namespaceHandle->Stringrep() : String::EMPTY; }
  const String &host() const {
    return _namespaceHandle ? _namespaceHandle->host() : String::EMPTY; }
  const String &path() {
    return _namespaceHandle ? _namespaceHandle->path() : String::EMPTY; }
  const String &modelpath() {
    return _modelPath ? _modelPath->Stringrep() : String::EMPTY; }
  const String &className() const {
    return _modelPath ? _modelPath->className() : String::EMPTY; }
  const String &keyString() {
    return _modelPath ? _modelPath->keyString() : String::EMPTY; }
  const Array<CIMKeyBinding> &KeyBindings() const {
    return _modelPath ? _modelPath->KeyBindings() : _empty; }
  const CIMInstance *instance() { return _instance; }
};

PEGASUS_NAMESPACE_END


#endif
