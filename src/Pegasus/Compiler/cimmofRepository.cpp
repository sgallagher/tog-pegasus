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
// $Log: cimmofRepository.cpp,v $
// Revision 1.2  2001/02/18 19:02:17  mike
// Fixed CIM debacle
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
// CIMRepository implementation for use in the cimmof compiler.
// Basically, it adds compiler-level methods and provides error handling
//

#include "cimmofRepository.h"

cimmofRepository::cimmofRepository(const String &path) :
	CIMRepository(path)
{;}

cimmofRepository::~cimmofRepository() {;}

int 
cimmofRepository::addClass(CIMClass *classdecl)
{
  const String &Sns = (cimmofParser::Instance())->getNamespacePath();
  CIMRepository::createClass( Sns,  *classdecl);
  // FIXME:  catch errors
  return 0;
}


int 
cimmofRepository::addInstance(CIMInstance *instance)
{ 
  const String &Sns = (cimmofParser::Instance())->getNamespacePath();
  CIMRepository::createInstance(Sns, *instance);
  // FIXME:  catch errors
  return 0;
}

int 
cimmofRepository::addQualifier(CIMQualifierDecl *qualifier)
{ 
  const String &Sns = cimmofParser::Instance()->getNamespacePath();
  CIMRepository::setQualifier(Sns, *qualifier);
  // FIXME:  catch errors
  return 0; 
}

CIMQualifierDecl
cimmofRepository::getQualifierDecl(const String &name)
{
  const String &Sns = cimmofParser::Instance()->getNamespacePath();
  return CIMRepository::getQualifier(Sns, name);
}


