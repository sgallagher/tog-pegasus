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
//%/////////////////////////////////////////////////////////////////////////////

#include "CWS_FileUtils.h"
#include "cwsutil.h"
#include <Pegasus/Provider/CMPI/cmpidt.h>
#include <Pegasus/Provider/CMPI/cmpift.h>
#include <Pegasus/Provider/CMPI/cmpimacs.h>

#ifndef PEGASUS_PLATFORM_WIN32_IX86_MSVC
#include <unistd.h>
#endif

#include <string.h>


static char cscn[] = "CIM_UnitaryComputerSystem";
static char fscn[] = "CIM_FileSystem";
static char csn[500] = "";
static char fsn[] = CWS_FILEROOT;

int silentMode()
{
   return SILENT;
}

char * CSCreationClassName()
{
  return cscn;
}

char * CSName()
{
  if (*csn == 0)
#ifdef SIMULATED
    strcpy(csn,"localhost");
#else
    gethostname(csn,sizeof(csn));
#endif
  return csn;
}

char * FSCreationClassName()
{
  return fscn;
}

char * FSName()
{
  return fsn;
}


CMPIObjectPath *makePath( const CMPIBroker *broker, const char * classname,
			 const char * Namespace, CWS_FILE *cwsf)
{
  CMPIObjectPath *op;
  op = CMNewObjectPath(broker,
		       (char*)Namespace,
		       (char*)classname,
		       NULL);  CMSetHostname(op,CSName());
  if (!CMIsNullObject(op)) {
    CMAddKey(op,"CSCreationClassName",CSCreationClassName(),CMPI_chars);
    CMAddKey(op,"CSName",CSName(),CMPI_chars);
    CMAddKey(op,"FSCreationClassName",FSCreationClassName(),CMPI_chars);
    CMAddKey(op,"FSName",FSName(),CMPI_chars);
    CMAddKey(op,"CreationClassName",classname,CMPI_chars);
    CMAddKey(op,"Name",cwsf->cws_name,CMPI_chars);
  }
  return op;
}

CMPIInstance   *makeInstance(const CMPIBroker *broker, const char * classname,
			     const char * Namespace, CWS_FILE *cwsf)
{
  CMPIInstance   *in = NULL;
  CMPIValue       val;
  CMPIObjectPath *op = CMNewObjectPath(broker,
				       (char*)Namespace,
				       (char*)classname,
				       NULL);  CMSetHostname(op,CSName());

  if (!CMIsNullObject(op)) {
    in = CMNewInstance(broker,op,NULL);
    if (!CMIsNullObject(in)) {
      CMSetProperty(in,"CSCreationClassName",CSCreationClassName(),CMPI_chars);
      CMSetProperty(in,"CSName",CSName(),CMPI_chars);
      CMSetProperty(in,"FSCreationClassName",FSCreationClassName(),CMPI_chars);
      CMSetProperty(in,"FSName",FSName(),CMPI_chars);
      CMSetProperty(in,"CreationClassName",classname,CMPI_chars);
      CMSetProperty(in,"Name",cwsf->cws_name,CMPI_chars);
      CMSetProperty(in,"FileSize",(CMPIValue*)&cwsf->cws_size,CMPI_uint64);
#ifndef SIMULATED
/* We don't want this code in the simulated env - time is dynamic (diff timezones)
   and the testing system might using a diff timezone and report failure */
      val.uint64 = cwsf->cws_ctime;
      val.dateTime = CMNewDateTimeFromBinary(broker,val.uint64*1000000,0,NULL);
      CMSetProperty(in,"CreationDate",&val,CMPI_dateTime);
      val.uint64 = cwsf->cws_mtime;
      val.dateTime = CMNewDateTimeFromBinary(broker,val.uint64*1000000,0,NULL);
      CMSetProperty(in,"LastModified",&val,CMPI_dateTime);
      val.uint64 = cwsf->cws_atime;
      val.dateTime = CMNewDateTimeFromBinary(broker,val.uint64*1000000,0,NULL);
      CMSetProperty(in,"LastAccessed",&val,CMPI_dateTime);
#endif
      val.uint64=0L;
      val.boolean=(cwsf->cws_mode & 0400) != 0;
      CMSetProperty(in,"Readable",&val,CMPI_boolean);
      val.boolean=(cwsf->cws_mode & 0200) != 0;
      CMSetProperty(in,"Writeable",&val,CMPI_boolean);
      val.boolean=(cwsf->cws_mode & 0100) != 0;
      CMSetProperty(in,"Executable",&val,CMPI_boolean);
    }
  }
  return in;
}

int makeFileBuf(const CMPIInstance *instance, CWS_FILE *cwsf)
{
  CMPIData dt;
  if (instance && cwsf) {
    dt=CMGetProperty(instance,"Name",NULL);
    strcpy(cwsf->cws_name,CMGetCharPtr(dt.value.string));
    dt=CMGetProperty(instance,"FileSize",NULL);
    cwsf->cws_size=dt.value.uint64;
#ifndef SIMULATED
     dt=CMGetProperty(instance,"CreationDate",NULL);
    cwsf->cws_ctime=CMGetBinaryFormat(dt.value.dateTime,NULL);
    dt=CMGetProperty(instance,"LastModified",NULL);
    cwsf->cws_mtime=CMGetBinaryFormat(dt.value.dateTime,NULL);
    dt=CMGetProperty(instance,"LastAccessed",NULL);
    cwsf->cws_atime=CMGetBinaryFormat(dt.value.dateTime,NULL);
#endif
    dt=CMGetProperty(instance,"Readable",NULL);
    cwsf->cws_mode=dt.value.boolean ? 0400 : 0;
    dt=CMGetProperty(instance,"Writeable",NULL);
    cwsf->cws_mode+=(dt.value.boolean ? 0200 : 0);
    dt=CMGetProperty(instance,"Executable",NULL);
    cwsf->cws_mode+=(dt.value.boolean ? 0100 : 0);
    return 1;
  }
  return 0;
}
