//%LICENSE////////////////////////////////////////////////////////////////
//
// Licensed to The Open Group (TOG) under one or more contributor license
// agreements.  Refer to the OpenPegasusNOTICE.txt file distributed with
// this work for additional information regarding copyright ownership.
// Each contributor licenses this file to you under the OpenPegasus Open
// Source License; you may not use this file except in compliance with the
// License.
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//////////////////////////////////////////////////////////////////////////
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPI_String.h"
#include "CMPI_Value.h"
#include "CMPISCMOUtilities.h"

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

CIMDateTimeRep* CMPISCMOUtilities::scmoDateTimeFromCMPI(CMPIDateTime* cmpidt)
{
    CIMDateTimeRep* cimdt = 0;

    if (cmpidt && cmpidt->hdl)
    {
        cimdt = ((CIMDateTime*)cmpidt->hdl)->_rep;
    }
    return cimdt;
}

// Function to convert a SCMO Value into a CMPIData structure
CMPIrc CMPISCMOUtilities::scmoValue2CMPIData(
    const void* scmoValue, CMPIType type, CMPIData *data)
{
    //Initialize CMPIData object
    data->type = type;
    data->state = 0;
    data->value.uint64 = 0;

    //Check for NULL CIMValue
    if( scmoValue == 0 )
    {
        data->state=CMPI_nullValue;
        return CMPI_RC_OK;
    }

    if (type & CMPI_ARRAY)
    {
        fprintf(stderr,"scmoValue2CMPIData: Arrays not yet supported!!!\n");
    }
    else
    {
        data->state = CMPI_goodValue;
        switch (type)
        {
        case CMPI_chars:
        case CMPI_string:
            {
                data->value.string = reinterpret_cast<CMPIString*>(
                    new CMPI_Object((const char*)scmoValue));
                data->type = CMPI_string;
                break;
            }

        case CMPI_dateTime:
            {
                CIMDateTimeRep* dtrep = (CIMDateTimeRep*)scmoValue;
                CIMDateTime* cimdt = new CIMDateTime();
                *(cimdt->_rep) = *dtrep;
                data->value.dateTime = reinterpret_cast<CMPIDateTime*>
                    (new CMPI_Object(cimdt));
                break;
            }

        case CMPI_ref:
            {
                data->value.ref = reinterpret_cast<CMPIObjectPath*>
                    (new CMPI_Object((SCMOInstance*)scmoValue));
            }
            break;

        case CMPI_instance:
            {
                data->value.inst = reinterpret_cast<CMPIInstance*>
                    (new CMPI_Object((SCMOInstance*)scmoValue, true));
            }
            break;
        default:
            {
                data->value.uint64 = *(Uint64*)scmoValue;
                break;
            }
        }
    }
    return CMPI_RC_OK;
}

PEGASUS_NAMESPACE_END

