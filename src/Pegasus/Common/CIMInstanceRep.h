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
// Author:
//
// $Log: CIMInstanceRep.h,v $
// Revision 1.6  2001/04/08 01:13:21  mike
// Changed "ConstCIM" to "CIMConst"
//
// Revision 1.4  2001/02/20 07:25:57  mike
// Added basic create-instance in repository and in client.
//
// Revision 1.3  2001/02/20 05:16:57  mike
// Implemented CIMInstance::getInstanceName()
//
// Revision 1.2  2001/02/19 01:47:16  mike
// Renamed names of the form CIMConst to CIMConst.
//
// Revision 1.1  2001/02/18 18:39:06  mike
// new
//
// Revision 1.2  2001/02/18 03:56:01  mike
// Changed more class names (e.g., ConstClassDecl -> CIMConstClass)
//
// Revision 1.1  2001/02/16 02:06:06  mike
// Renamed many classes and headers.
//
// Revision 1.1.1.1  2001/01/14 19:52:40  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef Pegasus_InstanceDeclRep_h
#define Pegasus_InstanceDeclRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierList.h>
#include <Pegasus/Common/CIMProperty.h>

PEGASUS_NAMESPACE_BEGIN

class DeclContext;
class CIMInstance;
class CIMConstInstance;
class CIMClass;
class CIMConstClass;

class PEGASUS_COMMON_LINKAGE CIMInstanceRep : public Sharable
{
public:
    
    CIMInstanceRep(const String& className);

    ~CIMInstanceRep();

    const String& getClassName() const { return _className; }

    void addQualifier(const CIMQualifier& qualifier)
    {
	_qualifiers.add(qualifier);
    }

    Uint32 findQualifier(const String& name) const
    {
	return _qualifiers.find(name);
    }

    CIMQualifier getQualifier(Uint32 pos)
    {
	return _qualifiers.getQualifier(pos);
    }

    CIMConstQualifier getQualifier(Uint32 pos) const
    {
	return _qualifiers.getQualifier(pos);
    }

    Uint32 getQualifierCount() const
    {
	return _qualifiers.getCount();
    }

    void addProperty(const CIMProperty& x);

    Uint32 findProperty(const String& name);

    Uint32 findProperty(const String& name) const
    {
	return ((CIMInstanceRep*)this)->findProperty(name);
    }

    CIMProperty getProperty(Uint32 pos);

    CIMConstProperty getProperty(Uint32 pos) const
    {
	return ((CIMInstanceRep*)this)->getProperty(pos);
    }

    Uint32 getPropertyCount() const;

    void resolve(DeclContext* declContext, const String& nameSpace);

    void toXml(Array<Sint8>& out) const;

    void print(std::ostream &o=std::cout) const;

    Boolean identical(const CIMInstanceRep* x) const;

    CIMInstanceRep* clone() const
    {
	return new CIMInstanceRep(*this);
    }

    String getInstanceName(const CIMConstClass& cimClass) const;

private:

    CIMInstanceRep();

    CIMInstanceRep(const CIMInstanceRep& x);

    CIMInstanceRep& operator=(const CIMInstanceRep& x);

    String _className;
    CIMQualifierList _qualifiers;
    Array<CIMProperty> _properties;
    Boolean _resolved;

    friend class CIMInstance;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_InstanceDeclRep_h */
