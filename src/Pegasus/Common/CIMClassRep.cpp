//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open group, BMC Software, Tivoli Systems, IBM
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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Karl Schopmeyer(k.schopmeyer@attglobal.net)
//              Roger Kumpf, Hewlett-Packard Company (roger_kumpf@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CIMClassRep.h"
#include "DeclContext.h"
#include "Indentor.h"
#include "CIMName.h"
#include "CIMQualifierNames.h"
#include "XmlWriter.h"
#include <Pegasus/Common/Tracer.h>

PEGASUS_NAMESPACE_BEGIN
PEGASUS_USING_STD;

CIMClassRep::CIMClassRep(
    const CIMReference& reference,
    const String& superClassName)
    :
    CIMObjectRep(reference),
    _superClassName(superClassName)
{
    if (superClassName.size() && !CIMName::legal(superClassName))
	throw IllegalName();
}

CIMClassRep::~CIMClassRep()
{

}

Boolean CIMClassRep::isAssociation() const
{
    Uint32 pos = findQualifier(CIMQualifierNames::ASSOCIATION);

    if (pos == PEG_NOT_FOUND)
	return false;

    Boolean flag;

    const CIMValue& value = getQualifier(pos).getValue();

    if (value.getType() != CIMType::BOOLEAN)
	return false;

    value.get(flag);
    return flag;
}

Boolean CIMClassRep::isAbstract() const
{
    Uint32 pos = findQualifier(CIMQualifierNames::ABSTRACT);

    if (pos == PEG_NOT_FOUND)
	return false;

    Boolean flag;
    const CIMValue& value = getQualifier(pos).getValue();

    if (value.getType() != CIMType::BOOLEAN)
	return false;

    value.get(flag);
    return flag;
}

Boolean CIMClassRep::isTrueQualifier(const String& name) const
{
    Uint32 pos = findQualifier(name);

    if (pos == PEG_NOT_FOUND)
	return false;

    Boolean flag;
    const CIMValue& value = getQualifier(pos).getValue();

    if (value.getType() != CIMType::BOOLEAN)
	return false;

    value.get(flag);
    return flag;
}

void CIMClassRep::setSuperClassName(const String& superClassName)
{
    if (!CIMName::legal(superClassName))
	throw IllegalName();

    _superClassName = superClassName;
}

void CIMClassRep::addProperty(const CIMProperty& x)
{
    if (!x)
	throw UnitializedHandle();

    // Reject addition of duplicate property name:

    if (findProperty(x.getName()) != PEG_NOT_FOUND)
	throw AlreadyExists();

    // Reject addition of references to non-associations:

    if (!isAssociation() && x.getValue().getType() == CIMType::REFERENCE)
	throw AddedReferenceToClass(_reference.getClassName());

    // Set the class origin:
    // ATTN: put this check in other places:

    if (x.getClassOrigin().size() == 0)
	CIMProperty(x).setClassOrigin(_reference.getClassName());

    // Add the property:

    _properties.append(x);
}

void CIMClassRep::addMethod(const CIMMethod& x)
{
    if (!x)
	throw UnitializedHandle();

    // Reject duplicate method names:

    if (findMethod(x.getName()) != PEG_NOT_FOUND)
	throw AlreadyExists();

    // Add the method:

    _methods.append(x);
}

Uint32 CIMClassRep::findMethod(const String& name) const
{
    for (Uint32 i = 0, n = _methods.size(); i < n; i++)
    {
	if (CIMName::equal(_methods[i].getName(), name))
	    return i;
    }

    return PEG_NOT_FOUND;
}

Boolean CIMClassRep::existsMethod(const String& name) const
{
    return(findMethod(name) == PEG_NOT_FOUND) ? false : true;
}

CIMMethod CIMClassRep::getMethod(Uint32 pos)
{
    if (pos >= _methods.size())
	throw OutOfBounds();

    return _methods[pos];
}

Uint32 CIMClassRep::getMethodCount() const
{
    return _methods.size();
}

void CIMClassRep::removeMethod(Uint32 pos)
{
    if (pos >= _methods.size())
	throw OutOfBounds();

    _methods.remove(pos);
}

void CIMClassRep::resolve(
    DeclContext* context,
    const String& nameSpace)
{
	PEG_METHOD_ENTER(TRC_OBJECTRESOLUTION, "CIMClassRep::resolve()");
#if 0
    if (_resolved)
	throw ClassAlreadyResolved(_reference.getClassName());
#endif
    if (!context)
	throw NullPointer();

	PEG_TRACE_STRING(TRC_OBJECTRESOLUTION, Tracer::LEVEL3,
		String("CIMClassRep::resolve  class = ") +
		_reference.getClassName() + ", superclass = " +
		_superClassName);

    if (_superClassName.size())
	{
		//cout << "KSTEST Class Resolve with Super class " << getClassName() 
		//<< " superClass " << _superClassName << endl;
		//----------------------------------------------------------------------
		// First check to see if the super-class really exists and the subclassing legal:
		//----------------------------------------------------------------------
		CIMConstClass superClass
			= context->lookupClass(nameSpace, _superClassName);
	
		if (!superClass)
			throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_SUPERCLASS,_superClassName);
	
#if 0
		if (!superClass._rep->_resolved)
			throw ClassNotResolved(_superClassName);
#endif
		// If subclass is abstract but superclass not, throw CIM Exception
	
		/* ATTN:KS-24 Mar 2002 P1 - Test this and confirm that rule is correct
		if isAbstract() && !superclass.isAbstract()
			throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_SUPERCLASS,_superClassName);
		*/
		/*if(superclass.isTrueQualifier(CIMQualifierNames::TERMINAL)
			throw PEGASUS_CIM_EXCEPTION(CIM_ERR_INVALID_SUPERCLASS,_superClassName);
		*/
		//----------------------------------------------------------------------
		// Iterate all the properties of *this* class. Resolve each one and
		// set the class-origin:
		//----------------------------------------------------------------------
	
		for (Uint32 i = 0, n = _properties.size(); i < n; i++)
		{
			CIMProperty& property = _properties[i];
			Uint32 pos = superClass.findProperty(property.getName());
	
			if (pos == PEG_NOT_FOUND)
			{
				property.resolve(context, nameSpace, false, true);
			}
			else
			{
				CIMConstProperty superClassProperty =
				superClass.getProperty(pos);
				property.resolve(
					context, nameSpace, false, superClassProperty, true);
			}
		}
	
		//----------------------------------------------------------------------
		// Now prepend all properties inherited from the super-class (that
		// are not overriden by this sub-class).
		//----------------------------------------------------------------------
	
		// Iterate super-class properties:
	
		for (Uint32 i = 0, m = 0, n = superClass.getPropertyCount(); i < n; i++)
		{
			CIMConstProperty superClassProperty = superClass.getProperty(i);
	
			// Find the property in *this* class; if not found, then clone and
			// insert it (setting the propagated flag). Otherwise, change
			// the class-origin and propagated flag accordingly.
	
			Uint32 pos = PEG_NOT_FOUND;
			/*	 ATTN: KS move to simpler version of the find
			for (Uint32 j = m, n = _properties.size(); j < n; j++)
			{
				if (CIMName::equal(_properties[j].getName(),
								   superClassProperty.getName()))
				{
					pos = j;
					break;
				}
			}
			*/
			pos = findProperty(superClassProperty.getName());
	
			// If property exists in super class but not in this one, then
			// clone and insert it. Otherwise, the properties class
			// origin was set above.
	
			CIMProperty superproperty = superClassProperty.clone(true);
	
			if (pos == PEG_NOT_FOUND)
			{
				superproperty.setPropagated(true);
				_properties.insert(m++, superproperty);
			} 
			else 
			{
				// Property Qualifiers must propagate if allowed
				// If property exists in the superclass and in the subclass,
				// then, enumerate the qualifiers of the superclass's property.
				// If a qualifier is defined on the superclass's property
				// but not on the subclass's, then add it to the subclass's
				// property's qualifier list.
				CIMProperty subproperty = _properties[pos];
				for (Uint32 i = 0, n = superproperty.getQualifierCount();
					i < n; i++) 
				{
					Uint32 pos = PEG_NOT_FOUND;
					CIMQualifier superClassQualifier = 
											superproperty.getQualifier(i);
					const String name = superClassQualifier.getName();
					/* ATTN KS This is replacement find function.
					if((Uint32 j = subproperty.findQualifier(q.getName()) == PEG_NOT_FOUND)
					{
						subproperty.addQualifier(superClassQualifier);
						
					}
					*/
					for (Uint32 j = 0, m = subproperty.getQualifierCount();
						 j < m;
						 j++) 
					{
						CIMConstQualifier q = subproperty.getQualifier(j);
						if (CIMName::equal(name,
							   q.getName())) 
						{
							pos = j;
							break;
						}
					}  // end comparison of subclass property's qualifiers
					if (pos == PEG_NOT_FOUND)
					{
						subproperty.addQualifier(superClassQualifier);
					}
					/*
					if ((pos = subproperty.findQualifier(name)) == PEG_NOT_FOUND)
					{
						subproperty.addQualifier(superClassQualifier);
					}
					*/
				} // end iteration over superclass property's qualifiers
			}
		}
	
		//----------------------------------------------------------------------
		// Iterate all the methods of *this* class. Resolve each one and
		// set the class-origin:
		//----------------------------------------------------------------------
	
		for (Uint32 i = 0, n = _methods.size(); i < n; i++)
		{
			CIMMethod& method = _methods[i];
			Uint32 pos = superClass.findMethod(method.getName());
	
			if (pos == PEG_NOT_FOUND)
			{
				method.resolve(context, nameSpace);
			}
			else
			{
				CIMConstMethod superClassMethod = superClass.getMethod(pos);
				method.resolve(context, nameSpace, superClassMethod);
			}
		}
	
		//----------------------------------------------------------------------
		// Now prepend all methods inherited from the super-class (that
		// are not overriden by this sub-class).
		//----------------------------------------------------------------------
	
		for (Uint32 i = 0, m = 0, n = superClass.getMethodCount(); i < n; i++)
		{
			CIMConstMethod superClassMethod = superClass.getMethod(i);
	
			// Find the method in *this* class; if not found, then clone and
			// insert it (setting the propagated flag). Otherwise, change
			// the class-origin and propagated flag accordingly.
	
			Uint32 pos = PEG_NOT_FOUND;
			/**********************	 KS move to simpler version
			for (Uint32 j = m, n = _methods.size(); j < n; j++)
			{
				if (CIMName::equal(_methods[j].getName(),
									superClassMethod.getName()))
				{
					pos = j;
					break;
				}
			}
	
			// If method exists in super class but not in this one, then
			// clone and insert it. Otherwise, the method's class origin
			// has already been set above.
	
			if (pos == PEG_NOT_FOUND)
			{
				CIMMethod method = superClassMethod.clone();
				method.setPropagated(true);
				_methods.insert(m++, method);
			}
			*/
			if((pos = findMethod(superClassMethod.getName())) == PEG_NOT_FOUND)
			{
				CIMMethod method = superClassMethod.clone();
				method.setPropagated(true);
				_methods.insert(m++, method);
			}
		}
	
		//----------------------------------------------------------------------
		// Validate the qualifiers of this class:
		//----------------------------------------------------------------------
		//cout << "KSTEST Class Qualifiers resolve for class" << getClassName() << endl;
		_qualifiers.resolve(
			context,
			nameSpace,
			isAssociation() ? CIMScope::ASSOCIATION : CIMScope::CLASS,
			false,
			superClass._rep->_qualifiers,
			true);
    }
    else 	// No SuperClass exsts
    {
		//----------------------------------------------------------------------
		// Resolve each property:
		//----------------------------------------------------------------------
		//cout << "KSTEST Class Resolve, No Super class " << getClassName() << endl;

		for (Uint32 i = 0, n = _properties.size(); i < n; i++)
			_properties[i].resolve(context, nameSpace, false, true);
	
		//----------------------------------------------------------------------
		// Resolve each method:
		//----------------------------------------------------------------------
	
		for (Uint32 i = 0, n = _methods.size(); i < n; i++)
			_methods[i].resolve(context, nameSpace);
	
		//----------------------------------------------------------------------
		// Resolve the qualifiers:
		//----------------------------------------------------------------------
	
		CIMQualifierList dummy;
	
		_qualifiers.resolve(
			context,
			nameSpace,
			isAssociation() ? CIMScope::ASSOCIATION : CIMScope::CLASS,
			false,
			dummy, 
			true);
    }

    // _resolved = true;
}

void CIMClassRep::toXml(Array<Sint8>& out) const
{
    // Class opening element:

    out << "<CLASS ";
    out << " NAME=\"" << _reference.getClassName() << "\" ";

    if (_superClassName.size())
	out << " SUPERCLASS=\"" << _superClassName << "\" ";

    out << ">\n";

    // Qualifiers:

    _qualifiers.toXml(out);

    // Parameters:

    for (Uint32 i = 0, n = _properties.size(); i < n; i++)
	_properties[i].toXml(out);

    // Methods:

    for (Uint32 i = 0, n = _methods.size(); i < n; i++)
	_methods[i].toXml(out);

    // Class closing element:

    out << "</CLASS>\n";
}
/** toMof prepares an 8-bit string with the MOF for the class.
    The BNF for this is:
    <pre>
    classDeclaration 	=    [ qualifierList ]
			     CLASS className [ alias ] [ superClass ]
			     "{" *classFeature "}" ";"
			
    superClass 		=    :" className

    classFeature 	=    propertyDeclaration | methodDeclaration

*/

void CIMClassRep::toMof(Array<Sint8>& out) const
{
    // Get and format the class qualifiers
    out << "\n//    Class " << _reference.getClassName();
    if (_qualifiers.getCount())
	out << "\n";
    out << "\n";
    _qualifiers.toMof(out);

    // Separate qualifiers from Class Name
    out << "\n";

    // output class statement
    out << "class " << _reference.getClassName();

    if (_superClassName.size())
	out << " : " << _superClassName;

    out << "\n{";

    // format the Properties:
    for (Uint32 i = 0, n = _properties.size(); i < n; i++)
    {
	// Generate MOF if this property not propogated
	// Note that the test is required only because
	// there is an error in getclass that does not
	// test the localOnly flag.
	if (!_properties[i].getPropagated())
	    _properties[i].toMof(out);
    }

    // Format the Methods:  for non-propagated methods
    for (Uint32 i = 0, n = _methods.size(); i < n; i++)
    {
	if (!_methods[i].getPropagated())
	_methods[i].toMof(out);
    }

    // Class closing element:
    out << "\n};\n";
}

void CIMClassRep::print(PEGASUS_STD(ostream) &os) const
{
    Array<Sint8> tmp;
    toXml(tmp);
    tmp.append('\0');
    XmlWriter::indentedPrint(os, tmp.getData(), 4);
}

void CIMClassRep::printMof(PEGASUS_STD(ostream) &os) const
{
    Array<Sint8> tmp;
    toMof(tmp);
    tmp.append('\0');
    os << tmp.getData() << PEGASUS_STD(endl);
}


CIMClassRep::CIMClassRep()
{

}

CIMClassRep::CIMClassRep(const CIMClassRep& x) :
    CIMObjectRep(x),
    _superClassName(x._superClassName)
{
    _methods.reserve(x._methods.size());

    for (Uint32 i = 0, n = x._methods.size(); i < n; i++)
	_methods.append(x._methods[i].clone());
}

Boolean CIMClassRep::identical(const CIMObjectRep* x) const
{
    if (!CIMObjectRep::identical(x))
	return false;

    const CIMClassRep* tmprep = dynamic_cast<const CIMClassRep*>(x);
    if (!tmprep)
        return false;

    if (_superClassName != tmprep->_superClassName)
	return false;

    //
    // Check methods:
    //

    {
	const Array<CIMMethod>& tmp1 = _methods;
	const Array<CIMMethod>& tmp2 = tmprep->_methods;

	if (tmp1.size() != tmp2.size())
	    return false;

	for (Uint32 i = 0, n = tmp1.size(); i < n; i++)
	{
	    if (!tmp1[i].identical(tmp2[i]))
		return false;

	    if (tmp1[i].getClassOrigin() != tmp2[i].getClassOrigin())
		return false;

	    if (tmp1[i].getPropagated() != tmp2[i].getPropagated())
		return false;
	}
    }

    if (_resolved != tmprep->_resolved)
	return false;

    return true;
}

void CIMClassRep::getKeyNames(Array<String>& keyNames) const
{
    keyNames.clear();

    for (Uint32 i = 0, n = getPropertyCount(); i < n; i++)
    {
	CIMConstProperty property = getProperty(i);

	if (property.isKey())
	    keyNames.append(property.getName());
    }
}

Boolean CIMClassRep::hasKeys() const
{
    for (Uint32 i = 0, n = getPropertyCount(); i < n; i++)
    {
	CIMConstProperty property = getProperty(i);

	if (getProperty(i).isKey())
	    return true;
    }

    return false;
}

PEGASUS_NAMESPACE_END

