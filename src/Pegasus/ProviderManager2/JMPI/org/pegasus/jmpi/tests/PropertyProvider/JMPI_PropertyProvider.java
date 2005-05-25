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
package org.pegasus.jmpi.tests.PropertyProvider;

import org.pegasus.jmpi.*;
import java.util.*;

public class JMPI_PropertyProvider implements PropertyProvider
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

    final String      className          = "JMPI_PropertyProvider";
    final String      nameSpaceClass     = "test/static";
    final String      nameSpacePGInterOp = "root/PG_InterOp";

    CIMOMHandle       ch                 = null;
    ConvertibleVector paths              = new ConvertibleVector ();
    ConvertibleVector instances          = new ConvertibleVector ();
    boolean           fDebug             = true;

    public JMPI_PropertyProvider ()
    {
        if (this.fDebug)
            System.out.println ("JMPI_PropertyProvider::JMPI_PropertyProvider");
    }

    public void cleanup ()
       throws CIMException
    {
        if (this.fDebug)
            System.out.println ("JMPI_PropertyProvider::cleanup");
    }

    public void initialize (CIMOMHandle ch)
       throws CIMException
    {
        if (this.fDebug)
            System.out.println ("JMPI_PropertyProvider::initialize: ch = " + ch);

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
            System.out.println ("JMPI_PropertyProvider::createInstance: cop = " + cop);
            System.out.println ("JMPI_PropertyProvider::createInstance: ci  = " + ci);
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
            System.out.println ("JMPI_PropertyProvider::createInstance: propInstanceId = " + propInstanceId);

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
            System.out.println ("JMPI_PropertyProvider::getInstance: cop       = " + cop);
            System.out.println ("JMPI_PropertyProvider::getInstance: cc        = " + cc);
            System.out.println ("JMPI_PropertyProvider::getInstance: localOnly = " + localOnly);
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
            System.out.println ("JMPI_PropertyProvider::setInstance: cop         = " + cop);
            System.out.println ("JMPI_PropertyProvider::setInstance: cimInstance = " + cimInstance);
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
            System.out.println ("JMPI_PropertyProvider::deleteInstance: cop = " + cop);

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
            System.out.println ("JMPI_PropertyProvider::enumInstances: cop      = " + cop);
            System.out.println ("JMPI_PropertyProvider::enumInstances: deep     = " + deep);
            System.out.println ("JMPI_PropertyProvider::enumInstances: cimClass = " + cimClass);
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
            System.out.println ("JMPI_PropertyProvider::enumInstances: cop       = " + cop);
            System.out.println ("JMPI_PropertyProvider::enumInstances: deep      = " + deep);
            System.out.println ("JMPI_PropertyProvider::enumInstances: cimClass  = " + cimClass);
            System.out.println ("JMPI_PropertyProvider::enumInstances: localOnly = " + localOnly);
        }

        // ensure the Namespace is valid
        if (!cop.getNameSpace ().equalsIgnoreCase (nameSpaceClass))
            throw new CIMException(CIMException.CIM_ERR_INVALID_NAMESPACE);

        // ensure the class existing in the specified namespace
        if (!cop.getObjectName ().equalsIgnoreCase (className))
            throw new CIMException(CIMException.CIM_ERR_INVALID_CLASS);

        return instances;
    }

    private int findObjectPath (CIMObjectPath path)
    {
        HashMap hashPaths = new HashMap ();
        String  p         = path.toString ();

        if (this.fDebug)
            System.out.println ("JMPI_PropertyProvider::findObjectPath: comparing \"" + p + "\"");

        // Iterate through each element in our instances
        for (int i = 0; i < paths.size (); i++)
        {
            HashMap       hashCop  = new HashMap ();
            CIMObjectPath cop      = (CIMObjectPath)paths.elementAt (i);
            Vector        keysPath = path.getKeys ();

            if (this.fDebug)
                System.out.println ("JMPI_PropertyProvider::findObjectPath: to \"" + cop + "\"");

            // Iterate though each property in the ObjectPath
            for (int idxPath = 0; idxPath < keysPath.size (); idxPath++)
            {
                CIMProperty cp          = (CIMProperty)keysPath.elementAt (idxPath);
                String      cpKeyValue  = cp.getValue ().toString ();
                String      copKeyValue = cop.getKeyValue (cp.getName ());

                if (this.fDebug)
                {
                    System.out.println ("JMPI_PropertyProvider::findObjectPath: cpKeyValue  \"" + cpKeyValue + "\"");
                    System.out.println ("JMPI_PropertyProvider::findObjectPath: copKeyValue \"" + copKeyValue + "\"");
                }

                // Compare the property values and save it for later
                hashCop.put (cp.toString (), new Boolean (cpKeyValue.equals (copKeyValue)));
            }

            // Save the result of all of the property comparisons
            hashPaths.put (cop, hashCop);
        }

        if (this.fDebug)
            System.out.println ("JMPI_PropertyProvider::findObjectPath: hashPaths = " + hashPaths);

        Iterator itrHashPaths = hashPaths.keySet ().iterator ();

        // Iterate through all of our results
        while (itrHashPaths.hasNext ())
        {
            Object   key     = itrHashPaths.next ();
            HashMap  hash    = (HashMap)hashPaths.get (key);
            boolean  found   = true;
            Iterator itrHash = hash.values ().iterator ();

            while (itrHash.hasNext ())
            {
                if (!((Boolean)itrHash.next ()).booleanValue ())
                {
                    found = false;
                }
            }

            if (this.fDebug)
                System.out.println ("JMPI_PropertyProvider::findObjectPath: found = " + found + ", key = " + key);

            // Were they all equal?
            if (found)
            {
                // Return the index of it
                return paths.indexOf (key);
            }
        }

        return -1;
    }

    private void testPropertyTypesValue (CIMInstance instanceObject)
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

    public void setPropertyValue (CIMObjectPath cop,
                                  String        oclass,
                                  String        pName,
                                  CIMValue      val)
    {
        if (this.fDebug)
        {
            System.out.println ("JMPI_PropertyProvider::setPropertyValue: cop    = " + cop   );
            System.out.println ("JMPI_PropertyProvider::setPropertyValue: oclass = " + oclass);
            System.out.println ("JMPI_PropertyProvider::setPropertyValue: pName  = " + pName );
            System.out.println ("JMPI_PropertyProvider::setPropertyValue: val    = " + val   );
        }

        try
        {
            CIMInstance ci = getInstance (cop, new CIMClass (oclass), true);

            if (this.fDebug)
                System.out.println ("JMPI_PropertyProvider::setPropertyValue: ci     = " + ci);

            CIMProperty cp = ci.getProperty (pName);

            if (this.fDebug)
                System.out.println ("JMPI_PropertyProvider::setPropertyValue: cp     = " + cp);

            cp.setValue (val);

            if (this.fDebug)
                System.out.println ("JMPI_PropertyProvider::setPropertyValue: cp     = " + cp);
        }
        catch (Exception e)
        {
            if (this.fDebug)
                System.out.println ("JMPI_PropertyProvider::setPropertyValue: caught = " + e);
        }
    }

    public CIMValue getPropertyValue (CIMObjectPath cop,
                                      String        oclass,
                                      String        pName)
    {
        CIMValue cv = null;

        if (this.fDebug)
        {
            System.out.println ("JMPI_PropertyProvider::setPropertyValue: cop    = " + cop   );
            System.out.println ("JMPI_PropertyProvider::setPropertyValue: oclass = " + oclass);
            System.out.println ("JMPI_PropertyProvider::setPropertyValue: pName  = " + pName );
        }

        try
        {
            CIMInstance ci = getInstance (cop, new CIMClass (oclass), true);

            if (this.fDebug)
                System.out.println ("JMPI_PropertyProvider::setPropertyValue: ci     = " + ci);

            CIMProperty cp = ci.getProperty (pName);

            if (this.fDebug)
                System.out.println ("JMPI_PropertyProvider::setPropertyValue: cp     = " + cp);

            if (cp != null)
            {
                cv = cp.getValue ();
            }

            if (this.fDebug)
                System.out.println ("JMPI_PropertyProvider::setPropertyValue: cv     = " + cv);
        }
        catch (Exception e)
        {
            if (this.fDebug)
                System.out.println ("JMPI_PropertyProvider::setPropertyValue: caught = " + e);
        }

        return cv;
    }
}
