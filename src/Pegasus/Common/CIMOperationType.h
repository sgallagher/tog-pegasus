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
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_CIMOperationType_h
#define Pegasus_CIMOperationType_h


#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Linkage.h>
    

PEGASUS_NAMESPACE_BEGIN

/**This enum defines the CIM operation types.
    These are all intrinsic operations, plus one for
    extrinsic method invocation.
*/
enum CIMOperationType
 {
        CIMOPTYPE_INVOKE_METHOD = 0,
        CIMOPTYPE_GET_CLASS = 1,
        CIMOPTYPE_GET_INSTANCE = 2,
        CIMOPTYPE_EXPORT_INDICATION = 3,
        CIMOPTYPE_DELETE_CLASS = 4,
        CIMOPTYPE_DELETE_INSTANCE = 5,
        CIMOPTYPE_CREATE_CLASS = 6,
        CIMOPTYPE_CREATE_INSTANCE = 7,
        CIMOPTYPE_MODIFY_CLASS = 8,
        CIMOPTYPE_MODIFY_INSTANCE = 9,
        CIMOPTYPE_ENUMERATE_CLASSES = 10,
        CIMOPTYPE_ENUMERATE_CLASS_NAMES = 11,
        CIMOPTYPE_ENUMERATE_INSTANCES = 12,
        CIMOPTYPE_ENUMERATE_INSTANCE_NAMES = 13,
        CIMOPTYPE_EXEC_QUERY = 14,
        CIMOPTYPE_ASSOCIATORS = 15,
        CIMOPTYPE_ASSOCIATOR_NAMES = 16,
        CIMOPTYPE_REFERENCES = 17,
        CIMOPTYPE_REFERENCE_NAMES = 18,
        CIMOPTYPE_GET_PROPERTY = 19,
        CIMOPTYPE_SET_PROPERTY = 20,
        CIMOPTYPE_GET_QUALIFIER = 21,
        CIMOPTYPE_SET_QUALIFIER = 22,
        CIMOPTYPE_DELETE_QUALIFIER = 23,
        CIMOPTYPE_ENUMERATE_QUALIFIERS = 24
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_CIMOperationType_h */
