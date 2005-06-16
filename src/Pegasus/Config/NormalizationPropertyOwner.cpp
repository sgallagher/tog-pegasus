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
// Author:  Chip Vincent (cvincent@us.ibm.com)
//
// Modified By:  Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3614
//              Vijay Eli, IBM, (vijayeli@in.ibm.com) for Bug# 3613
//              Aruran, IBM (ashanmug@in.ibm.com) for Bug# 3613
//
//%/////////////////////////////////////////////////////////////////////////////

#include "NormalizationPropertyOwner.h"

PEGASUS_NAMESPACE_BEGIN

static struct ConfigPropertyRow properties[] =
{
#ifdef PEGASUS_USE_RELEASE_CONFIG_OPTIONS
    { "enableNormalization", "false", IS_STATIC, 0, 0, IS_VISIBLE },
#else
    { "enableNormalization", "true", IS_STATIC, 0, 0, IS_VISIBLE },
#endif
    { "excludeModulesFromNormalization", "", IS_STATIC, 0, 0, IS_VISIBLE }
};

const Uint32 NUM_PROPERTIES = sizeof(properties) / sizeof(properties[0]);

NormalizationPropertyOwner::NormalizationPropertyOwner(void)
{
    _providerObjectNormalizationEnabled.reset(new ConfigProperty());
    _providerObjectNormalizationModuleExclusions.reset(new ConfigProperty());
}

void NormalizationPropertyOwner::initialize(void)
{
    for(Uint8 i = 0; i < NUM_PROPERTIES; i++)
    {
        if(String::equalNoCase(properties[i].propertyName, "enableNormalization"))
        {
            _providerObjectNormalizationEnabled->propertyName = properties[i].propertyName;
            _providerObjectNormalizationEnabled->defaultValue = properties[i].defaultValue;
            _providerObjectNormalizationEnabled->currentValue = properties[i].defaultValue;
            _providerObjectNormalizationEnabled->plannedValue = properties[i].defaultValue;
            _providerObjectNormalizationEnabled->dynamic = properties[i].dynamic;
            _providerObjectNormalizationEnabled->domain = properties[i].domain;
            _providerObjectNormalizationEnabled->domainSize = properties[i].domainSize;
            _providerObjectNormalizationEnabled->externallyVisible = properties[i].externallyVisible;
        }
        else if(String::equalNoCase(properties[i].propertyName, "excludeModulesFromNormalization"))
        {
            _providerObjectNormalizationModuleExclusions->propertyName = properties[i].propertyName;
            _providerObjectNormalizationModuleExclusions->defaultValue = properties[i].defaultValue;
            _providerObjectNormalizationModuleExclusions->currentValue = properties[i].defaultValue;
            _providerObjectNormalizationModuleExclusions->plannedValue = properties[i].defaultValue;
            _providerObjectNormalizationModuleExclusions->dynamic = properties[i].dynamic;
            _providerObjectNormalizationModuleExclusions->domain = properties[i].domain;
            _providerObjectNormalizationModuleExclusions->domainSize = properties[i].domainSize;
            _providerObjectNormalizationModuleExclusions->externallyVisible = properties[i].externallyVisible;
        }
    }
}

void NormalizationPropertyOwner::getPropertyInfo(const String & name,
                         Array<String> & propertyInfo) const
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    propertyInfo.clear();

    propertyInfo.append(configProperty->propertyName);
    propertyInfo.append(configProperty->defaultValue);
    propertyInfo.append(configProperty->currentValue);
    propertyInfo.append(configProperty->plannedValue);

    if(configProperty->dynamic == IS_DYNAMIC)
    {
        propertyInfo.append(STRING_TRUE);
    }
    else
    {
        propertyInfo.append(STRING_FALSE);
    }

    if(configProperty->externallyVisible == IS_VISIBLE)
    {
        propertyInfo.append(STRING_TRUE);
    }
    else
    {
        propertyInfo.append(STRING_FALSE);
    }
}

String NormalizationPropertyOwner::getDefaultValue(const String & name) const
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    return(configProperty->defaultValue);
}

String NormalizationPropertyOwner::getCurrentValue(const String & name)const
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    return(configProperty->currentValue);
}

String NormalizationPropertyOwner::getPlannedValue(const String & name) const
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    return(configProperty->plannedValue);
}

void NormalizationPropertyOwner::initCurrentValue(const String & name, const String & value)
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    configProperty->currentValue = value;
}

void NormalizationPropertyOwner::initPlannedValue(const String & name, const String & value)
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    configProperty->plannedValue = value;
}

void NormalizationPropertyOwner::updateCurrentValue(const String & name, const String & value)
{
    // make sure the property is dynamic before updating the value.
    if(!isDynamic(name))
    {
        throw NonDynamicConfigProperty(name);
    }

    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    configProperty->currentValue = value;
}

void NormalizationPropertyOwner::updatePlannedValue(const String & name, const String & value)
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    configProperty->plannedValue = value;
}

Boolean NormalizationPropertyOwner::isValid(const String & name,
                                 const String & value) const
{
    if(String::equalNoCase(name, "enableNormalization"))
    {
        // valid values are "true" and "false"
        if(String::equal(value, "true") || String::equal(value, "false"))
        {
            return(true);
        }
    }
    else if(String::equalNoCase(name, "excludeModulesFromNormalization"))
    {
        // valid values must be in the form "n.n.n"

        // TODO: validate value

        return(true);
    }

    return(false);
}

Boolean NormalizationPropertyOwner::isDynamic(const String & name) const
{
    struct ConfigProperty * configProperty = _lookupConfigProperty(name);

    return(configProperty->dynamic == IS_DYNAMIC);
}

struct ConfigProperty * NormalizationPropertyOwner::_lookupConfigProperty(const String & name) const
{
    if(String::equalNoCase(name, _providerObjectNormalizationEnabled->propertyName))
    {
        return(_providerObjectNormalizationEnabled.get());
    }
    else if(String::equalNoCase(name, _providerObjectNormalizationModuleExclusions->propertyName))
    {
        return(_providerObjectNormalizationModuleExclusions.get());
    }
    else
    {
        throw UnrecognizedConfigProperty(name);
    }

    PEGASUS_UNREACHABLE( return(0); )
}

PEGASUS_NAMESPACE_END
