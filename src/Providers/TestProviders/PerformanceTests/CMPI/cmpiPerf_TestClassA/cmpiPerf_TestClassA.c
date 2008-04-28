//%2006////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2005 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; VERITAS Software Corporation; The Open Group.
// Copyright (c) 2006 Hewlett-Packard Development Company, L.P.; IBM Corp.;
// EMC Corporation; Symantec Corporation; The Open Group.
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "cmpiPerf_TestClassA.h"
#include <Pegasus/Provider/CMPI/cmpimacs.h>

/* ---------------------------------------------------------------------------*/
/*                            Factory functions                               */
/* ---------------------------------------------------------------------------*/

/* ---------- method to create a CMPIObjectPath of this class ----------------*/

CMPIObjectPath * _makePath_TestClassA(
    const CMPIBroker * _broker,
    const CMPIContext * ctx,
    const CMPIObjectPath * cop,
    CMPIStatus * rc )
{
   CMPIObjectPath * op = NULL;
   int theKey=2;

   op=CMNewObjectPath(
       _broker,
       CMGetCharPtr(CMGetNameSpace(cop,rc)),
       _ClassName,
       rc);
   
   if ( CMIsNullObject(op) )
   {
      CMSetStatusWithChars(
          _broker,
          rc,
          CMPI_RC_ERR_FAILED,
          "Create CMPIObjectPath failed." );
   }
   else
   {
       CMAddKey(op, "theKey", &theKey, CMPI_uint32);
   }
   return op;
}

/* ----------- method to create a CMPIInstance of this class ----------------*/

CMPIInstance * _makeInst_TestClassA(
    const CMPIBroker * _broker,
    const CMPIContext * ctx,
    const CMPIObjectPath * cop,
    const char ** properties,
    CMPIStatus * rc )
{
   CMPIObjectPath * op     = NULL;
   CMPIInstance   * ci     = NULL;
   CMPIArray       *array  = NULL;
   unsigned short   opstatus  = 0; /* Unknown */
   unsigned short   status    = 2; /* Enabled */
   int theKey=2;

   //
   // Construct ObjectPath
   //
   op=CMNewObjectPath(
       _broker,
       CMGetCharPtr(CMGetNameSpace(cop,rc)),
       _ClassName,
       rc);
   if (CMIsNullObject(op))
   {
      CMSetStatusWithChars(
          _broker,
          rc,
          CMPI_RC_ERR_FAILED,
          "Create CMPIObjectPath failed.");
      return ci;
   }

   //
   // Create a new instance and fill it's properties
   //
   ci = CMNewInstance( _broker, op, rc);
   if (CMIsNullObject(ci))
   {
      CMSetStatusWithChars(
          _broker,
          rc,
          CMPI_RC_ERR_FAILED,
          "Create CMPIInstance failed.");
      return ci;
   }

   //
   // Properties of CIM_ManagedElement
   //
   CMSetProperty(ci,"Caption","Test class A",CMPI_chars);
   CMSetProperty(
       ci,
       "Description",
       "Test class used for all kinds of testing",
       CMPI_chars);
   CMSetProperty(ci,"ElementName","Instance1",CMPI_chars);

   //
   // Properties of CIM_ManagedSystemElement
   //
   array = CMNewArray(_broker,1,CMPI_uint16,rc);
   CMSetArrayElementAt(array,0,(CMPIValue*)&(opstatus),CMPI_uint16);
   CMSetProperty(ci,"OperationalStatus",(CMPIValue*)&(array),CMPI_uint16A);

   //
   // Properties of CIM_EnabledLogicalElement
   //
   CMSetProperty(ci,"EnabledState",(CMPIValue*)&(status),CMPI_uint16);
   CMSetProperty(ci,"OtherEnabledState","NULL",CMPI_chars);
   CMSetProperty(ci,"RequestedState",(CMPIValue*)&(status),CMPI_uint16);
   CMSetProperty(ci,"EnabledDefault",(CMPIValue*)&(status),CMPI_uint16);

   //
   // Properties of CIM_TestClass
   //
   CMSetProperty(ci,"theKey",&theKey,CMPI_uint32 );
   CMSetProperty(ci,"theData",&theKey,CMPI_uint32 );
   CMSetProperty(ci,"theString","Test Instance Number One",CMPI_chars);

   return ci;
}

/* -----------------------------------------------*/
/*          end of cmpiPerf_TestClassA.c          */
/* -----------------------------------------------*/

