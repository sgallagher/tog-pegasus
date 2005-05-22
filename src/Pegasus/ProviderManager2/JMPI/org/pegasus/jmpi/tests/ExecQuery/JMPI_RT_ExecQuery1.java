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
// Author:      Mark Hamzy, hamzy@us.ibm.com
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////
package org.pegasus.jmpi.tests.ExecQuery;

import org.pegasus.jmpi.*;
import java.util.*;

public class JMPI_RT_ExecQuery1 implements InstanceProvider
{
    public class ConvertibleVector extends Vector
    {
        public Object buildArray (Class type)
        {
            Object copy = java.lang.reflect.Array.newInstance (type, elementCount);

            System.arraycopy (elementData, 0, copy, 0, elementCount);

            return copy;
        }

        // Assumes there's at least one element and it's not null!
        public Object buildArray ()
        {
            if (elementCount > 0 && elementData[0] != null)
            {
                return buildArray(elementData[0].getClass());
            }
            else
            {
                throw new IllegalArgumentException("cannot convert to array");
            }
        }
    }

    final String      className          = "JMPI_RT_ExecQuery1";
    final String      nameSpaceClass     = "test/static";
    final String      nameSpacePGInterOp = "root/PG_InterOp";

    CIMOMHandle       ch                 = null;
    ConvertibleVector paths              = new ConvertibleVector ();
    ConvertibleVector instances          = new ConvertibleVector ();
    boolean           fDebug             = true;

    public JMPI_RT_ExecQuery1 ()
    {
        if (this.fDebug)
            System.out.println ("JMPI_RT_ExecQuery1::JMPI_RT_ExecQuery1");
    }

    public void cleanup ()
       throws CIMException
    {
        if (this.fDebug)
            System.out.println ("JMPI_RT_ExecQuery1::cleanup");
    }

    public void initialize (CIMOMHandle ch)
       throws CIMException
    {
        if (this.fDebug)
            System.out.println ("JMPI_RT_ExecQuery1::initialize: ch = " + ch);

        this.ch = ch;

        CIMInstance instance1 = new CIMInstance (className);

        instance1.setProperty ("CreationClassName", new CIMValue (new String (className)));
        instance1.setProperty ("InstanceId",        new CIMValue (new UnsignedInt64 ("1")));
        instance1.setProperty ("PropertyString",    new CIMValue (new String ("bob lives!")));

        instances.addElement (instance1);

        CIMInstance instance2 = new CIMInstance (className);

        instance2.setProperty ("CreationClassName", new CIMValue (new String (className)));
        instance2.setProperty ("InstanceId",        new CIMValue (new UnsignedInt64 ("2")));
        instance2.setProperty ("PropertyString",    new CIMValue (new String ("bill dies.")));

        instances.addElement (instance2);

        CIMObjectPath cop1 = new CIMObjectPath (className, nameSpaceClass);

        cop1.addKey ("CreationClassName", new CIMValue (new String (className)));
        cop1.addKey ("InstanceId",        new CIMValue (new UnsignedInt64 ("1")));
        cop1.addKey ("PropertyString",    new CIMValue (new String ("bob lives!")));

        paths.addElement (cop1);

        CIMObjectPath cop2 = new CIMObjectPath (className, nameSpaceClass);

        cop2.addKey ("CreationClassName", new CIMValue (new String (className)));
        cop2.addKey ("InstanceId",        new CIMValue (new UnsignedInt64 ("2")));
        cop2.addKey ("PropertyString",    new CIMValue (new String ("bill dies.")));

        paths.addElement (cop2);
    }

    public CIMObjectPath createInstance (CIMObjectPath cop,
                                         CIMInstance   ci)
       throws CIMException
    {
        if (this.fDebug)
        {
            System.out.println ("JMPI_RT_ExecQuery1::createInstance: cop = " + cop);
            System.out.println ("JMPI_RT_ExecQuery1::createInstance: ci  = " + ci);
        }

        // ensure the Namespace is valid
        if (!cop.getNameSpace ().equalsIgnoreCase (nameSpaceClass))
            throw new CIMException (CIMException.CIM_ERR_INVALID_NAMESPACE);

        // ensure the class existing in the specified namespace
        if (!cop.getObjectName ().equalsIgnoreCase (className))
            throw new CIMException (CIMException.CIM_ERR_INVALID_CLASS);

        // ensure the requested object does not exist
        if (findObjectPath (cop) >= 0)
           throw new CIMException (CIMException.CIM_ERR_ALREADY_EXISTS);

        // ensure the InstanceId key is valid
        CIMProperty propInstanceId = ci.getProperty ("InstanceId");

        if (this.fDebug)
            System.out.println ("JMPI_RT_ExecQuery1::createInstance: propInstanceId = " + propInstanceId);

        if (propInstanceId == null)
            throw new CIMException (CIMException.CIM_ERR_INVALID_PARAMETER);

        // ensure the property values are valid
        testPropertyTypesValue (ci);

        CIMObjectPath returnOp = new CIMObjectPath (className, nameSpaceClass);

        returnOp.addKey ("InstanceId",        propInstanceId.getValue ());
        returnOp.addKey ("CreationClassName", new CIMValue (className));

        return returnOp;
    }

    public CIMInstance getInstance (CIMObjectPath cop,
                                    CIMClass      cc,
                                    boolean       localOnly)
       throws CIMException
    {
        if (this.fDebug)
        {
            System.out.println ("JMPI_RT_ExecQuery1::getInstance: cop       = " + cop);
            System.out.println ("JMPI_RT_ExecQuery1::getInstance: cc        = " + cc);
            System.out.println ("JMPI_RT_ExecQuery1::getInstance: localOnly = " + localOnly);
        }

        // ensure the InstanceId key is valid
        Vector keys = cop.getKeys ();
        int    i;

        for (i = 0;
                i < keys.size ()
             && !((CIMProperty)keys.elementAt (i)).getName().equalsIgnoreCase("InstanceId");
             i++)
            ;

        if (i == keys.size ())
            throw new CIMException (CIMException.CIM_ERR_INVALID_PARAMETER);

        // ensure the Namespace is valid
        if (!cop.getNameSpace ().equalsIgnoreCase (nameSpaceClass))
            throw new CIMException (CIMException.CIM_ERR_INVALID_NAMESPACE);

        // ensure the class existing in the specified namespace
        if (!cop.getObjectName ().equalsIgnoreCase (className))
            throw new CIMException (CIMException.CIM_ERR_INVALID_CLASS);

        // ensure the request object exists
        int index = findObjectPath (cop);

        if (index < 0)
            throw new CIMException(CIMException.CIM_ERR_NOT_FOUND);

        return (CIMInstance)instances.elementAt (index);
    }

    public void setInstance (CIMObjectPath cop,
                             CIMInstance   cimInstance)
       throws CIMException
    {
        if (this.fDebug)
        {
            System.out.println ("JMPI_RT_ExecQuery1::setInstance: cop         = " + cop);
            System.out.println ("JMPI_RT_ExecQuery1::setInstance: cimInstance = " + cimInstance);
        }

        // ensure the Namespace is valid
        if (!cop.getNameSpace ().equalsIgnoreCase (nameSpaceClass))
            throw new CIMException (CIMException.CIM_ERR_INVALID_NAMESPACE);

        // ensure the class existing in the specified namespace
        if (!cop.getObjectName ().equalsIgnoreCase (className))
            throw new CIMException (CIMException.CIM_ERR_INVALID_CLASS);

        // ensure the property values are valid
        testPropertyTypesValue (cimInstance);

        // ensure the request object exists
        int index = findObjectPath (cop);

        if (index < 0)
            throw new CIMException (CIMException.CIM_ERR_NOT_FOUND);
    }

    public void deleteInstance (CIMObjectPath cop)
       throws CIMException
    {
        if (this.fDebug)
            System.out.println ("JMPI_RT_ExecQuery1::deleteInstance: cop = " + cop);

        // ensure the Namespace is valid
        if (!cop.getNameSpace ().equalsIgnoreCase (nameSpaceClass))
            throw new CIMException(CIMException.CIM_ERR_INVALID_NAMESPACE);

        // ensure the class existing in the specified namespace
        if (!cop.getObjectName ().equalsIgnoreCase (className))
            throw new CIMException(CIMException.CIM_ERR_INVALID_CLASS);

        // ensure the request object exists
        int index = findObjectPath (cop);

        if (index < 0)
            throw new CIMException (CIMException.CIM_ERR_NOT_FOUND);
    }

    public Vector enumInstances (CIMObjectPath cop,
                                 boolean       deep,
                                 CIMClass      cimClass)
       throws CIMException
    {
        if (this.fDebug)
        {
            System.out.println ("JMPI_RT_ExecQuery1::enumInstances: cop      = " + cop);
            System.out.println ("JMPI_RT_ExecQuery1::enumInstances: deep     = " + deep);
            System.out.println ("JMPI_RT_ExecQuery1::enumInstances: cimClass = " + cimClass);
        }

        // ensure the Namespace is valid
        if (!cop.getNameSpace ().equalsIgnoreCase (nameSpaceClass))
            throw new CIMException(CIMException.CIM_ERR_INVALID_NAMESPACE);

        // ensure the class existing in the specified namespace
        if (!cop.getObjectName ().equalsIgnoreCase (className))
            throw new CIMException(CIMException.CIM_ERR_INVALID_CLASS);

        return paths;
    }

    public Vector enumInstances (CIMObjectPath cop,
                                 boolean       deep,
                                 CIMClass      cimClass,
                                 boolean       localOnly)
       throws CIMException
    {
        if (this.fDebug)
        {
            System.out.println ("JMPI_RT_ExecQuery1::enumInstances: cop       = " + cop);
            System.out.println ("JMPI_RT_ExecQuery1::enumInstances: deep      = " + deep);
            System.out.println ("JMPI_RT_ExecQuery1::enumInstances: cimClass  = " + cimClass);
            System.out.println ("JMPI_RT_ExecQuery1::enumInstances: localOnly = " + localOnly);
        }

        // ensure the Namespace is valid
        if (!cop.getNameSpace ().equalsIgnoreCase (nameSpaceClass))
            throw new CIMException(CIMException.CIM_ERR_INVALID_NAMESPACE);

        // ensure the class existing in the specified namespace
        if (!cop.getObjectName ().equalsIgnoreCase (className))
            throw new CIMException(CIMException.CIM_ERR_INVALID_CLASS);

        return instances;
    }

/////**
//// * SNIA version
//// */
////public Vector execQuery (CIMObjectPath cop,
////                         String        queryStatement,
////                         int           ql,
////                         CIMClass      cimClass)
////   throws CIMException
////{
////    System.out.println ("JMPI_RT_ExecQuery1::execQuery: cop            = " + cop);
////    System.out.println ("JMPI_RT_ExecQuery1::execQuery: queryStatement = " + queryStatement);
////    System.out.println ("JMPI_RT_ExecQuery1::execQuery: ql             = " + ql);
////    System.out.println ("JMPI_RT_ExecQuery1::execQuery: cimClass       = " + cimClass);
////
////    System.out.println ("JMPI_RT_ExecQuery1::execQuery: Should not be here!");
////
////    return null;
////}

    /**
     * Pegasus 2.4 version
     */
    public CIMInstance[] execQuery (CIMObjectPath cop,
                                    String        queryStatement,
                                    String        queryLanguage,
                                    CIMClass      cimClass)
       throws CIMException
    {
        CIMInstance[] ret = null;

        if (this.fDebug)
        {
            System.out.println ("JMPI_RT_ExecQuery1::execQuery: cop            = " + cop);
            System.out.println ("JMPI_RT_ExecQuery1::execQuery: queryStatement = " + queryStatement);
            System.out.println ("JMPI_RT_ExecQuery1::execQuery: queryLanguage  = " + queryLanguage);
            System.out.println ("JMPI_RT_ExecQuery1::execQuery: cimClass       = " + cimClass);
        }

        if (instances.size () > 0)
        {
            ConvertibleVector returnVector = new ConvertibleVector ();

            if (queryLanguage.equalsIgnoreCase ("WQL"))
            {
                if (  queryStatement.equalsIgnoreCase ("Select * from " + className + " where InstanceId = 1")
                   && instances.size () >= 1
                   )
                {
                    returnVector.addElement ((CIMInstance)instances.elementAt (0));
                }
                else if (  queryStatement.equalsIgnoreCase ("Select * from " + className + " where InstanceId = 2")
                        && instances.size () >= 2
                        )
                {
                    returnVector.addElement ((CIMInstance)instances.elementAt (1));
                }
            }

            if (returnVector.size () > 0)
            {
                ret = (CIMInstance[])returnVector.buildArray ();
            }
        }

        return ret;
    }

    int findObjectPath (CIMObjectPath path)
    {
        String p = path.toString ();
        for (int i = 0; i < paths.size(); i++)
        {
            if (((CIMObjectPath)paths.elementAt (i)).toString().equalsIgnoreCase (p))
                return i;
        }

        return -1;
    }

    void testPropertyTypesValue (CIMInstance instanceObject)
       throws CIMException
    {
        Vector properties     = instanceObject.getProperties ();
        int    iPropertyCount = properties.size ();

        for (int j = 0; j < iPropertyCount; j++)
        {
            CIMProperty property      = (CIMProperty)properties.elementAt (j);
            String      propertyName  = property.getName();
            CIMValue    propertyValue = property.getValue();
            Object      value         = propertyValue.getValue();
            int         type          = property.getType().getType();

            switch (type)
            {
            case CIMDataType.UINT8:
                if (!(value instanceof UnsignedInt8))
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                if (((UnsignedInt8)value).intValue() >= 255)
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                break;
            case CIMDataType.UINT16:
                if (!(value instanceof UnsignedInt16))
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                if (((UnsignedInt16)value).intValue() >= 10000)
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                break;
            case CIMDataType.UINT32:
                if (!(value instanceof UnsignedInt32))
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                if (((UnsignedInt32)value).intValue() >= 10000000)
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                break;
            case CIMDataType.UINT64:
                if (!(value instanceof UnsignedInt64))
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                if (((UnsignedInt64)value).longValue() >= 1000000000)
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                break;
            case CIMDataType.SINT8:
                if (!(value instanceof Byte))
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                if (((Byte)value).intValue() <= -120)
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                break;
            case CIMDataType.SINT16:
                if (!(value instanceof Short))
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                if (((Short)value).intValue() < -10000)
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                break;
            case CIMDataType.SINT32:
                if (!(value instanceof Integer))
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                if (((Integer)value).intValue() <= -10000000)
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                break;
            case CIMDataType.SINT64:
                if (!(value instanceof Long))
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                if (((Long)value).intValue() <= -1000000000)
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                break;
            case CIMDataType.REAL32:
                if (!(value instanceof Float))
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                if (((Float)value).floatValue() >= 10000000.32)
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                break;
            case CIMDataType.REAL64:
                if (!(value instanceof Double))
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                if (((Double)value).doubleValue() >= 1000000000.64)
                    throw new CIMException(CIMException.CIM_ERR_INVALID_PARAMETER);
                break;
            default:
                break;
            }
        }
    }
}
