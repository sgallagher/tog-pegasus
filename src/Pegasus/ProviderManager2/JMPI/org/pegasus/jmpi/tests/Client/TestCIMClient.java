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
// TestCIMClient.java
//
// The contents of this file are subject to the SNIA Public License Version 1.0
// (the "License"); you may not use this file except in compliance with the
// License. You may obtain a copy of the License at
//
//      http://www.snia.org/resources/openSource.html
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
// License for the specific language governing rights and limitations
// under the License.
//
// The Original Code is TestCIMClient.java
//
// The Initial Developers of the Original Code are Frank Piorko and
// David Simons
//
// Contributor(s):
//
// $Id: TestCIMClient.java,v 1.3 2005/02/05 23:00:34 karl Exp $
//
// Revision 1.7  2001/08/05 15:13:31   bpatil
// -  Added Pegasus CIMServer support
//  
// Revision 1.8  2002/08/30 16:35:00   jyu
// -  Added more test cases
// -  Test output goes to log file /tmp/javaclienttest.log
//


package org.pegasus.jmpi.tests.Client;

import org.pegasus.jmpi.*;
import java.util.*;

class Log {
   public static void assignLogFileName(String fn) {}
   public static void startTransaction() {}
   public static void printlnTransaction(String line) { return; }//System.out.println(line); }
}

public class TestCIMClient {

    private void TestNameSpaceOperations(CIMClient cc)
    {
        System.out.println("\n++++ Test Namespace Operations ++++");
        Log.printlnTransaction("\n++++ Test Namespace Operations ++++");

        int numNameSpaces;
        boolean deepInheritance;
        String hostName = "localhost";
        String nsName = "myNameSpace";

        CIMNameSpace newNS1 = new CIMNameSpace(hostName, "root/"+nsName);
        CIMNameSpace newNS2 = new CIMNameSpace(hostName, "root/"+nsName+"/two");

        //
        // create namespace
        //
        try 
        {
            System.out.println("\n*** Create namespace ");
            Log.printlnTransaction("\n*** Create namespace ");
            Log.printlnTransaction("  " + newNS1.getNameSpace());
            cc.createNameSpace(newNS1);

            Log.printlnTransaction("  " + newNS2.getNameSpace());
            cc.createNameSpace(newNS2);
        } 
        catch (CIMException ce) 
        {
            if (ce.getID().equals(CIMException.CIM_ERR_ALREADY_EXISTS)) 
            {
                Log.printlnTransaction("\nUnable to create namespace: " + ce);
            }
            else
            {
                System.err.println("\nFailed to create namespace: " + ce);
                System.exit(1);
            }
        }

        //
        // list namespaces
        //
        try
        {
            // list current namespace
            //
            // NOTE:  Pegasus CIMOM returns the current namespace.  SNIA
            //        returns all namesapces under "root"
            //
            System.out.println("\n*** List current namespace");
            Log.printlnTransaction("\n*** List current namespace");
            CIMObjectPath pa1=new CIMObjectPath("root", (String)null);

            deepInheritance = true;
            Enumeration elist1 = cc.enumerateNameSpaces(pa1, deepInheritance);
            if (elist1!=null)
            {
                while (elist1.hasMoreElements())
                {
                    String nsString=(String)(elist1.nextElement());
                    Log.printlnTransaction("  Name space: " + nsString);
                }
            }

            // list namespacs under root with deep inheritance
            //
            System.out.println("\n*** List namespaces under root - DEEP");
            Log.printlnTransaction("\n*** List namespaces under root - DEEP");
            numNameSpaces = 0;
            CIMObjectPath pa2=new CIMObjectPath("root", "root");

            deepInheritance = true;
            Enumeration elist2 = cc.enumerateNameSpaces(pa2, deepInheritance);
            if (elist2!=null)
            {
                while (elist2.hasMoreElements())
                {
                    String nsString=(String)(elist2.nextElement());
                    Log.printlnTransaction("  Name space: " + nsString);
                    numNameSpaces++;
                }
            }
            System.out.println(numNameSpaces + " namespaces found");
            Log.printlnTransaction(numNameSpaces  + " namespaces found");

            // list namespacs under root without deep inheritance
            //
            System.out.println("\n*** List namespaces under root - No DEEP");
            Log.printlnTransaction("\n*** List namespaces under root - No DEEP");
            numNameSpaces = 0;
            deepInheritance = false;
            Enumeration elist3 = cc.enumerateNameSpaces(pa2, deepInheritance);
            if (elist3!=null)
            {
                while (elist3.hasMoreElements())
                {
                    String nsString=(String)(elist3.nextElement());
                    Log.printlnTransaction("  Name space: " + nsString);
                    numNameSpaces++;
                }
            }
            System.out.println(numNameSpaces + " namespaces found");
            Log.printlnTransaction(numNameSpaces + " namespaces found");

            //
            // list namespaces under root/myNameSpace
            //
            System.out.println("\n*** List namespaces under root/myNameSpace");
            Log.printlnTransaction("\n*** List namespaces under root/myNameSpace");
            numNameSpaces = 0;
            CIMObjectPath pa3=new CIMObjectPath("root", "root/myNameSpace");

            deepInheritance = true;
            Enumeration elist4 = cc.enumerateNameSpaces(pa3, deepInheritance);
            if (elist3!=null)
            {
                while (elist4.hasMoreElements())
                {
                    String nsString=(String)(elist4.nextElement());
                    Log.printlnTransaction("  Name space: " + nsString);
                    numNameSpaces++;
                }
            }
            System.out.println(numNameSpaces + " namespaces found");
            Log.printlnTransaction(numNameSpaces + " namespaces found");
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to enumerate namespaces: " + ce);
            System.exit(1);
        }

        //
        // delete namespaces
        //
        try
        {
            System.out.println("\n*** delete namespace ");
            Log.printlnTransaction("\n*** delete namespace ");

            cc.deleteNameSpace(newNS2);
            Log.printlnTransaction("  " + newNS1.getNameSpace());

            cc.deleteNameSpace(newNS1);
            Log.printlnTransaction("  " + newNS2.getNameSpace());
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to delete namespace: " + ce);
            System.exit(1);
        }
    }

    private void TestClassOperations(CIMClient cc, CIMNameSpace clientNameSpace)
    {
        System.out.println("\n++++ Test Class Operations ++++");
        Log.printlnTransaction("\n++++ Test Class Operations ++++");

        boolean localOnly;
        boolean includeQualifiers;
        boolean includeClassOrigin;
        boolean deepInheritance;

        //
        // get class
        //
        try
        {
            Log.printlnTransaction("\n*** Get class - include qualifiers");
            System.out.println("\n*** Get class - include qualifiers");

            CIMObjectPath pa1=new CIMObjectPath("CIM_ComputerSystem");
            localOnly = true;
            includeQualifiers = true;
            includeClassOrigin = false;

            CIMClass cl=cc.getClass(pa1, localOnly, includeQualifiers,
                                    includeClassOrigin, null);
            Log.printlnTransaction("  class name: "+ pa1);
            Vector qualifiers1 = cl.getQualifiers();
            for(int i=0;i<qualifiers1.size();i++)
            {
                CIMQualifier qt= (CIMQualifier)qualifiers1.elementAt(i);
                Log.printlnTransaction("  qualifier: " + qt.getName());
            }
            Log.printlnTransaction(qualifiers1.size() + " qualifiers found");
            System.out.println(qualifiers1.size() + " qualifiers found");

            // NOTE:  Pegasus cimom currently ignores the includeQualifiers
            //        parameter.  Qualifiers are always returned.
            //
            Log.printlnTransaction("\n*** Get class - Not include qualifiers");
            System.out.println("\n*** Get class - Not include qualifiers");

            localOnly = true;
            includeQualifiers = false;
            includeClassOrigin = false;

            CIMClass c2=cc.getClass(pa1, localOnly, includeQualifiers,
                                    includeClassOrigin, null);
            Log.printlnTransaction("  class name: "+ pa1);
            Vector qualifiers2 = c2.getQualifiers();
            Log.printlnTransaction(qualifiers2.size() + " qualifiers found");
            System.out.println(qualifiers2.size() + " qualifiers found");
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to fetch a single class: " + ce);
            System.exit(1);
        }

        //
        // list all classnames
        //
        Log.printlnTransaction("\n*** Enumerate class names ");
        System.out.println("\n*** Enumerate class names ");
        CIMObjectPath pa2=new CIMObjectPath((String)null);
        int numClassNames = 0;
        try
        {
            deepInheritance = false;
            Enumeration en1=cc.enumerateClassNames(pa2, deepInheritance);
            if (en1!=null)
            {
                while (en1.hasMoreElements())
                {
                    CIMObjectPath obj=(CIMObjectPath)(en1.nextElement());
                    Log.printlnTransaction("  class name: "+obj);
                    numClassNames++;
                }
            }
            System.out.println(numClassNames + " classes found");
            Log.printlnTransaction(numClassNames + " classes found");
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to enumerate class names: " + ce);
            System.exit(1);
        }

        //
        // enumerate classes
        //
        int numClasses = 0;

        try
        {
            Log.printlnTransaction("\n*** Enumerate classes - DEEP");
            System.out.println("\n*** Enumerate classes - DEEP");

            CIMObjectPath pa3=new CIMObjectPath("CIM_ComputerSystem");

            localOnly = true;
            includeQualifiers = true;
            includeClassOrigin = true;
            deepInheritance = true;
            Enumeration en2=cc.enumerateClasses(pa3, deepInheritance,
                                                localOnly, includeQualifiers,
                                                includeClassOrigin);
            if (en2!=null)
            {
                while (en2.hasMoreElements())
                {
                    CIMClass cimclass=(CIMClass)(en2.nextElement());
                    Log.printlnTransaction("  class name: " + cimclass.getName());
                    numClasses++;
                }
            }
            System.out.println(numClasses + " classes found");
            Log.printlnTransaction(numClasses + " classes found");
        }
        catch (CIMException ce) 
        {
            System.err.println("\nFailed to enumerate classes: " + ce);
            System.exit(1);
        }

        try 
        {
            Log.printlnTransaction("\n*** Enumerate classes - No DEEP");
            System.out.println("\n*** Enumerate classes - No DEEP");
            CIMObjectPath pa4=new CIMObjectPath("CIM_ComputerSystem");

            localOnly = true;
            includeQualifiers = true;
            includeClassOrigin = true;
            deepInheritance = false;

            Enumeration en3=cc.enumerateClasses(pa4, deepInheritance, 
                                                localOnly, includeQualifiers, 
                                                includeClassOrigin);
            numClasses = 0;
            if (en3!=null) 
            {
                while (en3.hasMoreElements()) 
                {
                    CIMClass cimclass2=(CIMClass)(en3.nextElement());
                    Log.printlnTransaction("  class name: " + cimclass2.getName());
                    numClasses++;
                }
            }
            System.out.println(numClasses + " classes found");
            Log.printlnTransaction(numClasses + " classes found");
        }
        catch (CIMException ce) 
        {
            System.err.println("\nFailed to enumerate classes: " + ce);
            System.exit(1);
        }
    }

    private void TestClassModifyOperations(CIMClient cc, CIMNameSpace clientNameSpace)
    {
        System.out.println("\n++++ Test Class Modify Operations ++++");
        Log.printlnTransaction("\n++++ Test Class Modify Operations ++++");

        boolean localOnly;
        boolean includeQualifiers;
        boolean includeClassOrigin;
        boolean deepInheritance;
        boolean classHasInstances = false;
        
        String className = "MyTestClass";
        CIMObjectPath cp = new CIMObjectPath(className,
                                             clientNameSpace.getNameSpace());

        //
        // delete the class if it exists
        //
        System.out.println("\n*** Delete class ");
        Log.printlnTransaction("\n*** Delete class ");

        try
        {
            cc.deleteClass(cp);
        }
        catch (CIMException ce) 
        {
            // NOTE:  Pegasus throws CIM_ERR_INVALID_CLASS.  It should
            //        throw CIM_ERR_NOT_FOUND exception instead (according
            //        to the DMTF spec)
            //
            if (ce.getID().equals(CIMException.CIM_ERR_NOT_FOUND) ||
	        ce.getID().equals(CIMException.CIM_ERR_INVALID_CLASS))
            {
                Log.printlnTransaction("\nUnable to delete class: " + ce);
            }
            else
            {
                if (ce.getID().equals(CIMException.CIM_ERR_CLASS_HAS_INSTANCES))
                {
                    classHasInstances = true;
                }
                else
                {
                    System.err.println("\nUnexpected test failure: " + ce);
                    System.exit(1);
                }
            }
        }

        // if class has instances, delete the instances, then delete the class
        if (classHasInstances)
        {
            try
            {
                localOnly = false;
                includeQualifiers = true;
                includeClassOrigin = false;
                CIMClass cimClass = cc.getClass(new CIMObjectPath(className),
                                            localOnly, 
                                            includeQualifiers,
                                            includeClassOrigin,
                                            null);
                CIMInstance cimInstance = cimClass.newInstance();
                cimInstance.setProperty("theKey", new CIMValue("true",
                    CIMDataType.getPredefinedType(CIMDataType.STRING)));
                CIMObjectPath path = new CIMObjectPath(className,
                                         cimInstance.getKeyValuePairs());
                cc.deleteInstance(path);
            }
            catch (CIMException ce)
            {
                if (ce.getID().equals(CIMException.CIM_ERR_NOT_FOUND))
                {
                    System.out.println("\nUnable to delete instances:" + ce);
                    Log.printlnTransaction("\nUnable to delete instances:" + ce);
                }
                else
                {
                    System.err.println("\nFailed to delete instances: " + ce);
                    System.exit(1);
                }
            }
            catch (Exception e)
            {
	    }

            try
            {
                cc.deleteClass(cp);
            }
            catch (CIMException ce) 
            {
                if (ce.getID().equals(CIMException.CIM_ERR_NOT_FOUND)) 
                {
                    System.out.println("\nUnable to delete class:" + ce);
                    Log.printlnTransaction("\nUnable to delete class:" + ce);
                }
                else
                {
                    System.err.println("\nFailed to delete class: " + ce);
                    System.exit(1);
                }
            }
        }

        //
        // create class
        //
        System.out.println("\n*** Create class ");
        Log.printlnTransaction("\n*** Create class ");
        String propName1="theKey";      
        String propName2="count";
        String propName3="ratio";      
        String propName4="message";      
        int numProperties = 4;

        CIMClass cimClass = new CIMClass(className);

        try
        {
//            cimClass.setName(className);

            CIMQualifier cimQualifierKey = new CIMQualifier("key");
            cimQualifierKey.setValue(new CIMValue(new Boolean(true),
                    CIMDataType.getPredefinedType(CIMDataType.BOOLEAN)));

            CIMProperty cimProp1=new CIMProperty();
            cimProp1.setName(propName1);
            cimProp1.setType(CIMDataType.getPredefinedType(CIMDataType.STRING));
            cimProp1.addQualifier(cimQualifierKey);

            CIMProperty cimProp2=new CIMProperty();
            cimProp2.setName(propName2);
            cimProp2.setType(CIMDataType.getPredefinedType(CIMDataType.UINT32));

            CIMProperty cimProp3=new CIMProperty();
            cimProp3.setName(propName3);
            cimProp3.setType(CIMDataType.getPredefinedType(CIMDataType.UINT32));

            CIMProperty cimProp4=new CIMProperty();
            cimProp4.setName(propName4);
            cimProp4.setType(CIMDataType.getPredefinedType(CIMDataType.STRING));

            cimClass.addProperty(cimProp1);
            cimClass.addProperty(cimProp2);
            cimClass.addProperty(cimProp3);
            cimClass.addProperty(cimProp4);

            cc.createClass(cp, cimClass);
            Log.printlnTransaction("  class name: "+ cimClass.getName());
        } 
        catch (CIMException ce) 
        {
            System.err.println("\nFailed to create class: " + ce);
            System.exit(1);
        }
        catch (Exception e)
        {
        }


        //
        // verify that the class gets created
        //
        CIMClass modClass = null;
        try
        {
            localOnly = false;
            includeQualifiers = true;
            includeClassOrigin = false;
            modClass = cc.getClass(cp,
                                   localOnly, includeQualifiers, 
                                   includeClassOrigin, null);
        }
        catch (CIMException ce) 
        {
            System.err.println("\nFailed to get class: " + ce);
            System.exit(1);
        }
        
        //
        // modify class - remove a property
        //
        try
        {
            Log.printlnTransaction("\n*** Modify class ");
            System.out.println("\n*** Modify class ");

            // remove one of the properties
            Vector properties = modClass.getAllProperties();
            Vector newProperties=new Vector();
            if (properties != null)
            {
                int psize = properties.size();
                if (psize != 4)
                {
                    System.err.println("\nInvalid number of class properties " +
                                       "in the new class.");
                    System.exit(1);
                }
                for (int i = 0; i < psize; i++)
                {
                    CIMProperty prop=(CIMProperty) (properties.elementAt(i));
                    if (prop.getName().equalsIgnoreCase(propName2))
                    {
                        // skip this one
                    }
                    else
                    {
                        newProperties.addElement(prop);
                    }
                }
            }
            modClass.setProperties(newProperties);

            // modify the class
            cc.modifyClass(cp, modClass);
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to modify class: " + ce);
            System.exit(1);
        }

        //
        // verify that the class was modified correctly
        //
        CIMClass modifiedClass = null;
        try
        {
            localOnly = false;
            includeQualifiers = true;
            includeClassOrigin = false;
            modifiedClass=cc.getClass(cp, localOnly, 
                                      includeQualifiers, 
                                      includeClassOrigin, null);
        }
        catch (CIMException ce) 
        {
            System.err.println("\nFailed to get class: " + ce);
            System.exit(1);
        }
        
        Vector properties2 = modifiedClass.getAllProperties();
        if (properties2 == null)
        {
            System.err.println("\nNo CIM properties returned.");
            System.exit(1);
        }
        else
        {
            if (properties2.size() != 3)
            {
                System.err.println("\nInvalid number of class properties in " +
                                   "the modified class");
                System.exit(1);
            }
        }
    }

    private void TestInstanceGetOperations(CIMClient cc)
    {
        boolean localOnly;
        boolean includeQualifiers;
        boolean includeClassOrigin;

        //
        // enumerate instance names
        //
        System.out.println("\n++++ Test Instance Get Operations ++++");
        Log.printlnTransaction("\n++++ Test Instance Get Operations ++++");

        Log.printlnTransaction("\n*** enumerate instance names");
        System.out.println("\n*** enumerate instance names");
        CIMObjectPath pa3=new CIMObjectPath("CIM_UnitaryComputerSystem");
        try 
        {
            int numInstanceNames = 0;
            Enumeration en1=cc.enumerateInstanceNames(pa3);
            if (en1!=null) 
            {
                while (en1.hasMoreElements()) 
                {
                    CIMObjectPath obj=(CIMObjectPath)(en1.nextElement());
                    Log.printlnTransaction("Instance name: "+obj.toString());
                    numInstanceNames++;
                }
            }
            System.out.println(numInstanceNames + " instance names found");
            Log.printlnTransaction(numInstanceNames + " instance names found");
        } 
        catch (CIMException ce) 
        {
            System.err.println("\nFailed to enumerate instance names: " + ce);
            System.exit(1);
        } 
     
        //
        // enumerate instances
        //
        Log.printlnTransaction("\n*** Enumerate instances");
        System.out.println("\n*** Enumerate instances");
        CIMObjectPath pa4=new CIMObjectPath("CIM_UnitaryComputerSystem");
        try
        {
            localOnly = false;
            includeQualifiers = false;
            includeClassOrigin = false;
            Enumeration en1=cc.enumerateInstances(pa4, CIMClient.DEEP,
                                                  localOnly,
                                                  includeQualifiers,
                                                  includeClassOrigin, null);
            int numInstances = 0;
            if (en1!=null) 
            {
                while (en1.hasMoreElements()) 
                {
                    CIMInstance obj=(CIMInstance)(en1.nextElement());
                    Log.printlnTransaction("Instance: " + obj);
                    numInstances++;
                }
            }
            System.out.println(numInstances + " instances found");
            Log.printlnTransaction(numInstances + " instances found");
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to enumerate instances: "+ce);
            System.exit(1);
        }

        //
        // get instance
        //
        Log.printlnTransaction("\n*** Get instances of PG_UnixProcess");
        System.out.println("\n*** Get instances of PG_UnixProcess");
        try
        {
            CIMObjectPath pa7=new CIMObjectPath("PG_UnixProcess");
            Enumeration enum = cc.enumerateInstanceNames(pa7);

            int numInstances = 0;
            while (enum.hasMoreElements())
            {
                //
                // get value of a property for each instance
                //
                Object obj=enum.nextElement();
                CIMObjectPath currentInstancePath=(CIMObjectPath)obj;

                localOnly = false;
                includeQualifiers = false;
                includeClassOrigin = false;
                CIMInstance currentInstance=cc.getInstance(currentInstancePath,
                                                           localOnly,
                                                           includeQualifiers,
                                                           includeClassOrigin,
                                                           null);
                numInstances++;

                CIMProperty valProp = currentInstance.getProperty("Description");
                Log.printlnTransaction(" Property : "+ valProp);

                CIMValue value = valProp.getValue();

                if (value != null && value.getValue() != null)
                    Log.printlnTransaction(" Property value: " + value.toString());
                else
                    Log.printlnTransaction(" Property value is null");
            }
            System.out.println(numInstances + " instances found");
            Log.printlnTransaction(numInstances + " instances found");
        }
        catch (CIMException ce) 
        {
            if (ce.getID().equals(CIMException.CIM_ERR_NOT_FOUND))
            {
                System.err.println("\nUnable to get instance: " + ce);
            }
            else
            {
                System.err.println("\nFailed to get instances: " + ce);
                System.exit(1);
            }
        }
    }

    private void TestInstanceOperations(CIMClient cc, CIMNameSpace clientNameSpace)
    {
        System.out.println("\n++++ Test Instance Operations ++++");
        Log.printlnTransaction("\n++++ Test Instance Operations ++++");

        //
        // Create instance
        //
        // NOTE:  Pegasus cimom does not support createInstance without
        //        a provider.  If the repositoryIsDefaultInstanceProvider
        //        config property is not set.  It returns 
        //        CIM_ERR_NOT_SUPPORTED.
        //
        Log.printlnTransaction("\n*** create instance ");
        System.out.println("\n*** create instance ");
        String className = "MyTestClass";
        CIMInstance cimInstance = null;
        CIMObjectPath path = null;

        String message1 = "Hello";
        String message2 = "Hello world!";

        boolean localOnly;
        boolean includeQualifiers;
        boolean includeClassOrigin;
        boolean deepInheritance;

        try
        {
            // NOTE:  localOnly flag must be set to false, or else getClass
            //        will not work properly with SNIA cimom.
            //
            localOnly = false;
            includeQualifiers = true;
            includeClassOrigin = true;

            CIMClass cimClass = cc.getClass(new CIMObjectPath(className),
                                            localOnly,
                                            includeQualifiers,
                                            includeClassOrigin,
                                            null);
            cimInstance = cimClass.newInstance();
            Log.printlnTransaction("Instance = " + cimInstance);

            cimInstance.setProperty("theKey", new CIMValue("true",
                    CIMDataType.getPredefinedType(CIMDataType.STRING)));
            cimInstance.setProperty("ratio", new CIMValue(new UnsignedInt32(10),
                                    new CIMDataType(CIMDataType.UINT32)));
            cimInstance.setProperty("message", new CIMValue(message1));
            Log.printlnTransaction("Instance = " + cimInstance);

            // create instance
            path = new CIMObjectPath(className,
                                     cimInstance.getKeyValuePairs());
            Log.printlnTransaction("instance path = " + path);
            cc.createInstance(path, cimInstance);
        }
        catch (CIMException ce)
        {
            if (ce.getID().equals(CIMException.CIM_ERR_NOT_SUPPORTED))
            {
                System.err.println("\nUnable to create instance: " + ce);
            }
            else
            {
                System.err.println("\nFailed to create instance: " + ce);
                System.exit(1);
            }
        }
        catch (Exception e)
        {
	}


        //
        // Verify that the instance gets created
        //
        CIMInstance newInstance = null;
        try
        {
            localOnly = false;
            includeQualifiers = false;
            includeClassOrigin = false;

            newInstance=cc.getInstance(path,
                                       localOnly,
                                       includeQualifiers,
                                       includeClassOrigin,
                                       null);
            Log.printlnTransaction("getInstance() Instance = " + cimInstance);
        }
        catch (CIMException ce)
        {
            if (ce.getID().equals(CIMException.CIM_ERR_NOT_SUPPORTED))
            {
                System.err.println("\nUnable to get instance: " + ce);
            }
            else
            {
                System.err.println("\nFailed to get instance: " + ce);
                System.exit(1);
            }
        }
    
        //
        // modify instance
        //
        if (newInstance != null)
        {
            Log.printlnTransaction("\n*** Modify instance ");
            System.out.println("\n*** Modify instance ");
            try
            {
                newInstance.setProperty("message", new CIMValue(message2));
            Log.printlnTransaction("\n*** Modify instance xx");
                includeQualifiers = false;
                cc.modifyInstance(path, newInstance, includeQualifiers, null);
            }
            catch (CIMException ce) 
            {
                if (ce.getID().equals(CIMException.CIM_ERR_NOT_SUPPORTED))
                {
                    System.err.println("\nUnable to modify instance: " + ce);
                }
                else
                {
                    if (ce.getID().equals(CIMException.CIM_ERR_NOT_FOUND))
                    {
                        System.err.println("\nUnable to modify instance: " + ce);
                    }
                    else
                    {
                        System.err.println("\nFailed to modify instance: " + ce);
                        System.exit(1);
                    }
                }
            }

            //
            // Verify that the instance gets modified
            //
            try
            {
                localOnly = false;
                includeQualifiers = false;
                includeClassOrigin = false;
                CIMInstance  currentInstance=cc.getInstance(path,
                                                            localOnly,
                                                            includeQualifiers,
                                                            includeClassOrigin,
                                                            null);

                CIMProperty prop = currentInstance.getProperty("message");
                CIMValue value = prop.getValue();
                if (value.toString().equals(message2))
                {
                    Log.printlnTransaction("Instance modification successful.");
                }
                else
                {
                    System.err.println("\nInstance was not correctly modified.");
                    System.exit(1);
                }
            }
            catch (CIMException ce) 
            {
                System.err.println("\nFailed to get instance: " + ce);
                System.exit(1);
            }
        }   

        //
        // delete instance
        //
        Log.printlnTransaction("\n*** Delete instance ");
        System.out.println("\n*** Delete instance ");
        try
        {
            cc.deleteInstance(path);
        }
        catch (CIMException ce) 
        {
            if (ce.getID().equals(CIMException.CIM_ERR_NOT_SUPPORTED))
            {
                System.err.println("\nUnable to delete instance: " + ce);
            }
            else
            {
                System.err.println("\nFailed to delete instance: " + ce);
                System.exit(1);
            }
        }
    }

    // NOTE:  The following test requires the Sample_InstanceProviderClass
    //        and the sample instance provider.
    //
    private void TestInstanceModifyOperations(CIMClient cc,
                                              CIMNameSpace clientNameSpace)
    {
        System.out.println("\n++++ Test Instance Modify Operations ++++");
        Log.printlnTransaction("\n++++ Test Instance Modify Operations ++++");

        String nameSpace = "root/SampleProvider";
        String className = "Sample_InstanceProviderClass";

        CIMInstance cimInstance = null;
        CIMObjectPath classPath = null;
        CIMObjectPath instancePath = null;

        boolean localOnly;
        boolean includeQualifiers;
        boolean includeClassOrigin;

        //
        // Create instance
        //
        System.out.println("\n*** create instance ");
        Log.printlnTransaction("\n*** create instance ");

        try
        {
            // NOTE:  localOnly flag must be set to false, or else getClass
            //        will not work properly with SNIA cimom (will not return
            //        any properties).
            //
            classPath = new CIMObjectPath(className);
            classPath.setNameSpace(nameSpace);

            localOnly = false;
            includeQualifiers = false;
            includeClassOrigin = false;
            CIMClass cimClass = cc.getClass(classPath,
                                            localOnly, includeQualifiers,
                                            includeClassOrigin, null);
            cimInstance = cimClass.newInstance();
            Log.printlnTransaction("Instance = " + cimInstance);

            cimInstance.setProperty("Identifier", new CIMValue(new UnsignedInt8((short)55),
                                    new CIMDataType(CIMDataType.UINT8)));
            cimInstance.setProperty("Message", new CIMValue("Hello World"));

            instancePath = new CIMObjectPath(className,
                                     cimInstance.getKeyValuePairs());
            instancePath.setNameSpace(nameSpace);

            Log.printlnTransaction("instancePath = " + instancePath);

            try
            {
               cc.createInstance(instancePath, cimInstance);
	    }
            catch (CIMException ce)
	    {
	       if (ce.getID().equals(CIMException.CIM_ERR_ALREADY_EXISTS))
               cc.deleteInstance(instancePath);
               cc.createInstance(instancePath, cimInstance);
	    }
        }
        catch (CIMException ce)
        {
            if (ce.getID().equals(CIMException.CIM_ERR_NOT_SUPPORTED))
            {
                System.err.println("\nUnable to create instance: " + ce);
            }
            else
            {
                if (ce.getID().equals(CIMException.CIM_ERR_ALREADY_EXISTS))
                {
                    System.err.println("\nUnable to create instance: " + ce);
                }
                else
                {
                    System.err.println("\nFailed to create instance: " + ce);
                    System.exit(1);
                }
            }
        }
        catch (Exception e)
        {
	}

/*  ATTN:  Something may be wrong with the provider?  Instance is not 
           coming back. 
        //
        // Verify that the instance gets created
        //
        CIMInstance newInstance = null;
        try
        {
            localOnly = false;
            includeQualifiers = false;
            includeClassOrigin = false;

            newInstance=cc.getInstance(instancePath,
                                       localOnly,
                                       includeQualifiers,
                                       includeClassOrigin,
                                       null);
        }
        catch (CIMException ce) 
        {
            if (ce.getID().equals(CIMException.CIM_ERR_NOT_SUPPORTED))
            {
                System.err.println("\nUnable to get instance: " + ce);
            }
            else
            {
                System.err.println("\nFailed to get instance: " + ce);
                System.exit(1);
            }
        }
*/
    }

    private void TestQualifierOperations(CIMClient cc, CIMNameSpace clientNameSpace)
    {
        System.out.println("\n++++ Test Qualifier Operations ++++");
        Log.printlnTransaction("\n++++ Test Qualifier Operations ++++");

        //
        // enumerateQualifiers
        //
        Log.printlnTransaction("\n*** enumerate Qualifiers ");
        System.out.println("\n*** enumerate Qualifiers ");
        CIMObjectPath pa1=new CIMObjectPath((String)null);

        try
        {
            Enumeration en1=cc.enumerateQualifiers(pa1);
            if (en1!=null)
            {
                while (en1.hasMoreElements())
                {
                    CIMQualifierType obj=(CIMQualifierType)(en1.nextElement());
                    Log.printlnTransaction("  qualifier definition: " +
                                           obj.getName());
                }
            }
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to enumerate qualifiers: " + ce);
            System.exit(1);
        }

        //
        // enumerateQualifiers (on the "Association" qualifier)
        //
        System.out.println("\n*** Enumerate Qualifier on association");
        Log.printlnTransaction("\n*** Enumerate Qualifier on association");
        try
        {
            CIMObjectPath pa9 = new CIMObjectPath("Association");
            Enumeration enumQl = cc.enumerateQualifiers(pa9);
            while (enumQl.hasMoreElements())
            {
                CIMQualifierType qt =(CIMQualifierType)(enumQl.nextElement());
                if (qt !=null)
                {
//                    Log.printlnTransaction((String)(qt.toXml()));
                }
            }
        }
 //       catch (MalformedURLException me) { }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to enumerate qualifier " +
                               "on association: " + ce);
            System.exit(1);
        }

        CIMQualifierType qt = new CIMQualifierType();

        //
        // create qualifier type
        //
        String qualifierName = "myQualifier";

	try {
          qt.setName(qualifierName);
           qt.setDefaultValue(new CIMValue(new Boolean(true),
                CIMDataType.getPredefinedType(CIMDataType.BOOLEAN)));
           qt.setType(CIMDataType.getPredefinedType(CIMDataType.BOOLEAN));

           CIMFlavor cf=CIMFlavor.getFlavor(CIMFlavor.ENABLEOVERRIDE);
           qt.addFlavor(cf);

           CIMScope cs=CIMScope.getScope(CIMScope.CLASS);
           qt.addScope(cs);
	}
        catch (Exception e)
        {
	}

        //
        // delete qualifier if it exists
        //
        // NOTE:  Pegasus has a bug in handling deleteQualifier requests.  It
        //        returns "Bad Request".  Waiting for a fix.
        //
        System.out.println("\n*** Delete qualifier ");
        Log.printlnTransaction("\n*** Delete qualifier ");
        try
        {
            CIMObjectPath path = new CIMObjectPath(qualifierName,
                                     clientNameSpace.getNameSpace());
            cc.deleteQualifier(path);
        }
        catch (CIMException ce)
        {
           System.err.println("\nUnable to delete qualifier: " + ce);
           //System.exit(1);
        }
        System.out.println("\n*** Delete qualifier done");

        //
        // create qualifier (setQualifier)
        //
        Log.printlnTransaction("\n*** Set Qualifier ");
        System.out.println("\n*** Set Qualifier ");

        try
        {
            CIMObjectPath path = new CIMObjectPath((String)null,clientNameSpace.getNameSpace());
            cc.setQualifier(path, qt);
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to create qualifier:" + ce);
            //System.exit(1);
        }

        //
        // get qualifier
        //
        // NOTE:  Pegasus has a bug in handling getQualifier requests.  It
        //        returns "Bad Request".  Waiting for a fix.
        //
        Log.printlnTransaction("\n*** Get Qualifier ");
        System.out.println("\n*** Get Qualifier ");

        try
        {
            CIMObjectPath path = new CIMObjectPath(qualifierName,
                                     clientNameSpace.getNameSpace());
            CIMQualifierType qtype = cc.getQualifier(path);
            Log.printlnTransaction("Loaded qualifier: " + qtype.getName());
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to get qualifier:" + ce);
            //System.exit(1);
        }

        //
        // get qualifiers using CIMClass method
        //
        System.out.println("\n*** getQualifiers on CIM_Process class using CIMClass method");
        Log.printlnTransaction("\n*** getQualifiers on CIM_Process class using CIMClass method");
        try
        {
            CIMObjectPath pa10 = new CIMObjectPath("CIM_Process");
            boolean localOnly = false;
            boolean includeQualifiers = true;
            boolean includeClassOrigin = false;

            CIMClass  cclass = cc.getClass(pa10, localOnly, includeQualifiers,
                                           includeClassOrigin, null);
            Vector qualifiers=  cclass.getQualifiers();     // CIMQualifier
            for(int i=0;i<qualifiers.size();i++)
            {
                CIMQualifier qtype= (CIMQualifier)qualifiers.elementAt(i);
                Log.printlnTransaction("Loaded qualifier:"+qtype.getName());
            }
        }
        catch (CIMException ce)
        {
            if (ce.getID().equals(CIMException.CIM_ERR_NOT_FOUND))
            {
                System.err.println("\nUnable to get qualifiers: " + ce);
            }
            else
            {
                System.err.println("\nFailed to get qualifier: " + ce);
                System.exit(1);
            }
        }
    }

    private void TestDeprecatedMethods(CIMClient cc, CIMNameSpace clientNameSpace)
    {
        System.out.println("\n++++ Test Deprecated Methods ++++");
        Log.printlnTransaction("\n++++ Test Deprecated Methods ++++");

        boolean localOnly = true;;
        boolean deepInheritance = true;;
        String className = "MyTestClass";

        // enumNameSpace
        //
        Log.printlnTransaction("\n*** enumNameSpace");
        try
        {
            CIMObjectPath pa1=new CIMObjectPath("root", (Vector)null);
            Enumeration elist1 = cc.enumNameSpace(pa1, deepInheritance);
            if (elist1!=null)
            {
                while (elist1.hasMoreElements())
                {
                    String nsString=(String)(elist1.nextElement());
                    Log.printlnTransaction("  Name space: " + nsString);
                }
            }
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to enumerate namespaces: " + ce);
            System.exit(1);
        }

        // getClass
        //
        Log.printlnTransaction("\n*** getClass");
        CIMClass cl = null;
        CIMObjectPath cp = null;

        try
        {
            cp = new CIMObjectPath(className, clientNameSpace.getNameSpace());
            localOnly = false;
            cl = cc.getClass(cp, localOnly);
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to get class: " + ce);
            System.exit(1);
        }

        // setClass
        //
        Log.printlnTransaction("\n*** setClass");
        Vector properties = cl.getAllProperties();
        Vector newProperties=new Vector();
        if (properties == null)
        {
            System.err.println("\nNo CIM properties returned.");
            System.exit(1);
        }

        int psize = properties.size();
        for (int i = 0; i < psize; i++)
        {
            CIMProperty prop=(CIMProperty) (properties.elementAt(i));
            if (prop.getName().equalsIgnoreCase("ratio"))
            {
                // skip this one
            }
            else
            {
                newProperties.addElement(prop);
            }
            cl.setProperties(newProperties);

            try
            {
                // modify the class
                cc.setClass(cp, cl);
            }
            catch (CIMException ce) 
            {
                System.err.println("\nFailed to set class: " + ce);
                System.exit(1);
            }
        }

        //
        // verify that the class was modified correctly
        //
        CIMClass modifiedClass = null;
        try
        {
            localOnly = false;
            modifiedClass=cc.getClass(cp, localOnly);
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to get the modified class: " + ce);
            System.exit(1);
        }

        Vector properties2 = modifiedClass.getAllProperties();
        if (properties2 == null)
        {
            System.err.println("\nNo CIM properties returned.");
            System.exit(1);
        }
        else
        {
            if (properties2.size() != 2)
            {
                System.err.println("\nInvalid number of class properties in " +
                                   "the modified class");
                System.exit(1);
            }
        }

        // enumClass
        //
        try
        {
            Log.printlnTransaction("\n*** Enumerate classes - DEEP");

            deepInheritance = true;
            localOnly = true;
            CIMObjectPath pa3=new CIMObjectPath("CIM_ComputerSystem");

            Enumeration en2=cc.enumClass(pa3, deepInheritance, localOnly);
            if (en2!=null)
            {
                while (en2.hasMoreElements())
                {
                    CIMClass cimclass=(CIMClass)(en2.nextElement());
                    Log.printlnTransaction("  class name: " + cimclass.getName());
                }
            }
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to enumerate classes: " + ce);
            System.exit(1);
        }

        // enumClass
        //
        Log.printlnTransaction("\n*** Enumerate class names ");
        CIMObjectPath pa2=new CIMObjectPath((String)null);
        try
        {
            Enumeration en1=cc.enumClass(pa2, deepInheritance);
            if (en1!=null)
            {
                while (en1.hasMoreElements())
                {
                    CIMObjectPath obj=(CIMObjectPath)(en1.nextElement());
                    Log.printlnTransaction("  class name: "+obj);
                }
            }
        }
        catch (CIMException ce)
        {
            System.err.println("\nFailed to enumerate class names: " + ce);
            System.exit(1);
        }

        //
        // setInstance  (first create a new instance, then call setInstance
        //               to modify it)
        //
        // NOTE:  Pegasus cimom does not support instance operations without
        //        a provider.  If the repositoryIsDefaultInstanceProvider
        //        config property is not set, CIM_ERR_NOT_SUPPORTED exception
        //        will be returned.
        //
        Log.printlnTransaction("\n*** set instance ");
        CIMInstance cimInstance = null;
        CIMObjectPath path = null;

        String message1 = "Hello";
        String message2 = "Hello world!";

        try
        {
            // NOTE:  localOnly flag must be set to false, or else getClass
            //        will not work properly with SNIA cimom.
            //
            localOnly = false;
            deepInheritance = false;

            CIMClass cimClass = cc.getClass(new CIMObjectPath(className),
                                            localOnly);
            cimInstance = cimClass.newInstance();
            Log.printlnTransaction("Instance = " + cimInstance);

            cimInstance.setProperty("theKey", new CIMValue("true",
                    CIMDataType.getPredefinedType(CIMDataType.STRING)));
            //cimInstance.setProperty("ratio", new CIMValue(new Integer(10),
            //                        new CIMDataType(CIMDataType.UINT32)));
            cimInstance.setProperty("message", new CIMValue(message1));

            // create the instance
            path = new CIMObjectPath(className,
                                     cimInstance.getKeyValuePairs());
            Log.printlnTransaction("instance path = " + path);
            cc.createInstance(path, cimInstance);
        }
        catch (CIMException ce)
        {
            if (ce.getID().equals(CIMException.CIM_ERR_NOT_SUPPORTED))
            {
                System.err.println("\nUnable to create instance: " + ce);
            }
            else
            {
                System.err.println("\nFailed to create instance: " + ce);
                System.exit(1);
            }
        }
        catch (Exception e)
        {
	}

        // get the new instance
        CIMInstance newInstance = null;
        try
        {
            localOnly = false;
            newInstance=cc.getInstance(path, localOnly);
        }
        catch (CIMException ce)
        {
            if (ce.getID().equals(CIMException.CIM_ERR_NOT_SUPPORTED))
            {
                System.err.println("\nUnable to get instance: " + ce);
            }
            else
            {
                System.err.println("\nFailed to get instance: " + ce);
                System.exit(1);
            }
        }
    
        // modify the instance
        if (newInstance != null)
        {
            try
            {
                newInstance.setProperty("message", new CIMValue(message2));
                cc.setInstance(path, newInstance);
            }
            catch (CIMException ce) 
            {
                if (ce.getID().equals(CIMException.CIM_ERR_NOT_SUPPORTED))
                {
                    System.err.println("\nUnable to set instance: " + ce);
                }
                else
                {
                    if (ce.getID().equals(CIMException.CIM_ERR_NOT_FOUND))
                    {
                        System.err.println("\nUnable to set instance: " + ce);
                    }
                    else
                    {
                        System.err.println("\nFailed to set instance: " + ce);
                        System.exit(1);
                    }
                }
            }

            //
            // Verify that the instance gets modified
            //
            try
            {
                localOnly = false;
                CIMInstance  currentInstance=cc.getInstance(path, localOnly);
                CIMProperty prop = currentInstance.getProperty("message");
                CIMValue value = prop.getValue();
                if (value.toString().equals(message2))
                {
                    Log.printlnTransaction("Instance modification successful.");
                }
                else
                {
                    System.err.println("\nInstance was not correctly modified.");
                    System.exit(1);
                }
            }
            catch (CIMException ce) 
            {
                System.err.println("\nFailed to get instance: " + ce);
                System.exit(1);
            }
        }   

        //
        // enumInstances - enumerate instances
        //
        Log.printlnTransaction("\n*** enumInstances - enumerate instances ");
        CIMObjectPath pa4=new CIMObjectPath("MyTestClass");
        try 
        {
            localOnly = false;
            Enumeration en1=cc.enumInstances(pa4, CIMClient.DEEP, localOnly);
            if (en1!=null)
            {
                while (en1.hasMoreElements()) 
                {
                    CIMInstance obj=(CIMInstance)(en1.nextElement());
                    Log.printlnTransaction("Instance: " + obj);
                }
            }
        } 
        catch (CIMException ce) 
        {
            if (ce.getID().equals(CIMException.CIM_ERR_NOT_SUPPORTED))
            {
                System.err.println("\nUnable to enumerate instances: " + ce);
            }
            else
            {
                System.err.println("\nFailed to enumerate instances: "+ce);
                System.exit(1);
            }
        } 

        // enumInstances - enumerate instance names
        //
       Log.printlnTransaction("\n*** enumInstances - enumerate instance names");
        CIMObjectPath pa3=new CIMObjectPath("MyTestClass");
        try 
        {
            Enumeration en1=cc.enumInstances(pa3, CIMClient.DEEP);
            if (en1!=null) 
            {
                while (en1.hasMoreElements()) 
                {
                    CIMObjectPath obj=(CIMObjectPath)(en1.nextElement());
                    Log.printlnTransaction("Instance name: "+obj.toString());
                }
            }
        } 
        catch (CIMException ce)
        {
            if (ce.getID().equals(CIMException.CIM_ERR_NOT_SUPPORTED))
            {
                System.err.println("\nUnable to enumerate instance names: " + ce);
            }
            else
            {
                System.err.println("\nFailed to enumerate instance names: " + ce);
                System.exit(1);
            }
        } 

        // delete the instance
        Log.printlnTransaction("\n*** Delete instance ");
        try
        {
            cc.deleteInstance(path);
        }
        catch (CIMException ce) 
        {
            if (ce.getID().equals(CIMException.CIM_ERR_NOT_SUPPORTED))
            {
                System.err.println("\nUnable to delete instance: " + ce);
            }
            else
            {
                System.err.println("\nFailed to delete instance: " + ce);
                System.exit(1);
            }
        }

        //
        // enumQualifierTypes
        //
        Log.printlnTransaction("\n*** enumQualifierTypes");
        CIMObjectPath pa1=new CIMObjectPath((String)null);
        try 
        {
            Enumeration en1=cc.enumQualifierTypes(pa1);
            if (en1!=null) 
            {
                while (en1.hasMoreElements()) 
                {
                    CIMQualifierType obj=(CIMQualifierType)(en1.nextElement());
                    Log.printlnTransaction("  qualifier definition:"+obj.getName());
                }
            }
        }
        catch (CIMException ce) 
        {
            System.err.println("\nFailed to enumerate qualifier types: " + ce);
            System.exit(1);
        }

        // getQualifierType
        // setQualifierType
        // createQualifierType
        // deleteQualifierType

    }

    private void TestAssociations(CIMClient cc)
    {
        //
        // Note at the moment associators requires a path to an instance and
        // won't operate on a class (and its unclear what a class would return).
        //
        CIMObjectPath paen1 = new CIMObjectPath("Association");

        Log.printlnTransaction("\n*** Test associatorNames() on Association");
        try 
        {
            //Enumeration en1=cc.associatorNames(paen1, "PG_ComputerSystem", 
            //                                   null, null, null);
            Enumeration en1=cc.associatorNames(paen1, null, null, null, null);
            Log.printlnTransaction("Fetched associator names --");
            if (en1!=null) 
            {
                while (en1.hasMoreElements()) 
                {
                    CIMObjectPath obj=(CIMObjectPath)(en1.nextElement());
                    Log.printlnTransaction("Associated path:"+obj.toString());
                }
            }
        }
        catch (CIMException ce) 
        {
            System.out.println("Failed to get associator names: " + ce);
            //System.exit(1);
        }

        //
        // return associated objects
        //
        Log.printlnTransaction("\n*** Test associators() objects on CIM_System");
        try 
        {
            //Enumeration en2=cc.associators(paen1, "PG_ComputerSystem", null,
            //                                  null, null, true, true, null);
            Enumeration en2=cc.associators(paen1, null, null,
                                          null, null, true, true, null);

            Log.printlnTransaction("Fetched associated objects --");
            if (en2!=null) 
            {
                while (en2.hasMoreElements()) 
                {
                    CIMInstance obj=(CIMInstance)(en2.nextElement());
                    Log.printlnTransaction("Associated object: "+ obj.toString());
                }
            }
        } 
        catch (CIMException ce) 
        {
            System.err.println("\nFailed to get associated objects:"+ce);
            //System.exit(1);
        }
    }

    private void TestReferences(CIMClient cc)
    {
        //
        // return Referencenames
        //
        Log.printlnTransaction("\n*** Test Referencenames() ");

        CIMObjectPath paen1 = new CIMObjectPath("Association");

        try 
        {

            Enumeration en3=cc.referenceNames(paen1, null, null);
            Log.printlnTransaction("Fetched reference names");
            if (en3!=null) 
            {
                while (en3.hasMoreElements()) 
                {
                    CIMObjectPath obj=(CIMObjectPath)(en3.nextElement());
                    Log.printlnTransaction("reference name:"+obj.toString());
                }
            }
        }
        catch (CIMException ce) 
        {
            System.err.println("\nFailed to get reference names: " + ce);
            System.exit(1);
        }

        //
        // return references Objects
        //
        Log.printlnTransaction("\n**** Test references() ");
        try 
        {
            Enumeration en4=cc.references(paen1, null, null,true, true, null);
            Log.printlnTransaction("Fetched reference objects");
            if (en4!=null) 
            {
                while (en4.hasMoreElements()) 
                {
                    CIMInstance obj=(CIMInstance)(en4.nextElement());
                    Log.printlnTransaction("reference object: " + obj.toString());
                }
            }
        }
        catch (CIMException ce) 
        {
            System.err.println("\nFailed to get reference names: " + ce);
            System.exit(1);
        }
    }

    // NOTE:  The following test requires the Sample_MethodProviderClass
    //        and the sample method provider.
    //
    private void TestInvokeMethod(CIMClient cc)
    {
        System.out.println("\n*** Test invokeMethod() ");
        Log.printlnTransaction("\n*** Test invokeMethod() ");

        String testNameSpace = "root/SampleProvider";
        String testClassName = "Sample_MethodProviderClass";
        String methodName = "SayHello";
        String inParamValue = "Yoda";
        String goodReply = "Hello, " + inParamValue + "!";
        String goodParam = "From Neverland";

        CIMObjectPath currentPath;
        Vector inParams = new Vector();
        Vector outParams = new Vector();
        String reply;
        CIMValue retValue;

        try 
        {
            //
            // construct CIMObjectPath
            //
            currentPath = new CIMObjectPath(testClassName);
            currentPath.setNameSpace(testNameSpace);
         
            inParams.addElement( new CIMProperty("Name", 
	        new CIMValue(inParamValue,
	            CIMDataType.getPredefinedType(CIMDataType.STRING))));

            retValue=cc.invokeMethod(currentPath, methodName, inParams,
				     outParams);

            if (retValue==null)
                reply="No Return Value";
            else
                reply=retValue.toString();

            Log.printlnTransaction("reply = " + reply);
            Log.printlnTransaction("goodReply = " + goodReply);

            if (reply.equalsIgnoreCase(goodReply))
            {
                if (outParams.size() == 0)
                {
                    System.err.println("\nOutput parameter missing from the " +
                                       "InvokeMethod response.");
                }

                Log.printlnTransaction("Number of output parameters = " +
                                       outParams.size());

                if (outParams.size() > 1)
                {
                    System.err.println("\nInvalid number of output parameters " +
                                       "returned from InvokeMethod.");
                }

                CIMProperty prop = (CIMProperty)outParams.elementAt(0);
                CIMValue value = prop.getValue();
                String outReply = value.toString();

                Log.printlnTransaction(" Property value: " + outReply);

                if (outReply.equalsIgnoreCase(goodParam))
                {
                    Log.printlnTransaction("Finished executing " + methodName +
	    	    	                   "\nOutput: "+ reply);
                }
                else
                {
                    System.err.println("\nInvokeMethod() Failed:  " +
                                       "Bad output parameter: " + outReply);
                    System.exit(1);
                }
            }
            else
            {
                System.err.println("\nInvokeMethod() Failed:  Bad reply.");
                System.exit(1);
            }
        }
        catch (CIMException ce)
        {
            System.err.println("\nInvokeMethod() Failed: " + ce);
            System.err.println(
               "\nNOTE:  This test uses the Sample_MethodProviderClass. "+
               "It requires the sample method provider to be loaded and registered.");

	    System.exit(1);
        }
        catch (Exception e)
        {
	}
    }

    public void runTests(String args[])
    {
        CIMNameSpace clientNameSpace = null;
        String hostName = "localhost";          // host for cimom
        String nameSpace = "root/cimv2";        // namespace
        CIMClient cc=null;
        int portNo=CIMNameSpace.DEFAULT_PORT;
        String hostURL;
        String isSSL=null;
                
        if (args.length>=3) {
            hostName=args[0];
            nameSpace=args[1];
            portNo=Integer.parseInt(args[2]);
        } else 
        if (args.length!=0) {
            System.out.println("TestClient [hostname namespace port [ssl] ]");
            Log.printlnTransaction("TestClient [hostname namespace port [ssl] ]");
            System.exit(1);
        }
        if (args.length==4 && args[3].equalsIgnoreCase("ssl") )
        {
            isSSL=args[3];
        }
        if ( isSSL != null )
            hostURL="https://"+hostName+":"+portNo;
        else
            hostURL="http://"+hostName+":"+portNo;
        
        System.out.println("Connecting to "+hostURL+
                           " for namespace "+nameSpace);
        Log.printlnTransaction("Connecting to "+hostURL+
                               " for namespace "+nameSpace);
        try 
        {
            clientNameSpace = new CIMNameSpace(hostURL, nameSpace);  
            cc = new CIMClient(clientNameSpace, "guest", "guest", CIMClient.XML);
        } catch (CIMException e) {
            System.err.println("\nFailed to connect to CIMOM: " + e);
            System.exit(1);
        }
        
        System.out.println("\nCIM client created.");
        Log.printlnTransaction("\nCIM client created.");

        TestNameSpaceOperations(cc);

        TestClassOperations(cc, clientNameSpace);

        TestClassModifyOperations(cc, clientNameSpace);

        TestInstanceGetOperations(cc);

        TestInstanceOperations(cc, clientNameSpace);

        TestInvokeMethod(cc);

        TestInstanceModifyOperations(cc, clientNameSpace);

        TestQualifierOperations(cc, clientNameSpace);

        TestDeprecatedMethods(cc, clientNameSpace);

        //TestAssociations(cc);
        //TestReferences(cc);
    }

    public TestCIMClient() 
    { 
    }

///////////////////////////////////////////////////////////////
//    Main
///////////////////////////////////////////////////////////////

   public static void main(String args[])
   {
        System.setProperty("org.snia.wbem.cimom.properties", "./cim.properties");

        String logFileName = "/tmp/javaclienttest.log";
//        File logFile = new File(logFileName);
//        logFile.delete();

//        Log.assignLogFileName(logFileName);
        Log.startTransaction();

        TestCIMClient ea=new TestCIMClient();
        ea.runTests(args);

        System.out.println("\n++++ Java TestClient Terminated Normally.");
        Log.printlnTransaction("\n++++ Java TestClient Terminated Normally.");
        System.exit(0);
   }
}
