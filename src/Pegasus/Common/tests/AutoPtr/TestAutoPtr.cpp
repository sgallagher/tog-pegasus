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
// Author:  Amit K Arora (amita@in.ibm.com) - Bug#2179
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <Pegasus/Common/AutoPtr.h>
#include <Pegasus/Common/Exception.h>

PEGASUS_USING_STD;
PEGASUS_USING_PEGASUS;

PEGASUS_NAMESPACE_BEGIN

class TestClass
{
public:
    TestClass()
    {
      staticVar  = 0;
    }

    TestClass(int i)
    {
      staticVar = i;
    }

    ~TestClass()
    {
      staticVar = -1;
    }

    void set(int i)
    {
       staticVar = i;
    }

    static int get(void)
    {
        return staticVar;
    }

private:
    static int staticVar;
};


int TestClass::staticVar=0;

void TestDefaultConstructor()
{
  AutoPtr<TestClass> autoP;
  AutoArrayPtr<TestClass> autoAP;
  
  if(autoP.get() != NULL || autoAP.get() != NULL)
  {
     Exception e("TestDefaultConstructor() failed.");
     throw e;
  }
}

void TestConstructorWithPtrArg()
{
  TestClass* tcPtr, *tcAPtr;

  tcPtr = new TestClass(); // This should set 'staticVar' to "0"
  tcAPtr = new TestClass[4]; // This should set 'staticVar' to "0"

  AutoPtr<TestClass> autoP(tcPtr); 
  AutoArrayPtr<TestClass> autoAP(tcAPtr); 

  if(autoP.get() == tcPtr && autoAP.get() == tcAPtr) 
       return;  // if true, test passed.
 
  // If we are here ... its a failure !
  Exception e("TestConstructorWithPtrArg() failed.");  
  throw e;

}

void TestDestructor()
{
  TestClass* tcPtr, *tcAPtr;
  Exception e("TestDestructor() failed.");  

  tcPtr = new TestClass();
  tcAPtr = new TestClass[5];

  // TEST AutoPtr //

  // Create a scope for AutoPtr object. 
  // The destructor of AutoPtr would be called at the end of the scope,
  // which in turn SHOULD call the destructor of TestClass.
  {
    AutoPtr<TestClass> autoP(tcPtr);
    autoP.get()->set(1);
  }

  // Destructor of AutoPtr should have deleted the memory pointed by
  // 'tc', and thus destructor of TestClass should have set the 'staticVar'
  // to "-1". Lets Confirm it ...

  if(TestClass::get() != -1) throw e;


  // TEST AutoArrayPtr //
  // Create a scope for AutoArrayPtr object. 
  {
    AutoArrayPtr<TestClass> autoAP(tcAPtr);
    autoAP.get()->set(1);
  }

  // Destructor of AutoArrayPtr should have deleted the memory pointed by
  // 'tc', and thus destructor of TestClass should have set the 'staticVar'
  // to "-1". Lets Confirm it ...

  if(TestClass::get() == -1) return;


  // If we are here ... its an error !
  throw e;
}


void TestCopyConstructor()
{
   int testVal=100;
   Exception e("TestCopyConstructor() failed.");

   // TEST AutoPtr //
   AutoPtr<TestClass> autoP1(new TestClass(testVal));
   AutoPtr<TestClass> autoP2(autoP1);
   if(autoP1.get() != NULL) throw e;
   if(autoP2.get()->get() != testVal) throw e;

   // TEST AutoArrayPtr //
   AutoArrayPtr<TestClass> autoAP1(new TestClass[4]);
   for(int i=0;i<4;i++)
   {
    ((TestClass *)autoAP1.get() + i)->set(testVal);
   }

   AutoArrayPtr<TestClass> autoAP2(autoAP1);
   if(autoAP1.get() != NULL) throw e;
   for(int i=0;i<4;i++)
   {
    if(((TestClass *)autoAP1.get() + i)->get() != testVal) throw e;
   }

   // If we are here, the test passed.
   return;
}

void TestEqualsOperator()
{
   int testVal=200;
   Exception e("TestEqualsOperator() failed.");

   // TEST AutoPtr //
   AutoPtr<TestClass> autoP1(new TestClass(testVal));
   AutoPtr<TestClass> autoP2;

   autoP2 = autoP1;

   if(autoP1.get() != NULL) throw e;
   if(autoP2.get()->get() != testVal) throw e;

   // TEST AutoArrayPtr //
   AutoArrayPtr<TestClass> autoAP1(new TestClass[4]);
   for(int i=0;i<4;i++)
   {
     ((TestClass *)autoAP1.get() + i)->set(testVal);
   }

   AutoArrayPtr<TestClass> autoAP2;

   autoAP2 = autoAP1;

   if(autoAP1.get() != NULL) throw e;
   for(int i=0;i<4;i++)
   {
       if(((TestClass *)autoAP1.get() + i)->get() != testVal) throw e;
   }

   // If we are here, the test passed.
   return; 
}

void TestArrowOperator()   // Test "->" Operator
{
   int testVal=300;
   Exception e("TestArrowOperator() failed.");

   // TEST AutoPtr //
   AutoPtr<TestClass> autoP(new TestClass(testVal));
   if(autoP->get() != testVal) throw e;

   // TEST AutoArrayPtr //
   AutoArrayPtr<TestClass> autoAP(new TestClass[3]);
   if(autoAP->get() != 0) throw e;
   // Test passed if we are here ..
   return;
}


void TestOtherMethods()
{
   int testVal=400;

   AutoPtr<TestClass> autoP(new TestClass(testVal));
   AutoArrayPtr<TestClass> autoAP(new TestClass[3]);
   for(int i=0; i<4; i++)
   {
     ((TestClass *)autoAP.get() + i)->set(testVal);
   }
   
   // TestGetMethod 
   if(autoP.get()->get() != testVal)
   {
      Exception e("TestGetMethod failed in TestOtherMethods().");
      throw e;
   }

   for(int i=0; i<4; i++)
   {
     if(((TestClass *)autoAP.get() + i)->get() != testVal)
     {
       Exception e("TestGetMethod failed in TestOtherMethods().");
       throw e;
     }
   }


   // TestReleaseMethod 
   TestClass* tcPtr = autoP.release();
   TestClass* tcAPtr = autoAP.release();
   delete tcPtr;
   delete [] tcAPtr;

   if(autoP.get() != NULL || autoAP.get() != NULL)
   {
      Exception e("TestReleaseMethod failed in TestOtherMethods().");
      throw e;
   }

   // TestResetMethod 
   tcPtr = new TestClass(testVal);
   tcAPtr = new TestClass[3];

   for(int i=0; i<4; i++)
   {
     (tcAPtr + i)->set(testVal);
   }


   autoP.reset(tcPtr);
   autoAP.reset(tcAPtr);
   if(autoP.get()->get() != testVal)
   {
      Exception e("TestResetMethod failed in TestOtherMethods().");
      throw e;
   }

   for(int i=0; i<4; i++)
   {
     if(((TestClass *)autoAP.get() + i)->get() != testVal)
     {
       Exception e("TestGetMethod failed in TestOtherMethods().");
       throw e;
     }
   }
   
   autoP.reset();
   autoAP.reset();

   if(TestClass::get() != -1 || autoP.get() != NULL || autoAP.get() != NULL)
   {
      Exception e("TestResetMethod failed in TestOtherMethods().");
      throw e;
   }
}

PEGASUS_NAMESPACE_END


// main function
int main(int argc, char** argv)
{

 try
 {

  TestDefaultConstructor();

  TestConstructorWithPtrArg();

  TestDestructor();

  TestCopyConstructor();

  TestEqualsOperator();

  TestArrowOperator();  // "->" operator

  TestOtherMethods();

 }
 catch(Exception &e)
 {
    cout << "AutoPtrTest: " << e.getMessage() << endl;
    exit(1);
 }
 catch(...)
 {
    cout << "AutoPtrTest Failed." << endl;
    exit(1);
 }

cout << argv[0] << " +++++ passed all tests" << endl;

exit(0);
}

