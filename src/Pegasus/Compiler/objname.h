//BEGIN_LICENSE
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//END_LICENSE
//BEGIN_HISTORY
//
// Author: Bob Blair (bblair@bmc.com)
//
// $Log: objname.h,v $
// Revision 1.1  2001/02/16 23:59:09  bob
// Initial checkin
//
//
//
//END_HISTORY
//
// Header for a class to generate CIMValue objects from string values
//
//
//
// Make an object reference available in its parts.
//
// An object name has 3 parts:  namespaceType, namespaceHandle and
modelPath
// For example, in the object name
//       http://somehost:1234/a/b/c:myobj.first="top", last="cat"
// the parts are
//       namespace CIMType -- http:
//       namespaceHandle -- /somehost/a/b/c
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
// The modelPath has two components as well:  className and keyBindings.

// In the example,
//       className -- myobj
//       keyBindings -- first="top", last="cat"
//
// This class allows you to convert among various forms of the object
name:
//     -- the string (as above)
//     -- the decomposed pieces
//     -- an instance object
//     -- a reference object
//
// This class uses components from pegasus/Common

#include <Pegasus/Common/CIMReference.h>
#include <Pegasus/Common/Instance.h>

class PEGASUS_COMPILER_LINKAGE namespaceHandle {
 private:
  String _Stringrep;
  String _host;
  String _path;
  void namespaceHandleRepToComponets(const String &rep);
  const String &namespaceHandleComponentsToRep();
 public:
  namespaceHandle(const String &Stringrep);
  namespaceHandle(const String &host, const String &path);
  ~namespaceHandle();
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
  Array<KeyBinding> _keyBindings;
  modelPathRepToComponents(const String &rep);
  String &modelPathComponentsToRep();
 public:
  modelPath(const String &Stringrep);
  modelPath(const String &classname, const String &keyString);
  // modelPath(const String &classname, const Array<KeyBinding>&bindings);
  ~modelPath();
  const String &Stringrep() { return _Stringrep == "" ?
				modelPathComponentsToRep() :
    _Stringrep; }
  const String &className() { return _className; }
  const String &keyString() { return _keyString; }
  const Array<KeyBinding>& keyBindings() { return _keyBindings; }
};

class PEGASUS_COMPILER_LINKAGE objectName {
 private:
  String _Stringrep;
  String _namespaceType;
  namespaceHandle *_namespaceHandle;
  modelPath *_modelPath;
  CIMReference *_reference;
  Instance *_instance;
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
  objectName(const Instance &instance);
  ~objectName();
  objectName::set(const String &rep);
  objectName::set(const String &namespaceType, const String &namespaceHandle,
		  const String &modelPath);
  const String &Stringrep() { return _Stringrep; }
  const String &namespaceType() { return _namespaceType; }
  const String &namespaceHandle() const {
    return _namespaceHandle ? _namespaceHandle->Stringrep() ? ""; }
  const String &host() {
    return _namespaceHandle ? _namespaceHandle->host() : ""; }
  const String &path() {
    return _namespaceHandle ? _namespaceHandle->path() : ""; }
  const String &modelpath() {
    return _modelPath ? _modelPath->Stringrep() : ""; }
  const String &className() {
    return _modelPath ? _modelPath->className() : ""; }
  const String &keyString() {
    return _modelPath ? _modelPath->keyString() : ""; }
  const Array<keyBinding> &keyBindings() {
    return _modelPath ? _modelPath->keyBindings() :
Array<keyBindings>(); }
  const Instance *instance() { return _instance; }
};

