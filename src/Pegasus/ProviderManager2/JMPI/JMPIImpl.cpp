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
// Author:      Adrian Schuur, schuur@de.ibm.com
//
// Modified By: Adrian Dutta
//              Andy Viciu
//              Magda Vacarelu
//
//%/////////////////////////////////////////////////////////////////////////////


#include "JMPIImpl.h"

#include <dlfcn.h>
#include <iostream>

#include <Pegasus/Common/Config.h>
#include <Pegasus/Common/System.h>
#include <Pegasus/Common/CIMClass.h>
#include <Pegasus/Common/CIMInstance.h>
#include <Pegasus/Common/CIMObjectPath.h>
#include <Pegasus/Common/CIMProperty.h>
#include <Pegasus/Common/CIMType.h>
#include <Pegasus/Common/OperationContext.h>
#include <Pegasus/Provider/CIMOMHandle.h>
#include <Pegasus/Client/CIMClient.h>
#include <Pegasus/ProviderManager2/JMPI/JMPIProviderManager.h>

PEGASUS_USING_STD;
PEGASUS_NAMESPACE_BEGIN

JavaVM *JMPIjvm::jvm=NULL;
JvmVector JMPIjvm::jv;
int JMPIjvm::trace=0;

typedef struct {
  int clsIndex;
  const char * methodName;
  const char * signature;
} METHOD_STRUCT;

const char* classNames[]={
      "java/util/Vector" , // 0
      "java/lang/Boolean", // 1
      "java/lang/Byte",    // 2
      "java/lang/Short",   // 3
      "java/lang/Integer", // 4
      "java/lang/Long",    // 5
      "java/lang/Float",   // 6
      "java/lang/Double",  // 7
      "org/pegasus/jmpi/UnsignedInt8",    // 8
      "org/pegasus/jmpi/UnsignedInt16",   // 9
      "org/pegasus/jmpi/UnsignedInt32",   // 10
      "org/pegasus/jmpi/UnsignedInt64",   // 11
      "org/pegasus/jmpi/CIMObjectPath",   // 12
      "org/pegasus/jmpi/CIMException",    // 13
      "java/math/BigInteger",             // 14
      "org/pegasus/jmpi/CIMProperty",     // 15

      "org/pegasus/jmpi/CIMOMHandle",     // 16
      "org/pegasus/jmpi/CIMClass",        // 17
      "org/pegasus/jmpi/CIMInstance",     // 18
      "org/pegasus/jmpi/CIMValue",        // 19
      "java/lang/Object",                 // 20
      "java/lang/Throwable",              // 21
      "java/lang/String",                 // 22
      "org/pegasus/jmpi/JarClassLoader",  // 23
      "org/pegasus/jmpi/CIMDateTime",     // 24
      "org/pegasus/jmpi/SelectExp",       // 25
};


const METHOD_STRUCT instanceMethodNames[]={
/*00*/ { 0, "<init>",        "()V" },
/*01*/ { 1, "<init>",        "(Z)V" },
/*02*/ { 2, "<init>",        "(B)V" },
/*03*/ { 3, "<init>",        "(S)V" },
/*04*/ { 4, "<init>",        "(I)V" },
/*05*/ { 5, "<init>",        "(J)V" },    // ???
/*06*/ { 6, "<init>",        "(F)V" },
/*07*/ { 7, "<init>",        "(D)V" },
/*08*/ { 8, "<init>",        "(S)V" },
/*09*/ { 9, "<init>",        "(I)V" },
/*10*/ { 10,"<init>",        "(J)V" },
/*11*/ { 11,"<init>",        "(Ljava/math/BigInteger;)V" },
/*12*/ { 12,"<init>",        "(I)V" },
/*13*/ { 13,"<init>",        "(Ljava/lang/String;)V" },
/*14*/ { 15,"<init>",        "(I)V" },
/*15*/ { 0, "addElement",    "(Ljava/lang/Object;)V" },

/*16*/ { 0, "elementAt",     "(I)Ljava/lang/Object;" },
/*17*/ { 16,"<init>",        "(ILjava/lang/String;)V" },
/*18*/ { 13,"<init>",        "(I)V" },
/*19*/ { 17,"<init>",        "(I)V" },
/*20*/ { 18,"<init>",        "(I)V" },
/*21*/ { 12,"<init>",        "(I)V" },
/*22*/ { 12,"cInst",         "()I" },
/*23*/ { 18,"cInst",         "()I" },
/*24*/ { 17,"cInst",         "()I" },
/*25*/ { 20,"toString",      "()Ljava/lang/String;" },
/*26*/ { 21,"getMessage",    "()Ljava/lang/String;" },
/*27*/ { 13,"getID",         "()Ljava/lang/String;" },
/*28*/ { 0, "size",          "()I" },
/*29*/ { 15,"cInst",         "()I" },
/*30*/ { 16,"getClass",      "(Lorg/pegasus/jmpi/CIMObjectPath;Z)Lorg/pegasus/jmpi/CIMClass;" },
/*31*/ { 0, "removeElementAt",     "(I)V" },
/*32*/ { 19,"cInst",         "()I" },
/*33*/ { 13,"<init>",         "(ILjava/lang/String;)V" },
/*34*/ { 13,"getCode",        "()I" },
/*35*/ { 12,"<init>",         "(I)V" },
/*36*/ { 25,"<init>",         "(I)V" },
};

const METHOD_STRUCT staticMethodNames[]={
      { 14, "valueOf",      "(J)Ljava/math/BigInteger;" },
      { 23, "load",         "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Class;" },
};


static int methodInitDone=0;

jclass classRefs[sizeof(classNames)/sizeof(char*)];
jmethodID instanceMethodIDs[sizeof(instanceMethodNames)/sizeof(METHOD_STRUCT)];
jmethodID staticMethodIDs[sizeof(staticMethodNames)/sizeof(METHOD_STRUCT)];
static jclass providerClassRef;

jclass JMPIjvm::getGlobalClassRef(JNIEnv *env, const char* name) {
  jclass localRefCls=env->FindClass(name);
//  if (env->ExceptionOccurred())
//  env->ExceptionDescribe();
  if (localRefCls==NULL) return JNI_FALSE;
  jclass globalRefCls=(jclass) env->NewGlobalRef(localRefCls);
  env->DeleteLocalRef(localRefCls);
  return globalRefCls;
}

JMPIjvm::JMPIjvm() {
   initJVM();
}

JMPIjvm::~JMPIjvm() {
  
}

int JMPIjvm::cacheIDs(JNIEnv *env) {
   if (methodInitDone==1) return JNI_TRUE;
   if (methodInitDone==-1) return JNI_FALSE;

   methodInitDone=-1;
   for (unsigned i=0; i<(sizeof(classNames)/sizeof(char*)); i++) {
//      cerr<<"--- Trying "<< classNames[i]<<endl;
      if ((classRefs[i]=getGlobalClassRef(env,classNames[i]))==NULL) return JNI_FALSE;
   }
  
   for (unsigned j=0; j<(sizeof(instanceMethodNames)/sizeof(METHOD_STRUCT)); j++) {
//      cerr<<"--- Trying "<<j<<": "<<classNames[instanceMethodNames[j].clsIndex]<<": "<<instanceMethodNames[j].methodName<<endl;
      if ((instanceMethodIDs[j]=env->GetMethodID( 
           classRefs[instanceMethodNames[j].clsIndex],
           instanceMethodNames[j].methodName,instanceMethodNames[j].signature))==NULL) return 0;
      if ((instanceMethodIDs[j]=env->GetMethodID(classRefs[instanceMethodNames[j].clsIndex],instanceMethodNames[j].methodName,instanceMethodNames[j].signature))==NULL)
         return 0;
   }

   for (unsigned k=0; k<(sizeof(staticMethodNames)/sizeof(METHOD_STRUCT)); k++) {
//      cerr<<"--- Trying "<<k<<endl;
      if ((staticMethodIDs[k]=env->GetStaticMethodID( 
          classRefs[staticMethodNames[k].clsIndex],
          staticMethodNames[k].methodName,staticMethodNames[k].signature))==NULL) return 0;
   }
//   cerr<<"--- cacheIDs() done"<<endl;
   methodInitDone=1;
   return JNI_TRUE;
}

static void throwCIMException(JNIEnv *env,char *e) {
   env->ThrowNew(classRefs[13],e);
}

int JMPIjvm::destroyJVM()
{
   if (trace) cerr<<"--- JPIjvm::destroyJVM()\n";
   #ifdef JAVA_DESTROY_VM_WORKS
   if (jvm!=NULL) {
      JvmVector *jv;
      attachThread(&jv);
      jvm->DestroyJavaVM();
      jvm=NULL;
      return 0;
   }
   #endif
   return -1;
}

int JMPIjvm::initJVM()
{
   JavaVMInitArgs vm_args;
   JavaVMOption options[1];
   jint res;
   char *envcp;
   char classpath[1024]="-Djava.class.path=";
   JNIEnv *env;

   if (getenv("JMPI_TRACE")) trace=1;
   else trace=0;

   if (trace) cerr<<"--- JPIjvm::initJVM()\n";
   jv.initRc=0;

   envcp=getenv("CLASSPATH");
   if (envcp==NULL) {
      jv.initRc=1;
      cerr<<"--- jmpiJvm::initJVM(): No PEGASUS_PROVIDER_CLASSPATH environment variable found\n";
      return -1;
   }

   strcat(classpath,envcp);
   options[0].optionString=classpath;
   vm_args.version=0x00010002;
   vm_args.options=options;
   vm_args.nOptions=1;
   vm_args.ignoreUnrecognized=JNI_TRUE;

   res=JNI_CreateJavaVM(&jvm,(void**)&env,&vm_args);
   if (res!=0) {
      fprintf(stderr,"Can not create Java VM\n");
      exit(1);
   }
   jv.jvm=jvm;
   jv.env=env;

   if (cacheIDs(env)==1) {
      jv.classRefs=classRefs;
      jv.instMethodIDs=instanceMethodIDs;
   }

   if (env->ExceptionOccurred()) {
      env->ExceptionDescribe();
      exit(1);
   }

   return res;
}

JNIEnv* JMPIjvm::attachThread(JvmVector **jvp) {
   JNIEnv* env;
   if (jvm==NULL) initJVM();
   jvm->AttachCurrentThread((void**)&env,NULL);
   *jvp=&jv;
   return env;
}

void JMPIjvm::detachThread() {
   jvm->DetachCurrentThread();
}

jobject JMPIjvm::getProvider(JNIEnv *env, String jar, String cln,
     const char *cn, jclass *cls)
{
   static jobject gProv=NULL;
   static jclass scls=NULL;
   if (gProv) {
      *cls=scls;
      return gProv;
   }

   /*
   cout<<"--- jar: "<<jar<<endl;
   cout<<"--- cln: "<<cln<<endl;

   jstring jjar=env->NewStringUTF((const char*)jar.getCString());
   jstring jcln=env->NewStringUTF((const char*)cln.getCString());

   jclass jcls=(jclass)env->CallStaticObjectMethod(classRefs[23],staticMethodIDs[1],
      jjar,jcln);
   if (env->ExceptionCheck()) {
      env->ExceptionDescribe();
      cerr<<"--- Unable to instantiate provider "<<cn<<endl;
 //     return NULL;
   }
*/

   scls=getGlobalClassRef(env,(const char*)cln.getCString());
   if (env->ExceptionCheck()) {
      cerr<<"--- Provider "<<cn<<" not found"<<endl;
      return NULL;
   }
	*cls=scls;

   jmethodID id=env->GetMethodID(*cls,"<init>","()V");
   jobject lProv=env->NewObject(*cls,id);
   gProv=(jobject)env->NewGlobalRef(lProv);
   if (env->ExceptionCheck()) {
      cerr<<"--- Unable to instantiate provider "<<cn<<endl;
      return NULL;
   }
   return gProv;
}

jobject JMPIjvm::getProvider(JNIEnv *env, const char *cn, jclass *cls)
{
   static jobject gProv=NULL;
   static jclass scls=NULL;
   if (gProv) {
      *cls=scls;
      return gProv;
   }

   scls=getGlobalClassRef(env,cn);
   if (env->ExceptionCheck()) {
      cerr<<"--- Provider "<<cn<<" not found"<<endl;
      return NULL;
   }
	*cls=scls;

   jmethodID id=env->GetMethodID(*cls,"<init>","()V");
   jobject lProv=env->NewObject(*cls,id);
   gProv=(jobject)env->NewGlobalRef(lProv);
   if (env->ExceptionCheck()) {
      cerr<<"--- Unable to instantiate provider "<<cn<<endl;
      return NULL;
   }
   return gProv;
}

void JMPIjvm::checkException(JNIEnv *env)
{
   jstring msg=NULL,id=NULL;
   int code;
   const char *cp;
   char hcp[512]="",hcp1[128];
   String m=String::EMPTY;

   if (env->ExceptionCheck()) {
      jthrowable err=env->ExceptionOccurred();
      if (trace)
         env->ExceptionDescribe();
      env->ExceptionClear();
      if (err) {
         msg=(jstring)env->CallObjectMethod(err,JMPIjvm::jv.ThrowableGetMessage);
         code=(int)env->CallObjectMethod(err,JMPIjvm::jv.CIMExceptionGetCode);
         id=(jstring)env->CallObjectMethod(err,JMPIjvm::jv.CIMExceptionGetID);
         if (id) {
            const char *cp=env->GetStringUTFChars(id,NULL);
            strncpy(hcp1,cp,511);
            env->ReleaseStringUTFChars(id,cp);
         }

         if (msg) {
            const char *cp=env->GetStringUTFChars(msg,NULL);
            strncpy(hcp,cp,511);
            env->ReleaseStringUTFChars(msg,cp);
            m=String(hcp);
         }
	 if (trace)
            cerr<<"--- throwing Pegasus exception: "<<hcp1<<" ("<<hcp<<") "<<endl;
         throw CIMException((CIMStatusCode)code,m);
      }
   }
}

/**************************************************************************
 * name         - NewPlatformString
 * description  - Returns a new Java string object for the specified
 *                platform string.
 * parameters   - env
 *                s     Platform encoded string
 * returns      - Java string object pointer or null (0)
 **************************************************************************/
jstring JMPIjvm::NewPlatformString(JNIEnv *env, char *s)
{
    size_t len = strlen(s);
    jclass cls;
    jmethodID mid;
    jbyteArray ary;

    NULL_CHECK0(cls = (*env).FindClass("java/lang/String"));
    NULL_CHECK0(mid = (*env).GetMethodID(cls, "<init>", "([B)V"));
    ary = (*env).NewByteArray((jsize)len);
    if (ary != 0) {
        jstring str = 0;
        (*env).SetByteArrayRegion(ary, 0, (jsize)len,
                                   (jbyte *)s);
        if (!(*env).ExceptionOccurred()) {
            str = (jstring)(*env).NewObject(cls, mid, ary);
        }
        (*env).DeleteLocalRef(ary);
        return str;
    }
    return 0;
}


/**************************************************************************
 * name         - NewPlatformStringArray
 * description  - Returns a new array of Java string objects for the specified
 *                array of platform strings.
 * parameters   - env
 *                strv      Platform encoded string array
 *                strc      Number of strings in strv
 * returns      - Java string array object pointer
 **************************************************************************/
jobjectArray JMPIjvm::NewPlatformStringArray(JNIEnv *env, char **strv, int strc)
{
    jarray cls;
    jarray ary;
    int i;

    NULL_CHECK0(cls = (jarray)(*env).FindClass("java/lang/String"));
    NULL_CHECK0(ary = (*env).NewObjectArray(strc, (jclass)cls, 0));
    for (i = 0; i < strc; i++) {
        jstring str = NewPlatformString(env, *strv++);
        NULL_CHECK0(str);
        (*env).SetObjectArrayElement((jobjectArray)ary, i, str);
        (*env).DeleteLocalRef(str);
    }
    return (jobjectArray)ary;
}





extern "C" {


void throwCimException(JNIEnv *jEnv, CIMException & e) {
      JMPIjvm::cacheIDs(jEnv);
      jobject ev=jEnv->NewObject(classRefs[13],instanceMethodIDs[18],(jint)e.getCode());
      jEnv->Throw((jthrowable)ev);
}

void throwFailedException(JNIEnv *jEnv) {
      JMPIjvm::cacheIDs(jEnv);
      jobject ev=jEnv->NewObject(classRefs[13],instanceMethodIDs[18],1);
      jEnv->Throw((jthrowable)ev);
}

void throwNotSupportedException(JNIEnv *jEnv) {
      JMPIjvm::cacheIDs(jEnv);
      jobject ev=jEnv->NewObject(classRefs[13],instanceMethodIDs[18],7);
      jEnv->Throw((jthrowable)ev);
}


// -------------------------------------
// ---
// -		CIMOMHandle
// ---
// -------------------------------------

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMOMHandle__1getClass
  (JNIEnv *jEnv, jobject jThs, jint jCh, jint jCop, jboolean lo) {
   CIMOMHandle *ch=(CIMOMHandle*)jCh;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   OperationContext ctx;
   try {
      CIMClass cc=ch->getClass(ctx,cop->getNameSpace(),cop->getClassName(),
         (Boolean)lo,
         true,true,CIMPropertyList());
      return (jint)new CIMClass(cc);
   }
   Catch(jEnv);
   return 0;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMOMHandle__1enumClass
  (JNIEnv *jEnv, jobject jThs, jint jCh, jint jCop, jboolean lo, jobject jVec) {
   CIMOMHandle *ch=(CIMOMHandle*)jCh;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   OperationContext ctx;
   try {
      Array<CIMClass> en=ch->enumerateClasses(ctx,cop->getNameSpace(),cop->getClassName(),
         true,(Boolean)lo,true,true);
      if (!cop->getClassName().isNull())
         en.append(ch->getClass(ctx,cop->getNameSpace(),cop->getClassName(),(Boolean)lo,
            true,true,CIMPropertyList()));
      for (int i=0,m=en.size(); i<m; i++) {
         CIMClass *cls=new CIMClass(en[i]);
         jobject jCls=jEnv->NewObject(classRefs[17],instanceMethodIDs[19],(jint)cls);
         jEnv->CallVoidMethod(jVec,instanceMethodIDs[15],jCls);
      }
      return;
   }
   Catch(jEnv);
   return;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMOMHandle__1getProperty
   (JNIEnv *jEnv, jobject jThs, jint jCh, jint jCop, jstring jN) {

   CIMOMHandle *ch=(CIMOMHandle*)jCh;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   OperationContext ctx;
   const char *str=jEnv->GetStringUTFChars(jN,NULL);

   try {
      CIMName prop(str);
      CIMValue *cv = new CIMValue(ch->getProperty(ctx, cop->getNameSpace(),*cop,prop));
      jEnv->ReleaseStringUTFChars(jN,str);
      return (jint)(void*)cv;
   }
   Catch(jEnv);
   return -1;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMOMHandle__1deleteInstance
   (JNIEnv *jEnv, jobject jThs, jint jCh, jint jCop) {

   CIMOMHandle *ch=(CIMOMHandle*)jCh;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   OperationContext ctx;

   try {
      ch->deleteInstance(ctx,cop->getNameSpace(),*cop);
   }
   Catch(jEnv);
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMOMHandle__1enumInstances
(JNIEnv *jEnv, jobject jThs, jint jCh, jint jCop, jboolean jDeep, jboolean jLocalOnly, jobject jVec) {

   CIMOMHandle *ch=(CIMOMHandle*)jCh;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   OperationContext ctx;

   try {
      Array<CIMInstance> inst=ch->enumerateInstances(ctx,cop->getNameSpace(),cop->getClassName(),
          (Boolean)jDeep,(Boolean)jLocalOnly,true,true,CIMPropertyList());
      for (int i=0,s=inst.size(); i<s; i++){
         CIMInstance *ci=new CIMInstance(inst[i]);
         jobject jCi=jEnv->NewObject(classRefs[18],instanceMethodIDs[20],(jint)ci);
         jEnv->CallVoidMethod(jVec,instanceMethodIDs[15],jCi);
      }
   }
   Catch(jEnv);
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMOMHandle__1getInstance
   (JNIEnv *jEnv, jobject jThs, jint jCh, jint jCop, jboolean jLocalOnly) {

   CIMOMHandle *ch=(CIMOMHandle*)jCh;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   OperationContext ctx;

   try {
      CIMInstance *inst=new CIMInstance(ch->getInstance(ctx,cop->getNameSpace(),*cop,
          (Boolean)jLocalOnly,false,false,CIMPropertyList()));
      return (jint)(void*)inst;
   }
   Catch(jEnv);
   return -1;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMOMHandle__1deliverEvent
   (JNIEnv *jEnv, jobject jThs, jint jCh, jstring jName, jstring jNs, jint jInd) {

   CIMOMHandle *ch=(CIMOMHandle*)jCh;
   CIMInstance *ind=(CIMInstance*)jInd;

   const char *str=jEnv->GetStringUTFChars(jName,NULL);
   String name(str);
   jEnv->ReleaseStringUTFChars(jName,str);
   str=jEnv->GetStringUTFChars(jNs,NULL);
   String ns(str);
   jEnv->ReleaseStringUTFChars(jNs,str);

   JMPIProviderManager::indProvRecord *prec;
   OperationContext* context;

   if (JMPIProviderManager::provTab.lookup(name,prec)) {
      if (prec->enabled) {
         context=prec->ctx;
         try {
            prec->handler->deliver(*context, *ind);
         }
         Catch(jEnv);
      }
   }
   else {
      cout<<"_deliverEvent() "<<name<<" not found"<<endl;
   }
}

// -------------------------------------
// ---
// -		CIMClass
// ---
// -------------------------------------

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClass__1newInstance
  (JNIEnv *jEnv, jobject jThs, jint jCls) {
   CIMClass *cls=(CIMClass*)jCls;
//      Array<Sint8> ar;
//      cls->toXml(ar);
//      cout<<"--- class: "<<ar.getData()<<endl;

   try {
      CIMInstance *ci=new CIMInstance(cls->getClassName());
      for (int i=0,m=cls->getQualifierCount(); i<m; i++)
         ci->addQualifier(cls->getQualifier(i).clone());
      for (int i=0,m=cls->getPropertyCount(); i<m; i++) {
         CIMProperty cp= cls->getProperty(i);
         ci->addProperty(cp.clone());
       //     CIMProperty(cp.getName(), cp.getValue(), cp.getArraySize(),
       //                 cp.getReferenceClassName(), cp.getClassOrigin()));
         for (int j=0, s=cp.getQualifierCount(); j<s; j++)
            ci->getProperty(i).addQualifier(cp.getQualifier(j));
      }
      return (jint)ci;
   }
   Catch(jEnv);
   return 0;
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMClass__1getName
   (JNIEnv *jEnv, jobject jThs, jint jCls) {
   CIMClass *cls=(CIMClass*)jCls;
   
   try {
      const String &cn=cls->getClassName().getString();
      jstring str=jEnv->NewStringUTF(cn.getCString());
      return str;
   }
   Catch(jEnv);
   return 0;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClass__1getQualifier
   (JNIEnv *jEnv, jobject jThs, jint jCls, jstring jN) {
   CIMClass *cls=(CIMClass*)jCls;
   const char *str=jEnv->GetStringUTFChars(jN,NULL);
   jint rv=-1;
   Uint32 pos=cls->findQualifier(String(str));
   if (pos!=PEG_NOT_FOUND)
      rv=(jint)new CIMQualifier(cls->getQualifier(pos));
   jEnv->ReleaseStringUTFChars(jN,str);
   return rv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClass__1getProperty
     (JNIEnv *jEnv, jobject jThs, jint jCls, jstring jN) {
   CIMClass *cls=(CIMClass*)jCls;
   const char *str=jEnv->GetStringUTFChars(jN,NULL);
   jint rv=-1;
   Uint32 pos=cls->findProperty(CIMName(str));
   if (pos!=PEG_NOT_FOUND)
      rv=(jint)new CIMProperty(cls->getProperty(pos));
   jEnv->ReleaseStringUTFChars(jN,str);
   return rv;
}

JNIEXPORT jboolean JNICALL Java_org_pegasus_jmpi_CIMClass__1hasQualifier
     (JNIEnv *jEnv, jobject jThs, jint jCls, jstring jQ) {
   CIMClass *cls=(CIMClass*)jCls;
   const char *str=jEnv->GetStringUTFChars(jQ,NULL);
   Uint32 pos=cls->findQualifier(String(str));
   jEnv->ReleaseStringUTFChars(jQ,str);
   return (jboolean)(pos!=PEG_NOT_FOUND);
}

JNIEXPORT jobject JNICALL Java_org_pegasus_jmpi_CIMClass__1getProperties
      (JNIEnv *jEnv, jobject jThs, jint jCls, jobject jVec) {
   CIMClass *cls=(CIMClass*)jCls;
   
   for (int i=0,s=cls->getPropertyCount(); i<s; i++) {
      CIMProperty *cp=new CIMProperty(cls->getProperty(i));
      jobject prop=jEnv->NewObject(classRefs[15],instanceMethodIDs[14],(jint)cp);
      jEnv->CallVoidMethod(jVec,instanceMethodIDs[15],prop);
   }
   return jVec;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClass__1new
      (JNIEnv *jEnv, jobject jThs, jstring jN) {
   const char *str=jEnv->GetStringUTFChars(jN,NULL);
   CIMClass *cls = new CIMClass(CIMName(str), CIMName());
   jEnv->ReleaseStringUTFChars(jN,str);
   return (jint)(void*)cls;
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMClass__1getSuperClass
      (JNIEnv *jEnv, jobject jThs, jint jCls) {
   CIMClass *cls =(CIMClass *)jCls;
   const String &cn=cls->getSuperClassName().getString();
   jstring str=jEnv->NewStringUTF(cn.getCString());
   return str;
}

JNIEXPORT jobject JNICALL Java_org_pegasus_jmpi_CIMClass__1getKeys
      (JNIEnv *jEnv, jobject jThs, jint jCls, jobject jVec){
   CIMClass *cls=(CIMClass*)jCls;
   if (cls->hasKeys()) {
      Array<CIMName> keyNames;
      cls->getKeyNames(keyNames);
      for(int i=0, s=keyNames.size();i<s;i++){
         Uint32 pos=cls->findProperty(keyNames[i]);
         if (pos!=PEG_NOT_FOUND){
            CIMProperty *cp=new CIMProperty(cls->getProperty(pos));
            jobject prop=jEnv->NewObject(classRefs[15],instanceMethodIDs[14],(jint)cp);
            jEnv->CallVoidMethod(jVec,instanceMethodIDs[15],prop);
         }
      }
   }
   return jVec;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClass__1getMethod
      (JNIEnv *jEnv, jobject jThs, jint jCls, jstring jN) {
   CIMClass *cls=(CIMClass*)jCls;
   const char *str=jEnv->GetStringUTFChars(jN,NULL);
   jint rv=-1;
   Uint32 pos=cls->findMethod(String(str));
   if (pos!=PEG_NOT_FOUND) {
      rv=(jint)new CIMMethod(cls->getMethod(pos));
   }
   jEnv->ReleaseStringUTFChars(jN,str);
   return rv;
}

JNIEXPORT jboolean JNICALL Java_org_pegasus_jmpi_CIMClass__1equals
      (JNIEnv *jEnv, jobject jThs, jint jCls, jint jClsToBeCompared) {
   CIMClass *cls = (CIMClass*)jCls;
   CIMClass *clsToBeCompared = (CIMClass*)jClsToBeCompared;
   return cls->identical(*clsToBeCompared);
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMClass__1finalize
     (JNIEnv *jEnv, jobject jThs, jint jCls) {
   CIMClass *cls=(CIMClass*)jCls;
   delete cls;
}



// -------------------------------------
// ---
// -		CIMInstance
// ---

// -------------------------------------
JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMInstance__1new
      (JNIEnv *jEnv, jobject jThs) {
   return (jint)new CIMInstance();
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMInstance__1newCn
      (JNIEnv *jEnv, jobject jThs, jstring jN) {
   const char *str=jEnv->GetStringUTFChars(jN,NULL);
   return (jint)new CIMInstance(CIMName(str));
}

//Added by Andy Viciu
JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMInstance__1setName
      (JNIEnv *jEnv, jobject jThs, jint jInst, jstring jN) {
   CIMInstance *ci=(CIMInstance*)jInst;
   const char *str=jEnv->GetStringUTFChars(jN,NULL);
   /* NOT SUPPORTED AND NOT NEEDED*/
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMInstance__1setProperty
      (JNIEnv *jEnv, jobject jThs, jint jInst, jstring jN, jint jV) {
   CIMInstance *ci=(CIMInstance*)jInst;
   CIMValue *cv=(CIMValue*)jV;
   const char *str=jEnv->GetStringUTFChars(jN,NULL);
   Uint32 pos=ci->findProperty(CIMName(str));

   if (pos!=PEG_NOT_FOUND) {
      CIMProperty cp=ci->getProperty(pos);
     if (cp.getType()==cv->getType())
         cp.setValue(*cv);
     else {
         throw CIMException(CIM_ERR_FAILED, String("Type mismatch"));
         cerr<<"!!! CIMInstance.setProperty - Wrong type of CIMValue (instance name:"<<ci->getClassName().getString()<<", property name: "<<str<<")";
      }
   }
   else {
      CIMProperty *cp=new CIMProperty(CIMName(str),*cv);
      ci->addProperty(*cp);
      //throw CIMException(CIM_ERR_FAILED, String(str).append(String(" - Property not found")));
   }

   jEnv->ReleaseStringUTFChars(jN,str);
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMInstance__1getProperty
      (JNIEnv *jEnv, jobject jThs, jint jInst, jstring jN) {
   CIMInstance *ci=(CIMInstance*)jInst;
   const char *str=jEnv->GetStringUTFChars(jN,NULL);
   jint rv=-1;
   try {
      Uint32 pos=ci->findProperty(CIMName(str));
      if (pos!=PEG_NOT_FOUND) {
         CIMProperty *cp=new CIMProperty(ci->getProperty(pos));
         rv=(jint)cp;
      }
   }
   Catch(jEnv);
   jEnv->ReleaseStringUTFChars(jN,str);
   return rv;
}

JNIEXPORT jobject JNICALL Java_org_pegasus_jmpi_CIMInstance__1getKeyValuePairs
      (JNIEnv *jEnv, jobject jThs, jint jInst, jobject jVec) {
   CIMInstance *ci=(CIMInstance*)jInst;
   
   for (int i=0,s=ci->getPropertyCount(); i<s; i++) {
      if (ci->getProperty(i).findQualifier(String("key"))!=PEG_NOT_FOUND) {
         CIMProperty *cp=new CIMProperty(ci->getProperty(i));
         jobject prop=jEnv->NewObject(classRefs[15],instanceMethodIDs[14],(jint)cp);
         jEnv->CallVoidMethod(jVec,instanceMethodIDs[15],prop);
      }
   }

   return jVec;
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMInstance__1getClassName
      (JNIEnv *jEnv, jobject jThs, jint jInst) {
   CIMInstance *ci=(CIMInstance*)jInst;
   const String &cn=ci->getClassName().getString();
   jstring str=jEnv->NewStringUTF(cn.getCString());
   return str;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMInstance__1getQualifier
      (JNIEnv *jEnv, jobject jThs, jint jInst, jstring jN) {
   CIMInstance *ci=(CIMInstance*)jInst;
   const char *str=jEnv->GetStringUTFChars(jN,NULL);
   jint rv=-1;
   Uint32 pos=ci->findQualifier(String(str));
   if (pos!=PEG_NOT_FOUND) {
      rv=(jint)new CIMQualifier(ci->getQualifier(pos));
   }
   jEnv->ReleaseStringUTFChars(jN,str);
   return rv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMInstance__1clone
      (JNIEnv *jEnv, jobject jThs, jint jInst) {
   CIMInstance *ci=(CIMInstance *)jInst;
   CIMInstance* cl=new CIMInstance(ci->clone());
   return (jint)(void*)cl;
}

JNIEXPORT jobject JNICALL Java_org_pegasus_jmpi_CIMInstance__1getProperties
      (JNIEnv *jEnv, jobject jThs, jint jInst, jobject jVec) {
   CIMInstance *ci=(CIMInstance*)jInst;
   for (int i=0,s=ci->getPropertyCount(); i<s; i++) {
      CIMProperty *cp=new CIMProperty(ci->getProperty(i));
      jobject prop=jEnv->NewObject(classRefs[15],instanceMethodIDs[14],(jint)cp);
      jEnv->CallVoidMethod(jVec,instanceMethodIDs[15],prop);
   }
   return jVec;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMInstance__1finalize
      (JNIEnv *jEnv, jobject jThs, jint jInst) {
   CIMInstance *ci=(CIMInstance*)jInst;
   delete ci;
}


// -------------------------------------
// ---
// -		CIMObjectPath
// ---
// -------------------------------------

CIMObjectPath* construct() {
   CIMObjectPath *cop=new CIMObjectPath();
   _nameSpace n;
   cop->setNameSpace(n.nameSpace());
   cop->setHost(n.hostName());
   return cop;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1new
      (JNIEnv *jEnv, jobject jThs) {
   return (jint)construct();
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1newCn
      (JNIEnv *jEnv, jobject jThs, jstring jCn) {
   CIMObjectPath *cop=construct();
   const char *str=jEnv->GetStringUTFChars(jCn,NULL);
   if (str) cop->setClassName(str);
   jEnv->ReleaseStringUTFChars(jCn,str);
   return (jint)(void*)cop;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1newCnNs
      (JNIEnv *jEnv, jobject jThs, jstring jCn, jstring jNs) {
   CIMObjectPath *cop=construct();

   const char *str1=jEnv->GetStringUTFChars(jCn,NULL);
   const char *str2=jEnv->GetStringUTFChars(jNs,NULL);
   try {
      if (str1) cop->setClassName(str1);
      if (str2) cop->setNameSpace(str2);
   }
   catch (Exception e) {
      jobject ev=jEnv->NewObject(classRefs[13],instanceMethodIDs[33],(jint)1,jEnv->NewStringUTF(e.getMessage().getCString()));
      jEnv->Throw((jthrowable)ev);
   }

   jEnv->ReleaseStringUTFChars(jCn,str1);
   jEnv->ReleaseStringUTFChars(jNs,str2);
   return (jint)(void*)cop;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1_newCi
      (JNIEnv *jEnv, jobject jThs, jint jInst) {
   CIMInstance *ci = (CIMInstance *)jInst;
   CIMObjectPath *cop=new CIMObjectPath(ci->getPath());
   _nameSpace n;
   if (cop->getNameSpace().isNull()) cop->setNameSpace(n.nameSpace());
   if (cop->getHost()==NULL) cop->setHost(n.hostName());
   return (jint)(void*)cop;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1finalize
      (JNIEnv *jEnv, jobject jThs, jint jCop) {
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   delete cop;
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1getHost
      (JNIEnv *jEnv, jobject jThs, jint jOp) {
   CIMObjectPath *cop=(CIMObjectPath*)jOp;
   const String &hn=cop->getHost();
   jstring str=jEnv->NewStringUTF(hn.getCString());
   return str;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1setHost
      (JNIEnv *jEnv, jobject jThs, jint jOp, jstring jName) {
   CIMObjectPath *cop=(CIMObjectPath*)jOp;
   const char *str=jEnv->GetStringUTFChars(jName,NULL);
   cop->setHost(String(str));
   jEnv->ReleaseStringUTFChars(jName,str);
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1getObjectName
      (JNIEnv *jEnv, jobject jThs, jint jOp) {
   CIMObjectPath *cop=(CIMObjectPath*)jOp;
   const String &cn=cop->getClassName().getString();
   jstring str=jEnv->NewStringUTF(cn.getCString());
   return str;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1setObjectName
      (JNIEnv *jEnv, jobject jThs, jint jOp, jstring jName) {
   CIMObjectPath *cop=(CIMObjectPath*)jOp;
   const char *str=jEnv->GetStringUTFChars(jName,NULL);
   cop->setClassName(String(str));
   jEnv->ReleaseStringUTFChars(jName,str);
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1getNameSpace
      (JNIEnv *jEnv, jobject jThs, jint jOp) {
   CIMObjectPath *cop=(CIMObjectPath*)jOp;
   const String &ns=cop->getNameSpace().getString();
   jstring str=jEnv->NewStringUTF(ns.getCString());
   return str;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1setNameSpace
      (JNIEnv *jEnv, jobject jThs, jint jOp, jstring jName) {
   CIMObjectPath *cop=(CIMObjectPath*)jOp;
   const char *str=jEnv->GetStringUTFChars(jName,NULL);
   cop->setNameSpace(CIMNamespaceName(str));
   jEnv->ReleaseStringUTFChars(jName,str);
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1addKey
      (JNIEnv *jEnv, jobject jThs, jint jOp, jstring jId, jint jVal) {
   CIMObjectPath *cop=(CIMObjectPath*)jOp;
   const char *str=jEnv->GetStringUTFChars(jId,NULL);
   CIMValue *cv=(CIMValue*)jVal;
   Array<CIMKeyBinding> keyBindings=cop->getKeyBindings();
   keyBindings.append(CIMKeyBinding(str,*cv));
   cop->setKeyBindings(Array<CIMKeyBinding>(keyBindings));
   jEnv->ReleaseStringUTFChars(jId,str);
   return;
}

JNIEXPORT jobject JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1getKeys
      (JNIEnv *jEnv, jobject jThs, jint jOp, jobject jVec) {
   CIMObjectPath *cop=(CIMObjectPath*)jOp;
   const Array<CIMKeyBinding> &akb=cop->getKeyBindings();

   for (Uint32 i=0,s=akb.size(); i<s; i++) {
      const String &n=akb[i].getName().getString();
      const String &v=akb[i].getValue();
      CIMKeyBinding::Type t=akb[i].getType();
      CIMValue *cv;
      switch (t) {
      case CIMKeyBinding::NUMERIC:
         cv=new CIMValue((Sint32)atol(v.getCString()));
         break;
      case CIMKeyBinding::STRING:
         cv=new CIMValue(v);
         break;
      case CIMKeyBinding::BOOLEAN:
         cv=new CIMValue((Boolean)(v.getCString()));
         break;
      case CIMKeyBinding::REFERENCE:
         cv = new CIMValue(CIMObjectPath(akb[i].getValue()));
         break;
      default:
         throwCIMException(jEnv,"+++ unsupported type: ");
      }
      
      CIMProperty *cp;
      if(t!=CIMKeyBinding::REFERENCE)
         cp=new CIMProperty(n,*cv);
      else cp=new CIMProperty(n,*cv, 0, ((CIMObjectPath) akb[i].getValue()).getClassName());

      jobject prop=jEnv->NewObject(classRefs[15],instanceMethodIDs[14],(jint)cp);
      jEnv->CallVoidMethod(jVec,instanceMethodIDs[15],prop);
   }
   return jVec;
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1getKeyValue
      (JNIEnv *jEnv, jobject jThs, jint jOp, jstring jStr) {
   CIMObjectPath *cop=(CIMObjectPath*)jOp;
   const Array<CIMKeyBinding> &akb=cop->getKeyBindings();
   const char *strKeyName=jEnv->GetStringUTFChars(jStr,NULL);
   jstring retStr=NULL;
   for (Uint32 i=0,s=akb.size(); i<s; i++) {
      const String &n=akb[i].getName().getString();
      if (n==String(strKeyName)) {
         retStr=jEnv->NewStringUTF(akb[i].getValue().getCString());
         break;
      }
   }
   jEnv->ReleaseStringUTFChars(jStr,strKeyName);
   return retStr;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1set
      (JNIEnv *jEnv, jobject jThs, jstring jStr) {
   const char *strCop=jEnv->GetStringUTFChars(jStr,NULL);
   CIMObjectPath *cop=new CIMObjectPath();
   cop->set(String(strCop));
   jEnv->ReleaseStringUTFChars(jStr,strCop);
   return (jint)cop;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1setKeys
      (JNIEnv *jEnv, jobject jThs, jint jOp, jobject jVec) {
   CIMObjectPath *cop=(CIMObjectPath*)jOp;
   Array<CIMKeyBinding> akb;
   for (Uint32 i=0,s=jEnv->CallIntMethod(jVec,JMPIjvm::jv.VectorSize); i<s; i++) {
      jobject o=jEnv->CallObjectMethod(jVec,JMPIjvm::jv.VectorElementAt,(i));
      CIMProperty *cp=(CIMProperty*)jEnv->CallIntMethod(o,JMPIjvm::jv.PropertyCInst);
      akb.append(CIMKeyBinding(cp->getName(),cp->getValue()));
   }
   cop->setKeyBindings(akb);
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1clone
      (JNIEnv *jEnv, jobject jThs, jint jOp) {
   CIMObjectPath *cop=(CIMObjectPath*)jOp;
   CIMObjectPath *copl=new CIMObjectPath(cop->getHost(), cop->getNameSpace(), cop->getClassName(), cop->getKeyBindings());
   return (jint)(void*)copl;
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMObjectPath__1toString
      (JNIEnv *jEnv, jobject jThs, jint jOp) {
   CIMObjectPath *cop=(CIMObjectPath*)jOp;
   const String &ns=cop->toString();
   jstring str=jEnv->NewStringUTF(ns.getCString());
   return str;
}


// -------------------------------------
// ---
// -		CIMDataType
// ---
// -------------------------------------

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMDataType__1new
  (JNIEnv *jEnv, jobject jThs, jint type) {
  return (jint)(void*) new _dataType(type);
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMDataType__1newAr
  (JNIEnv *jEnv, jobject jThs, jint type, jint size) {
  return (jint)(void*) new _dataType(type,size);
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMDataType__1newRef
  (JNIEnv *jEnv, jobject jThs, jint type, jstring jRef) {
   const char *ref=jEnv->GetStringUTFChars(jRef,NULL);
   jint cInst=(jint)(void*)new _dataType(type,String(ref));
   jEnv->ReleaseStringUTFChars(jRef,ref);
   return cInst;
}

JNIEXPORT jboolean JNICALL Java_org_pegasus_jmpi_CIMDataType__1isArray
  (JNIEnv *jEnv, jobject jThs, jint jDt) {
   _dataType *dt=(_dataType*)jDt;
   return dt->_array==true;
}

JNIEXPORT jboolean JNICALL Java_org_pegasus_jmpi_CIMDataType__1isreference
  (JNIEnv *jEnv, jobject jThs, jint jDt) {
   _dataType *dt=(_dataType*)jDt;
   return dt->_reference==true;
}

JNIEXPORT jboolean JNICALL Java_org_pegasus_jmpi_CIMDataType__1isReference
  (JNIEnv *jEnv, jobject jThs, jint jDt) {
   _dataType *dt=(_dataType*)jDt;
   return dt->_reference==true;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMDataType__1getType
  (JNIEnv *jEnv, jobject jThs, jint jDt) {
   _dataType *dt=(_dataType*)jDt;
   return dt->_type;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMDataType__1getSize
  (JNIEnv *jEnv, jobject jThs, jint jDt) {
   _dataType *dt=(_dataType*)jDt;
   return dt->_size;
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMDataType__1getRefClassName
  (JNIEnv *jEnv, jobject jThs, jint jDt) {
   _dataType *dt=(_dataType*)jDt;
   jstring str=jEnv->NewStringUTF(dt->_refClass.getCString());
   return str;
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMDataType__1toString
  (JNIEnv *jEnv, jobject jThs, jint jDt) {
   _dataType *dt=(_dataType*)jDt;
   jstring str=NULL;
   if (dt->_type & 0x10) {
      char tmp[32];
      strcpy(tmp,jTypeToChars[dt->_type-0x10]);
      strcat(tmp,"[]");
      str=jEnv->NewStringUTF(tmp);
   } 
   else if (dt->_type & 0x20) {
      String tmp=dt->_refClass+" REF";
      str=jEnv->NewStringUTF(tmp.getCString());
   }  
   else {
      str=jEnv->NewStringUTF(jTypeToChars[dt->_type]);
   }
   return str;
}



// -------------------------------------
// ---
// -		CIMProperty
// ---
// -------------------------------------

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMProperty__1getValue
      (JNIEnv *jEnv, jobject jThs, jint jP) {
   CIMProperty *cp=(CIMProperty*)jP;
   CIMValue *cv=new CIMValue(cp->getValue());
   return (jint)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMProperty__1property
      (JNIEnv *jEnv, jobject jThs, jstring jN, jint jV) {
   try {
      CIMValue *cv=(CIMValue*)jV;
      const char *str=jEnv->GetStringUTFChars(jN,NULL);
      CIMProperty *cp;

      if (cv->getType()!=CIMTYPE_REFERENCE)
         cp=new CIMProperty(String(str),*cv);
      else {
         if (!cv->isArray()) {
            CIMObjectPath cop;
            cv->get(cop);
            cp=new CIMProperty(String(str),*cv,0, cop.getClassName());
         }
         else {
            throwCIMException(jEnv,"+++ unsupported type in CIMProperty.property");
         }
      }

      jEnv->ReleaseStringUTFChars(jN,str);
      return (jint)cp;
   }
   Catch(jEnv);
   return -1;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMProperty__1setValue
      (JNIEnv *jEnv, jobject jThs, jint jP,jint jV) {
   CIMProperty *cp=(CIMProperty*)jP;
   CIMValue *cv=(CIMValue*)jV;
   cp->setValue(*cv);
}

JNIEXPORT jboolean JNICALL Java_org_pegasus_jmpi_CIMProperty__1isArray
      (JNIEnv *jEnv, jobject jThs, jint jP) {
   CIMProperty *cp=(CIMProperty*)jP;
   return (jboolean)cp->isArray();
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMProperty__1addValue
      (JNIEnv *jEnv, jobject jThs, jint jP, jint jV) {
   CIMProperty *cp=(CIMProperty*)jP;
   CIMValue *cvin=(CIMValue*)jV;
   CIMValue cv=cp->getValue();
   if (cvin->isNull())
      throwCIMException(jEnv,"+++ null cvin value ");
   if (!cv.isArray())
      throwCIMException(jEnv,"+++ not an array ");
   if (cvin->getType()!=cv.getType())
      throwCIMException(jEnv,"+++ type mismatch ");
   CIMType type=cv.getType();
   switch (type) {
   case CIMTYPE_BOOLEAN: {
         Boolean bo;
         cvin->get(bo);
         Array<Boolean> boarr;
         cv.get(boarr);
         boarr.append(bo);
      }
      break;
   case CIMTYPE_UINT8: {
         Uint8 u8;
         cvin->get(u8);
         Array<Uint8> u8arr;
         cv.get(u8arr);
         u8arr.append(u8);
      }
      break;
   case CIMTYPE_SINT8: {
         Sint8 s8;
         cvin->get(s8);
         Array<Sint8> s8arr;
         cv.get(s8arr);
         s8arr.append(s8);
      }
      break;
   case CIMTYPE_UINT16: {
         Uint16 u16;
         cvin->get(u16);
         Array<Uint16> u16arr;
         cv.get(u16arr);
         u16arr.append(u16);
      }
      break;
   case CIMTYPE_SINT16: {
         Sint16 s16;
         cvin->get(s16);
         Array<Sint16> s16arr;
         cv.get(s16arr);
         s16arr.append(s16);
      }
      break;
   case CIMTYPE_UINT32: {
         Uint32 u32;
         cvin->get(u32);
         Array<Uint32> u32arr;
         cv.get(u32arr);
         u32arr.append(u32);
      }
      break;
   case CIMTYPE_SINT32: {
         Sint32 s32;
         cvin->get(s32);
         Array<Sint32> s32arr;
         cv.get(s32arr);
         s32arr.append(s32);
      }
      break;
   case CIMTYPE_UINT64: {
         Uint64 u64;
         cvin->get(u64);
         Array<Uint64> u64arr;
         cv.get(u64arr);
         u64arr.append(u64);
      }
      break;
   case CIMTYPE_SINT64: {
         Sint64 s64;
         cvin->get(s64);
         Array<Sint64> s64arr;
         cv.get(s64arr);
         s64arr.append(s64);
      }
      break;
   case CIMTYPE_REAL32: {
         Real32 f;
         cvin->get(f);
         Array<Real32> farr;
         cv.get(farr);
         farr.append(f);
      }
      break;
   case CIMTYPE_REAL64: {
         Real64 d;
         cvin->get(d);
         Array<Real64> darr;
         cv.get(darr);
         darr.append(d);
      }
      break;
   case CIMTYPE_STRING: {
         String str;
         cvin->get(str);
         Array<String> strarr;
         cv.get(strarr);
         strarr.append(str);
      }
      break;
   case CIMTYPE_REFERENCE: {
         CIMObjectPath ref;
         cvin->get(ref);
         Array<CIMObjectPath> refarr;
         cv.get(refarr);
         refarr.append(ref);
      }
      break;
   default:
      throwCIMException(jEnv,"+++ unsupported type ");
   }
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMProperty__1getName
      (JNIEnv *jEnv, jobject jThs, jint jP) {
   CIMProperty *cp=(CIMProperty*)jP;
   const String &n=cp->getName().getString();
   jstring str=jEnv->NewStringUTF(n.getCString());
   return str;
}

JNIEXPORT jboolean JNICALL Java_org_pegasus_jmpi_CIMProperty__1isReference
      (JNIEnv *jEnv, jobject jThs, jint jP) {
   CIMProperty *cp=(CIMProperty*)jP;
   return (jboolean)(cp->getType()==CIMTYPE_REFERENCE);
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMProperty__1getRefClassName
     (JNIEnv *jEnv, jobject jThs, jint jP) {
   CIMProperty *cp=(CIMProperty*)jP;
   const String &n=cp->getReferenceClassName().getString();
   jstring str=jEnv->NewStringUTF(n.getCString());
   return str;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMProperty__1getType
      (JNIEnv *jEnv, jobject jThs, jint jP) {
   CIMProperty *cp=(CIMProperty*)jP;
   String ref=cp->getReferenceClassName().getString();
   _dataType *type=new _dataType(pTypeToJType[cp->getType()],
        cp->getArraySize(),
        ref.size() ? true : false,
        false,
        cp->isArray(),
        ref,
        true);
   return (jint)type;
}

JNIEXPORT jobject JNICALL Java_org_pegasus_jmpi_CIMProperty__1getIdentifier
      (JNIEnv *jEnv, jobject jThs, jint jP) {
   CIMProperty *cp=(CIMProperty*)jP;
   const String &n=cp->getName().getString();
   jstring str=jEnv->NewStringUTF(n.getCString());
   return str;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMProperty__1finalize
      (JNIEnv *jEnv, jobject jThs, jint jP) {
   CIMProperty *cp=(CIMProperty*)jP;
   delete cp;
}
   


// -------------------------------------
// ---
// -     CIMQualifier
// ---
// -------------------------------------

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMQualifier__1finalize
      (JNIEnv *jEnv, jobject jThs, jint jQ) {
   CIMQualifier *cq=(CIMQualifier*)jQ;
   delete cq;
}


// -------------------------------------
// ---
// -		CIMDateTime
// ---
// -------------------------------------

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMDateTime__1datetime
      (JNIEnv *jEnv, jobject jThs, jstring jN) {
   const char *str=jEnv->GetStringUTFChars(jN,NULL);
   CIMDateTime *dt;
   if (strlen(str)==0)
      dt=new CIMDateTime();
   else
      dt=new CIMDateTime(String(str));
   jEnv->ReleaseStringUTFChars(jN,str);
   return (jint)dt;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMDateTime__1datetimeempty
      (JNIEnv *jEnv, jobject jThs) {
   CIMDateTime *dt=new CIMDateTime(CIMDateTime::getCurrentDateTime ());
   return (jint)dt;
}

JNIEXPORT jboolean JNICALL Java_org_pegasus_jmpi_CIMDateTime__1after
      (JNIEnv *jEnv, jobject jThs, jint jC, jint jD) {
   CIMDateTime *ct = (CIMDateTime *) jC;
   CIMDateTime *dt = (CIMDateTime *) jD;
   return (jboolean)(ct->getDifference(*ct, *dt)>0);
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMDateTime__1finalize
      (JNIEnv *jEnv, jobject jThs, jint jDT) {
   CIMDateTime *cdt = (CIMDateTime *) jDT;
   delete cdt;
}


// -------------------------------------
// ---
// -		CIMMethod
// ---
// -------------------------------------

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMMethod__1getType
      (JNIEnv *jEnv, jobject jThs, jint jM) {
   CIMMethod *cm=(CIMMethod*)jM;
   return (jint)cm->getType();
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMMethod__1finalize
      (JNIEnv *jEnv, jobject jThs, jint jM) {
   CIMMethod *cm=(CIMMethod*)jM;
   delete cm;
}


// -------------------------------------
// ---
// -		CIMValue
// ---
// -------------------------------------


JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1byte
      (JNIEnv *jEnv, jobject jThs, jbyte jb, jboolean notSigned) {
   CIMValue *cv=NULL;
   if (notSigned) cv=new CIMValue((Uint8)jb);
   else cv=new CIMValue((Sint8)jb);
   return (jint)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1short
      (JNIEnv *jEnv, jobject jThs, jshort js, jboolean notSigned) {
   CIMValue *cv=NULL;
   if (notSigned) cv=new CIMValue((Uint16)js);
   else cv=new CIMValue((Sint16)js);
   return (jint)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue_makeInt
      (JNIEnv *jEnv, jobject jThs, jlong ji, jboolean notSigned) {
   CIMValue *cv=NULL;
   if (notSigned) cv=new CIMValue((Uint32)ji);
   else cv=new CIMValue((Sint32)ji);
   return (jint)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1long
      (JNIEnv *jEnv, jobject jThs, jlong jl, jboolean notSigned) {
   CIMValue *cv=NULL;
   if (notSigned) cv=new CIMValue((Uint64)jl);
   else cv=new CIMValue((Sint64)jl);
   return (jint)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1float
      (JNIEnv *jEnv, jobject jThs, jfloat jF) {
   CIMValue *cv=new CIMValue(jF);
   return (jint)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1double
      (JNIEnv *jEnv, jobject jThs, jdouble jD) {
   CIMValue *cv=new CIMValue(jD);
   return (jint)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1string
      (JNIEnv *jEnv, jobject jThs, jstring jS) {
   const char *str=jEnv->GetStringUTFChars(jS,NULL);
   CIMValue *cv=new CIMValue(String(str));
   jEnv->ReleaseStringUTFChars(jS,str);
   return (jint)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1ref
      (JNIEnv *jEnv, jobject jThs, jint jR) {
   CIMObjectPath *ref=(CIMObjectPath*)jR;
   CIMValue *cv=new CIMValue(*ref);
   return (jint)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1datetime
      (JNIEnv *jEnv, jobject jThs, jint jDT) {
   CIMDateTime *dt=(CIMDateTime*)jDT;
   CIMValue *cv=new CIMValue(*dt);
   return (jint)cv;
}

JNIEXPORT jboolean JNICALL Java_org_pegasus_jmpi_CIMValue__1isArray
      (JNIEnv *jEnv, jobject jThs, jint jP) {
   CIMValue *cv=(CIMValue*)jP;
   return (jboolean)cv->isArray();
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1boolean
      (JNIEnv *jEnv, jobject jThs, jboolean jB) {
   CIMValue *cv=new CIMValue((Boolean)jB);
   return (jint)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1byteArray
      (JNIEnv *jEnv, jobject jThs, jshortArray jshortA, jboolean notSigned) {
   CIMValue *cv=NULL;
   jboolean b;
   jsize len=jEnv->GetArrayLength(jshortA);
   jshort* jsA=jEnv->GetShortArrayElements(jshortA,&b);
   if (notSigned) {
      Array<Uint8> u8;
      for (jsize i=0;i<len;i++)
         u8.append((Uint8)jsA[i]);
      cv=new CIMValue(u8);
   }
   else {
      Array<Sint8> s8;
      for (jsize i=0;i<len;i++)
         s8.append((Sint8)jsA[i]);
      cv=new CIMValue(s8);
   }
   return (jint)(void*)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1shortArray
      (JNIEnv *jEnv, jobject jThs, jintArray jintA, jboolean notSigned) {
   CIMValue *cv=NULL;
   jboolean b;
   jsize len=jEnv->GetArrayLength(jintA);
   jint* jiA=jEnv->GetIntArrayElements(jintA,&b);
   if (notSigned) {
      Array<Uint16> u16;
      for (jsize i=0;i<len;i++)
         u16.append((Uint16)jiA[i]);
      cv=new CIMValue(u16);
   }
   else {
      Array<Sint16> s16;
      for (jsize i=0;i<len;i++)
         s16.append((Sint16)jiA[i]);
      cv=new CIMValue(s16);
   }
   return (jint)(void*)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1intArray
      (JNIEnv *jEnv, jobject jThs, jlongArray jlongA, jboolean notSigned) {
   CIMValue *cv=NULL;
   jboolean b;
   jsize len=jEnv->GetArrayLength(jlongA);
   jlong* jlA=jEnv->GetLongArrayElements(jlongA,&b);
   if (notSigned) {
      Array<Uint32> u32;
      for (jsize i=0;i<len;i++)
         u32.append((Uint32)jlA[i]);
      cv=new CIMValue(u32);
   }
   else {
      Array<Sint32> s32;
      for (jsize i=0;i<len;i++)
         s32.append((Sint32)jlA[i]);
      cv=new CIMValue(s32);
   }
   return (jint)(void*)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1longArray
      (JNIEnv *jEnv, jobject jThs, jlongArray jlongA, jboolean notSigned) {
   CIMValue *cv=NULL;
   jboolean b;
   jsize len=jEnv->GetArrayLength(jlongA);
   jlong* jlA=jEnv->GetLongArrayElements(jlongA,&b);
   if (notSigned) {
      Array<Uint64> u64;
      for (jsize i=0;i<len;i++)
         u64.append((Uint64)jlA[i]);
      cv=new CIMValue(u64);
   }
   else {
      Array<Sint64> s64;
      for (jsize i=0;i<len;i++)
         s64.append((Sint64)jlA[i]);
      cv=new CIMValue(s64);
   }
   return (jint)(void*)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1stringArray
      (JNIEnv *jEnv, jobject jThs, jobjectArray jstringA) {
   CIMValue *cv=NULL;
   jboolean b;
   jsize len=jEnv->GetArrayLength(jstringA);
   Array<String> strA;

   for (jsize i=0;i<len;i++) {
      jstring jsA=(jstring)jEnv->GetObjectArrayElement(jstringA,i);
      const char *str=jEnv->GetStringUTFChars(jsA,NULL);
      strA.append(String(str));
      jEnv->ReleaseStringUTFChars(jsA,str);
   }

   cv=new CIMValue(strA);
   return (jint)(void*)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1booleanArray
      (JNIEnv *jEnv, jobject jThs, jbooleanArray jboolA) {
   CIMValue *cv=NULL;
   jboolean b;
   jsize len=jEnv->GetArrayLength(jboolA);
   jboolean* jbA=jEnv->GetBooleanArrayElements(jboolA,&b);
   Array<Boolean> bA;
   for (jsize i=0;i<len;i++)
      bA.append((Boolean)jbA[i]);
   cv=new CIMValue(bA);
   return (jint)(void*)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1refArray
      (JNIEnv *jEnv, jobject jThs, jintArray jintA) {
   CIMValue *cv=NULL;
   jboolean b;
   jsize len=jEnv->GetArrayLength(jintA);
   jint* jiA=jEnv->GetIntArrayElements(jintA,&b);
   Array<CIMObjectPath> cA;
   for (jsize i=0;i<len;i++)
      cA.append(*((CIMObjectPath*)jiA[i]));
   cv=new CIMValue(cA);
   return (jint)(void*)cv;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMValue__1getType
      (JNIEnv *jEnv, jobject jThs, jint jP) {
   CIMValue *cv=(CIMValue*)jP;
   return (jint)cv->getType();
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMValue__1toString
      (JNIEnv *jEnv, jobject jThs, jint jV) {
   CIMValue *cv=(CIMValue*)jV;
   return (jstring)jEnv->NewStringUTF(cv->toString().getCString());
}

JNIEXPORT jobject JNICALL Java_org_pegasus_jmpi_CIMValue__1getValue
      (JNIEnv *jEnv, jobject jThs, jint jV) {
   CIMValue *cv=(CIMValue*)jV;
   if (cv->isNull())
      return NULL;
   CIMType type=cv->getType();

   if (!cv->isArray()) {
      switch (type) {
      case CIMTYPE_BOOLEAN:
         Boolean bo;
         cv->get(bo);
         return jEnv->NewObject(classRefs[1],instanceMethodIDs[1],bo);
         break;
      case CIMTYPE_SINT8:
         Sint8 s8;
         cv->get(s8);
         return jEnv->NewObject(classRefs[2],instanceMethodIDs[2],s8);
         break;
      case CIMTYPE_UINT8:
         Uint8 u8;
         cv->get(u8);
         return jEnv->NewObject(classRefs[8],instanceMethodIDs[8],u8);
         break;
      case CIMTYPE_SINT16:
         Sint16 s16;
         cv->get(s16);
         return jEnv->NewObject(classRefs[3],instanceMethodIDs[3],s16);
         break;
      case CIMTYPE_UINT16:
         Uint16 u16;
         cv->get(u16);
         return jEnv->NewObject(classRefs[9],instanceMethodIDs[9],u16);
         break;
      case CIMTYPE_SINT32:
         Sint32 s32;
         cv->get(s32);
         return jEnv->NewObject(classRefs[4],instanceMethodIDs[4],s32);
         break;
      case CIMTYPE_UINT32:
         Uint32 u32;
         cv->get(u32);
         return jEnv->NewObject(classRefs[10],instanceMethodIDs[10],u32);
         break;
      case CIMTYPE_SINT64:
         Sint64 s64;
         cv->get(s64);
         return jEnv->NewObject(classRefs[5],instanceMethodIDs[5],s64);
         break;
      case CIMTYPE_UINT64: {
            Uint64 u64;
            cv->get(u64);
            jobject big=jEnv->CallStaticObjectMethod(classRefs[14],staticMethodIDs[0],u64);
            return jEnv->NewObject(classRefs[11],instanceMethodIDs[11],big);
         }
         break;
      case CIMTYPE_REAL32:
         float f;
         cv->get(f);
         return jEnv->NewObject(classRefs[6],instanceMethodIDs[6],f);
         break;
      case CIMTYPE_REAL64:
         double d;
         cv->get(d);
         return jEnv->NewObject(classRefs[7],instanceMethodIDs[7],d);
         break;
      case CIMTYPE_STRING: {
            String s;
            jstring str;
            cv->get(s);
            str=jEnv->NewStringUTF(s.getCString());
            return str;
         }
         break;
      case CIMTYPE_REFERENCE:  {
            CIMObjectPath ref;
            cv->get(ref);
            return jEnv->NewObject(classRefs[12],instanceMethodIDs[12],
               (jint)new CIMObjectPath(ref));
         }
         break;
      case CIMTYPE_CHAR16:
         throwCIMException(jEnv,"+++ Char16 not yet supported");
         break;
      case CIMTYPE_DATETIME: {
            CIMDateTime dt;
            cv->get(dt);
            return jEnv->NewObject(classRefs[24],instanceMethodIDs[35],
               (jint)new CIMDateTime(dt));
         }
//         throwCIMException(jEnv,"+++ DateTime not yet supported");
         break;
      default:
      	throwCIMException(jEnv,"+++ unsupported type: ");
      }
   }

   else {
      switch (type) {
      case CIMTYPE_BOOLEAN: {
            Array<Boolean> bo;
            cv->get(bo);
            int s=bo.size();
            jobjectArray jbooleanA=(jobjectArray)jEnv->NewObjectArray(s,classRefs[1],0);
            for (int i=0; i < s; i++) jEnv->SetObjectArrayElement(jbooleanA, i, 
                  jEnv->NewObject(classRefs[1],instanceMethodIDs[1],bo[i]));
            return jbooleanA;
         }
         break;
      case CIMTYPE_SINT8: {
            Array<Sint8> s8;
            cv->get(s8);
            int s=s8.size();
            jobjectArray jbyteA=(jobjectArray)jEnv->NewObjectArray(s,classRefs[2],0);
            for (int i=0; i < s; i++) jEnv->SetObjectArrayElement(jbyteA, i, 
               jEnv->NewObject(classRefs[2],instanceMethodIDs[2],s8[i]));
            return jbyteA;
         }
         break;
      case CIMTYPE_UINT8: {
            Array<Uint8> u8;
            cv->get(u8);
            int s=u8.size();
            jobjectArray jshortA=(jobjectArray)jEnv->NewObjectArray(s,classRefs[8],0);
            for (int i=0; i < s; i++) jEnv->SetObjectArrayElement(jshortA, i, 
               jEnv->NewObject(classRefs[8],instanceMethodIDs[8],u8[i]));
            return jshortA;
         }
         break;
      case CIMTYPE_SINT16: {
            Array<Sint16> s16;
            cv->get(s16);
            int s=s16.size();
            jobjectArray jshortA=(jobjectArray)jEnv->NewObjectArray(s,classRefs[3],0);
            for (int i=0; i < s; i++) jEnv->SetObjectArrayElement(jshortA, i, 
               jEnv->NewObject(classRefs[3],instanceMethodIDs[3],s16[i]));
            return jshortA;
         }
         break;
      case CIMTYPE_UINT16: {
            Array<Uint16> u16;
            cv->get(u16);
            int s=u16.size();
            jobjectArray jintA=(jobjectArray)jEnv->NewObjectArray(s,classRefs[9],0);
            for (int i=0; i < s; i++) jEnv->SetObjectArrayElement(jintA, i, 
               jEnv->NewObject(classRefs[9],instanceMethodIDs[9],u16[i]));
            return jintA;
         }
         break;
      case CIMTYPE_SINT32: {
            Array<Sint32> s32;
            cv->get(s32);
            int s=s32.size();
            jobjectArray jintA=(jobjectArray)jEnv->NewObjectArray(s,classRefs[4],0);
            for (int i=0; i < s; i++) jEnv->SetObjectArrayElement(jintA, i, 
               jEnv->NewObject(classRefs[4],instanceMethodIDs[4],s32[i]));
            return jintA;
         }
         break;
      case CIMTYPE_UINT32: {
            Array<Uint32> u32;
            cv->get(u32);
            int s=u32.size();
            jobjectArray jlongA=(jobjectArray)jEnv->NewObjectArray(s,classRefs[10],0);
            for (int i=0; i < s; i++) jEnv->SetObjectArrayElement(jlongA, i, 
               jEnv->NewObject(classRefs[10],instanceMethodIDs[10],u32[i]));
            return jlongA;
         }
         break;
      case CIMTYPE_SINT64: {
            Array<Sint64> s64;
            cv->get(s64);
            int s=s64.size();
            jobjectArray jlongA=(jobjectArray)jEnv->NewObjectArray(s,classRefs[5],0);
            for (int i=0; i < s; i++) jEnv->SetObjectArrayElement(jlongA, i, 
               jEnv->NewObject(classRefs[5],instanceMethodIDs[5],s64[i]));
            return jlongA;
         }
         break;
      case CIMTYPE_UINT64:
         throwCIMException(jEnv,"+++ UnisgnetInt64 not yet supported");
         break;
      case CIMTYPE_REAL32: {
            Array<Real32> r32;
            cv->get(r32);
            int s=r32.size();
            jobjectArray jfloatA=(jobjectArray)jEnv->NewObjectArray(s,classRefs[6],0);
            for (int i=0; i < s; i++) jEnv->SetObjectArrayElement(jfloatA, i, 
               jEnv->NewObject(classRefs[6],instanceMethodIDs[6],r32[i]));
            return jfloatA;
         }
         break;
      case CIMTYPE_REAL64: {
            Array<Real64> r64;
            cv->get(r64);
            int s=r64.size();
            jobjectArray jdoubleA=(jobjectArray)jEnv->NewObjectArray(s,classRefs[7],0);
            for (int i=0; i < s; i++) jEnv->SetObjectArrayElement(jdoubleA, i, 
               jEnv->NewObject(classRefs[7],instanceMethodIDs[7],r64[i]));
            return jdoubleA;
         }
         break;
      case CIMTYPE_STRING: {
            Array<String> str;
            cv->get(str);
            int s=str.size();
            jobjectArray jstringA=(jobjectArray)jEnv->NewObjectArray(s,classRefs[22],0);
            for (int i=0; i < s; i++) jEnv->SetObjectArrayElement(jstringA, i, 
               jEnv->NewStringUTF(str[i].getCString()));
            return jstringA;
         }
         break;
      case CIMTYPE_REFERENCE:
         throwCIMException(jEnv,"+++ Reference not yet supported");
         break;
      case CIMTYPE_CHAR16:
         throwCIMException(jEnv,"+++ Char16 not yet supported");
         break;
      case CIMTYPE_DATETIME:
         throwCIMException(jEnv,"+++ DateTime not yet supported");
         break;
      default:
      throwCIMException(jEnv,"+++ unsupported type: ");
      }
   }
   return NULL;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMValue__1finalize
      (JNIEnv *jEnv, jobject jThs, jint jV) {
   CIMValue *cv=(CIMValue*)jV;
   delete cv;
}


// -------------------------------------
// ---
// -		CIMNameSpace
// ---
// -------------------------------------

_nameSpace::_nameSpace() {
   port_=0;
   hostName_=System::getHostName();
   nameSpace_="root/cimv2";
}

_nameSpace::_nameSpace(String hn) {
   port_=0;
   hostName_=hn;
   nameSpace_="root/cimv2";
}

_nameSpace::_nameSpace(String hn, String ns) {
    port_=0;
    hostName_=hn;
    nameSpace_=ns;
}

int _nameSpace::port() {
   if (port_) return port_;
   port_=5988;
   if (hostName_.subString(0,7)=="http://") {
      protocol_="http://";
      hostName_=hostName_.subString(7);
   }
   Sint32 p=hostName_.reverseFind(':');
   if (p>=0) {
      if (isdigit(hostName_[p+1]))
         port_=atoi(hostName_.subString(p+1).getCString());
      hostName_.remove(p);
   }
   return port_;
}

String _nameSpace::hostName() {
   port();
   return hostName_;
}

String _nameSpace::nameSpace() {
   return nameSpace_;
}


JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMNameSpace__1new
  (JNIEnv *jEnv, jobject jThs) {
  return (jint)(void*) new _nameSpace();
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMNameSpace__1newHn
  (JNIEnv *jEnv, jobject jThs, jstring jHn) {
   const char *hn=jEnv->GetStringUTFChars(jHn,NULL);
   jint cInst=(jint)(void*)new _nameSpace(hn);
   jEnv->ReleaseStringUTFChars(jHn,hn);
   return cInst;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMNameSpace__1newHnNs
  (JNIEnv *jEnv, jobject jThs, jstring jHn, jstring jNs) {
   const char *hn=jEnv->GetStringUTFChars(jHn,NULL);
   const char *ns=jEnv->GetStringUTFChars(jNs,NULL);
   jint cInst=(jint)(void*)new _nameSpace(String(hn),String(ns));
   jEnv->ReleaseStringUTFChars(jHn,hn);
   jEnv->ReleaseStringUTFChars(jNs,ns);
   return cInst;
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMNameSpace__1getHost
  (JNIEnv *jEnv, jobject jThs, jint jNs) {
   _nameSpace *cNs=(_nameSpace*)jNs;
   const String &hn=cNs->hostName_;
   jstring str=jEnv->NewStringUTF(hn.getCString());
   return str;
}

JNIEXPORT jstring JNICALL Java_org_pegasus_jmpi_CIMNameSpace__1getNameSpace
  (JNIEnv *jEnv, jobject jThs, jint jNs) {
   _nameSpace *cNs=(_nameSpace*)jNs;
   const String &ns=cNs->nameSpace_;
   jstring str=jEnv->NewStringUTF(ns.getCString());
   return str;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMNameSpace__1setHost
  (JNIEnv *jEnv, jobject jThs, jint jNs, jstring jHn) {
   _nameSpace *cNs=(_nameSpace*)jNs;
   const char *str=jEnv->GetStringUTFChars(jHn,NULL);
   cNs->port_=0;
   cNs->hostName_=str;
   jEnv->ReleaseStringUTFChars(jHn,str);
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMNameSpace__1setNameSpace
  (JNIEnv *jEnv, jobject jThs, jint jNs, jstring jN) {
   _nameSpace *cNs=(_nameSpace*)jN;
   const char *str=jEnv->GetStringUTFChars(jN,NULL);
   cNs->nameSpace_=str;
   jEnv->ReleaseStringUTFChars(jN,str);
}


// -------------------------------------
// ---
// -		Enumerators
// ---
// -------------------------------------


JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_InstEnumeration__1getInstance
  (JNIEnv *jEnv, jobject jThs, jint jEnum, jint pos) {
   Array<CIMInstance> *enm=(Array<CIMInstance>*)jEnum;
   return (jint) new CIMInstance((*enm)[pos]);
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_InstEnumeration__1size
  (JNIEnv *jEnv, jobject jThs, jint jEnum) {
   Array<CIMInstance> *enm=(Array<CIMInstance>*)jEnum;
   return enm->size();
}


JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_PathEnumeration__1getObjectPath
  (JNIEnv *jEnv, jobject jThs, jint jEnum, jint pos) {
   Array<CIMObjectPath> *enm=(Array<CIMObjectPath>*)jEnum;
   return (jint) new CIMObjectPath((*enm)[pos]);
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_PathEnumeration__1size
  (JNIEnv *jEnv, jobject jThs, jint jEnum) {
   Array<CIMObjectPath> *enm=(Array<CIMObjectPath>*)jEnum;
   return enm->size();
}

// -------------------------------------
// ---
// -		CIMClient
// ---
// -------------------------------------

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1newNaUnPw
  (JNIEnv *jEnv, jobject jThs, jint jNs, jstring jUn, jstring jPw) {
   _nameSpace *cNs=(_nameSpace*)jNs;
   
   const char *un=jEnv->GetStringUTFChars(jUn,NULL);
   const char *pw=jEnv->GetStringUTFChars(jPw,NULL);
   
   try {
      CIMClient *cc=new CIMClient();
      cc->connect(cNs->hostName(),cNs->port(),un,pw);
      jEnv->ReleaseStringUTFChars(jUn,un);
      jEnv->ReleaseStringUTFChars(jPw,pw);
      cout<<"### connect ok "<<(jint)(void*)cc<<endl;
      return (jint)(void*)cc;
   }
   catch (CIMException e) {
      jEnv->ReleaseStringUTFChars(jUn,un);
      jEnv->ReleaseStringUTFChars(jPw,pw);
      throwCimException(jEnv,e);
   }
   catch (...) {
      jEnv->ReleaseStringUTFChars(jUn,un);
      jEnv->ReleaseStringUTFChars(jPw,pw);
      throwFailedException(jEnv);
   }
   return 0;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMClient__1disconnect
  (JNIEnv *jEnv, jobject jThs, jint jCc) {
   CIMClient *cCc=(CIMClient*)jCc;
   try {
      cCc->disconnect();
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
}
  
JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1getClass
  (JNIEnv *jEnv, jobject jThs, jint jCc, int jCop, jboolean lo) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   try {
      CIMClass cls=cCc->getClass(cop->getNameSpace(),cop->getClassName(),(Boolean)lo);
      return (jint) new CIMClass(cls);
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return 0;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMClient__1deleteClass
  (JNIEnv *jEnv, jobject jThs, jint jCc, int jCop) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   try {
      cCc->deleteClass(cop->getNameSpace(),cop->getClassName());
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMClient__1createClass
  (JNIEnv *jEnv, jobject jThs, jint jCc, int jCop, int jCl) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   CIMClass *cl=(CIMClass*)jCl;

   try {
      cCc->createClass(cop->getNameSpace(),*cl);
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMClient__1setClass
  (JNIEnv *jEnv, jobject jThs, jint jCc, jint jCop, jint jCl) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   CIMClass *cl=(CIMClass*)jCl;

   try {
     cCc->modifyClass(cop->getNameSpace(),*cl);
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return ;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1enumerateClasses
  (JNIEnv *jEnv, jobject jThs, jint jCc, int jCop, jboolean deep, jboolean lo) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   try {
      Array<CIMClass> enm=cCc->enumerateClasses(
         cop->getNameSpace(),cop->getClassName(),(Boolean)deep,(Boolean)lo);
      return (jint) new Array<CIMClass>(enm);
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return 0;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1enumerateClassNames
  (JNIEnv *jEnv, jobject jThs, jint jCc, int jCop, jboolean deep) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   CIMNamespaceName ns=cop->getNameSpace();
   try {
      Array<CIMName> enm=cCc->enumerateClassNames(
         ns,cop->getClassName(),(Boolean)deep);
      Array<CIMObjectPath> *enmop=new Array<CIMObjectPath>();
      for (int i=0,m=enm.size(); i<m; i++) {
         enmop->append(CIMObjectPath(String::EMPTY,ns,enm[i]));
      }
      return (jint)enmop;
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return 0;
}



JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1getInstance
  (JNIEnv *jEnv, jobject jThs, jint jCc, int jCop, jboolean lo) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   try {
      CIMInstance inst=cCc->getInstance(cop->getNameSpace(),*cop,(Boolean)lo);
      return (jint) new CIMInstance(inst);
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return 0;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMClient__1deleteInstance
  (JNIEnv *jEnv, jobject jThs, jint jCc, jint jCop) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   try {
      cCc->deleteInstance(cop->getNameSpace(),*cop);
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1createInstance
  (JNIEnv *jEnv, jobject jThs, jint jCc, int jCop, int jCi) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   CIMInstance *ci=(CIMInstance*)jCi;

   try {
      CIMObjectPath obj=cCc->createInstance(cop->getNameSpace(),*ci);
      return (jint) new CIMObjectPath(obj);
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
  return 0;
}

JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMClient__1setInstance
  (JNIEnv *jEnv, jobject jThs, jint jCc, jint jCop, jint jCi) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   CIMInstance *ci=(CIMInstance*)jCi;

   try {
     cCc->modifyInstance(cop->getNameSpace(),*ci);
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return ;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1enumerateInstances
  (JNIEnv *jEnv, jobject jThs, jint jCc, int jCop, jboolean deep, jboolean lo) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   try {
      Array<CIMInstance> enm=cCc->enumerateInstances(
         cop->getNameSpace(),cop->getClassName(),(Boolean)deep,(Boolean)lo);
      return (jint) new Array<CIMInstance>(enm);
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return 0;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1enumerateInstanceNames
  (JNIEnv *jEnv, jobject jThs, jint jCc, int jCop, jboolean deep) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   try {
      Array<CIMObjectPath> enm=cCc->enumerateInstanceNames(
         cop->getNameSpace(),cop->getClassName()); //,(Boolean)deep);
      return (jint) new Array<CIMObjectPath>(enm);
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return 0;
}


JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1getProperty
  (JNIEnv *jEnv, jobject jThs, jint jCc, int jCop, jstring jPn) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   const char *str=jEnv->GetStringUTFChars(jPn,NULL);
   CIMName pName(str);
   jEnv->ReleaseStringUTFChars(jPn,str);
   try {
      CIMValue *val=new CIMValue(cCc->getProperty(cop->getNameSpace(),*cop,pName));
      return (jint)val;
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return 0;
}
   
JNIEXPORT void JNICALL Java_org_pegasus_jmpi_CIMClient__1setProperty
  (JNIEnv *jEnv, jobject jThs, jint jCc, jint jCop, jstring jPn, jint jV) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   CIMValue *val=(CIMValue*)jCop;
   const char *str=jEnv->GetStringUTFChars(jPn,NULL);
   CIMName pName(str);
   jEnv->ReleaseStringUTFChars(jPn,str);
   try {
      cCc->setProperty(cop->getNameSpace(),*cop,pName,*val);
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1invokeMethod
  (JNIEnv *jEnv, jobject jThs, jint jCc, jint jCop, jstring jMn, jobject jIn, jobject jOut) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;
   const char *str=jEnv->GetStringUTFChars(jMn,NULL);
   CIMName method(str);
   jEnv->ReleaseStringUTFChars(jMn,str);

   Array<CIMParamValue> in;
   Array<CIMParamValue> out;

   for (int i=0,m=jEnv->CallIntMethod(jIn,JMPIjvm::jv.VectorSize); i<m; i++) {
       JMPIjvm::checkException(jEnv);
       jobject jProp=jEnv->CallObjectMethod(jIn,JMPIjvm::jv.VectorElementAt,i);
       JMPIjvm::checkException(jEnv);
       CIMProperty *p=((CIMProperty*)jEnv->CallIntMethod(jProp,JMPIjvm::jv.PropertyCInst));
       JMPIjvm::checkException(jEnv);
       in.append(CIMParamValue(p->getName().getString(),p->getValue()));
   }
   try {
      CIMValue *val=new CIMValue(cCc->invokeMethod(cop->getNameSpace(),*cop,method,in,out));
      return (jint)val;

      for (int i=0,m=out.size(); i<m; i++) {
         const CIMParamValue & parm=out[i];
	 const CIMValue v=parm.getValue();
         CIMProperty *p=new CIMProperty(parm.getParameterName(),v,v.getArraySize());
         jobject prop=jEnv->NewObject(JMPIjvm::jv.CIMPropertyClassRef,JMPIjvm::jv.CIMPropertyNewI,(jint)p);
         jEnv->CallVoidMethod(jOut,JMPIjvm::jv.VectorAddElement,prop);
      }
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return 0;
}


JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1execQuery
  (JNIEnv *jEnv, jobject jThs, jint jCc, int jCop, jstring jQuery, jstring jQl) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;

   const char *str=jEnv->GetStringUTFChars(jQuery,NULL);
   String query(str);
   jEnv->ReleaseStringUTFChars(jQuery,str);
   str=jEnv->GetStringUTFChars(jQl,NULL);
   String ql(str);
   jEnv->ReleaseStringUTFChars(jQl,str);

   try {
      Array<CIMObject> enm=cCc->execQuery(
         cop->getNameSpace(),query,ql);
      Array<CIMInstance> *enmInst=new Array<CIMInstance>();
      for (int i=0,m=enm.size(); i<m; i++) {
         enmInst->append(CIMInstance(enm[i]));
      }
      return (jint)enmInst;
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return 0;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1associatorNames
  (JNIEnv *jEnv, jobject jThs, jint jCc, jint jCop,  
   jstring jAssocClass, jstring jResultClass, jstring jRole, jstring jResultRole) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;

   const char *str=jEnv->GetStringUTFChars(jAssocClass,NULL);
   CIMName assocClass(str);
   jEnv->ReleaseStringUTFChars(jAssocClass,str);
   str=jEnv->GetStringUTFChars(jResultClass,NULL);
   CIMName resultClass(str);
   jEnv->ReleaseStringUTFChars(jResultClass,str);
   str=jEnv->GetStringUTFChars(jRole,NULL);
   String role(str);
   jEnv->ReleaseStringUTFChars(jRole,str);
   str=jEnv->GetStringUTFChars(jResultRole,NULL);
   String resultRole(str);
   jEnv->ReleaseStringUTFChars(jResultRole,str);

   try {
      Array<CIMObjectPath> enm=cCc->associatorNames(
         cop->getNameSpace(),*cop,assocClass,resultClass,role,resultRole);
      return (jint) new Array<CIMObjectPath>(enm);
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return 0;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1associators
  (JNIEnv *jEnv, jobject jThs, jint jCc, jint jCop,  
   jstring jAssocClass, jstring jResultClass, jstring jRole, jstring jResultRole,
   jboolean includeQualifiers, jboolean includeClassOrigin, jobjectArray propertyList) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;

   const char *str=jEnv->GetStringUTFChars(jAssocClass,NULL);
   CIMName assocClass(str);
   jEnv->ReleaseStringUTFChars(jAssocClass,str);
   str=jEnv->GetStringUTFChars(jResultClass,NULL);
   CIMName resultClass(str);
   jEnv->ReleaseStringUTFChars(jResultClass,str);
   str=jEnv->GetStringUTFChars(jRole,NULL);
   String role(str);
   jEnv->ReleaseStringUTFChars(jRole,str);
   str=jEnv->GetStringUTFChars(jResultRole,NULL);
   String resultRole(str);
   jEnv->ReleaseStringUTFChars(jResultRole,str);

   try {
      Array<CIMObject> enm=cCc->associators(
         cop->getNameSpace(),*cop,assocClass,resultClass,role,resultRole,
	 (Boolean)includeQualifiers,(Boolean)includeClassOrigin,CIMPropertyList());
      Array<CIMInstance> *enmInst=new Array<CIMInstance>();
      for (int i=0,m=enm.size(); i<m; i++) {
         enmInst->append(CIMInstance(enm[i]));
      }
      return (jint)enmInst;
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return 0;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1referenceNames
  (JNIEnv *jEnv, jobject jThs, jint jCc, jint jCop,
   jstring jAssocClass, jstring jRole) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;

   const char *str=jEnv->GetStringUTFChars(jAssocClass,NULL);
   CIMName assocClass(str);
   jEnv->ReleaseStringUTFChars(jAssocClass,str);
   str=jEnv->GetStringUTFChars(jRole,NULL);
   String role(str);
   jEnv->ReleaseStringUTFChars(jRole,str);

   try {
      Array<CIMObjectPath> enm=cCc->referenceNames(
         cop->getNameSpace(),*cop,assocClass,role);
      return (jint) new Array<CIMObjectPath>(enm);
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return 0;
}

JNIEXPORT jint JNICALL Java_org_pegasus_jmpi_CIMClient__1references
  (JNIEnv *jEnv, jobject jThs, jint jCc, jint jCop,
   jstring jAssocClass, jstring jRole,
   jboolean includeQualifiers, jboolean includeClassOrigin, jobjectArray propertyList) {
   CIMClient *cCc=(CIMClient*)jCc;
   CIMObjectPath *cop=(CIMObjectPath*)jCop;

   const char *str=jEnv->GetStringUTFChars(jAssocClass,NULL);
   CIMName assocClass(str);
   jEnv->ReleaseStringUTFChars(jAssocClass,str);
   str=jEnv->GetStringUTFChars(jRole,NULL);
   String role(str);
   jEnv->ReleaseStringUTFChars(jRole,str);

   try {
      Array<CIMObject> enm=cCc->references(
         cop->getNameSpace(),*cop,assocClass,role,
	 (Boolean)includeQualifiers,(Boolean)includeClassOrigin,CIMPropertyList());
      Array<CIMInstance> *enmInst=new Array<CIMInstance>();
      for (int i=0,m=enm.size(); i<m; i++) {
         enmInst->append(CIMInstance(enm[i]));
      }
      return (jint)enmInst;
   }
   catch (CIMException e) {
      throwCimException(jEnv,e);
   }
   return 0;
}

} // extern "C" 

PEGASUS_NAMESPACE_END







