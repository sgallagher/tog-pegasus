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
//                (carolann_graves@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_WQLPropertySource_h
#define Pegasus_WQLPropertySource_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/WQL/Linkage.h>
#include <Pegasus/WQL/WQLOperand.h>

PEGASUS_NAMESPACE_BEGIN

/** This class must be implemented to provide a source of properties for the
    WQLSelectStatement::evaluateWhereClause() method which calls methods of
    this class to obtain real values for property names used in the where
    clause.
*/
class PEGASUS_WQL_LINKAGE WQLPropertySource
{
public:

    /** Virtual destructor.
    */
    virtual ~WQLPropertySource();

    /** Returns the value whose property has the given name. The output
	parameter value is populated with the value of that parameter.
	Note that only integer, double, and string types are supported
	(see the WQLOperand class). The implementer of this method must
	perform appropriate conversions to the appropriate type.

	@param propertyName name of property to be gotten.
	@param value holds the value of the property upon return.
	@return true if such a property was found and false otherwise.
    */
    virtual Boolean getValue(
	const CIMName& propertyName, 
	WQLOperand& value) const = 0;
};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_WQLPropertySource_h */
