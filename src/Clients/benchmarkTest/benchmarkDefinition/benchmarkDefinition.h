//%/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software, Hewlett-Packard Company, IBM,
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
// Author: Denise Eckstein, Hewlett-Packard Company
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#ifndef Pegasus_benchmarkDefinition_h
#define Pegasus_benchmarkDefinition_h

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/Constants.h>


PEGASUS_NAMESPACE_BEGIN

class benchmarkDefinition
{
public:

   benchmarkDefinition();

   const CIMName& getClassName(Uint32 index);

   Uint32 getNumberOfClassDefinitions();

   Uint32 getConfiguration(
                          const CIMName& className,
                          Uint32& numberOfProperties,
                          Uint32& sizeOfPropertyValue,
                          Uint32& numberOfInstances);

private:

   Array<CIMName> _benchmarkDefinitionClassNames;
   Uint32 _numberOfClassDefinitions;
   Uint32 _maxClassDefinitionIndex;

};

PEGASUS_NAMESPACE_END

#endif /* Pegasus_benchmarkDefinition_h */

