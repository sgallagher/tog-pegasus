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
// $Log: objname.cpp,v $
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

#define WHITESPACE(x) (x==' ' || x=='\t' || x=='\n' || x=='\r')
#define DIGIT(x) (x=='0' || x=='1' || x=='2' || x=='3' || x=='4' || x=='5' || x=='6' || x=='7' || x=='8' || x=='9')

// The string representation of the namespaceHandle begins
// with 0 or more slashes, followed by a host designation,
// followed by a path.  The path begins with a slash and
// terminates with the end of the string

namespaceHandle::namespaceHandle(const String &rep) {
  namespaceHandleRepToComponents(rep);
}

namespaceHandle::namespaceHandle(const String &host, const String &path) {
  String str = "/" + host + "/" + path;
  namespaceHandleRepToComponents(str);
}

const String &
namespaceHandle::namespaceHandleComponentsToRep() {
  _Stringrep = _host + "/" + _path;
  return _Stringrep;
}

void
namespaceHandle::namespaceHandleRepToComponents(const String &rep) {
  _host.clear();
  _path.clear();
  Uint32 n = rep.getLength();
  typedef enum _states {BEGIN, INHOST, INPATH, DONE} states;
  states state = BEGIN;
  for (Uint32 i = 0; i < n; i++) {
    Char16 c = rep[i];
    switch (state) {
    case BEGIN:
      if (c != '/' && !WHITESPACE(c)) {
	_host.append(c);
	state = INHOST;
      }
      break;
    case INHOST:
      if (c != '/') {
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
    }
  }
}
    
modelPath::modelPath(const String &rep) {
  modelPathRepToComponents(rep);
}

modelPath::modelPath(const String &rep, const String &keyString) {
  String str = rep + "." + keyString;
  modelPathRepToComponents(str);
}

modelPath::~modelPath() {
  for (Uint32 i = _keyBindings.size(), i > 0, i--) {
    if (_keyBindings[i]) delete _keyBindings[i];
    _keyBindings[i] = 0;
  }
}

const String &
modelPath::modelPathComponensToString() {
  _Stringrep = _className + "." + _keyString;
  return _Stringrep;
}

void
modelPath::modelPathRepToComponents(rep) {
  KeyBinding kb;
  String keyname;
  KeyBinding::CIMType kbtype;
  String kbvalue;
  Uint32 n = rep.length();
  typedef _states{BEGIN, INCLASS, KEYBEGIN, INKEYNAME, KEYVALBEGIN,
		    INKEYVAL, INSTRINGKEYVAL, INNUMERICKEYVAL,
		    INBOOLEANKEYVAL, ENDINGKEYVAL} states;
  states state;
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
	keyname.append(c);
        _keyString += keyname;
	state = KEYVALUEBEGIN;
      } else {
	if (!WHITESPACE(c)) {
	  keyname.append(c);
	}
      }
      break;
    case KEYVALUEBEGIN:
      if (!WHITESPACE(c)) {
	keyvalue.clear();
	if (c == '"') {
	  kbtype = KeyBinding::CIMType::STRING;
	  state = INSTRINGKEYVAL;
	} else {
	  if (DIGIT(c)) {
	    kbtype = KeyBinding::CIMType::NUMERIC;
	    keyvalue.append(c);
	    state = INNUMERICKEYVAL;
	  } else {
	    kbtype = KeyBinding::CIMType::BOOLEAN;
	    keyvalue.append(c);
	    state = INBOOLEANKEYVAL;
	  }
	}
      }
      break;
      case INSTRINGKEYVAL {
	if (c != '"') { // What about escaped quotes in the value?
	  keyvalue.append(c);
	} else {
	  state = ENDINGKEYVAL;
	}
	break;
      case INNUMERICKEYVAL:
      case INBOOLEANKEYVAL:
      if (!WHITESPACE(c) && c != ',') {
	keyvalue.append(c);
	} else {
	  _keyString += keyvalue;
	  kb.setName(keyname);
	  kb.setValue(keyvalue);
	  kb.setType(kbtype);
	  _keybindings.append(kb);
	  state = KEYBEGIN;
	}
	break;
      case ENDINGKEYVAL: // (applies only to string value)
	if (c == ',') {
	  _keyString += keyvalue;
	  kb.setName(keyname);
	  kb.setValue(keyvalue);
	  kb.setType(kbtype);
	  _keyBindings.append(kb);
	  state = KEYBEGIN;
	} else {
	  // it's really a syntactical error, but we'll let it go
	}
      }
    } // end switch
  } // end for length of input
  // At this point, we should be either at the beginning (there
  // was no model path) or at the beginning of a key.  But
  // we'll probably just take what we have and leave.
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
		       cibst String &modelPath)
{
  set(namespaceType, namespaceHandle, modelPath);
}

objectName::set(const String &strinerep) {
  Uint32 n = stringrep.length();
  Char16 lastchar = '\0';
  Array<Uint32> colons;
  for (Uint32 i = 0; i < n; i++) {
    Char16 c = stringrep[i];
    if (c == ':' && lastchar != '\\') {
      colons.append(i);
    }
    lastchar = c;
  }
  int delimiter1 = 0;
  int delimiter2 = 0;
  int numcolons = colons.size();
  if (numcolons == 2) {
    delimiter1 = colons[0];
    delimiter2 = colons[1];
  } else if (numcolons == 1 {
    delimiter1 = 0;
    delimiter2 = colons[0];
  } else // very  bad karma: this doesn't look like an object name
  }
  if (delimiter1) {
    _namespaceType = stringrep.substr(0, delimiter1);
  }
  _namespaceHandle = new namespaceHandle(stringrep.substr(delimiter1,
							  delimiter2));
  _modelPath = new modelPath(stringrep.substr(delimiter2));
}

objectName::set(const String &namespaceType, const String &snamespaceHandle,
		const String &smodelPath) : _namespaceHandle(0),
  _modelPath(0), _reference(0), _instance(0) {
  _namespaceType = namespaceType;
  _namespaceHandle = new namespaceHandle(snamespaceHandle);
  _modelPath = new modelPath(smodelPath);
}

objectName::objectName() : _namespaceHandle(0), _modelPath(0), 
  _reference(0), _instance(0} 
{}
