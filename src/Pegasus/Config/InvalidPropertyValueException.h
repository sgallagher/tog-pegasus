//%////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 The Open Group, BMC Software, Tivoli Systems, IBM
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
//==============================================================================
//
// Author: Nag Boranna (nagaraja_boranna@hp.com)
//
// Modified By:	
//
//%////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_InvalidPropertyValueException_h
#define Pegasus_InvalidPropertyValueException_h

#define INVALID_CONFIG_PROPERTY_ERR  "Error: Invalid property value."
#define INVALID_CONFIG_PROPERTY_ERR1 "Value '"
#define INVALID_CONFIG_PROPERTY_ERR2 "' is not valid for property '"
#define INVALID_CONFIG_PROPERTY_ERR3 "'."

#include <Pegasus/Config/PropertyException.h>


PEGASUS_NAMESPACE_BEGIN

/**
 * Exception class indicating that the value for a configuration property is
 * not valid.
 *
 * @author Hewlett-Packard Company
 */
class InvalidPropertyValueException : PropertyException
{
public:

    /**
     * Constructor that uses a default message.
     */
    InvalidPropertyValueException() : PropertyException(String ())
    {
		_message = INVALID_CONFIG_PROPERTY_ERR;
    }
    

    /**
     * Constructor that generates a message based on the property name and
     * invalid value.
     *
     * @param propertyName  The name of the property that was given an invalid
     *                      value.
     * @param propertyValue The invalid property value.
     */
    InvalidPropertyValueException
        (
            String propertyName,
            String propertyValue
        ) : PropertyException(String ())
    {
		_message = INVALID_CONFIG_PROPERTY_ERR1;
		_message.append (propertyValue);
		_message.append (INVALID_CONFIG_PROPERTY_ERR2);
		_message.append (propertyName);
		_message.append (INVALID_CONFIG_PROPERTY_ERR3);
    }

};

#endif  /* Pegasus_InvalidPropertyValueException_h */
