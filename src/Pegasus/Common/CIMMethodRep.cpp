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
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By: Carol Ann Krug Graves, Hewlett-Packard Company
//                  (carolann_graves@hp.com)
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Config.h>
#include "CIMMethod.h"
#include "CIMMethodRep.h"
#include "Resolver.h"
#include "Indentor.h"
#include "CIMName.h"
#include "CIMScope.h"
#include "XmlWriter.h"
#include "MofWriter.h"
#include <Pegasus/Common/MessageLoader.h> //l10n

PEGASUS_NAMESPACE_BEGIN

CIMMethodRep::CIMMethodRep()
{
}

CIMMethodRep::CIMMethodRep(const CIMMethodRep& x) :
    Sharable(),
    _name(x._name),
    _type(x._type),
    _classOrigin(x._classOrigin),
    _propagated(x._propagated)
{
    x._qualifiers.cloneTo(_qualifiers);

    _parameters.reserveCapacity(x._parameters.size());

    for (Uint32 i = 0, n = x._parameters.size(); i < n; i++)
    {
        _parameters.append(x._parameters[i].clone());
    }
}

CIMMethodRep::CIMMethodRep(
    const CIMName& name,
    CIMType type,
    const CIMName& classOrigin,
    Boolean propagated)
    : _name(name), _type(type),
    _classOrigin(classOrigin), _propagated(propagated)
{
    // ensure name is not null
    if(name.isNull())
    {
        throw UninitializedObjectException();
    }
}

CIMMethodRep::~CIMMethodRep()
{
}

void CIMMethodRep::setName(const CIMName& name)
{
    // ensure name is not null
    if(name.isNull())
    {
        throw UninitializedObjectException();
    }

    _name = name;
}

void CIMMethodRep::setClassOrigin(const CIMName& classOrigin)
{
    _classOrigin = classOrigin;
}

void CIMMethodRep::addParameter(const CIMParameter& x)
{
    if (x.isUninitialized())
        throw UninitializedObjectException();

    if (findParameter(x.getName()) != PEG_NOT_FOUND){
        //l10n
                //throw AlreadyExistsException
            //("parameter \"" + x.getName().getString () + "\"");
        MessageLoaderParms parms("Common.CIMMethodRep.PARAMETER",
                                                         "parameter \"$0\"",
                                                         x.getName().getString());
        throw AlreadyExistsException(parms);
    }

    _parameters.append(x);
}

Uint32 CIMMethodRep::findParameter(const CIMName& name) const
{
    for (Uint32 i = 0, n = _parameters.size(); i < n; i++)
    {
        if (name.equal(_parameters[i].getName()))
            return i;
    }

    return PEG_NOT_FOUND;
}

CIMParameter CIMMethodRep::getParameter(Uint32 index)
{
    if (index >= _parameters.size())
        throw IndexOutOfBoundsException();

    return _parameters[index];
}

void CIMMethodRep::removeParameter(Uint32 index)
{
    if (index >= _parameters.size())
        throw IndexOutOfBoundsException();

    _parameters.remove (index);
}

Uint32 CIMMethodRep::getParameterCount() const
{
    return _parameters.size();
}

void CIMMethodRep::resolve(
    DeclContext* declContext,
    const CIMNamespaceName& nameSpace,
    const CIMConstMethod& inheritedMethod)
{
    // ATTN: Check to see if this method has same signature as
    // inherited one.

    // Check for type mismatch between return types.

    PEGASUS_ASSERT(!inheritedMethod.isUninitialized());

    // Validate the qualifiers of the method (according to
    // superClass's method with the same name). This method
    // will throw an exception if the validation fails.

    _qualifiers.resolve(
        declContext,
        nameSpace,
        CIMScope::METHOD,
        false,
        inheritedMethod._rep->_qualifiers,
        true);

    // Validate each of the parameters:

    for (Uint32 i = 0; i < _parameters.size(); i++)
        Resolver::resolveParameter (_parameters[i], declContext, nameSpace);

    _classOrigin = inheritedMethod.getClassOrigin();
}

void CIMMethodRep::resolve(
    DeclContext* declContext,
    const CIMNamespaceName& nameSpace)
{
    // Validate the qualifiers:

    CIMQualifierList dummy;

    _qualifiers.resolve(
        declContext,
        nameSpace,
        CIMScope::METHOD,
        false,
        dummy,
        true);

    // Validate each of the parameters:

    for (Uint32 i = 0; i < _parameters.size(); i++)
        Resolver::resolveParameter (_parameters[i], declContext, nameSpace);
}

static const char* _toString(Boolean x)
{
    return x ? "true" : "false";
}

void CIMMethodRep::toXml(Array<char>& out) const
{
    out << "<METHOD";

    out << " NAME=\"" << _name << "\"";

    out << " TYPE=\"" << cimTypeToString (_type) << "\"";

    if (!_classOrigin.isNull())
        out << " CLASSORIGIN=\"" << _classOrigin << "\"";

    if (_propagated != false)
        out << " PROPAGATED=\"" << _toString(_propagated) << "\"";

    out << ">\n";

    _qualifiers.toXml(out);

    for (Uint32 i = 0, n = _parameters.size(); i < n; i++)
        XmlWriter::appendParameterElement(out, _parameters[i]);

    out << "</METHOD>\n";
}

/**
    The BNF for this is;
    methodDeclaration   =  [ qualifierList ] dataType methodName
                           "(" [ parameterList ] ")" ";"

    parameterList       =  parameter *( "," parameter )
    Format with qualifiers on one line and declaration on another. Start
    with newline but none at the end.
*/
void CIMMethodRep::toMof(Array<char>& out) const   //ATTNKS:
{
    // Output the qualifier list starting on new line
    if (_qualifiers.getCount())
        out << "\n";

    _qualifiers.toMof(out);

    // output the type, MethodName and ParmeterList left enclosure
    out << "\n" << cimTypeToString (_type) << " " << _name << "(";

    // output the param list separated by commas.

    for (Uint32 i = 0, n = _parameters.size(); i < n; i++)
    {
        // If not first, output comma separator
        if (i)
            out << ", ";

        MofWriter::appendParameterElement(out, _parameters[i]);
    }

    // output the parameterlist and method terminator
    out << ");";
}


Boolean CIMMethodRep::identical(const CIMMethodRep* x) const
{
    if (!_name.equal (x->_name))
        return false;

    if (_type != x->_type)
        return false;

    if (!_qualifiers.identical(x->_qualifiers))
        return false;

    if (_parameters.size() != x->_parameters.size())
        return false;

    for (Uint32 i = 0, n = _parameters.size(); i < n; i++)
    {
        if (!_parameters[i].identical(x->_parameters[i]))
            return false;
    }

    return true;
}

void CIMMethodRep::setType(CIMType type)
{
    _type = type;
}

PEGASUS_NAMESPACE_END
