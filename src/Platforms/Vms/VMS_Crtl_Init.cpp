

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
  set ("DECC$UMASK", 027);
}


int LIB$INITIALIZE();

#pragma extern_model save
#pragma extern_model strict_refdef "LIB$INITIALIZ" nopic,con,rel,gbl,noshr,noexe,nowrt,novec,long
    int spare[8] = {0};

#pragma extern_model strict_refdef "LIB$INITIALIZE" nopic,con,rel,gbl,noshr,noexe,nowrt,novec,long
    void (*x_set_coe)() = set_coe;

#pragma extern_model restore

