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
// Authors: David Rosckes (rosckes@us.ibm.com)
//          Bert Rivero (hurivero@us.ibm.com)
//          Chuck Carmack (carmack@us.ibm.com)
//          Brian Lucier (lucier@us.ibm.com) 
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////


#include <cstdlib>
#include <iostream>
#include <cassert>
 
#include <Pegasus/Common/CIMObjectPath.h>  
#include <Pegasus/Common/CIMDateTime.h>

#include <Pegasus/Common/String.h>
#include <Pegasus/Common/Array.h>
#include <Pegasus/CQL/CQLValue.h>
#include <Pegasus/CQL/CQLValueRep.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/Repository/RepositoryQueryContext.h>


PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;



void drive_operation()
{

   // Uint64 tests
   
   CQLValue a1(Uint64(10));
   CQLValue a2(Uint64(15));
   CQLValue a3(Uint64(25));
   CQLValue a4(Uint64(30));
   CQLValue a5(Uint64(150));

   assert(a1 != a2);
   assert(a5 == a5);
   assert(a1 < a2);
   assert(a2 >= a1);
   assert(a1 <= a2);
   assert(a2 > a1);
 
   // Sint64 tests
   
   CQLValue b1(Sint64(10));
   CQLValue b2(Sint64(15));
   CQLValue b3(Sint64(25));
   CQLValue b4(Sint64(30));
   CQLValue b5(Sint64(150));

   assert(b1 != b2);
   assert(b5 == b5);
   assert(b1 < b2);
   assert(b2 >= b1);
   assert(b1 <= b2);
   assert(b2 > b1);
  
   // Real64 tests

   CQLValue c1(Real64(10.00));
   CQLValue c2(Real64(15.00));
   CQLValue c3(Real64(25.00));
   CQLValue c4(Real64(30.00));
   CQLValue c5(Real64(150.00));

   assert(c1 != c2);
   assert(c5 == c5);
   assert(c1 < c2);
   assert(c2 >= c1);
   assert(c1 <= c2);
   assert(c2 > c1);

   // Misc
   assert(a1 == b1);
   assert(a1 == c1);
   assert(b1 == a1);
   assert(b1 == c1);
   assert(c1 == a1);
   assert(c1 == b1);

   assert(a2 != b1);
   assert(a2 != c1);
   assert(b2 != a1);
   assert(b2 != c1);
   assert(c2 != a1);
   assert(c2 != b1);

   assert(a2 >= b1);
   assert(a2 >= c1);
   assert(b2 >= a1);
   assert(b2 >= c1);
   assert(c2 >= a1);
   assert(c2 >= b1);

   assert(a2 <= b3);
   assert(a2 <= c3);
   assert(b2 <= a3);
   assert(b2 <= c3);
   assert(c2 <= a3);
   assert(c2 <= b3);

   assert(a2 > b1);
   assert(a2 > c1);
   assert(b2 > a1);
   assert(b2 > c1);
   assert(c2 > a1);
   assert(c2 > b1);

   assert(a2 < b3);
   assert(a2 < c3);
   assert(b2 < a3);
   assert(b2 < c3);
   assert(c2 < a3);
   assert(c2 < b3);

   //Overflow testing
   CQLValue real1(Real64(0.00000001));
   CQLValue sint1(Sint64(-1));
   CQLValue uint1(Sint64(1));
   CQLValue uint2(Uint64(0));

   assert(uint1 > sint1);
   assert(real1 > sint1);
   assert(uint2 > sint1);
   assert(real1 > uint2);

   CQLValue real2(Real64(25.00000000000001));
   CQLValue real3(Real64(24.99999999999999));
   CQLValue sint2(Sint64(25));
   CQLValue uint3(Uint64(25));

   assert(real2 > real3);
   assert(real2 > sint2);
   assert(real2 > uint3);
   assert(real3 < sint2);
   assert(real3 < uint3);
 
   // String tests

   CQLValue d1(String("HELLO"));
   CQLValue d2(String("HEL"));
   CQLValue d3(String("LO"));
   CQLValue d4(String("AHELLO"));
   CQLValue d5(String("ZHELLO"));

   String tmp1 =  d1.getString();
   CQLValue tmp3 = d2+d3;
   String tmp2 = tmp3.getString();

   // cout  << tmp1 << endl;
   //cout << tmp2 << endl;

   assert(d1 == d2 + d3);
   assert(d1 != d2 + d4);

   assert(d1 <= d5);
   assert(d1 <  d5);

   assert(d1 >= d4);
   assert(d1 >  d4);

   String str1("0x10");
   String str2("10");
   String str3("10B");
   String str4("10.10");

   
   CQLValue e1( str1, CQLValue::Hex);
   CQLValue e2( str2, CQLValue::Decimal);
   CQLValue e3( str3, CQLValue::Binary);
   CQLValue e4( str4, CQLValue::Real);

   CQLValue e5(Uint64(16));
   CQLValue e6(Uint64(10));
   CQLValue e7(Uint64(2));
   CQLValue e8(Real64(10.10));

   assert(e1 == e5);
   assert(e2 == e6);
   assert(e3 == e7);
   assert(e4 == e8);

   Array<Uint64> array1;

   array1.append(1);
   array1.append(2);
   array1.append(3);
   array1.append(4);
   array1.append(5);
   array1.append(6);
   array1.append(7);
   array1.append(8);
   array1.append(9);
   array1.append(10);

 Array<Sint64> array2;

   array2.append(1);
   array2.append(2);
   array2.append(3);
   array2.append(4);
   array2.append(5);
   array2.append(6);
   array2.append(7);
   array2.append(8);
   array2.append(9);
   array2.append(10);
   array2.append(3);

 Array<Real64> array3;

   array3.append(1.00);
   array3.append(2.00);
   array3.append(3.00);
   array3.append(9.00);
   array3.append(10.00);
   array3.append(3.00);
   array3.append(4.00);
   array3.append(5.00);
   array3.append(6.00);
   array3.append(7.00);
   array3.append(8.00);

   Array<Uint64> array4;

   array4.append(1);
   array4.append(23);
   array4.append(3);
   array4.append(4);
   array4.append(5);
   array4.append(6);
   array4.append(7);
   array4.append(88);
   array4.append(9);
   array4.append(10);

 Array<Sint64> array5;

   array5.append(-1);
   array5.append(2);
   array5.append(3);
   array5.append(4);
   array5.append(5);
   array5.append(-6);
   array5.append(7);
   array5.append(8);
   array5.append(9);
   array5.append(10);
   array5.append(-3);

 Array<Real64> array6;

   array6.append(1.23);
   array6.append(2.00);
   array6.append(3.00);
   array6.append(9.00);
   array6.append(10.00);
   array6.append(3.00);
   array6.append(4.14);
   array6.append(5.00);
   array6.append(6.00);
   array6.append(7.00);
   array6.append(8.00);

   CIMValue cv1(array1);
   CIMValue cv2(array2);
   CIMValue cv3(array3);
   CIMValue cv4(array4);
   CIMValue cv5(array5);
   CIMValue cv6(array6);
 
   CQLValue vr1(cv1);
   CQLValue vr2(cv1);
   CQLValue vr3(cv2);
   CQLValue vr4(cv3);
   CQLValue vr5(cv4);
   CQLValue vr6(cv5);
   CQLValue vr7(cv6);
 
   assert(vr1 == vr2);
   assert(vr1 == vr3);
   assert(vr1 == vr4);
   assert(vr4 == vr3);

   assert(vr1 != vr5);
   assert(vr3 != vr6);
   assert(vr4 != vr7);

   const CIMName _cimName(String("CIM_OperatingSystem"));

   CIMInstance _i1(_cimName);
   CIMProperty _p1(CIMName("Description"),CIMValue(String("Dave Rules")));
   CIMProperty _p2(CIMName("EnabledState"),CIMValue(Uint16(2)));
   CIMProperty _p3(CIMName("CurrentTimeZone"),CIMValue(Sint16(-600)));
   CIMProperty _p4(CIMName("TimeOfLastStateChange"),
                  CIMValue(CIMDateTime(String("20040811105625.000000-360"))));
                                    
   _i1.addProperty(_p1);
   _i1.addProperty(_p2);
   _i1.addProperty(_p3);
   _i1.addProperty(_p4);

   CIMInstance _i2(_cimName);
   CIMProperty _p5(CIMName("Description"),CIMValue(String("Dave Rules Everything")));
   CIMProperty _p6(CIMName("EnabledState"),CIMValue(Uint16(2)));
   CIMProperty _p7(CIMName("CurrentTimeZone"),CIMValue(Sint16(-600)));
   CIMProperty _p8(CIMName("TimeOfLastStateChange"),
                  CIMValue(CIMDateTime(String("20040811105625.000000-360"))));
                                    
   _i2.addProperty(_p5);
   _i2.addProperty(_p6);
   _i2.addProperty(_p7);
   _i2.addProperty(_p8);

   CQLValue cql1(_i1);
   CQLValue cql2(_i1);
   CQLValue cql3(_i2);
   CQLValue cql4(_i2);

   //assert(cql1 == cql1);

   return;
}

void drive_get_misc_functions()
{
   
   
  
      const char* env = getenv("PEGASUS_HOME");
   	String repositoryDir(env);
		repositoryDir.append("/repository");
      //String repositoryDir("c:/pegasus-cvs/pegasus/repository");	
	   CIMNamespaceName _ns("root/cimv2");
	   CIMRepository *_rep = new CIMRepository(repositoryDir);
	   RepositoryQueryContext _query(_ns, _rep);
try{
      // Get function tests
      CQLValue a1(Uint64(123));
      CQLValue a2(Sint64(-123));
      CQLValue a3(Real64(25.24));
      CQLValue a4(String("Hellow"));
      CQLValue a5(Boolean(true));

      String _date("20040811105625.000000-360");
      CIMDateTime date(_date);
      CQLValue a6(date);
      String _date1("20040811105626.000000-360");
      CIMDateTime date1(_date1);
      CQLValue a61(date1);

      String opStr("MyClass.z=true,y=1234,x=\"Hello World\"");
      CIMObjectPath op(opStr);
      CQLValue a7(op);

      const CIMName _cimName(String("CIM_OperatingSystem"));
      CIMInstance _i1(_cimName);

      CQLValue a8(_i1);

      CQLValue a9(_query.getClass(CIMName("CIM_OperatingSystem")));

      assert(a1.getUint() == Uint64(123));
      assert(a2.getSint() == Sint64(-123));
      assert(a3.getReal() == Real64(25.24));
      assert(a4.getString() == String("Hellow"));
      assert(a5.getBool() == Boolean(true));
      assert(a6.getDateTime() == CIMDateTime(_date));
      assert(a6 != a61);
      assert(a6 < a61);
      assert(a7.getReference() == 
            CIMObjectPath(opStr));

      try
      {
         a1.getSint();
         assert(0);
      }
      catch(...)
      {
         assert(1);
      }
   }
   catch(Exception & e)
   {
      cout << e.getMessage() << endl;
      assert(0);
   }
   delete _rep;
   return;
}


void drive_resolve_primitive()
{
   
   
   const char* env = getenv("PEGASUS_HOME");
	String repositoryDir(env);
	repositoryDir.append("/repository");
	//String repositoryDir("c:/pegasus-cvs/pegasus/repository");	
	CIMNamespaceName _ns("root/cimv2");
	CIMRepository *_rep = new CIMRepository(repositoryDir);
	RepositoryQueryContext _query(_ns, _rep);
   RepositoryQueryContext _query1(_ns, _rep);
try{
   const CQLIdentifier _Id1(String("CIM_OperatingSystem"));

   _query.insertClassPath(_Id1);

   const CIMName _cimName(String("CIM_OperatingSystem"));

   CIMInstance _i1(_cimName);
   CIMProperty _p1(CIMName("Description"),CIMValue(String("Dave Rules")));
   CIMProperty _p2(CIMName("EnabledState"),CIMValue(Uint16(2)));
   CIMProperty _p3(CIMName("CurrentTimeZone"),CIMValue(Sint16(-600)));
   CIMProperty _p4(CIMName("TimeOfLastStateChange"),
                  CIMValue(CIMDateTime(String("20040811105625.000000-360"))));
                                    
   _i1.addProperty(_p1);
   _i1.addProperty(_p2);
   _i1.addProperty(_p3);
   _i1.addProperty(_p4);

   CQLChainedIdentifier ci1(String("CIM_OperatingSystem.CIM_OperatingSystem::Description"));
   CQLChainedIdentifier 
         ci2(String("CIM_OperatingSystem.CIM_OperatingSystem::EnabledState"));
   CQLChainedIdentifier ci3(String("CIM_OperatingSystem.CIM_OperatingSystem::CurrentTimeZone"));
   CQLChainedIdentifier ci4(String("CIM_OperatingSystem.CIM_OperatingSystem::TimeOfLastStateChange"));

   CQLChainedIdentifier 
         ci5(String(
         "CIM_OperatingSystem.CIM_EnabledLogicalElement::TimeOfLastStateChange"));


   
   CQLChainedIdentifier 
         ci7(String("CIM_OperatingSystem"));


   CQLChainedIdentifier 
         ci9(String(
         "CIM_EnabledLogicalElement.CIM_OperatingSystem::CSCreationClassName"));

   CQLChainedIdentifier 
         ci10(String("CIM_OperatingSystem.CIM_OperatingSystem::Bubba"));

   CQLValue a1(ci1); 
   CQLValue a2(ci2);
   CQLValue a3(ci3);
   CQLValue a4(ci4);
   CQLValue a5(ci5);

   CQLValue a7(ci7);

   CQLValue a9(ci9);
   CQLValue a10(ci10);

      a1.resolve(_i1, _query);
      a2.resolve(_i1, _query);
      a3.resolve(_i1, _query);
      a4.resolve(_i1, _query);
      a5.resolve(_i1, _query);
      a7.resolve(_i1, _query);
      a10.resolve(_i1, _query1);




      a9.resolve(_i1, _query);



   assert(a1 == CQLValue(String("Dave Rules")));
   assert(a2 == CQLValue(Uint64(2)));
   assert(a3 == CQLValue(Sint64(-600)));
   assert(a4 == CQLValue(CIMDateTime(String("20040811105625.000000-360"))));
   assert(a5 == CQLValue(CIMDateTime(String("20040811105625.000000-360"))));
   //assert(a7 == CQLValue(_i1));
   assert(a9.getValueType() == CQLValue::Null_type);
   assert(a10.getValueType() == CQLValue::Null_type);

   }
   catch(Exception & e)
   {
      cout << e.getMessage() << endl;
      assert(0);
   }
   delete _rep;
   return;
}

void drive_resolve_specialChars()
{
   
   
      const char* env = getenv("PEGASUS_HOME");
		String repositoryDir(env);
      repositoryDir.append("/repository");
   	//String repositoryDir("c:/pegasus-cvs/pegasus/repository");	
   	CIMNamespaceName _ns("root/cimv2");
   	CIMRepository *_rep = new CIMRepository(repositoryDir);
   	RepositoryQueryContext _query(_ns, _rep);
      RepositoryQueryContext _query1(_ns, _rep);
   try{
      const CQLIdentifier _Id1(String("CIM_OperatingSystem"));
   
      _query.insertClassPath(_Id1);
   
      const CIMName _cimName(String("CIM_OperatingSystem"));
   
      CIMInstance _i1(_cimName);
      CIMProperty _p1(CIMName("OSType"),CIMValue(Uint16(11)));
      CIMProperty _p2(CIMName("Status"),CIMValue(String("Degraded")));
      Array<Uint16> array16;
      array16.append(Uint16(0));
      array16.append(Uint16(1));
      array16.append(Uint16(2));
      array16.append(Uint16(3));
      CIMProperty _p3(CIMName("OperationalStatus"),CIMValue(array16));
                                       
      _i1.addProperty(_p1);
      _i1.addProperty(_p2);
      _i1.addProperty(_p3);
   
      CQLChainedIdentifier ci1(String("CIM_OperatingSystem.OSType#OS400"));
      CQLChainedIdentifier ci2(String("CIM_OperatingSystem.OSType#LINUX"));
      CQLChainedIdentifier ci3(String("CIM_OperatingSystem.Status#Degraded"));

      CQLChainedIdentifier ci5(String("CIM_OperatingSystem.Status#BOGUS"));

      CQLChainedIdentifier ci6(String("CIM_OperatingSystem.CIM_OperatingSystem::OperationalStatus[2]"));
      CQLValue a1(ci1);
      CQLValue a2(ci2);
      CQLValue a3(ci3);

      CQLValue a5(ci5);
      CQLValue a6(ci6);

      
      a1.resolve(_i1, _query);
      a2.resolve(_i1, _query);

      a6.resolve(_i1, _query);

      try
	{
	  a3.resolve(_i1, _query);
	  assert(0);
	}
      catch(...)
	{
	  assert(1);
	}

      try
      {
         a5.resolve(_i1, _query);
         assert(0);
      }
      catch(...)
      {
         assert(1);
      }

      assert(a1 == CQLValue(Uint64(11)));
      assert(a2 == CQLValue(Uint64(36)));


      assert(a6 == CQLValue(Uint64(2)));

      
   }
   catch(Exception & e)
   {
      cout << e.getMessage() << endl;
      assert(0);
   }
   delete _rep;
   return;
}





int main( int argc, char *argv[] ){

try
  {
   //BEGIN TESTS....
cout << "operation" << endl;
	drive_operation();
       
 cout << "misc" << endl;  
   drive_get_misc_functions();
  
 cout << "primitive" << endl;
   drive_resolve_primitive();
   
 cout << "special" << endl;
   drive_resolve_specialChars();
   
	
	//END TESTS....
	     
        cout << argv[0] << " +++++ passed all tests" << endl;
  }
catch(Exception e)
  {
    cout << e.getMessage() << endl;
    assert(0);
  } 
 return 0;
}

