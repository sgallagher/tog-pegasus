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


#include <unixlib.h>
#include <stdio.h>
#include <errno.h>

int decc$feature_get_index(const char *name);
int decc$feature_set_value(int index, int mode, int value);

//
// Sets current value for a feature
//
static void set(char *name, int value)
{
  int index;
  errno = 0;

  index = decc$feature_get_index(name);

  if (index == -1 || 
      (decc$feature_set_value(index, 1, value) == -1 && 
       errno != 0))
  {
    perror(name);
  }
}

static void set_coe(void)
{
  set ("DECC$ARGV_PARSE_STYLE", TRUE);
  set ("DECC$ENABLE_GETENV_CACHE", TRUE);
  set ("DECC$FILE_SHARING", TRUE);
  set ("DECC$DISABLE_TO_VMS_LOGNAME_TRANSLATION", TRUE);
  set ("DECC$EFS_CASE_PRESERVE", TRUE);
  set ("DECC$EFS_CHARSET", TRUE);
  set ("DECC$EFS_FILE_TIMESTAMPS", TRUE);
  set ("DECC$FILENAME_UNIX_NO_VERSION", TRUE);
  set ("DECC$FILENAME_UNIX_REPORT", TRUE);
  set ("DECC$FILE_OWNER_UNIX", TRUE);
  set ("DECC$FILE_PERMISSION_UNIX", TRUE);
  set ("DECC$READDIR_DROPDOTNOTYPE", TRUE);
  set ("DECC$FILENAME_UNIX_ONLY", FALSE);
  set ("DECC$UMASK", 027);
}


int LIB$INITIALIZE();

#pragma extern_model save
#pragma extern_model strict_refdef "LIB$INITIALIZ" nopic,con,rel,gbl,noshr,noexe,nowrt,novec,long
    int spare[8] = {0};

#pragma extern_model strict_refdef "LIB$INITIALIZE" nopic,con,rel,gbl,noshr,noexe,nowrt,novec,long
    void (*x_set_coe)() = set_coe;

#pragma extern_model restore

