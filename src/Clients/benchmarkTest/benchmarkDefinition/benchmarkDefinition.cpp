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

#include "benchmarkDefinition.h"

PEGASUS_NAMESPACE_BEGIN

#define MAX_NUMBER_OF_PROPERTIES   50
#define MAX_SIZE_OF_PROPERTY_VALUE 1000
#define MAX_NUMBER_OF_INSTANCES    100

benchmarkDefinition::benchmarkDefinition ()
{
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0001S0010I0001");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0001S0100I0001");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0001S1000I0001");

   _benchmarkDefinitionClassNames.append ("benchmarkClassP0010S0010I0001");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0010S0100I0001");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0010S1000I0001");

   _benchmarkDefinitionClassNames.append ("benchmarkClassP0050S0010I0001");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0050S0100I0001");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0050S1000I0001");

   _benchmarkDefinitionClassNames.append ("benchmarkClassP0001S0010I0010");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0001S0100I0010");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0001S1000I0010");

   _benchmarkDefinitionClassNames.append ("benchmarkClassP0010S0010I0010");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0010S0100I0010");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0010S1000I0010");

   _benchmarkDefinitionClassNames.append ("benchmarkClassP0050S0010I0010");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0050S0100I0010");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0050S1000I0010");

   _benchmarkDefinitionClassNames.append ("benchmarkClassP0001S0010I0100");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0001S0100I0100");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0001S1000I0100");

   _benchmarkDefinitionClassNames.append ("benchmarkClassP0010S0010I0100");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0010S0100I0100");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0010S1000I0100");

   _benchmarkDefinitionClassNames.append ("benchmarkClassP0050S0010I0100");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0050S0100I0100");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0050S1000I0100");

   _benchmarkDefinitionClassNames.append ("benchmarkClassP0010S0020I0050");
   _benchmarkDefinitionClassNames.append ("benchmarkClassP0030S0010I0001");

   _numberOfClassDefinitions = _benchmarkDefinitionClassNames.size();
   _maxClassDefinitionIndex = _numberOfClassDefinitions - 1;
}


const CIMName& benchmarkDefinition::getClassName(Uint32 index)

{

   if (index > _maxClassDefinitionIndex)
   {
     throw (CIM_ERR_INVALID_PARAMETER);
   }
   else
   {
     return (_benchmarkDefinitionClassNames[index]);
   }

}

Uint32 benchmarkDefinition::getNumberOfClassDefinitions()
{

   return(_numberOfClassDefinitions);

}

Uint32 benchmarkDefinition::getConfiguration(
                        const CIMName& className,
                        Uint32& numberOfProperties,
                        Uint32& sizeOfPropertyValue,
                        Uint32& numberOfInstances)
{

    String classNameString = className.getString();

    if (!String::equal(classNameString.subString(0, 14), "benchmarkClass"))
    {
       return(CIM_ERR_NOT_SUPPORTED);
    }

    String numberPropertiesString = classNameString.subString(15, 4);
    numberOfProperties = atoi(numberPropertiesString.getCString());
    if (numberOfProperties > MAX_NUMBER_OF_PROPERTIES)
    {
       return(CIM_ERR_NOT_SUPPORTED);
    }

    String sizePropertiesString = classNameString.subString(20, 4);
    sizeOfPropertyValue = atoi(sizePropertiesString.getCString());
    if (sizeOfPropertyValue > MAX_SIZE_OF_PROPERTY_VALUE)
    {
       return(CIM_ERR_NOT_SUPPORTED);
    }

    String numberInstancesString = classNameString.subString(25, 4);
    numberOfInstances = atoi(numberInstancesString.getCString());
    if (numberOfInstances > MAX_NUMBER_OF_INSTANCES)
    {
       return(CIM_ERR_NOT_SUPPORTED);
    }

    return(CIM_ERR_SUCCESS);
}

PEGASUS_NAMESPACE_END

