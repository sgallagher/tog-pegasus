//%=============================================================================
//
// Copyright (c) 2000 The Open Group, BMC Software, Tivoli Systems, IBM
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
//------------------------------------------------------------------------------
//
// Author: Michael E. Brasher
//
//%=============================================================================

#include <string.h>
#include <ctype.h>

#include <descrip.h>
#include <rms.h>
#include <iodef.h>
#include <atrdef.h>
#include <fibdef.h>

#include <starlet.h>

#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdlib>
#include <stdio>
#include <gen64def.h>
#include <iosbdef.h>
#include <stsdef.h>
#include <lib$routines.h>
#include "Config.h"
#include <sys/types.h>
#include <sys/stat.h>
//include <utime.h>
#include <dirent.h>
#include <fcntl.h>
#include "Files.h"

//+
//
//     Check RMS service service status.  Include both the primary
//     VMS status (typically from 'Status = sys$xxxx' or sts ) and the
//     RMS secondary status (from the stv) in the main message.
//
//-

#define check_rms_status(vms$status, rms$status, text, routine)    \
  if (!$VMS_STATUS_SUCCESS (vms$status))                           \
  {   char msgbuf[256];                                            \
   fprintf (stderr,"RMS %s failed in routine %s %8.8X %8.8X\n",    \
              text,routine, vms$status,rms$status);                \
   fprintf (stderr,"%s\n", msgtxt ((vms$status), msgbuf, 256));    \
   fprintf (stderr,"%s\n", msgtxt ((rms$status), msgbuf, 256));    \
   return 1;                                                       \
  }                                                                \

//+
//
//    Check system service service status.  Print a message and
//    return if an error occurs.  Assumes existence of routine
//    'msgtxt' to return text of message.  Defined as follows -
//
//-

char *msgtxt (int msgid, char *buffer, int buflen);

#define check_status(vms$status, text, routine)                    \
  if (!$VMS_STATUS_SUCCESS (vms$status))                           \
  {   char msgbuf[256];                                            \
   fprintf (stderr,"%s system service failed in routine %s %8.8X\n", \
                text,routine, vms$status);                         \
   fprintf (stderr,"%s\n", msgtxt ((vms$status), msgbuf, 256));    \
   return 1;                                                       \
  }                                                                \

static struct FAB Fab;
static struct NAM Nam;
static struct _fibdef Fib;
static struct _iosb Iosb;

static struct dsc$descriptor FibDesc;
static struct dsc$descriptor_s DevDesc;
static struct dsc$descriptor_s FileName;

static char EName[NAM$C_MAXRSS];
static char RName[NAM$C_MAXRSS];

static unsigned short DevChan;

//char cstr[256];

static struct _generic_64 Cdate,
  Rdate,
  Edate,
  Bdate;
static struct _generic_64 CurTime;

struct IPFatr {
  unsigned short int atr$w_size;
  unsigned short int atr$w_type;
  void *atr$l_addr;
  int fill;
} ;

static struct IPFatr MyAtr;

static int status;
static long int date[2];

char *msgtxt (int, char *, int);

//+
//
// GetCwd
//
//-

bool GetCwd (string & path)
{
  char tmp[4096];

  if (getcwd (tmp, (size_t) sizeof (tmp), 0) == NULL)
  {
    return false;
  }
  path = tmp;
  strcpy (cstr, (char *) path.c_str ());
  return true;
}

//+
//
// ChangeDir
//
//-

bool ChangeDir (const string & path)
{
  strcpy (cstr, (char *) path.c_str ());
  return chdir (path.c_str ()) == 0;
}

//+
//
// RemoveDir
//
//-

bool RemoveDir (const string & path)
{
  strcpy (cstr, (char *) path.c_str ());
  return rmdir (path.c_str ()) == 0;
}

//+
//
// RemoveFile
//
//-

bool RemoveFile (const string & path)
{
  strcpy (cstr, (char *) path.c_str ());
  return remove (path.c_str ()) == 0;
}

//+
//
// MakeDir
//
//-

bool MakeDir (const string & path)
{
  strcpy (cstr, (char *) path.c_str ());
  return mkdir (path.c_str (), 0777) == 0;
}

//+
//
// GetDirEntries
//
//-

bool GetDirEntries (const string & path, vector < string > &filenames)
{
  // 
  // Make a list of all the files at this directory level.
  // 

  strcpy (cstr, (char *) path.c_str ());

  filenames.erase (filenames.begin (), filenames.end ());

  DIR *dir = opendir (path.c_str ());

  if (!dir)
  {
    return false;
  }

  for (dirent * entry = readdir (dir); entry; entry = readdir (dir))
  {
    string name = entry->d_name;

    filenames.push_back (name);
  }

  closedir (dir);

  return true;
}

//+
//
// TouchFile
//
//-

bool TouchFile (const string & path)
{
  int i;

  FibDesc.dsc$w_length = sizeof (Fib);
  FibDesc.dsc$b_dtype = DSC$K_DTYPE_Z;
  FibDesc.dsc$b_class = DSC$K_CLASS_S;
  FibDesc.dsc$a_pointer = (char *) &Fib;

  DevDesc.dsc$b_dtype = DSC$K_DTYPE_T;
  DevDesc.dsc$b_class = DSC$K_CLASS_S;
  DevDesc.dsc$a_pointer = &Nam.nam$t_dvi[1];

  FileName.dsc$b_dtype = DSC$K_DTYPE_T;
  FileName.dsc$b_class = DSC$K_CLASS_S;

  MyAtr.atr$w_size = sizeof (Rdate);
  MyAtr.atr$w_type = ATR$C_REVDATE;
  MyAtr.atr$l_addr = &Rdate;
  MyAtr.fill = 0;

  struct stat sbuf;

  status = sys$gettim (&CurTime);
  check_status (status, "sys$gettim", "TouchFile");

  if (stat (path.c_str (), &sbuf) != 0)
  {
    // File does not exist, create it:

    int fd = open (path.c_str (), O_WRONLY | O_CREAT, 0666);

    if (fd < 0)
    {
      return false;
    }

    close (fd);
    return true;
  }

  // File does exist:

  /* initialize RMS structures, we need a NAM to retrieve the FID */

  Fab = cc$rms_fab;

  Fab.fab$l_fna = (char *) path.c_str ();	/* name of file */
  Fab.fab$b_fns = strlen (path.c_str ());
  Fab.fab$l_nam = &Nam;		/* FAB has an associated NAM */

  Nam = cc$rms_nam;

  Nam.nam$l_esa = EName;	/* expanded filename */
  Nam.nam$b_ess = sizeof (EName);
  Nam.nam$l_rsa = RName;	/* resultant filename */
  Nam.nam$b_rss = sizeof (RName);

  /* do $PARSE and $SEARCH here */

  status = sys$parse (&Fab);

  check_rms_status (status, Fab.fab$l_stv, "sys$parse", "parse_name");

  // Open the file.

  DevDesc.dsc$w_length = Nam.nam$t_dvi[0];

  status = sys$assign (&DevDesc,
		       &DevChan,
		       0, 0);

  check_status (status, "sys$assign", "assign_name");

  // Get current file revision date.

  FileName.dsc$a_pointer = Nam.nam$l_name;
  FileName.dsc$w_length = Nam.nam$b_name + Nam.nam$b_type + Nam.nam$b_ver;

  /* Initialize the FIB */
  for (i = 0; i < 3; i++)
  {
    Fib.fib$w_fid[i] = Nam.nam$w_fid[i];
    Fib.fib$w_did[i] = Nam.nam$w_did[i];
  }

  status = sys$qiow (0,
		     DevChan,
		     IO$_ACCESS,
		     &Iosb,
		     0,
		     0,
		     &FibDesc,
		     (__int64) & FileName,
		     0,
		     0,
		     (__int64) & MyAtr,
		     0);

  if ((status & 1) == 1)
  {
    status = Iosb.iosb$w_status;
  }
  check_status (status, "sys$qio", "get_attr");

  // Get current time.
  Rdate = CurTime;

  // Set new file revision time.

  status = sys$qiow (0,
		     DevChan,
		     IO$_MODIFY,
		     &Iosb,
		     0, 0,
		     &FibDesc,
		     (__int64) & FileName,
		     0,
		     0,
		     (__int64) & MyAtr,
		     0);

  if ((status & 1) == 1)
  {
    status = Iosb.iosb$w_status;
  }
  check_status (status, "sys$qio", "set_attr");

  // Release file.

  status = sys$dassgn (DevChan);

  check_status (status, "sys$dassgn", "deassign_name");

  return true;
}

//+
//
// Get FileSize
//
//-

bool GetFileSize (const string & path, size_t &size)
{
  struct stat st;

  strcpy (cstr, (char *) path.c_str ());
  if (stat (path.c_str (), &st) != 0)
  {
    return false;
  }

  size = (size_t) (st.st_size);
  return true;
}

//+
//
// Exists
//
//-

bool Exists (const string & path)
{
  strcpy (cstr, (char *) path.c_str ());
  return access (path.c_str (), F_OK) == 0;
}

//+
//
// Readable
//
//-

bool Readable (const string & path)
{
  strcpy (cstr, (char *) path.c_str ());
  return access (path.c_str (), R_OK) == 0;
}

//+
//
// Writable
//
//-

bool Writable (const string & path)
{
  strcpy (cstr, (char *) path.c_str ());
  return access (path.c_str (), W_OK) == 0;
}

//+
//
// IsDir
//
//-

bool IsDir (const string & path)
{
  struct stat st;
  strcpy (cstr, (char *) path.c_str ());
  return stat (path.c_str (), &st) == 0 && S_ISDIR (st.st_mode);
}

//+
//
// parse_name
//
//-

bool parse_name (const string & name)
{
  /* initialize RMS structures, we need a NAM to retrieve the FID */

  Fab = cc$rms_fab;

  Fab.fab$l_fna = (char *) name.c_str ();	/* name of file */
  Fab.fab$b_fns = strlen (name.c_str ());
  Fab.fab$l_nam = &Nam;		/* FAB has an associated NAM */

  Nam = cc$rms_nam;

  Nam.nam$l_esa = EName;	/* expanded filename */
  Nam.nam$b_ess = sizeof (EName);
  Nam.nam$l_rsa = RName;	/* resultant filename */
  Nam.nam$b_rss = sizeof (RName);

  /* do $PARSE and $SEARCH here */

  strcpy (cstr, (char *) name.c_str ());
  status = sys$parse (&Fab);

  check_rms_status (status, Fab.fab$l_stv, "sys$parse", "parse_name");
  return 0;
}

//+
//
// assign_name
//
//-

bool assign_name (void)
{
  DevDesc.dsc$w_length = Nam.nam$t_dvi[0];

  status = sys$assign (&DevDesc,
		       &DevChan,
		       0, 0);

  check_status (status, "sys$assign", "assign_name");
  return 0;
}

//+
//
// get_attr
//
//-

bool get_attr (void)
{
  int i;

  FileName.dsc$a_pointer = Nam.nam$l_name;
  FileName.dsc$w_length = Nam.nam$b_name + Nam.nam$b_type + Nam.nam$b_ver;

  /* Initialize the FIB */
  for (i = 0; i < 3; i++)
  {
    Fib.fib$w_fid[i] = Nam.nam$w_fid[i];
    Fib.fib$w_did[i] = Nam.nam$w_did[i];
  }

  FibDesc.dsc$w_length = sizeof (Fib);
  FibDesc.dsc$b_dtype = DSC$K_DTYPE_Z;
  FibDesc.dsc$b_class = DSC$K_CLASS_S;
  FibDesc.dsc$a_pointer = (char *) &Fib;

  MyAtr.atr$w_size = sizeof (Rdate);
  MyAtr.atr$w_type = ATR$C_REVDATE;
  MyAtr.atr$l_addr = &Rdate;
  MyAtr.fill = 0;

  status = sys$qiow (0,
		     DevChan,
		     IO$_ACCESS,
		     &Iosb,
		     0,
		     0,
		     &FibDesc,
		     (__int64) & FileName,
		     0,
		     0,
		     (__int64) & MyAtr,
		     0);

  if ((status & 1) == 1)
  {
    status = Iosb.iosb$w_status;
  }
  check_status (status, "sys$qio", "get_attr");
  return 0;
}

//+
//
// set_attr
//
//-

bool set_attr (void)
{
  int i;

  FileName.dsc$a_pointer = Nam.nam$l_name;
  FileName.dsc$w_length = Nam.nam$b_name + Nam.nam$b_type + Nam.nam$b_ver;

  for (i = 0; i < 3; i++)
  {
    Fib.fib$w_fid[i] = Nam.nam$w_fid[i];
    Fib.fib$w_did[i] = Nam.nam$w_did[i];
  }

  FibDesc.dsc$w_length = sizeof (Fib);
  FibDesc.dsc$b_dtype = DSC$K_DTYPE_Z;
  FibDesc.dsc$b_class = DSC$K_CLASS_S;
  FibDesc.dsc$a_pointer = (char *) &Fib;

  MyAtr.atr$w_size = sizeof (Rdate);
  MyAtr.atr$w_type = ATR$C_REVDATE;
  MyAtr.atr$l_addr = &Rdate;
  MyAtr.fill = 0;

  status = sys$qiow (0,
		     DevChan,
		     IO$_MODIFY,
		     &Iosb,
		     0, 0,
		     &FibDesc,
		     (__int64) & FileName,
		     0, 0,
		     (__int64) & MyAtr,
		     0);

  if ((status & 1) == 1)
  {
    status = Iosb.iosb$w_status;
  }
  check_status (status, "sys$qio", "set_attr");
  return 0;
}

//+
//
// deassign_name
//
//-

bool deassign_name (void)
{

  status = sys$dassgn (DevChan);

  check_status (status, "sys$dassgn", "deassign_name");
  return 0;
}

//+
//
// msgtxt - Get the VMS message text and format as a null-terminated
//          string.
//
//    int msgid;                      VMS message ID
//    char *buffer;                   Buffer to hold text
//    int buflen;                     Length of buffer
//
//-

char *msgtxt (int msgid, char *buffer, int buflen)
{
  int status;
  unsigned short msglen;
  static struct dsc$descriptor bufdsc;
  bufdsc.dsc$w_length = buflen - 1;	// Leave room for null

  bufdsc.dsc$a_pointer = buffer;

  status = sys$getmsg (msgid, &msglen, &bufdsc, 15, 0);
  if (!$VMS_STATUS_SUCCESS (status))
  {
    lib$stop (status);
  }

  buffer[0] = '-';
  buffer[msglen] = '\0';
  return buffer;
}				// end msgtxt
