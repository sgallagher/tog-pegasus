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
// Author: Nag Boranna (nagaraja_boranna@hp.com)
//
// Modified By: Sushma Fernandes, Hewlett-Packard Company, sushma_fernandes@hp.com
//              Chip Vincent (cvincent@us.ibm.com)
//              Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3614
//              David Dillard, VERITAS Software Corp.
//                  (david.dillard@veritas.com)
//              Vijay Eli, IBM, (vijayeli@in.ibm.com) for Bug# 3613
//
//%/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// This file defines the configuration property owner class.
//
///////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_ConfigPropertyOwner_h
#define Pegasus_ConfigPropertyOwner_h

#include <Pegasus/Common/ArrayInternal.h>
#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Config.h>
#include <Pegasus/Config/ConfigExceptions.h>
#include <Pegasus/Config/Linkage.h>

PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
//  ConfigPropertyOwner Class
///////////////////////////////////////////////////////////////////////////////

/**
    This is an abstract class that the individual config property
    owners will extend and provide implementation.
*/
class PEGASUS_CONFIG_LINKAGE ConfigPropertyOwner
{
public:

    /** Constructors  */
    ConfigPropertyOwner() { }


    /** Destructor  */
    virtual ~ConfigPropertyOwner() { }

    /**
    Initialize the config properties.

    This method is expected to be called only once at the start of the
    CIMOM. It initializes the properties with the default values.
    */
    virtual void initialize() = 0;


    /**
    Get information about the specified property.

    @param name           The name of the property.
    @param propertyInfo   List to store the property info.
    @exception UnrecognizedConfigProperty  if the property is not defined.
    */
    virtual void getPropertyInfo(const String& name,
                         Array<String>& propertyInfo) const = 0;


    /**
    Get default value of the specified property.

    @param  name         The name of the property.
    @return string containing the default value of the property specified.
    @exception UnrecognizedConfigProperty  if the property is not defined.
    */
    virtual String getDefaultValue(const String& name) const = 0;


    /**
    Get current value of the specified property.

    @param  name         The name of the property.
    @return string containing the currnet value of the property specified.
    @exception UnrecognizedConfigProperty  if the property is not defined.
    */
    virtual String getCurrentValue(const String& name) const = 0;


    /**
    Get planned value of the specified property.

    @param  name         The name of the property.
    @return string containing the planned value of the property specified.
    @exception UnrecognizedConfigProperty  if the property is not defined.
    */
    virtual String getPlannedValue(const String& name) const = 0;


    /**
    Init current value of the specified property to the specified value.
    This method is expected to be called only once at the start of the
    CIMOM. The property value will be initialized irrespective of whether
    the property is dynamic or not.

    @param  name   The name of the property.
    @param  value  The current value of the property.
    @exception     UnrecognizedConfigProperty  if the property is not defined.
    @exception     InvalidPropertyValue  if the property value is not valid.
    */
    virtual void initCurrentValue(const String& name, const String& value) = 0;


    /**
    Init planned value of the specified property to the specified value.
    This method is expected to be called only once at the start of the
    CIMOM. The property value will be initialized irrespective of whether
    the property is dynamic or not.

    @param  name   The name of the property.
    @param  value  The planned value of the property.
    @exception     UnrecognizedConfigProperty  if the property is not defined.
    @exception     InvalidPropertyValue  if the property value is not valid.
    */
    virtual void initPlannedValue(const String& name, const String& value) = 0;


    /**
    Update current value of the specified property to the specified value.
    The property value will be updated only if the property is dynamically
    updatable.

    @param  name   The name of the property.
    @param  value  The current value of the property.
    @exception     NonDynamicConfigProperty  if the property is not dynamic.
    @exception     InvalidPropertyValue  if the property value is not valid.
    @exception     UnrecognizedConfigProperty  if the property is not defined.
    */
    virtual void updateCurrentValue(
        const String& name,
        const String& value) = 0;
        //throw (NonDynamicConfigProperty, InvalidPropertyValue,
        //    UnrecognizedConfigProperty) = 0;


    /**
    Update planned value of the specified property to the specified value.

    @param  name   The name of the property.
    @param  value  The planned value of the property.
    @exception     InvalidPropertyValue  if the property value is not valid.
    @exception     UnrecognizedConfigProperty  if the property is not defined.
    */
    virtual void updatePlannedValue(
        const String& name,
        const String& value) = 0;


    /**
    Checks to see if the given value is valid or not.

    @param  name   The name of the property.
    @param  value  The value of the property to be validated.
    @return true if the specified value for the property is valid.
    @exception UnrecognizedConfigProperty  if the property is not defined.
    */
    virtual Boolean isValid(const String& name, const String& value)const = 0;


    /**
    Checks to see if the specified property is dynamic or not.

    @param  name   The name of the property.
    @return true if the specified property is dynamic.
    @exception UnrecognizedConfigProperty  if the property is not defined.
    */
    virtual Boolean isDynamic(const String& name) const = 0;
};


///////////////////////////////////////////////////////////////////////////////
//  ConfigProperty
///////////////////////////////////////////////////////////////////////////////
/**
    The ConfigProperty struct used for defining the config properties.

    This structure is used by property owners that implement the
    ConfigPropertyOwner interface. Each config property they own will have
    their attributes defined in a structure of the type ConfigProperty.
    The structure members are initialized using the values defined in
    ConfigPropertyRow or by the set methods.
*/
///////////////////////////////////////////////////////////////////////////////

enum ConfigDynamic
{
		IS_DYNAMIC = 1,
		IS_STATIC = 0
};

enum ConfigVisible
{
		IS_VISIBLE =1,
		IS_HIDDEN = 0
};

struct ConfigProperty
{
    String     propertyName;    // Name of a config property
    String     defaultValue;    // Default value of a config property
    String     currentValue;    // Current value of a config property
    String     plannedValue;    // Planned of a config property
    ConfigDynamic    dynamic;            // Dynamic or non dynamic property
    char**     domain;            // List of valid values of a config property
    Uint32     domainSize;        // Size of the domain
    ConfigVisible    externallyVisible; // Determines whether a property wants to be
                                  // externally visible or not.
                                  // If a property chooses not to be externally
                                  // visible, it is not listed as a configurable
                                  // property but is still configurable.
};

///////////////////////////////////////////////////////////////////////////////
/**
    The ConfigPropertyRow used for uniformly defining the values of
    the properties.

    This structure is intended to be used by property owners that implement
    the ConfigPropertyOwner interface. Using this structure they can define
    the in memory default values for each attributes of the properties
    that they own.
*/
///////////////////////////////////////////////////////////////////////////////

struct ConfigPropertyRow
{
    const char* propertyName;
    const char* defaultValue;
    ConfigDynamic         dynamic;
    char**      domain;
    Uint32      domainSize;
    ConfigVisible         externallyVisible;
};

///////////////////////////////////////////////////////////////////////////////
/**
    Definition of commonly used constant string literals
*/
///////////////////////////////////////////////////////////////////////////////

PEGASUS_CONFIG_LINKAGE extern const char* STRING_TRUE;
PEGASUS_CONFIG_LINKAGE extern const char* STRING_FALSE;

PEGASUS_NAMESPACE_END

#endif /* Pegasus_ConfigPropertyOwner_h */
