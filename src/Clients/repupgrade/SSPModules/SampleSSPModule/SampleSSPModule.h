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
// Author: Sushma Fernandes, Hewlett-Packard Company 
//         (sushma_fernandes@hp.com)
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_SampleSSPModule_h
#define Pegasus_SampleSSPModule_h

#include <Clients/repupgrade/SSPModule.h>
#include <Pegasus/Common/Config.h>

PEGASUS_NAMESPACE_BEGIN

/**
    
    SampleSSPModule implements a sample Schema Special Process Module.
 
    The methods processClass and processInstance are implemented. Given
    a class or an instance the methods check whether it meets the criteria for
    special processing. In this example it checks whether a class or an instance
    uses the "Required qualifier" and if so whether the flavor on it is set 
    to OVERRIDABLE. If true the class or instance is updated 
    to unset flavor OVERRIDABLE.

 */

class SampleSSPModule : public SchemaSpecialProcessModule
{

public:

    SampleSSPModule();

    virtual ~SampleSSPModule();

    /**
       Performs special processing on a qualifier.
    */
    virtual Boolean processQualifier (CIMQualifierDecl& inputQual,
                                      CIMQualifierDecl& outputQual);

    /**
       Interface for special processing on a class.
    */
    virtual Boolean processClass (CIMClass& inputClass,
                                  CIMClass& outputClass);

    /**
       Interface for special processing on an instance.
    */
    virtual Boolean processInstance (CIMInstance& inputInstance,
                                     CIMInstance& outputInstance);


};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_SampleSSPModule_h */
