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
// $Log: CIMClassRep.h,v $
// Revision 1.4  2001/03/04 21:57:34  bob
// Changed print methods to take a stream instead of hardcoded cout
//
// Revision 1.3  2001/02/20 05:16:57  mike
// Implemented CIMInstance::getInstanceName()
//
// Revision 1.2  2001/02/19 01:47:16  mike
// Renamed names of the form CIMConst to ConstCIM.
//
// Revision 1.1  2001/02/18 18:39:05  mike
// new
//
// Revision 1.2  2001/02/18 03:56:00  mike
// Changed more class names (e.g., ConstClassDecl -> ConstCIMClass)
//
// Revision 1.1  2001/02/16 02:06:06  mike
// Renamed many classes and headers.
//
// Revision 1.2  2001/01/15 04:31:43  mike
// worked on resolve scheme
//
// Revision 1.1.1.1  2001/01/14 19:50:39  mike
// Pegasus import
//
//
//END_HISTORY

#ifndef Pegasus_ClassDeclRep_h
#define Pegasus_ClassDeclRep_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Exception.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/CIMQualifier.h>
#include <Pegasus/Common/CIMQualifierList.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMMethod.h>

PEGASUS_NAMESPACE_BEGIN

class DeclContext;
class CIMClass;
class ConstCIMClass;
class CIMInstanceRep;

class PEGASUS_COMMON_LINKAGE CIMClassRep : public Sharable
{
public:
    
    CIMClassRep(
	const String& className, 
	const String& superClassName);

    ~CIMClassRep();

    Boolean isAssociation() const;

    Boolean isAbstract() const;

    const String& getClassName() const { return _className; }

    const String& getSuperClassName() const { return _superClassName; }

    void setSuperClassName(const String& superClassName);

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

    void removeProperty(Uint32 pos);

    Uint32 findProperty(const String& name);

    Uint32 findProperty(const String& name) const
    {
	return ((CIMClassRep*)this)->findProperty(name);
    }

    CIMProperty getProperty(Uint32 pos);

    ConstCIMProperty getProperty(Uint32 pos) const
    {
	return ((CIMClassRep*)this)->getProperty(pos);
    }

    Uint32 getPropertyCount() const;

    void addMethod(const CIMMethod& x);

    Uint32 findMethod(const String& name);

    Uint32 findMethod(const String& name) const
    {
	return ((CIMClassRep*)this)->findMethod(name);
    }

    CIMMethod getMethod(Uint32 pos);

    CIMConstMethod getMethod(Uint32 pos) const
    {
	return ((CIMClassRep*)this)->getMethod(pos);
    }

    Uint32 getMethodCount() const;

    void resolve(
	DeclContext* context,
	const String& nameSpace);

    void toXml(Array<Sint8>& out) const;

    void print(std::ostream &o=std::cout) const;

    Boolean identical(const CIMClassRep* x) const;

    CIMClassRep* clone() const
    {
	return new CIMClassRep(*this);
    }

    /** Return the names of all properties which bear a true key qualifier. 
	Sort the keys in ascending order.
    */
    void getKeyNames(Array<String>& keyNames) const;

private:

    CIMClassRep();

    CIMClassRep(const CIMClassRep& x);

    CIMClassRep& operator=(const CIMClassRep& x);

    String _className;
    String _superClassName;
    CIMQualifierList _qualifiers;
    Array<CIMProperty> _properties;
    Array<CIMMethod> _methods;
    Boolean _resolved;

    friend class CIMClass;
    friend class CIMInstanceRep;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ClassDeclRep_h */
