//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
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
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Repository/CIMRepository.h>
#include <Pegasus/CQL/RepositoryQueryContext.h>


PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;



void drive_operation()
{

   // Uint64 tests
   CQLValue a1(Uint64(10));
   CQLValue a2(Real64(15));
   CQLValue a3(Uint64(25));
   CQLValue a4(Uint64(30));
   CQLValue a5(Uint64(150));

   assert(a3 == a1 + a2);
   assert(a4 != a1 + a2);

   assert(a2 == a3 - a1);
   assert(a4 != a3 - a1);

   assert(a5 == a1 * a2);
   assert(a4 != a1 * a2);

   assert(a2 == a5 / a1);
   assert(a4 != a5 / a1);

   assert(a1 >= (a3 - a2));
   assert(a1 >= (a2 - a2));
   assert(a1 >  (a2 - a2));

   assert(a1 <= (a3 - a2));
   assert(a1 <= (a4 - a2));
   assert(a1 <  (a4 - a2));

   // Sint64 tests
   CQLValue b1(Sint64(10));
   CQLValue b2(Sint64(15));
   CQLValue b3(Sint64(25));
   CQLValue b4(Sint64(30));
   CQLValue b5(Sint64(150));

   assert(b3 == b1 + b2);
   assert(b4 != b1 + b2);

   assert(b2 == b3 - b1);
   assert(b4 != b3 - b1);

   assert(b5 == b1 * b2);
   assert(b4 != b1 * b2);

   assert(b2 == b5 / b1);
   assert(b4 != b5 / b1);

   assert(b1 >= (b3 - b2));
   assert(b1 >= (b2 - b2));
   assert(b1 >  (b2 - b2));

   assert(b1 <= (b3 - b2));
   assert(b1 <= (b4 - b2));
   assert(b1 <  (b4 - b2));

   // Real64 tests
   CQLValue c1(Real64(10.00));
   CQLValue c2(Real64(15.00));
   CQLValue c3(Real64(25.00));
   CQLValue c4(Real64(30.00));
   CQLValue c5(Real64(150.00));

   assert(c3 == c1 + c2);
   assert(c4 != c1 + c2);

   assert(c2 == c3 - c1);
   assert(c4 != c3 - c1);

   assert(c5 == c1 * c2);
   assert(c4 != c1 * c2);

   assert(c2 == c5 / c1);
   assert(c4 != c5 / c1);

   assert(c1 >= (c3 - c2));
   assert(c1 >= (c2 - c2));
   assert(c1 >  (c2 - c2));

   assert(c1 <= (c3 - c2));
   assert(c1 <= (c4 - c2));
   assert(c1 <  (c4 - c2));

   // Test mix numbers
   assert(a3 == b1 + c2);
   assert(b4 != c1 + a2);

   assert(c2 == a3 - b1);
   assert(a4 != b3 - c1);

   assert(b5 == c1 * a2);
   assert(c4 != a1 * b2);

   assert(a2 == b5 / c1);
   assert(b4 != c5 / a1);

   assert(c1 >= (a3 - b2));
   assert(a1 >= (b2 - c2));
   assert(b1 >  (c2 - a2));

   assert(c1 <= (a3 - b2));
   assert(a1 <= (b4 - c2));
   assert(b1 <  (c4 - a2));

   // String tests
   CQLValue d1(String("HELLO"));
   CQLValue d2(String("HEL"));
   CQLValue d3(String("LO"));
   CQLValue d4(String("AHELLO"));
   CQLValue d5(String("ZHELLO"));

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

   CQLValue e1( str1, Hex);
   CQLValue e2( str2, Decimal);
   CQLValue e3( str3, Binary);
   CQLValue e4( str4, Real);

   CQLValue e5(Uint64(16));
   CQLValue e6(Uint64(10));
   CQLValue e7(Uint64(2));
   CQLValue e8(Real64(10.10));

   assert(e1 == e5);
   assert(e2 == e6);
   assert(e3 == e7);
   assert(e4 == e8);

   return;
}

void drive_get_misc_functions()
{
   try
   {
  
      const char* env = getenv("PEGASUS_HOME");
   	String repositoryDir(env);
      repositoryDir.append("/repository");	
	   CIMNamespaceName _ns("root/cimv2");
	   CIMRepository *_rep = new CIMRepository(repositoryDir);
	   RepositoryQueryContext _query(_ns, _rep);

      // Get function tests
      CQLValue a1(Uint64(123));
      CQLValue a2(Sint64(-123));
      CQLValue a3(Real64(25.24));
      CQLValue a4(String("Hellow"));
      CQLValue a5(Boolean(true));

      String _date("20040811105625.000000-360");
      CIMDateTime date(_date);
      CQLValue a6(date);

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
      assert(a7.getReference() == 
            CIMObjectPath(opStr));
      assert(a8.getInstance().identical(_i1));
      assert(a9.getClass().identical(
               _query.getClass(CIMName("CIM_OperatingSystem"))));
      
      a1.invert();   
      a2.invert(); 
      a3.invert(); 
      a4.invert(); 
      a5.invert(); 
      a6.invert(); 
      a7.invert(); 
      a8.invert(); 
      a9.invert(); 


      assert(a1.getUint() == Uint64(123));
      assert(a2.getSint() == Sint64(123));            // -123 before invert
      assert(a3.getReal() == Real64(-25.24));         // 25.24 before invert
      assert(a4.getString() == String("Hellow"));
      assert(a5.getBool() == Boolean(false));         // true before invert
      assert(a6.getDateTime() == CIMDateTime(_date));
      assert(a7.getReference() == CIMObjectPath(opStr));
      assert(a8.getInstance().identical(_i1));
      assert(a9.getClass().identical(
               _query.getClass(CIMName("CIM_OperatingSystem"))));

      assert(a1.toString() == String("123"));
      assert(a2.toString() == String("123"));
      assert(a3.toString() == String("-2.524000e+01"));
      assert(a4.toString() == String("Hellow"));
      assert(a5.toString() == String("FALSE"));
      assert(a6.toString() == _date);
      assert(a7.toString() == String("MyClass.x=\"Hello World\",y=1234,z=TRUE"));
      assert(a8.toString() == String("CIM_OperatingSystem"));
      assert(a9.toString() == String("CIM_OperatingSystem"));




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
   return;
}


void drive_resolve_primitive()
{
   try
   {
   const char* env = getenv("PEGASUS_HOME");
	String repositoryDir(env);
	repositoryDir.append("/repository");	
	CIMNamespaceName _ns("root/cimv2");
	CIMRepository *_rep = new CIMRepository(repositoryDir);
	RepositoryQueryContext _query(_ns, _rep);
   RepositoryQueryContext _query1(_ns, _rep);

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

   CQLChainedIdentifier ci1(String("Description"));
   CQLChainedIdentifier 
         ci2(String("CIM_OperatingSystem.CIM_OperatingSystem::EnabledState"));
   CQLChainedIdentifier ci3(String("CIM_OperatingSystem.CurrentTimeZone"));
   CQLChainedIdentifier ci4(String("CIM_OperatingSystem.TimeOfLastStateChange"));

   CQLChainedIdentifier 
         ci5(String(
         "CIM_OperatingSystem.CIM_EnabledLogicalElement::TimeOfLastStateChange"));


   
   CQLChainedIdentifier 
         ci7(String("CIM_OperatingSystem"));


   CQLChainedIdentifier 
         ci9(String(
         "CIM_EnabledLogicalElement.CIM_OperatingSystem::CSCreationClassName"));

   CQLChainedIdentifier 
         ci10(String("CIM_OperatingSystem::EnabledState"));

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



   try
   {
      a10.resolve(_i1, _query1);
      assert(0);
   }
   catch(...)
   {
      assert(1);
   }


   try
   {
      a9.resolve(_i1, _query);
      assert(0);
   }
   catch(...)
   {
      assert(1);
   }

   assert(a1 == CQLValue(String("Dave Rules")));
   assert(a2 == CQLValue(Uint64(2)));
   assert(a3 == CQLValue(Sint64(-600)));
   assert(a4 == CQLValue(CIMDateTime(String("20040811105625.000000-360"))));
   assert(a5 == CQLValue(CIMDateTime(String("20040811105625.000000-360"))));
   assert(a7 == CQLValue(_i1));

   }
   catch(Exception & e)
   {
      cout << e.getMessage() << endl;
      assert(0);
   }
   return;
}

void drive_resolve_specialChars()
{
   try
   {
      const char* env = getenv("PEGASUS_HOME");
   	String repositoryDir(env);
   	repositoryDir.append("/repository");	
   	CIMNamespaceName _ns("root/cimv2");
   	CIMRepository *_rep = new CIMRepository(repositoryDir);
   	RepositoryQueryContext _query(_ns, _rep);
      RepositoryQueryContext _query1(_ns, _rep);
   
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

      CQLChainedIdentifier ci6(String("CIM_OperatingSystem.OperationalStatus[2]"));
      CQLChainedIdentifier ci7(String("CIM_OperatingSystem.*"));

      CQLValue a1(ci1);
      CQLValue a2(ci2);
      CQLValue a3(ci3);

      CQLValue a5(ci5);
      CQLValue a6(ci6);
      CQLValue a7(ci7);

      a1.resolve(_i1, _query);
      a2.resolve(_i1, _query);
      a3.resolve(_i1, _query);

      a6.resolve(_i1, _query);
      a7.resolve(_i1, _query);

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
      assert(a2.getValueType() == CQLIgnore_type);
      assert(a3 == CQLValue(String("Degraded")));

      assert(a6 == CQLValue(Uint64(2)));
      assert(a7 == CQLValue(_i1));
      
   }
   catch(Exception & e)
   {
      cout << e.getMessage() << endl;
      assert(0);
   }
   return;
}


void drive_resolve_embedded()
{
   try
   {
      const char* env = getenv("PEGASUS_HOME");
   	String repositoryDir(env);
   	repositoryDir.append("/repository");	
   	CIMNamespaceName _ns("root/cimv2");
   	CIMRepository *_rep = new CIMRepository(repositoryDir);
   	RepositoryQueryContext _query(_ns, _rep);
   
   }
   catch(Exception & e)
   {
      cout << e.getMessage() << endl;
   }
   return;
}

void drive_like_function()
{
   try
   {
      const char* env = getenv("PEGASUS_HOME");
   	String repositoryDir(env);
   	repositoryDir.append("/repository");	
   	CIMNamespaceName _ns("root/cimv2");
   	CIMRepository *_rep = new CIMRepository(repositoryDir);
   	RepositoryQueryContext _query(_ns, _rep);
   
   }
   catch(Exception & e)
   {
      cout << e.getMessage() << endl;
   }
   return;
}

void drive_isa_function()
{
   try
   {
      const char* env = getenv("PEGASUS_HOME");
   	String repositoryDir(env);
   	repositoryDir.append("/repository");	
   	CIMNamespaceName _ns("root/cimv2");
   	CIMRepository *_rep = new CIMRepository(repositoryDir);
   	RepositoryQueryContext _query(_ns, _rep);

      String s1("CIM_ComputerSystem");
      String s2("CIM_System");
      String s3("CIM_OperatingSystem");
      String s4("CIM_ManagedElement");

      CIMName n1(s1);
      CIMName n2(s2);
      CIMName n3(s3);
      CIMName n4(s4);

      CIMInstance i1(n1);
      CIMInstance i2(n2);
      CIMInstance i3(n3);
      CIMInstance i4(n4);

      CQLValue vs1(s1);
      CQLValue vs2(s2);
      CQLValue vs3(s3);
      CQLValue vs4(s4);

      CQLValue vi1(i1);
      CQLValue vi2(i2);
      CQLValue vi3(i3);
      CQLValue vi4(i4);

      assert(vi1.isa(vs1,_query));
      assert(vi1.isa(vs2,_query));
      assert(!vi1.isa(vs3,_query));
      assert(vi1.isa(vs4,_query));

      assert(!vi2.isa(vs1,_query));
      assert(vi2.isa(vs2,_query));
      assert(!vi2.isa(vs3,_query));
      assert(vi2.isa(vs4,_query));

      assert(!vi3.isa(vs1,_query));
      assert(!vi3.isa(vs2,_query));
      assert(vi3.isa(vs3,_query));
      assert(vi3.isa(vs4,_query));

      assert(!vi4.isa(vs1,_query));
      assert(!vi4.isa(vs2,_query));
      assert(!vi4.isa(vs3,_query));
      assert(vi4.isa(vs4,_query));
      
   
   }
   catch(Exception & e)
   {
      cout << e.getMessage() << endl;
   }
   return;
}

int main( int argc, char *argv[] ){

   //BEGIN TESTS....
cout << "operation" << endl;
	drive_operation();
 cout << "misc" << endl;  
   drive_get_misc_functions();
 cout << "isa" << endl;
   drive_isa_function();
 cout << "like" << endl;
   drive_like_function();
 cout << "primitive" << endl;
   drive_resolve_primitive();
 cout << "special" << endl;
   drive_resolve_specialChars();
 cout << "embedded" << endl;
   drive_resolve_embedded();

	//END TESTS....
	                                                                                                                   
        cout << argv[0] << " +++++ passed all tests" << endl;
                                                                                                                                       
        return 0;
}

