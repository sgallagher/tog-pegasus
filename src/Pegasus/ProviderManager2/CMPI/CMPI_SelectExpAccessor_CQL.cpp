//%2004////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002 BMC Software; Hewlett-Packard Development
// Company, L.P.; IBM Corp.; The Open Group; Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation, The Open Group.
// Copyright (c) 2004 BMC Software; Hewlett-Packard Development Company, L.P.;
// IBM Corp.; EMC Corporation; VERITAS Software Corporation; The Open Group.
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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include "CMPI_Version.h"

#include "CMPI_SelectExpAccessor_CQL.h"
#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpimacs.h>

#include <Pegasus/Common/String.h>
#include <Pegasus/CQL/CQLParser.h>
#include <Pegasus/CQL/CQLParserState.h>
#include <Pegasus/CQL/CQLSelectStatement.h>
#include <Pegasus/Repository/RepositoryQueryContext.h>
#include <Pegasus/Common/CIMName.h>
#include <Pegasus/Common/MessageLoader.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/CQL/CQLExpression.h>
#include <Pegasus/CQL/CQLSimplePredicate.h>
#include <Pegasus/CQL/CQLPredicate.h>
#include <Pegasus/CQL/CQLValue.h>
PEGASUS_NAMESPACE_BEGIN PEGASUS_USING_STD;

CMPI_SelectExpAccessor_CQL::CMPI_SelectExpAccessor_CQL (CMPIAccessor * acc,
                                                        void *parm,
                                                        CQLSelectStatement *
                                                        stmt,
                                                        CIMObjectPath &
                                                        objPath):
accessor (acc),
accParm (parm),
_stmt (stmt),
_objPath (objPath)
{
  // Construct an _instance
  _constructInstance ();
}

void
CMPI_SelectExpAccessor_CQL::_constructInstance ()
{

  _instance = CIMInstance (_objPath.getClassName ().getString ());

  // Iterate throught the CQLPredicates

  CQLPredicate pred = _stmt->getPredicate ();

  Array < CQLPredicate > pred_Array;

  if (pred.isSimple ())
    {
      pred_Array.append (pred);
    }
  else
    {
      pred_Array = pred.getPredicates ();
    }
  for (Uint32 i = 0; i < pred_Array.size (); i++)
    {
      CQLPredicate pred = pred_Array[i];

      if (pred.isSimple ())
        {
          CQLSimplePredicate simple = pred.getSimplePredicate ();

          CQLExpression lhs = simple.getLeftExpression ();
          CQLExpression rhs = simple.getRightExpression ();

          // It is possible to have a NULL pointer.
          CQLValue lhs_val;
          CQLValue rhs_val;

          if (lhs.getTerms ().size () != 0)
            lhs_val = lhs.getTerms ()[0].getFactors ()[0].getValue ();
          if (rhs.getTerms ().size () != 0)
            rhs_val = rhs.getTerms ()[0].getFactors ()[0].getValue ();

          String name;
          if (lhs_val.getValueType () == CQLValue::CQLIdentifier_type)
            {
              // Ok, we have a winner!
              name = lhs.toString ();
            }
          else if (rhs_val.getValueType () == CQLValue::CQLIdentifier_type)
            {
              name = rhs.toString ();
            }
          // Call the accessor function with the name.            


          CMPIAccessor *get = (CMPIAccessor *) accessor;
          CMPIData data = get (name.getCString (), accParm);

          if (data.type & CMPI_ARRAY)
            {
			// Nothing yet
            }

          else if ((data.type & (CMPI_UINT | CMPI_SINT)) == CMPI_SINT)
            {
              switch (data.type)
                {
                case CMPI_sint32:
                  _instance.
                    addProperty (CIMProperty
                                 (CIMName (name),
                                  Sint32 (data.value.sint32)));
                  break;
                case CMPI_sint16:
                  _instance.
                    addProperty (CIMProperty
                                 (CIMName (name),
                                  Sint16 (data.value.sint16)));
                  break;
                case CMPI_sint8:
                  _instance.
                    addProperty (CIMProperty
                                 (CIMName (name),
                                  Sint16 (data.value.sint16)));
                  break;
                case CMPI_sint64:
                  _instance.
                    addProperty (CIMProperty
                                 (CIMName (name),
                                  Sint64 (data.value.sint64)));
                  break;
                default:
                  break;
                }
            }

          else if (data.type == CMPI_chars)
            _instance.
              addProperty (CIMProperty
                           (CIMName (name), String (data.value.chars)));
          else if (data.type == CMPI_string)
            {
              CMPIStatus rc;
              _instance.
                addProperty (CIMProperty
                             (CIMName (name),
                              String (CMGetCharsPtr
                                      (data.value.string, &rc))));
            }

          else if ((data.type & (CMPI_UINT | CMPI_SINT)) == CMPI_UINT)
            {
              switch (data.type)
                {
                case CMPI_uint32:
                  _instance.
                    addProperty (CIMProperty
                                 (CIMName (name),
                                  Uint32 (data.value.uint32)));
                  break;
                case CMPI_uint16:
                  _instance.
                    addProperty (CIMProperty
                                 (CIMName (name),
                                  Uint16 (data.value.uint16)));
                  break;
                case CMPI_uint8:
                  _instance.
                    addProperty (CIMProperty
                                 (CIMName (name), Uint8 (data.value.uint32)));
                  break;
                case CMPI_uint64:
                  _instance.
                    addProperty (CIMProperty
                                 (CIMName (name),
                                  Uint64 (data.value.uint64)));
                  break;
                default:;
                  break;
                }
            }

          else
            switch (data.type)
              {
              case CMPI_boolean:
                _instance.
                  addProperty (CIMProperty
                               (CIMName (name),
                                Boolean (data.value.boolean)));
                break;
              case CMPI_real32:
                _instance.
                  addProperty (CIMProperty
                               (CIMName (name), data.value.real32));
                break;
              case CMPI_real64:
                _instance.
                  addProperty (CIMProperty
                               (CIMName (name), data.value.real64));
                break;
              default:
                break;
              }
        }
    }

}

PEGASUS_NAMESPACE_END
