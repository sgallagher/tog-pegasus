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
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY
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
// $Log: objname.cpp,v $
// Revision 1.3  2001/04/24 00:00:15  mike
// Ported compiler to use String and Array (rather than STL equivalents)
//
// Revision 1.2  2001/03/04 22:18:00  bob
// Cleanup, support for reference, message moving, start of instance support
//
// Revision 1.1  2001/02/16 23:59:09  bob
// Initial checkin
//
//
//END_HISTORY
//
// implementation of valueFactory 
//
//
// implementation of objectName and its related classes
//

#include "objname.h"
#include <iostream> // for debug

using namespace std;
PEGASUS_NAMESPACE_BEGIN

#define WHITESPACE(x) (x==' ' || x=='\t' || x=='\n' || x=='\r')
#define DIGIT(x) (x=='0' || x=='1' || x=='2' || x=='3' || x=='4' || x=='5' || x=='6' || x=='7' || x=='8' || x=='9')

// The String representation of the namespaceHandle begins
// with 0 or more slashes, followed by a host designation,
// followed by a path.  The path begins with a slash and
// terminates with the end of the String

namespaceHandle::namespaceHandle(const String &rep) {
  namespaceHandleRepToComponents(rep);
}

namespaceHandle::namespaceHandle(const String &host, const String &path) {
  String str = "/" + host + "/" + path;
  namespaceHandleRepToComponents(str);
}

const String &
namespaceHandle::namespaceHandleComponentsToRep() {
  _Stringrep = _host;
  if (!String::equal(_host, String::EMPTY))
    _Stringrep += ":"; 
  _Stringrep +=  _path;
  return _Stringrep;
}

void
namespaceHandle::namespaceHandleRepToComponents(const String &rep) {
  _host.clear();
  _path.clear();
  Uint32 n = rep.getLength();
  typedef enum _states {BEGIN, INHOST, INPATH, DONE} states;
  states state = BEGIN;
  bool hashost = false;
  for (Uint32 i = 0; i < n; i++)
    if (rep[i] == ':')
	hashost = true;
  for (Uint32 i = 0; i < n; i++) {
    Char16 c = rep[i];
    switch (state) {
    case BEGIN:
      if (!WHITESPACE(c)) {
	if (hashost) {
	  _host.append(c);
	  state = INHOST;
	} else {
	  _path.append(c);
	  state = INPATH;
	}
      }
      break;
    case INHOST:
      if (c != ':') {
	_host.append(c);
      } else {
	state = INPATH;
      }
      break;
    case INPATH:
      if (c != ':') {
	_path.append(c);
      } else {
	state = DONE;
      }
      break;
    default:
      break;
    } // end switch
  } // end for
}

modelPath::modelPath(const String &rep) {
  modelPathRepToComponents(rep);
}

modelPath::modelPath(const String &rep, const String &keyString) {
  String str = rep + "." + keyString;
  modelPathRepToComponents(str);
}

modelPath::modelPath(const String &rep, const KeyBindingArray &kba) {
  _className = rep;
  _KeyBindings = kba;
}

modelPath::~modelPath() {
}

KeyBinding::CIMType
modelPath::KeyBindingTypeOf(const String &s)
{
  Char16 c = s[0];
  if (DIGIT(c)) return KeyBinding::NUMERIC;
  if (c == '\"') return KeyBinding::STRING;
  return KeyBinding::BOOLEAN;
}

 
const String &
modelPath::modelPathComponentsToRep() {
  String stringrep = _className;
  Uint32 numkeys = _KeyBindings.getSize();
  if (numkeys) {
    stringrep += ".";
  }
  stringrep += KeyBindingsToKeyString();
  _Stringrep = stringrep;
  return _Stringrep;
}

void
modelPath::modelPathRepToComponents(const String &rep) {
  KeyBinding kb;
  String keyname;
  KeyBinding::CIMType kbtype = KeyBinding::STRING;
  String keyvalue;
  Uint32 n = rep.getLength();
  enum _states{BEGIN, INCLASS, KEYBEGIN, INKEYNAME, KEYVALBEGIN,
		    INSTRINGKEYVAL, INNUMERICKEYVAL,
		    INBOOLEANKEYVAL, ENDINGKEYVAL};
  _states state = BEGIN;
  for (Uint32 i = 0; i < n; i++) {
    Char16 c = rep[i];
    switch(state) {
    case BEGIN:
    if (c != ':' && !WHITESPACE(c)) {
	_className.append(c);
	state = INCLASS;
      }
      break;
    case INCLASS:
    if (c != '.') {
	_className.append(c);
      } else {
	state = KEYBEGIN;
      }
      break;
    case KEYBEGIN:
    if (!WHITESPACE(c)) {
	keyname.clear();
	keyname.append(c);
	state = INKEYNAME;
      }
      break;
    case INKEYNAME:
      if (c == '=') {
        _keyString += keyname;
	state = KEYVALBEGIN;
      } else {
	if (!WHITESPACE(c)) {
	  keyname.append(c);
	}
      }
      break;
    case KEYVALBEGIN:
      if (!WHITESPACE(c)) {
	keyvalue.clear();
	if (c == '"') {
	  kbtype = KeyBinding::STRING;
	  state = INSTRINGKEYVAL;
	} else {
	  if (DIGIT(c)) {
	    kbtype = KeyBinding::NUMERIC;
	    keyvalue.append(c);
	    state = INNUMERICKEYVAL;
	  } else {
	    kbtype = KeyBinding::BOOLEAN;
	    keyvalue.append(c);
	    state = INBOOLEANKEYVAL;
	  }
	}
      }
      break;
    case INSTRINGKEYVAL: {
      if (c != '"') { // What about escaped quotes in the value?
	keyvalue.append(c);
      } else {
	state = ENDINGKEYVAL;
      }
      break;
    case INNUMERICKEYVAL:
    case INBOOLEANKEYVAL:
      if (WHITESPACE(c) || c == ',') {
	  _keyString += keyvalue;
	  kb.setName(keyname);
	  kb.setValue(keyvalue);
	  kb.setType(kbtype);
	  _KeyBindings.append(kb);
	  state = KEYBEGIN;
      } else {
	keyvalue.append(c);
      }
      break;
    case ENDINGKEYVAL: // (applies only to String value)
      if (c == ',') {
	_keyString += keyvalue;
	kb.setName(keyname);
	kb.setValue(keyvalue);
	kb.setType(kbtype);
	_KeyBindings.append(kb);
	state = KEYBEGIN;
      } else {
	// it's really a syntactical error, but we'll let it go
      }
    }
    } // end switch
  } // end for length of input
  if (state == ENDINGKEYVAL || state == INSTRINGKEYVAL 
    || state == INNUMERICKEYVAL || state == INBOOLEANKEYVAL) {
    _keyString += keyvalue;
    kb.setName(keyname);
    kb.setValue(keyvalue);
    kb.setType(kbtype);
    _KeyBindings.append(kb);
  }
}

const String &
modelPath::KeyBindingsToKeyString()
{
  String stringrep;
  Uint32 numkeys = _KeyBindings.getSize();
  for (Uint32 i = 0; i < numkeys; i++) {
    const KeyBinding &kb = _KeyBindings[i];
    const String &keyname = kb.getName();
    KeyBinding::CIMType keytype = kb.getType();
    const String &keyvalue = kb.getValue();
    if (i)
      stringrep += ",";
    stringrep += keyname + "=" + (keytype == KeyBinding::STRING ? "\"" : "") +
				   keyvalue + 
				   (keytype == KeyBinding::STRING ? "\"" : "");
  }
  _keyString = stringrep;
  return _keyString;
}

// The object name has three parts:  A namespace type; a
// namespace handle, and a model path.  It's in a form
// something like this:
//     namespaceType://namespacePath:modelPath
// The namespacePath has two parts:
//     host/rest_of_path
// The modelPath has two parts:
//     className.key=val[, key=val ...
// I'm working on the assumption that the colon is they key:
// if there are 2 then the name divides
//    namespaceType:namespaceHandle:modelPath
// if there is only 1, then the division is
//    namespaceHandle:modelPath
//

objectName::objectName(const String &stringrep) :
  _namespaceHandle(0), _modelPath(0), _reference(0), _instance(0)
{
  set(stringrep);
}

objectName::objectName(const String &namespaceType,
		       const String &namespaceHandle,
		       const String &modelPath)
{
  set(namespaceType, namespaceHandle, modelPath);
}

objectName::~objectName() {
  if (_namespaceHandle) delete _namespaceHandle;
  if (_modelPath)       delete _modelPath;
  if (_reference)       delete _reference;
  if (_instance)        delete _instance;
}

void
objectName::set(const String &stringrep) {
  Uint32 n = stringrep.getLength();
  Char16 lastchar = '\0';
  Array<Uint32> colons;
  for (Uint32 i = 0; i < n; i++) {
    Char16 c = stringrep[i];
    if (c == ':' && lastchar != '\\') {
      colons.append(i);
    }
    lastchar = c;
  }
  int delimiter1 = 0;  // if non-zero, this separates namespacetype from rest
  int delimiter2 = 0; // if non-zero, separates host from path
  int delimiter3 = 0; // if non-zero, separates path from object name & key
  int handlestart = 0;  // where the namspaceHandle, if any, starts
  int numcolons = colons.getSize();
  if (numcolons == 3) {
    delimiter1 = colons[0];
    delimiter2 = colons[1];
    delimiter3 = colons[2];
  } else if (numcolons == 2) {
    delimiter2 = colons[0];
    delimiter3 = colons[1];
  } else if (numcolons == 1) {
    delimiter3 = colons[0];
  } else  { // no problemo.  It's just a model path
  }

  if (delimiter1) {  // all three components are present
    _namespaceType = stringrep.subString(0, delimiter1 + 1);
    handlestart = delimiter1 + 1;
  }
  if (delimiter3) {  // the last two components are present
    _namespaceHandle = new namespaceHandle(stringrep.subString(handlestart,
						    delimiter3 - handlestart));
  }
  _modelPath = new modelPath(stringrep.subString(delimiter3));
}

void
objectName::set(const String &namespaceType, const String &snamespaceHandle,
		const String &smodelPath)
{
  _namespaceType = namespaceType;
  _namespaceHandle = new namespaceHandle(snamespaceHandle);
  _modelPath = new modelPath(smodelPath);
}

objectName::objectName() : _namespaceHandle(0), _modelPath(0),
			   _reference(0), _instance(0)
{}

PEGASUS_NAMESPACE_END
