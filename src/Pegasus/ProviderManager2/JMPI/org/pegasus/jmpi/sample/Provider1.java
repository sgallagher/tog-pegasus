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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


package org.pegasus.providers;

import java.util.Vector;
import java.util.Enumeration;
import org.pegasus.jmpi.InstanceProvider;
import org.pegasus.jmpi.CIMObjectPath;
import org.pegasus.jmpi.CIMClass;
import org.pegasus.jmpi.CIMInstance;
import org.pegasus.jmpi.CIMException;
import org.pegasus.jmpi.CIMValue;
import org.pegasus.jmpi.CIMProperty;
import org.pegasus.jmpi.CIMOMHandle;

public class Provider1 implements InstanceProvider   {
    static int count=0;
    static int[] arrayValues={1,2};
    static String myClassName = "EXP_UnitaryComputerSystem";
    private CIMClass myClass = null;
    protected CIMOMHandle handle = null;
    
    public void initialize(CIMOMHandle ch) throws CIMException {
    	handle = ch;
	Enumeration enum=ch.enumClass( new CIMObjectPath(null,"root"),true);
	while (enum.hasMoreElements()) {
	    CIMObjectPath cop=(CIMObjectPath)(enum.nextElement());
	    System.err.println("Loaded class name:"+cop);
	}
	buildClass();
    }

    private void buildClass() throws CIMException {
	if (myClass!=null)
	    return;
	CIMObjectPath findClass= new CIMObjectPath(myClassName,"root");
	myClass=handle.getClass(findClass,false);
    }


    public void cleanup() throws CIMException {
    }						    
					      
    
    public Vector enumInstances(CIMObjectPath op,
				boolean deep,CIMClass cc) throws CIMException {
	buildClass();
	Vector test=new Vector();

	if (!deep && !(cc.getName().equals(myClassName)))
	    return(test);

System.err.println("Deep = "+deep);
	CIMInstance ci=myClass.newInstance(); 
	ci.setName("example");
	ci.setProperty("Name",new CIMValue("Test"+count));
	ci.setProperty("CreationClassName",new CIMValue(myClassName));
	CIMObjectPath cop=new CIMObjectPath(myClassName,ci.getKeyValuePairs());
	test.addElement(cop);
	return(test);
    }
			     
    public Vector enumInstances(CIMObjectPath op,
					  boolean deep,
					  CIMClass cc,
					 boolean localOnly) throws CIMException {
	buildClass();
	Vector test=new Vector();
	if (!deep && !(cc.getName().equals(myClassName)))
	    return(test);
System.err.println("Deep = "+deep);
	CIMInstance ci=myClass.newInstance(); 
	ci.setName("example");
	ci.setProperty("Name",new CIMValue("Test"+count));
	ci.setProperty("CreationClassName",new CIMValue(myClassName));
	ci.setProperty("example",new CIMValue(arrayValues));
	test.addElement(ci);
	return(test);
    }

 
 
    public CIMInstance getInstance(CIMObjectPath op,
					 CIMClass cc,
					     boolean localOnly) throws CIMException {
	buildClass();
	CIMInstance ci=myClass.newInstance();
	ci.setProperty("Name",new CIMValue("Test"+count));
	ci.setProperty("CreationClassName",new CIMValue(myClassName));
	ci.setProperty("PrimaryOwnerContact",new CIMValue("Set by provider 1"));
	ci.setProperty("LastLoadInfo",new CIMValue("Set by provider 1"));
	ci.setProperty("example",new CIMValue(arrayValues));

return(ci);
    }

	public CIMObjectPath createInstance(CIMObjectPath op,CIMInstance ci) throws CIMException {return(op);}
    public void setInstance(CIMObjectPath op,CIMInstance ci) throws CIMException  {}
    public void deleteInstance(CIMObjectPath op) throws CIMException {}
    public Vector execQuery(CIMObjectPath op,String queryStatement,
				     int ql, CIMClass cimClass) throws CIMException {
	return(null);
    }
}
