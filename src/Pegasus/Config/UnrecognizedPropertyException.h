//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001 BMC Software, Hewlett-Packard Company, IBM,
// The Open Group, Tivoli Systems
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
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#ifndef Pegasus_UnrecognizedPropertyException_h
#define Pegasus_UnrecognizedPropertyException_h

#define UNRECOGNIZED_PROPERTY_ERR  "Error:  Unrecognized property name."
#define UNRECOGNIZED_PROPERTY_ERR1 "Property '"
#define UNRECOGNIZED_PROPERTY_ERR2 "' is not recognized."


#include <Pegasus/Config/PropertyException.h>
#include <Pegasus/Config/Linkage.h>


PEGASUS_NAMESPACE_BEGIN


/**
 * Exception class indicating that a property name is not recognized.
 *
 * @author Hewlett-Packard Company
 */
class UnrecognizedPropertyException : public PropertyException
{
public:

    /**
     * Constructor that uses a default message.
     */
    UnrecognizedPropertyException() :
        PropertyException(String ())
    {
        _message = UNRECOGNIZED_PROPERTY_ERR;
    }


    /**
     * Constructor that generates a message based on the name of the property
     * that is not recognized.
     *
     * @param propertyName The name of the property that is not recognized.
     */
    UnrecognizedPropertyException(String propertyName) :
        PropertyException(String ())
    {
        _message = UNRECOGNIZED_PROPERTY_ERR1 ;
        _message.append (propertyName);
        _message.append (UNRECOGNIZED_PROPERTY_ERR2);
    }
};

#endif /* Pegasus_UnrecognizedPropertyException_h */

