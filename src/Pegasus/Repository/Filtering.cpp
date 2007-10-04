//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/Tracer.h>
#include "Filtering.h"

PEGASUS_NAMESPACE_BEGIN

/** Check to see if the specified property is in the property list
    @param property the specified property
    @param propertyList the property list
    @return true if the property is in the list otherwise false.
*/
static Boolean _containsProperty(
    CIMProperty& property,
    const CIMPropertyList& propertyList)
{
    //  For each property in the propertly list
    for (Uint32 p=0; p<propertyList.size(); p++)
    {
        if (propertyList[p].equal(property.getName()))
            return true;
    }
    return false;
}

/* removes all Qualifiers from a CIMClass.  This function removes all
   of the qualifiers from the class, from all of the properties,
   from the methods, and from the parameters attached to the methods.
   @param cimClass reference to the class from which qualifiers are to
   be removed.
   NOTE: This would be logical to be moved to CIMClass since it may be
   more general than this usage.
*/
static void _removeAllQualifiers(CIMClass& cimClass)
{
    // remove qualifiers of the class
    Uint32 count = 0;
    while ((count = cimClass.getQualifierCount()) > 0)
        cimClass.removeQualifier(count - 1);

    // remove qualifiers from the properties
    for (Uint32 i = 0; i < cimClass.getPropertyCount(); i++)
    {
        CIMProperty p = cimClass.getProperty(i);
        count = 0;
        while ((count = p.getQualifierCount()) > 0)
            p.removeQualifier(count - 1);
    }

    // remove qualifiers from the methods
    for (Uint32 i = 0; i < cimClass.getMethodCount(); i++)
    {
        CIMMethod m = cimClass.getMethod(i);
        for (Uint32 j = 0 ; j < m.getParameterCount(); j++)
        {
            CIMParameter p = m.getParameter(j);
            count = 0;
            while ((count = p.getQualifierCount()) > 0)
                p.removeQualifier(count - 1);
        }
        count = 0;
        while ((count = m.getQualifierCount()) > 0)
            m.removeQualifier(count - 1);
    }
}

/////////////////////////////////////////////////////////////////////////
//
// _removePropagatedQualifiers - Removes all qualifiers from the class
// that are marked propagated
//
/////////////////////////////////////////////////////////////////////////

/* removes propagatedQualifiers from the defined CIMClass.
   This function removes the qualifiers from the class,
   from each of the properties, from the methods and
   the parameters if the qualifiers are marked propagated.
   NOTE: This could be logical to be moved to CIMClass since it may be
   more general than the usage here.
*/
static void _removePropagatedQualifiers(CIMClass& cimClass)
{
    Uint32 count = cimClass.getQualifierCount();
    // Remove nonlocal qualifiers from Class
    for (Sint32 i = (count - 1); i >= 0; i--)
    {
        CIMQualifier q = cimClass.getQualifier(i);
        if (q.getPropagated())
        {
            cimClass.removeQualifier(i);
        }
    }

    // remove  non localOnly qualifiers from the properties
    for (Uint32 i = 0; i < cimClass.getPropertyCount(); i++)
    {
        CIMProperty p = cimClass.getProperty(i);
        // loop to search qualifiers for nonlocal parameters
        count = p.getQualifierCount();
        for (Sint32 j = (count - 1); j >= 0; j--)
        {
            CIMQualifier q = p.getQualifier(j);
            if (q.getPropagated())
            {
                p.removeQualifier(j);
            }
        }
    }

    // remove non LocalOnly qualifiers from the methods and parameters
    for (Uint32 i = 0; i < cimClass.getMethodCount(); i++)
    {
        CIMMethod m = cimClass.getMethod(i);
        // Remove  nonlocal qualifiers from all parameters
        for (Uint32 j = 0 ; j < m.getParameterCount(); j++)
        {
            CIMParameter p = m.getParameter(j);
            count = p.getQualifierCount();
            for (Sint32 k = (count - 1); k >= 0; k--)
            {
                CIMQualifier q = p.getQualifier(k);
                if (q.getPropagated())
                {
                    p.removeQualifier(k);
                }
            }
        }

        // remove nonlocal qualifiers from the method
        count = m.getQualifierCount();
        for (Sint32 j = (count - 1); j >= 0; j--)
        {
            CIMQualifier q = m.getQualifier(j);
            if (q.getPropagated())
            {
                m.removeQualifier(j);
            }
        }
    }
}

/* remove the properties from an instance based on attributes.
    @param Instance from which properties will be removed.
    @param propertyList PropertyList is used in the removal algorithm
    @param localOnly - Boolean used in the removal.
    NOTE: This could be logical to move to CIMInstance since the
    usage is more general than just in the repository
*/
static void _removeProperties(
    CIMInstance& cimInstance,
    const CIMPropertyList& propertyList,
    Boolean localOnly)
{
    Boolean propertyListNull = propertyList.isNull();
    if ((!propertyListNull) || localOnly)
    {
        // Loop through properties to remove those that do not filter through
        // local only attribute and are not in the property list.
        Uint32 count = cimInstance.getPropertyCount();
        // Work backwards because removal may be cheaper. Sint32 covers count=0
        for (Sint32 i = (count - 1); i >= 0; i--)
        {
            CIMProperty p = cimInstance.getProperty(i);

            // if localOnly == true, ignore properties defined in super class
            if (localOnly && (p.getPropagated()))
            {
                cimInstance.removeProperty(i);
                continue;
            }

            // propertyList NULL means deliver properties.  PropertyList
            // empty, none.
            // Test for removal if propertyList not NULL. The empty list option
            // is covered by fact that property is not in the list.
            if (!propertyListNull)
                if (!_containsProperty(p, propertyList))
                    cimInstance.removeProperty(i);
        }
    }
}

/* remove all Qualifiers from a single CIMInstance. Removes
    all of the qualifiers from the instance and from properties
    within the instance.
    @param instance from which parameters are removed.
    NOTE: This could be logical to be moved to CIMInstance since
    the usage may be more general than just in the repository.
*/
static void _removeAllQualifiers(CIMInstance& cimInstance)
{
    // remove qualifiers from the instance
    Uint32 count = 0;
    while ((count = cimInstance.getQualifierCount()) > 0)
        cimInstance.removeQualifier(count - 1);

    // remove qualifiers from the properties
    for (Uint32 i = 0; i < cimInstance.getPropertyCount(); i++)
    {
        CIMProperty p = cimInstance.getProperty(i);
        count = 0;
        while ((count = p.getQualifierCount()) > 0)
            p.removeQualifier(count - 1);
    }
}

/* removes all ClassOrigin attributes from a single CIMInstance. Removes
    the classOrigin attribute from each property in the Instance.
   @param Instance from which the ClassOrigin Properties will be removed.
   NOTE: Logical to be moved to CIMInstance since it may be more general
   than just the repositoryl
*/
void _removeClassOrigins(CIMInstance& cimInstance)
{
    PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL4, "Remove Class Origins");

    Uint32 propertyCount = cimInstance.getPropertyCount();
    for (Uint32 i = 0; i < propertyCount ; i++)
        cimInstance.getProperty(i).setClassOrigin(CIMName());
}

/* Filters the properties, qualifiers, and classorigin out of a single instance.
    Based on the parameters provided for localOnly, includeQualifiers,
    and includeClassOrigin, this function simply filters the properties
    qualifiers, and classOrigins out of a single instance.  This function
    was created to have a single piece of code that processes getinstance
    and enumerateInstances returns.
    @param cimInstance reference to instance to be processed.
    @param localOnly defines if request is for localOnly parameters.
    @param includeQualifiers Boolean defining if qualifiers to be returned.
    @param includeClassOrigin Boolean defining if ClassOrigin attribute to
    be removed from properties.
*/
void Filtering::filterInstance(
    CIMInstance& cimInstance,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    // Remove properties based on propertyList and localOnly flag
    _removeProperties(cimInstance, propertyList, localOnly);

    // If includequalifiers false, remove all qualifiers from
    // properties.

    if (!includeQualifiers)
    {
        _removeAllQualifiers(cimInstance);
    }

    // if ClassOrigin Flag false, remove classOrigin info from Instance object
    // by setting the classOrigin to Null.

    if (!includeClassOrigin)
    {
        _removeClassOrigins(cimInstance);
    }
}

void Filtering::filterClass(
    CIMClass& cimClass,
    Boolean localOnly,
    Boolean includeQualifiers,
    Boolean includeClassOrigin,
    const CIMPropertyList& propertyList)
{
    // Remove properties based on propertylist and localOnly flag (Bug 565)
    Boolean propertyListNull = propertyList.isNull();

    // if localOnly OR there is a property list, process properties
    if ((!propertyListNull) || localOnly)
    {
        // Loop through properties to remove those that do not filter through
        // local only attribute and are not in the property list.
        Uint32 count = cimClass.getPropertyCount();
        // Work backwards because removal may be cheaper. Sint32 covers count=0
        for (Sint32 i = (count - 1); i >= 0; i--)
        {
            CIMProperty p = cimClass.getProperty(i);
            // if localOnly==true, ignore properties defined in super class
            if (localOnly && (p.getPropagated()))
            {
                cimClass.removeProperty(i);
                continue;
            }

            // propertyList NULL means all properties.  PropertyList
            // empty, none.
            // Test for removal if propertyList not NULL. The empty list option
            // is covered by fact that property is not in the list.
            if (!propertyListNull)
                if (!_containsProperty(p, propertyList))
                    cimClass.removeProperty(i);
        }
    }

    // remove methods based on localOnly flag
    if (localOnly)
    {
        Uint32 count = cimClass.getMethodCount();
        // Work backwards because removal may be cheaper.
        for (Sint32 i = (count - 1); i >= 0; i--)
        {
            CIMMethod m = cimClass.getMethod(i);

            // if localOnly==true, ignore properties defined in super class
            if (localOnly && (m.getPropagated()))
                cimClass.removeMethod(i);
        }

    }
    // If includequalifiers false, remove all qualifiers from
    // properties, methods and parameters.
    if (!includeQualifiers)
    {
        _removeAllQualifiers(cimClass);
    }
    else
    {
        // if includequalifiers and localOnly, remove nonLocal qualifiers
        if (localOnly)
        {
            _removePropagatedQualifiers(cimClass);
        }

    }


    // if ClassOrigin Flag false, remove classOrigin info from class object
    // by setting the property to Null.
    if (!includeClassOrigin)
    {
        PEG_TRACE_CSTRING(TRC_REPOSITORY, Tracer::LEVEL4,
            "Remove Class Origins");

        Uint32 propertyCount = cimClass.getPropertyCount();
        for (Uint32 i = 0; i < propertyCount ; i++)
            cimClass.getProperty(i).setClassOrigin(CIMName());

        Uint32 methodCount =  cimClass.getMethodCount();
        for (Uint32 i=0; i < methodCount ; i++)
            cimClass.getMethod(i).setClassOrigin(CIMName());
    }
}

PEGASUS_NAMESPACE_END
