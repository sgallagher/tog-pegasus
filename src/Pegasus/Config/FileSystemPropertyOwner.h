//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
// Author: Sushma Fernandes (sushma_fernandes@hp.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// 
// This file defines the trace property owner class.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_FileSystemPropertyOwner_h
#define Pegasus_FileSystemPropertyOwner_h

#include <Pegasus/Config/ConfigPropertyOwner.h>


PEGASUS_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////
//  FileSystemPropertyOwner Class
///////////////////////////////////////////////////////////////////

/** 
    This is FileSystem property owner class that extends ConfigPropertyOwner
    class and provide implementation.
*/
class PEGASUS_CONFIG_LINKAGE FileSystemPropertyOwner : public ConfigPropertyOwner
{
public:

    /** Constructors  */
    FileSystemPropertyOwner();

    /** Destructor  */
    ~FileSystemPropertyOwner();

    /**
    Initialize the config properties.

    This method is expected to be called only once at the start of the
    CIMOM. It initializes the properties with the default values.
    */
    void initialize();


    /** 
    Get information about the specified property.

    @param propertyName   The name of the property.
    @param propertyInfo   List to store the property info.
    @exception UnrecognizedConfigProperty  if the property is not defined.
    */
    void getPropertyInfo(const String& name, Array<String>& propertyInfo);
        //throw (UnrecognizedConfigProperty);

    /** 
    Get default value of the specified property.

    @param  name         The name of the property.
    @return string containing the default value of the property specified.
    @exception UnrecognizedConfigProperty  if the property is not defined.
    */
    const String getDefaultValue(const String& name);
        //throw (UnrecognizedConfigProperty);

    /** 
    Get current value of the specified property.

    @param  name         The name of the property.
    @return string containing the currnet value of the property specified.
    @exception UnrecognizedConfigProperty  if the property is not defined.
    */
    const String getCurrentValue(const String& name);
        //throw (UnrecognizedConfigProperty);

    /** 
    Get planned value of the specified property.

    @param  name         The name of the property.
    @return string containing the planned value of the property specified.
    @exception UnrecognizedConfigProperty  if the property is not defined.
    */
    const String getPlannedValue(const String& name);
        //throw (UnrecognizedConfigProperty);

    /** 
    Init current value of the specified property to the specified value.
    This method is expected to be called only once at the start of the
    CIMOM. The property value will be initialized irrespective of whether
    the property is dynamic or not.

    @param  name         The name of the property.
    @param  value        The current value of the property. 
    @exception     UnrecognizedConfigProperty  if the property is not defined.
    @exception     InvalidPropertyValue  if the property value is not valid.
    */
    void initCurrentValue(const String& name, const String& value);
        //throw (UnrecognizedConfigProperty, InvalidPropertyValue);


    /** 
    Init planned value of the specified property to the specified value.
    This method is expected to be called only once at the start of the
    CIMOM. The property value will be initialized irrespective of whether
    the property is dynamic or not.

    @param  name         The name of the property.
    @param  value        The planned value of the property. 
    @exception     UnrecognizedConfigProperty  if the property is not defined.
    @exception     InvalidPropertyValue  if the property value is not valid.
    */
    void initPlannedValue(const String& name, const String& value); 
        //throw (UnrecognizedConfigProperty, InvalidPropertyValue);

    /** 
    Update current value of the specified property to the specified value.
    The property value will be updated only if the property is dynamically
    updatable.

    @param  name         The name of the property.
    @param  value        The current value of the property. 
    @exception     NonDynamicConfigProperty  if the property is not dynamic.
    @exception     InvalidPropertyValue  if the property value is not valid.
    @exception     UnrecognizedConfigProperty  if the property is not defined.
    */
    void updateCurrentValue(const String& name, const String& value);
        //throw (NonDynamicConfigProperty, InvalidPropertyValue,
        //    UnrecognizedConfigProperty);

    /** 
    Update planned value of the specified property to the specified value.

    @param  name         The name of the property.
    @param  value        The planned value of the property. 
    @exception     InvalidPropertyValue  if the property value is not valid.
    @exception     UnrecognizedConfigProperty  if the property is not defined.
    */
    void updatePlannedValue(const String& name, const String& value);
        //throw (InvalidPropertyValue, UnrecognizedConfigProperty);

    /** 
    Checks to see if the given value is valid or not.

    @param  name         The name of the property.
    @param  value        The value of the property to be validated. 
    @return Boolean      True if the specified value for the property is valid.
    @exception UnrecognizedConfigProperty  if the property is not defined.
    */
    Boolean isValid(const String& name, const String& value);
        //throw (UnrecognizedConfigProperty);

    /** 
    Checks to see if the specified property is dynamic or not.

    @param  name         The name of the property.
    @return Boolean      True if the specified property is dynamic.
    @exception UnrecognizedConfigProperty  if the property is not defined.
    */
    Boolean isDynamic(const String& name);
        //throw (UnrecognizedConfigProperty);

private:
    struct ConfigProperty* _lookupConfigProperty(const String& name);

    /**
    The file system properties owned by this class
    */
    struct ConfigProperty*      _repositoryDir;

    struct ConfigProperty*      _providerDir;

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_FileSystemPropertyOwner_h */
