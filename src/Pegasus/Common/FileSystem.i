#line 1 "FileSystem.cpp"




























#line 1 "../../../src\\Pegasus/Common/Config.h"






































#line 1 "../../../src\\Pegasus/Common/ConfigWindows.h"























































namespace Pegasus {





#line 63 "../../../src\\Pegasus/Common/ConfigWindows.h"





#line 69 "../../../src\\Pegasus/Common/ConfigWindows.h"





#line 75 "../../../src\\Pegasus/Common/ConfigWindows.h"





#line 81 "../../../src\\Pegasus/Common/ConfigWindows.h"





#line 87 "../../../src\\Pegasus/Common/ConfigWindows.h"



typedef unsigned char Uint8;
typedef char Sint8;
typedef unsigned short Uint16;
typedef short Sint16;
typedef unsigned int Uint32;
typedef int Sint32;
typedef float Real32;
typedef double Real64;
typedef bool Boolean;
typedef unsigned __int64 Uint64;
typedef __int64 Sint64;

#pragma warning ( disable : 4251 )



}

#line 109 "../../../src\\Pegasus/Common/ConfigWindows.h"
#line 40 "../../../src\\Pegasus/Common/Config.h"


#line 43 "../../../src\\Pegasus/Common/Config.h"

#line 45 "../../../src\\Pegasus/Common/Config.h"
#line 30 "FileSystem.cpp"


#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"














#pragma once
#line 17 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"






#line 24 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"







#pragma pack(push,8)
#line 33 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"




extern "C" {
#line 39 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"









#line 49 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"
#line 50 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"









#line 60 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"
#line 61 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"














#line 76 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"
#line 77 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"
#line 78 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"






#line 85 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"



typedef unsigned short wchar_t;

#line 91 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"
#line 92 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"


typedef long time_t;		

#line 97 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"


typedef unsigned long _fsize_t; 

#line 102 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"





struct _finddata_t {
    unsigned	attrib;
    time_t	time_create;	
    time_t	time_access;	
    time_t	time_write;
    _fsize_t	size;
    char	name[260];
};


struct _finddatai64_t {
    unsigned	attrib;
    time_t	time_create;	
    time_t	time_access;	
    time_t	time_write;
    __int64	size;
    char	name[260];
};
#line 126 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"


#line 129 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"



struct _wfinddata_t {
    unsigned	attrib;
    time_t	time_create;	
    time_t	time_access;	
    time_t	time_write;
    _fsize_t	size;
    wchar_t	name[260];
};


struct _wfinddatai64_t {
    unsigned	attrib;
    time_t	time_create;	
    time_t	time_access;	
    time_t	time_write;
    __int64	size;
    wchar_t	name[260];
};
#line 151 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"


#line 154 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"










#line 165 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"



__declspec(dllimport) int __cdecl _access(const char *, int);
__declspec(dllimport) int __cdecl _chmod(const char *, int);
__declspec(dllimport) int __cdecl _chsize(int, long);
__declspec(dllimport) int __cdecl _close(int);
__declspec(dllimport) int __cdecl _commit(int);
__declspec(dllimport) int __cdecl _creat(const char *, int);
__declspec(dllimport) int __cdecl _dup(int);
__declspec(dllimport) int __cdecl _dup2(int, int);
__declspec(dllimport) int __cdecl _eof(int);
__declspec(dllimport) long __cdecl _filelength(int);

__declspec(dllimport) long __cdecl _findfirst(const char *, struct _finddata_t *);
__declspec(dllimport) int __cdecl _findnext(long, struct _finddata_t *);
__declspec(dllimport) int __cdecl _findclose(long);
#line 183 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"
__declspec(dllimport) int __cdecl _isatty(int);
__declspec(dllimport) int __cdecl _locking(int, int, long);
__declspec(dllimport) long __cdecl _lseek(int, long, int);
__declspec(dllimport) char * __cdecl _mktemp(char *);
__declspec(dllimport) int __cdecl _open(const char *, int, ...);

__declspec(dllimport) int __cdecl _pipe(int *, unsigned int, int);
#line 191 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"
__declspec(dllimport) int __cdecl _read(int, void *, unsigned int);
__declspec(dllimport) int __cdecl remove(const char *);
__declspec(dllimport) int __cdecl rename(const char *, const char *);
__declspec(dllimport) int __cdecl _setmode(int, int);
__declspec(dllimport) int __cdecl _sopen(const char *, int, int, ...);
__declspec(dllimport) long __cdecl _tell(int);
__declspec(dllimport) int __cdecl _umask(int);
__declspec(dllimport) int __cdecl _unlink(const char *);
__declspec(dllimport) int __cdecl _write(int, const void *, unsigned int);


__declspec(dllimport) __int64 __cdecl _filelengthi64(int);
__declspec(dllimport) long __cdecl _findfirsti64(const char *, struct _finddatai64_t *);
__declspec(dllimport) int __cdecl _findnexti64(long, struct _finddatai64_t *);
__declspec(dllimport) __int64 __cdecl _lseeki64(int, __int64, int);
__declspec(dllimport) __int64 __cdecl _telli64(int);
#line 208 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"






__declspec(dllimport) int __cdecl _waccess(const wchar_t *, int);
__declspec(dllimport) int __cdecl _wchmod(const wchar_t *, int);
__declspec(dllimport) int __cdecl _wcreat(const wchar_t *, int);
__declspec(dllimport) long __cdecl _wfindfirst(const wchar_t *, struct _wfinddata_t *);
__declspec(dllimport) int __cdecl _wfindnext(long, struct _wfinddata_t *);
__declspec(dllimport) int __cdecl _wunlink(const wchar_t *);
__declspec(dllimport) int __cdecl _wrename(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl _wopen(const wchar_t *, int, ...);
__declspec(dllimport) int __cdecl _wsopen(const wchar_t *, int, int, ...);
__declspec(dllimport) wchar_t * __cdecl _wmktemp(wchar_t *);


__declspec(dllimport) long __cdecl _wfindfirsti64(const wchar_t *, struct _wfinddatai64_t *);
__declspec(dllimport) int __cdecl _wfindnexti64(long, struct _wfinddatai64_t *);
#line 229 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"


#line 232 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"
#line 233 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"


__declspec(dllimport) long __cdecl _get_osfhandle(int);
__declspec(dllimport) int __cdecl _open_osfhandle(long, int);

































__declspec(dllimport) int __cdecl access(const char *, int);
__declspec(dllimport) int __cdecl chmod(const char *, int);
__declspec(dllimport) int __cdecl chsize(int, long);
__declspec(dllimport) int __cdecl close(int);
__declspec(dllimport) int __cdecl creat(const char *, int);
__declspec(dllimport) int __cdecl dup(int);
__declspec(dllimport) int __cdecl dup2(int, int);
__declspec(dllimport) int __cdecl eof(int);
__declspec(dllimport) long __cdecl filelength(int);
__declspec(dllimport) int __cdecl isatty(int);
__declspec(dllimport) int __cdecl locking(int, int, long);
__declspec(dllimport) long __cdecl lseek(int, long, int);
__declspec(dllimport) char * __cdecl mktemp(char *);
__declspec(dllimport) int __cdecl open(const char *, int, ...);
__declspec(dllimport) int __cdecl read(int, void *, unsigned int);
__declspec(dllimport) int __cdecl setmode(int, int);
__declspec(dllimport) int __cdecl sopen(const char *, int, int, ...);
__declspec(dllimport) long __cdecl tell(int);
__declspec(dllimport) int __cdecl umask(int);
__declspec(dllimport) int __cdecl unlink(const char *);
__declspec(dllimport) int __cdecl write(int, const void *, unsigned int);

#line 293 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"

#line 295 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"


}
#line 299 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"

#line 301 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"


#pragma pack(pop)
#line 305 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"

#line 307 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\io.h"
#line 33 "FileSystem.cpp"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"














#pragma once
#line 17 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"






#line 24 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"







#pragma pack(push,8)
#line 33 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"


extern "C" {
#line 37 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"













































#line 83 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"






#line 90 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"


typedef unsigned int size_t;

#line 95 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"





struct _diskfree_t {
	unsigned total_clusters;
	unsigned avail_clusters;
	unsigned sectors_per_cluster;
	unsigned bytes_per_sector;
	};


#line 109 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"
#line 110 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"



__declspec(dllimport) int __cdecl _chdir(const char *);
__declspec(dllimport) char * __cdecl _getcwd(char *, int);
__declspec(dllimport) int __cdecl _mkdir(const char *);
__declspec(dllimport) int __cdecl _rmdir(const char *);


__declspec(dllimport) int __cdecl _chdrive(int);
__declspec(dllimport) char * __cdecl _getdcwd(int, char *, int);
__declspec(dllimport) int __cdecl _getdrive(void);
__declspec(dllimport) unsigned long __cdecl _getdrives(void);
__declspec(dllimport) unsigned __cdecl _getdiskfree(unsigned, struct _diskfree_t *);
#line 125 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"







__declspec(dllimport) int __cdecl _wchdir(const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl _wgetcwd(wchar_t *, int);
__declspec(dllimport) wchar_t * __cdecl _wgetdcwd(int, wchar_t *, int);
__declspec(dllimport) int __cdecl _wmkdir(const wchar_t *);
__declspec(dllimport) int __cdecl _wrmdir(const wchar_t *);


#line 140 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"
#line 141 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"















__declspec(dllimport) int __cdecl chdir(const char *);
__declspec(dllimport) char * __cdecl getcwd(char *, int);
__declspec(dllimport) int __cdecl mkdir(const char *);
__declspec(dllimport) int __cdecl rmdir(const char *);

#line 162 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"



#line 166 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"

#line 168 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"


}
#line 172 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"


#pragma pack(pop)
#line 176 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"

#line 178 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\direct.h"
#line 34 "FileSystem.cpp"



#line 38 "FileSystem.cpp"

#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"















#pragma once
#line 18 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"






#line 25 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"



#pragma pack(push,8)
#line 30 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"


extern "C" {
#line 34 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"













































#line 80 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"


#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"















#pragma once
#line 18 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"






#line 25 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"










typedef unsigned short _ino_t;		






typedef unsigned short ino_t;
#line 44 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"
#line 45 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"


#line 48 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"







typedef unsigned int _dev_t;		
#line 57 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"






typedef unsigned int dev_t;
#line 65 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"
#line 66 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"


#line 69 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"




typedef long _off_t;			






typedef long off_t;
#line 82 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"
#line 83 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"


#line 86 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"

#line 88 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/types.h"
#line 83 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"













#line 97 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"






struct _stat {
	_dev_t st_dev;
	_ino_t st_ino;
	unsigned short st_mode;
	short st_nlink;
	short st_uid;
	short st_gid;
	_dev_t st_rdev;
	_off_t st_size;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
	};





struct stat {
	_dev_t st_dev;
	_ino_t st_ino;
	unsigned short st_mode;
	short st_nlink;
	short st_uid;
	short st_gid;
	_dev_t st_rdev;
	_off_t st_size;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
	};

#line 136 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"


struct _stati64 {
	_dev_t st_dev;
	_ino_t st_ino;
	unsigned short st_mode;
	short st_nlink;
	short st_uid;
	short st_gid;
	_dev_t st_rdev;
	__int64 st_size;
	time_t st_atime;
	time_t st_mtime;
	time_t st_ctime;
	};
#line 152 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"


#line 155 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"














__declspec(dllimport) int __cdecl _fstat(int, struct _stat *);
__declspec(dllimport) int __cdecl _stat(const char *, struct _stat *);


__declspec(dllimport) int __cdecl _fstati64(int, struct _stati64 *);
__declspec(dllimport) int __cdecl _stati64(const char *, struct _stati64 *);
#line 176 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"






__declspec(dllimport) int __cdecl _wstat(const wchar_t *, struct _stat *);


__declspec(dllimport) int __cdecl _wstati64(const wchar_t *, struct _stati64 *);
#line 187 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"


#line 190 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"
#line 191 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"




















__declspec(dllimport) int __cdecl fstat(int, struct stat *);
__declspec(dllimport) int __cdecl stat(const char *, struct stat *);
#line 214 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"

#line 216 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"



}
#line 221 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"


#pragma pack(pop)
#line 225 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"

#line 227 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\sys/stat.h"
#line 40 "FileSystem.cpp"

#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdio"








 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"















#pragma once
#line 18 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"






#line 25 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"







#pragma pack(push,8)
#line 34 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"


extern "C" {
#line 38 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"













































#line 84 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"
















typedef wchar_t wint_t;
typedef wchar_t wctype_t;

#line 104 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"
#line 105 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"









typedef char *  va_list;
#line 116 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

#line 118 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"






#line 125 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

#line 127 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"






























#line 158 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"





struct _iobuf {
        char *_ptr;
        int   _cnt;
        char *_base;
        int   _flag;
        int   _file;
        int   _charbuf;
        int   _bufsiz;
        char *_tmpfname;
        };
typedef struct _iobuf FILE;

#line 176 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"











#line 188 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"










#line 199 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"




























#line 228 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"
#line 229 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"



















__declspec(dllimport) extern FILE * __cdecl __p__iob(void);





#line 255 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

#line 257 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

#line 259 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"








typedef __int64 fpos_t;







#line 276 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"


#line 279 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"




























__declspec(dllimport) int __cdecl _filbuf(FILE *);
__declspec(dllimport) int __cdecl _flsbuf(int, FILE *);




__declspec(dllimport) FILE * __cdecl _fsopen(const char *, const char *, int);
#line 315 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

__declspec(dllimport) void __cdecl clearerr(FILE *);
__declspec(dllimport) int __cdecl fclose(FILE *);
__declspec(dllimport) int __cdecl _fcloseall(void);




__declspec(dllimport) FILE * __cdecl _fdopen(int, const char *);
#line 325 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

__declspec(dllimport) int __cdecl feof(FILE *);
__declspec(dllimport) int __cdecl ferror(FILE *);
__declspec(dllimport) int __cdecl fflush(FILE *);
__declspec(dllimport) int __cdecl fgetc(FILE *);
__declspec(dllimport) int __cdecl _fgetchar(void);
__declspec(dllimport) int __cdecl fgetpos(FILE *, fpos_t *);
__declspec(dllimport) char * __cdecl fgets(char *, int, FILE *);




__declspec(dllimport) int __cdecl _fileno(FILE *);
#line 339 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

__declspec(dllimport) int __cdecl _flushall(void);
__declspec(dllimport) FILE * __cdecl fopen(const char *, const char *);
__declspec(dllimport) int __cdecl fprintf(FILE *, const char *, ...);
__declspec(dllimport) int __cdecl fputc(int, FILE *);
__declspec(dllimport) int __cdecl _fputchar(int);
__declspec(dllimport) int __cdecl fputs(const char *, FILE *);
__declspec(dllimport) size_t __cdecl fread(void *, size_t, size_t, FILE *);
__declspec(dllimport) FILE * __cdecl freopen(const char *, const char *, FILE *);
__declspec(dllimport) int __cdecl fscanf(FILE *, const char *, ...);
__declspec(dllimport) int __cdecl fsetpos(FILE *, const fpos_t *);
__declspec(dllimport) int __cdecl fseek(FILE *, long, int);
__declspec(dllimport) long __cdecl ftell(FILE *);
__declspec(dllimport) size_t __cdecl fwrite(const void *, size_t, size_t, FILE *);
__declspec(dllimport) int __cdecl getc(FILE *);
__declspec(dllimport) int __cdecl getchar(void);
__declspec(dllimport) int __cdecl _getmaxstdio(void);
__declspec(dllimport) char * __cdecl gets(char *);
__declspec(dllimport) int __cdecl _getw(FILE *);
__declspec(dllimport) void __cdecl perror(const char *);
__declspec(dllimport) int __cdecl _pclose(FILE *);
__declspec(dllimport) FILE * __cdecl _popen(const char *, const char *);
__declspec(dllimport) int __cdecl printf(const char *, ...);
__declspec(dllimport) int __cdecl putc(int, FILE *);
__declspec(dllimport) int __cdecl putchar(int);
__declspec(dllimport) int __cdecl puts(const char *);
__declspec(dllimport) int __cdecl _putw(int, FILE *);
__declspec(dllimport) int __cdecl remove(const char *);
__declspec(dllimport) int __cdecl rename(const char *, const char *);
__declspec(dllimport) void __cdecl rewind(FILE *);
__declspec(dllimport) int __cdecl _rmtmp(void);
__declspec(dllimport) int __cdecl scanf(const char *, ...);
__declspec(dllimport) void __cdecl setbuf(FILE *, char *);
__declspec(dllimport) int __cdecl _setmaxstdio(int);
__declspec(dllimport) int __cdecl setvbuf(FILE *, char *, int, size_t);
__declspec(dllimport) int __cdecl _snprintf(char *, size_t, const char *, ...);
__declspec(dllimport) int __cdecl sprintf(char *, const char *, ...);
__declspec(dllimport) int __cdecl sscanf(const char *, const char *, ...);
__declspec(dllimport) char * __cdecl _tempnam(const char *, const char *);
__declspec(dllimport) FILE * __cdecl tmpfile(void);
__declspec(dllimport) char * __cdecl tmpnam(char *);
__declspec(dllimport) int __cdecl ungetc(int, FILE *);
__declspec(dllimport) int __cdecl _unlink(const char *);
__declspec(dllimport) int __cdecl vfprintf(FILE *, const char *, va_list);
__declspec(dllimport) int __cdecl vprintf(const char *, va_list);
__declspec(dllimport) int __cdecl _vsnprintf(char *, size_t, const char *, va_list);
__declspec(dllimport) int __cdecl vsprintf(char *, const char *, va_list);








#line 395 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"




__declspec(dllimport) FILE * __cdecl _wfsopen(const wchar_t *, const wchar_t *, int);
#line 401 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

__declspec(dllimport) wint_t __cdecl fgetwc(FILE *);
__declspec(dllimport) wint_t __cdecl _fgetwchar(void);
__declspec(dllimport) wint_t __cdecl fputwc(wint_t, FILE *);
__declspec(dllimport) wint_t __cdecl _fputwchar(wint_t);
__declspec(dllimport) wint_t __cdecl getwc(FILE *);
__declspec(dllimport) wint_t __cdecl getwchar(void);
__declspec(dllimport) wint_t __cdecl putwc(wint_t, FILE *);
__declspec(dllimport) wint_t __cdecl putwchar(wint_t);
__declspec(dllimport) wint_t __cdecl ungetwc(wint_t, FILE *);

__declspec(dllimport) wchar_t * __cdecl fgetws(wchar_t *, int, FILE *);
__declspec(dllimport) int __cdecl fputws(const wchar_t *, FILE *);
__declspec(dllimport) wchar_t * __cdecl _getws(wchar_t *);
__declspec(dllimport) int __cdecl _putws(const wchar_t *);

__declspec(dllimport) int __cdecl fwprintf(FILE *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl wprintf(const wchar_t *, ...);
__declspec(dllimport) int __cdecl _snwprintf(wchar_t *, size_t, const wchar_t *, ...);
__declspec(dllimport) int __cdecl swprintf(wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl vfwprintf(FILE *, const wchar_t *, va_list);
__declspec(dllimport) int __cdecl vwprintf(const wchar_t *, va_list);
__declspec(dllimport) int __cdecl _vsnwprintf(wchar_t *, size_t, const wchar_t *, va_list);
__declspec(dllimport) int __cdecl vswprintf(wchar_t *, const wchar_t *, va_list);
__declspec(dllimport) int __cdecl fwscanf(FILE *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl swscanf(const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl wscanf(const wchar_t *, ...);






__declspec(dllimport) FILE * __cdecl _wfdopen(int, const wchar_t *);
__declspec(dllimport) FILE * __cdecl _wfopen(const wchar_t *, const wchar_t *);
__declspec(dllimport) FILE * __cdecl _wfreopen(const wchar_t *, const wchar_t *, FILE *);
__declspec(dllimport) void __cdecl _wperror(const wchar_t *);
__declspec(dllimport) FILE * __cdecl _wpopen(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl _wremove(const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl _wtempnam(const wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl _wtmpnam(wchar_t *);



#line 446 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"
#line 447 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"


#line 450 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"





















#line 472 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"


























__declspec(dllimport) int __cdecl fcloseall(void);
__declspec(dllimport) FILE * __cdecl fdopen(int, const char *);
__declspec(dllimport) int __cdecl fgetchar(void);
__declspec(dllimport) int __cdecl fileno(FILE *);
__declspec(dllimport) int __cdecl flushall(void);
__declspec(dllimport) int __cdecl fputchar(int);
__declspec(dllimport) int __cdecl getw(FILE *);
__declspec(dllimport) int __cdecl putw(int, FILE *);
__declspec(dllimport) int __cdecl rmtmp(void);
__declspec(dllimport) char * __cdecl tempnam(const char *, const char *);
__declspec(dllimport) int __cdecl unlink(const char *);

#line 511 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

#line 513 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"


}
#line 517 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"



#pragma pack(pop)
#line 522 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"

#line 524 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdio.h"
#line 10 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdio"
#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdio"
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdio"





#line 42 "FileSystem.cpp"
#line 1 "Destroyer.h"
























































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 58 "Destroyer.h"

namespace Pegasus {

template<class T>
class Destroyer
{
public:

    Destroyer(T* ptr) : _ptr(ptr) { }

    ~Destroyer() { delete _ptr; }

    T* getPointer() { return _ptr; }

private:

    Destroyer(const Destroyer&) { }

    Destroyer& operator=(const Destroyer&) { return *this; }

    T* _ptr;
};

template<class T>
class ArrayDestroyer
{
public:

    ArrayDestroyer(T* ptr) : _ptr(ptr) { }

    ~ArrayDestroyer() { delete [] _ptr; }

    T* getPointer() { return _ptr; }

private:

    ArrayDestroyer(const ArrayDestroyer&) { }

    ArrayDestroyer& operator=(const ArrayDestroyer&) { return *this; }

    T* _ptr;
};

}

#line 104 "Destroyer.h"
#line 43 "FileSystem.cpp"
#line 1 "FileSystem.h"








































#line 1 "../../../src\\Pegasus/Common/Config.h"












































#line 42 "FileSystem.h"
#line 1 "../../../src\\Pegasus/Common/String.h"







































#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iostream"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\istream"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ostream"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\ios"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\streambuf"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cerrno"








 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"
















#pragma once
#line 19 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"






#line 26 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"



extern "C" {
#line 31 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"













































#line 77 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"





__declspec(dllimport) extern int * __cdecl _errno(void);



#line 87 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"
















































}
#line 137 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"

#line 139 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\errno.h"
#line 10 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cerrno"
#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cerrno"
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cerrno"





#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\climits"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"















#pragma once
#line 18 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"






#line 25 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"













#line 39 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"
















#line 56 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"





#line 62 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"





#line 68 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"








#line 77 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"








#line 86 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"
































#line 119 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\limits.h"
#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\climits"
#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\climits"





#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdio"
















#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdlib"








 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
















#pragma once
#line 19 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"






#line 26 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"







#pragma pack(push,8)
#line 35 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"


extern "C" {
#line 39 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"













































#line 85 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
































typedef int (__cdecl * _onexit_t)(void);



#line 122 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 124 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"






typedef struct _div_t {
        int quot;
        int rem;
} div_t;

typedef struct _ldiv_t {
        long quot;
        long rem;
} ldiv_t;


#line 142 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"




























__declspec(dllimport) int * __cdecl __p___mb_cur_max(void);



#line 175 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 177 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 179 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"





















#line 201 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"












__declspec(dllimport) int * __cdecl _errno(void);
__declspec(dllimport) unsigned long * __cdecl __doserrno(void);





#line 221 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"



#line 225 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

































































































__declspec(dllimport) extern char * _sys_errlist[];   
__declspec(dllimport) extern int _sys_nerr;   












#line 337 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
#line 338 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"






#line 345 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"




__declspec(dllimport) int *          __cdecl __p___argc(void);
__declspec(dllimport) char ***       __cdecl __p___argv(void);

__declspec(dllimport) wchar_t ***    __cdecl __p___wargv(void);
#line 354 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
__declspec(dllimport) char ***       __cdecl __p__environ(void);

__declspec(dllimport) wchar_t ***    __cdecl __p__wenviron(void);
#line 358 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
__declspec(dllimport) int *          __cdecl __p__fmode(void);
__declspec(dllimport) int *          __cdecl __p__fileinfo(void);
__declspec(dllimport) unsigned int * __cdecl __p__osver(void);
__declspec(dllimport) char **        __cdecl __p__pgmptr(void);

__declspec(dllimport) wchar_t **     __cdecl __p__wpgmptr(void);
#line 365 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
__declspec(dllimport) unsigned int * __cdecl __p__winver(void);
__declspec(dllimport) unsigned int * __cdecl __p__winmajor(void);
__declspec(dllimport) unsigned int * __cdecl __p__winminor(void);































#line 400 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 402 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"



__declspec(dllimport) void   __cdecl abort(void);



#line 410 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
        int    __cdecl abs(int);
#line 412 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
        int    __cdecl atexit(void (__cdecl *)(void));
__declspec(dllimport) double __cdecl atof(const char *);
__declspec(dllimport) int    __cdecl atoi(const char *);
__declspec(dllimport) long   __cdecl atol(const char *);



__declspec(dllimport) void * __cdecl bsearch(const void *, const void *, size_t, size_t,
        int (__cdecl *)(const void *, const void *));
__declspec(dllimport) void * __cdecl calloc(size_t, size_t);
__declspec(dllimport) div_t  __cdecl div(int, int);
__declspec(dllimport) void   __cdecl exit(int);
__declspec(dllimport) void   __cdecl free(void *);
__declspec(dllimport) char * __cdecl getenv(const char *);
__declspec(dllimport) char * __cdecl _itoa(int, char *, int);

__declspec(dllimport) char * __cdecl _i64toa(__int64, char *, int);
__declspec(dllimport) char * __cdecl _ui64toa(unsigned __int64, char *, int);
__declspec(dllimport) __int64 __cdecl _atoi64(const char *);
#line 432 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"


#line 435 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
        long __cdecl labs(long);
#line 437 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
__declspec(dllimport) ldiv_t __cdecl ldiv(long, long);
__declspec(dllimport) char * __cdecl _ltoa(long, char *, int);
__declspec(dllimport) void * __cdecl malloc(size_t);
__declspec(dllimport) int    __cdecl mblen(const char *, size_t);
__declspec(dllimport) size_t __cdecl _mbstrlen(const char *s);
__declspec(dllimport) int    __cdecl mbtowc(wchar_t *, const char *, size_t);
__declspec(dllimport) size_t __cdecl mbstowcs(wchar_t *, const char *, size_t);
__declspec(dllimport) void   __cdecl qsort(void *, size_t, size_t, int (__cdecl *)
        (const void *, const void *));
__declspec(dllimport) int    __cdecl rand(void);
__declspec(dllimport) void * __cdecl realloc(void *, size_t);
__declspec(dllimport) int    __cdecl _set_error_mode(int);
__declspec(dllimport) void   __cdecl srand(unsigned int);
__declspec(dllimport) double __cdecl strtod(const char *, char **);
__declspec(dllimport) long   __cdecl strtol(const char *, char **, int);



__declspec(dllimport) unsigned long __cdecl strtoul(const char *, char **, int);

__declspec(dllimport) int    __cdecl system(const char *);
#line 459 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
__declspec(dllimport) char * __cdecl _ultoa(unsigned long, char *, int);
__declspec(dllimport) int    __cdecl wctomb(char *, wchar_t);
__declspec(dllimport) size_t __cdecl wcstombs(char *, const wchar_t *, size_t);






__declspec(dllimport) wchar_t * __cdecl _itow (int, wchar_t *, int);
__declspec(dllimport) wchar_t * __cdecl _ltow (long, wchar_t *, int);
__declspec(dllimport) wchar_t * __cdecl _ultow (unsigned long, wchar_t *, int);
__declspec(dllimport) double __cdecl wcstod(const wchar_t *, wchar_t **);
__declspec(dllimport) long   __cdecl wcstol(const wchar_t *, wchar_t **, int);
__declspec(dllimport) unsigned long __cdecl wcstoul(const wchar_t *, wchar_t **, int);
__declspec(dllimport) wchar_t * __cdecl _wgetenv(const wchar_t *);
__declspec(dllimport) int    __cdecl _wsystem(const wchar_t *);
__declspec(dllimport) int __cdecl _wtoi(const wchar_t *);
__declspec(dllimport) long __cdecl _wtol(const wchar_t *);

__declspec(dllimport) wchar_t * __cdecl _i64tow(__int64, wchar_t *, int);
__declspec(dllimport) wchar_t * __cdecl _ui64tow(unsigned __int64, wchar_t *, int);
__declspec(dllimport) __int64   __cdecl _wtoi64(const wchar_t *);
#line 483 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"


#line 486 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
#line 487 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"



__declspec(dllimport) char * __cdecl _ecvt(double, int, int *, int *);
__declspec(dllimport) void   __cdecl _exit(int);
__declspec(dllimport) char * __cdecl _fcvt(double, int, int *, int *);
__declspec(dllimport) char * __cdecl _fullpath(char *, const char *, size_t);
__declspec(dllimport) char * __cdecl _gcvt(double, int, char *);
        unsigned long __cdecl _lrotl(unsigned long, int);
        unsigned long __cdecl _lrotr(unsigned long, int);

__declspec(dllimport) void   __cdecl _makepath(char *, const char *, const char *, const char *,
        const char *);
#line 501 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
        _onexit_t __cdecl _onexit(_onexit_t);
__declspec(dllimport) void   __cdecl perror(const char *);
__declspec(dllimport) int    __cdecl _putenv(const char *);
        unsigned int __cdecl _rotl(unsigned int, int);
        unsigned int __cdecl _rotr(unsigned int, int);
__declspec(dllimport) void   __cdecl _searchenv(const char *, const char *, char *);

__declspec(dllimport) void   __cdecl _splitpath(const char *, char *, char *, char *, char *);
#line 510 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
__declspec(dllimport) void   __cdecl _swab(char *, char *, int);






__declspec(dllimport) wchar_t * __cdecl _wfullpath(wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) void   __cdecl _wmakepath(wchar_t *, const wchar_t *, const wchar_t *, const wchar_t *,
        const wchar_t *);
__declspec(dllimport) void   __cdecl _wperror(const wchar_t *);
__declspec(dllimport) int    __cdecl _wputenv(const wchar_t *);
__declspec(dllimport) void   __cdecl _wsearchenv(const wchar_t *, const wchar_t *, wchar_t *);
__declspec(dllimport) void   __cdecl _wsplitpath(const wchar_t *, wchar_t *, wchar_t *, wchar_t *, wchar_t *);


#line 527 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
#line 528 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"




__declspec(dllimport) void __cdecl _seterrormode(int);
__declspec(dllimport) void __cdecl _beep(unsigned, unsigned);
__declspec(dllimport) void __cdecl _sleep(unsigned long);
#line 536 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"


#line 539 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"






__declspec(dllimport) int __cdecl tolower(int);
#line 547 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

__declspec(dllimport) int __cdecl toupper(int);
#line 550 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 552 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"











































__declspec(dllimport) char * __cdecl ecvt(double, int, int *, int *);
__declspec(dllimport) char * __cdecl fcvt(double, int, int *, int *);
__declspec(dllimport) char * __cdecl gcvt(double, int, char *);
__declspec(dllimport) char * __cdecl itoa(int, char *, int);
__declspec(dllimport) char * __cdecl ltoa(long, char *, int);
        _onexit_t __cdecl onexit(_onexit_t);
__declspec(dllimport) int    __cdecl putenv(const char *);
__declspec(dllimport) void   __cdecl swab(char *, char *, int);
__declspec(dllimport) char * __cdecl ultoa(unsigned long, char *, int);

#line 606 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 608 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 610 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"


}

#line 615 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"



#pragma pack(pop)
#line 620 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"

#line 622 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdlib.h"
#line 10 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdlib"
#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdlib"
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdlib"





#line 8 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocnum"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xiosbase"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocale"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\climits"










#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocale"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"








 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"















#pragma once
#line 18 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"






#line 25 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"



extern "C" {
#line 30 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"













































#line 76 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"













#line 90 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"




#line 95 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"
























        void *  __cdecl memcpy(void *, const void *, size_t);
        int     __cdecl memcmp(const void *, const void *, size_t);
        void *  __cdecl memset(void *, int, size_t);
        char *  __cdecl _strset(char *, int);
        char *  __cdecl strcpy(char *, const char *);
        char *  __cdecl strcat(char *, const char *);
        int     __cdecl strcmp(const char *, const char *);
        size_t  __cdecl strlen(const char *);
#line 128 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"
__declspec(dllimport) void *  __cdecl _memccpy(void *, const void *, int, unsigned int);
__declspec(dllimport) void *  __cdecl memchr(const void *, int, size_t);
__declspec(dllimport) int     __cdecl _memicmp(const void *, const void *, unsigned int);





__declspec(dllimport) void *  __cdecl memmove(void *, const void *, size_t);
#line 138 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"


__declspec(dllimport) char *  __cdecl strchr(const char *, int);
__declspec(dllimport) int     __cdecl _strcmpi(const char *, const char *);
__declspec(dllimport) int     __cdecl _stricmp(const char *, const char *);
__declspec(dllimport) int     __cdecl strcoll(const char *, const char *);
__declspec(dllimport) int     __cdecl _stricoll(const char *, const char *);
__declspec(dllimport) int     __cdecl _strncoll(const char *, const char *, size_t);
__declspec(dllimport) int     __cdecl _strnicoll(const char *, const char *, size_t);
__declspec(dllimport) size_t  __cdecl strcspn(const char *, const char *);
__declspec(dllimport) char *  __cdecl _strdup(const char *);
__declspec(dllimport) char *  __cdecl _strerror(const char *);
__declspec(dllimport) char *  __cdecl strerror(int);
__declspec(dllimport) char *  __cdecl _strlwr(char *);
__declspec(dllimport) char *  __cdecl strncat(char *, const char *, size_t);
__declspec(dllimport) int     __cdecl strncmp(const char *, const char *, size_t);
__declspec(dllimport) int     __cdecl _strnicmp(const char *, const char *, size_t);
__declspec(dllimport) char *  __cdecl strncpy(char *, const char *, size_t);
__declspec(dllimport) char *  __cdecl _strnset(char *, int, size_t);
__declspec(dllimport) char *  __cdecl strpbrk(const char *, const char *);
__declspec(dllimport) char *  __cdecl strrchr(const char *, int);
__declspec(dllimport) char *  __cdecl _strrev(char *);
__declspec(dllimport) size_t  __cdecl strspn(const char *, const char *);
__declspec(dllimport) char *  __cdecl strstr(const char *, const char *);
__declspec(dllimport) char *  __cdecl strtok(char *, const char *);
__declspec(dllimport) char *  __cdecl _strupr(char *);
__declspec(dllimport) size_t  __cdecl strxfrm (char *, const char *, size_t);









#line 175 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"





















__declspec(dllimport) void * __cdecl memccpy(void *, const void *, int, unsigned int);
__declspec(dllimport) int __cdecl memicmp(const void *, const void *, unsigned int);
__declspec(dllimport) int __cdecl strcmpi(const char *, const char *);
__declspec(dllimport) int __cdecl stricmp(const char *, const char *);
__declspec(dllimport) char * __cdecl strdup(const char *);
__declspec(dllimport) char * __cdecl strlwr(char *);
__declspec(dllimport) int __cdecl strnicmp(const char *, const char *, size_t);
__declspec(dllimport) char * __cdecl strnset(char *, int, size_t);
__declspec(dllimport) char * __cdecl strrev(char *);
        char * __cdecl strset(char *, int);
__declspec(dllimport) char * __cdecl strupr(char *);

#line 209 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"

#line 211 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"







__declspec(dllimport) wchar_t * __cdecl wcscat(wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcschr(const wchar_t *, wchar_t);
__declspec(dllimport) int __cdecl wcscmp(const wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcscpy(wchar_t *, const wchar_t *);
__declspec(dllimport) size_t __cdecl wcscspn(const wchar_t *, const wchar_t *);
__declspec(dllimport) size_t __cdecl wcslen(const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcsncat(wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) int __cdecl wcsncmp(const wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) wchar_t * __cdecl wcsncpy(wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) wchar_t * __cdecl wcspbrk(const wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcsrchr(const wchar_t *, wchar_t);
__declspec(dllimport) size_t __cdecl wcsspn(const wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcsstr(const wchar_t *, const wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcstok(wchar_t *, const wchar_t *);

__declspec(dllimport) wchar_t * __cdecl _wcsdup(const wchar_t *);
__declspec(dllimport) int __cdecl _wcsicmp(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl _wcsnicmp(const wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) wchar_t * __cdecl _wcsnset(wchar_t *, wchar_t, size_t);
__declspec(dllimport) wchar_t * __cdecl _wcsrev(wchar_t *);
__declspec(dllimport) wchar_t * __cdecl _wcsset(wchar_t *, wchar_t);

__declspec(dllimport) wchar_t * __cdecl _wcslwr(wchar_t *);
__declspec(dllimport) wchar_t * __cdecl _wcsupr(wchar_t *);
__declspec(dllimport) size_t __cdecl wcsxfrm(wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) int __cdecl wcscoll(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl _wcsicoll(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl _wcsncoll(const wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) int __cdecl _wcsnicoll(const wchar_t *, const wchar_t *, size_t);






















__declspec(dllimport) wchar_t * __cdecl wcsdup(const wchar_t *);
__declspec(dllimport) int __cdecl wcsicmp(const wchar_t *, const wchar_t *);
__declspec(dllimport) int __cdecl wcsnicmp(const wchar_t *, const wchar_t *, size_t);
__declspec(dllimport) wchar_t * __cdecl wcsnset(wchar_t *, wchar_t, size_t);
__declspec(dllimport) wchar_t * __cdecl wcsrev(wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcsset(wchar_t *, wchar_t);
__declspec(dllimport) wchar_t * __cdecl wcslwr(wchar_t *);
__declspec(dllimport) wchar_t * __cdecl wcsupr(wchar_t *);
__declspec(dllimport) int __cdecl wcsicoll(const wchar_t *, const wchar_t *);

#line 280 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"

#line 282 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"


#line 285 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"

#line 287 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"


}
#line 291 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"

#line 293 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\string.h"
#line 10 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"
#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"





#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xlocale"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdexcept"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"
















#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"




 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\use_ansi.h"



















#pragma comment(lib,"msvcprtd")


#line 24 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\use_ansi.h"







#line 32 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\use_ansi.h"







#line 40 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\use_ansi.h"

#line 42 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\use_ansi.h"
#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"















#pragma pack(push,8)
#line 22 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"

 #pragma warning(4: 4018 4114 4146 4244 4245)
 #pragma warning(4: 4663 4664 4665)
 #pragma warning(disable: 4237 4284 4290 4514)
		
 
  
  
  
  
 



#line 37 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"
namespace std {
		
 
typedef bool _Bool;
 #line 42 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"
		



		






		
 
  
  
 


#line 61 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"

		
 
class __declspec(dllimport) _Lockit
	{	
public:
  
   
	_Lockit();
	~_Lockit();
  





#line 78 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"
	};
 #line 80 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"
		




};

#pragma pack(pop)
#line 89 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"

#line 91 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\yvals.h"





#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"
#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstddef"




 
 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"















#pragma once
#line 18 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"






#line 25 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"



extern "C" {
#line 30 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"













































#line 76 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"



















__declspec(dllimport) extern int * __cdecl _errno(void);



#line 100 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"





typedef int ptrdiff_t;

#line 108 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"















__declspec(dllimport) extern unsigned long  __cdecl __threadid(void);

__declspec(dllimport) extern unsigned long  __cdecl __threadhandle(void);
#line 127 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"



}
#line 132 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"

#line 134 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stddef.h"
#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstddef"
 


#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstddef"
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstddef"





#line 8 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"















#pragma pack(push,8)
#line 25 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"
namespace std {
		
 
 
 
 
 
 
 
 
 
		
		
 
 
		


 
 
 
 
 
		
enum _Uninitialized {_Noinit};
		
__declspec(dllimport) void __cdecl _Nomemory();
};

#pragma pack(pop)
#line 56 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"

#line 58 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"





#line 18 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"













#pragma once
#line 16 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"






#line 23 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"





#pragma pack(push,8)
#line 30 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"











































typedef void (__cdecl *terminate_function)();
typedef void (__cdecl *unexpected_function)();
typedef void (__cdecl *terminate_handler)();
typedef void (__cdecl *unexpected_handler)();


struct _EXCEPTION_POINTERS;
typedef void (__cdecl *_se_translator_function)(unsigned int, struct _EXCEPTION_POINTERS*);
#line 82 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"

__declspec(dllimport) void __cdecl terminate(void);
__declspec(dllimport) void __cdecl unexpected(void);

__declspec(dllimport) terminate_function __cdecl set_terminate(terminate_function);
__declspec(dllimport) unexpected_function __cdecl set_unexpected(unexpected_function);

__declspec(dllimport) _se_translator_function __cdecl _set_se_translator(_se_translator_function);
#line 91 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"


#pragma pack(pop)
#line 95 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"

#line 97 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\eh.h"
#line 19 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"


#pragma pack(push,8)
#line 23 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"

 

#line 27 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"















typedef const char *__exString;

class __declspec(dllimport) exception
{
public:
    exception();
    exception(const __exString&);
    exception(const exception&);
    exception& operator= (const exception&);
    virtual ~exception();
    virtual __exString what() const;
private:
    __exString _m_what;
    int _m_doFree;
};
namespace std {
using ::exception;

		
class __declspec(dllimport) bad_exception : public exception {
public:
	bad_exception(const char *_S = "bad exception") throw ()
		: exception(_S) {}
	virtual ~bad_exception() throw ()
		{}
protected:
	virtual void _Doraise() const
		{throw (*this); }
	};

__declspec(dllimport) bool __cdecl uncaught_exception();

};






#pragma pack(pop)
#line 83 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"

#line 85 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"






#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\stdexcept"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstring"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdlib"
















#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\exception"


























































































#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"


#pragma pack(push,8)
#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"
namespace std {
		
class __declspec(dllimport) bad_alloc : public exception {
public:
	bad_alloc(const char *_S = "bad allocation") throw ()
		: exception(_S) {}
	virtual ~bad_alloc() throw ()
		{}
protected:
	virtual void _Doraise() const
		{throw (*this); }
	};
		
struct nothrow_t {};
extern __declspec(dllimport) const nothrow_t nothrow;
};

typedef void (__cdecl *new_handler)();
extern new_handler _New_hand;

		
void __cdecl operator delete(void *) throw ();
void *__cdecl operator new(size_t) throw (std::bad_alloc);
void *__cdecl operator new(size_t, const std::nothrow_t&)
	throw ();



inline void *__cdecl operator new(size_t, void *_P)
	{return (_P); }
#line 40 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"


__declspec(dllimport)
#line 44 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"
	new_handler __cdecl set_new_handler(new_handler) throw ();


#pragma pack(pop)
#line 49 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"

#line 51 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\new"





#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"


#pragma pack(push,8)
#line 10 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"
 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\utility"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"



#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstdio"
















#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cstring"
















#line 6 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cwchar"




 
 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


















#pragma once
#line 21 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"








#line 30 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"



#pragma pack(push,8)
#line 35 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


extern "C" {
#line 39 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"





















































#line 93 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


























































































































































__declspec(dllimport) extern unsigned short _ctype[];




__declspec(dllimport) unsigned short ** __cdecl __p__pctype(void);


__declspec(dllimport) wctype_t ** __cdecl ___p__pwctype(void);






#line 263 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"

#line 265 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"








                        
















__declspec(dllimport) int __cdecl iswalpha(wint_t);
__declspec(dllimport) int __cdecl iswupper(wint_t);
__declspec(dllimport) int __cdecl iswlower(wint_t);
__declspec(dllimport) int __cdecl iswdigit(wint_t);
__declspec(dllimport) int __cdecl iswxdigit(wint_t);
__declspec(dllimport) int __cdecl iswspace(wint_t);
__declspec(dllimport) int __cdecl iswpunct(wint_t);
__declspec(dllimport) int __cdecl iswalnum(wint_t);
__declspec(dllimport) int __cdecl iswprint(wint_t);
__declspec(dllimport) int __cdecl iswgraph(wint_t);
__declspec(dllimport) int __cdecl iswcntrl(wint_t);
__declspec(dllimport) int __cdecl iswascii(wint_t);
__declspec(dllimport) int __cdecl isleadbyte(int);

__declspec(dllimport) wchar_t __cdecl towupper(wchar_t);
__declspec(dllimport) wchar_t __cdecl towlower(wchar_t);

__declspec(dllimport) int __cdecl iswctype(wint_t, wctype_t);


__declspec(dllimport) int __cdecl is_wctype(wint_t, wctype_t);



#line 315 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"









































__declspec(dllimport) wchar_t * __cdecl _wsetlocale(int, const wchar_t *);


#line 360 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"





__declspec(dllimport) int __cdecl _wexecl(const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wexecle(const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wexeclp(const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wexeclpe(const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wexecv(const wchar_t *, const wchar_t * const *);
__declspec(dllimport) int __cdecl _wexecve(const wchar_t *, const wchar_t * const *, const wchar_t * const *);
__declspec(dllimport) int __cdecl _wexecvp(const wchar_t *, const wchar_t * const *);
__declspec(dllimport) int __cdecl _wexecvpe(const wchar_t *, const wchar_t * const *, const wchar_t * const *);
__declspec(dllimport) int __cdecl _wspawnl(int, const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wspawnle(int, const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wspawnlp(int, const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wspawnlpe(int, const wchar_t *, const wchar_t *, ...);
__declspec(dllimport) int __cdecl _wspawnv(int, const wchar_t *, const wchar_t * const *);
__declspec(dllimport) int __cdecl _wspawnve(int, const wchar_t *, const wchar_t * const *,
        const wchar_t * const *);
__declspec(dllimport) int __cdecl _wspawnvp(int, const wchar_t *, const wchar_t * const *);
__declspec(dllimport) int __cdecl _wspawnvpe(int, const wchar_t *, const wchar_t * const *,
        const wchar_t * const *);
__declspec(dllimport) int __cdecl _wsystem(const wchar_t *);


#line 387 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"



















inline int __cdecl iswalpha(wint_t _C) {return (iswctype(_C,(0x0100|0x1|0x2))); }
inline int __cdecl iswupper(wint_t _C) {return (iswctype(_C,0x1)); }
inline int __cdecl iswlower(wint_t _C) {return (iswctype(_C,0x2)); }
inline int __cdecl iswdigit(wint_t _C) {return (iswctype(_C,0x4)); }
inline int __cdecl iswxdigit(wint_t _C) {return (iswctype(_C,0x80)); }
inline int __cdecl iswspace(wint_t _C) {return (iswctype(_C,0x8)); }
inline int __cdecl iswpunct(wint_t _C) {return (iswctype(_C,0x10)); }
inline int __cdecl iswalnum(wint_t _C) {return (iswctype(_C,(0x0100|0x1|0x2)|0x4)); }
inline int __cdecl iswprint(wint_t _C)
	{return (iswctype(_C,0x40|0x10|(0x0100|0x1|0x2)|0x4)); }
inline int __cdecl iswgraph(wint_t _C)
	{return (iswctype(_C,0x10|(0x0100|0x1|0x2)|0x4)); }
inline int __cdecl iswcntrl(wint_t _C) {return (iswctype(_C,0x20)); }
inline int __cdecl iswascii(wint_t _C) {return ((unsigned)(_C) < 0x80); }

inline int __cdecl isleadbyte(int _C)
	{return ((*__p__pctype())[(unsigned char)(_C)] & 0x8000); }
#line 424 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"

#line 426 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"































































































































#line 554 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"










































































































#line 661 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"

















































































struct tm {
        int tm_sec;     
        int tm_min;     
        int tm_hour;    
        int tm_mday;    
        int tm_mon;     
        int tm_year;    
        int tm_wday;    
        int tm_yday;    
        int tm_isdst;   
        };

#line 755 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"





__declspec(dllimport) wchar_t * __cdecl _wasctime(const struct tm *);
__declspec(dllimport) wchar_t * __cdecl _wctime(const time_t *);
__declspec(dllimport) size_t __cdecl wcsftime(wchar_t *, size_t, const wchar_t *,
        const struct tm *);
__declspec(dllimport) wchar_t * __cdecl _wstrdate(wchar_t *);
__declspec(dllimport) wchar_t * __cdecl _wstrtime(wchar_t *);


#line 769 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"



typedef int mbstate_t;
typedef wchar_t _Wint_t;

__declspec(dllimport) wint_t __cdecl btowc(int);
__declspec(dllimport) size_t __cdecl mbrlen(const char *, size_t, mbstate_t *);
__declspec(dllimport) size_t __cdecl mbrtowc(wchar_t *, const char *, size_t, mbstate_t *);
__declspec(dllimport) size_t __cdecl mbsrtowcs(wchar_t *, const char **, size_t, mbstate_t *);

__declspec(dllimport) size_t __cdecl wcrtomb(char *, wchar_t, mbstate_t *);
__declspec(dllimport) size_t __cdecl wcsrtombs(char *, const wchar_t **, size_t, mbstate_t *);
__declspec(dllimport) int __cdecl wctob(wint_t);


inline int fwide(FILE *, int _M)
	{return (_M); }
inline int mbsinit(const mbstate_t *)
	{return (1); }
inline const wchar_t *wmemchr(const wchar_t *_S, wchar_t _C, size_t _N)
	{if (0) ; else for (; 0 < _N; ++_S, --_N)
		if (*_S == _C)
			return (_S);
	return (0); }
inline int wmemcmp(const wchar_t *_S1, const wchar_t *_S2, size_t _N)
	{if (0) ; else for (; 0 < _N; ++_S1, ++_S2, --_N)
		if (*_S1 != *_S2)
			return (*_S1 < *_S2 ? -1 : +1);
	return (0); }
inline wchar_t *wmemcpy(wchar_t *_S1, const wchar_t *_S2, size_t _N)
	{wchar_t *_Su1 = _S1;
	if (0) ; else for (; 0 < _N; ++_Su1, ++_S2, --_N)
		*_Su1 = *_S2;
	return (_S1); }
inline wchar_t *wmemmove(wchar_t *_S1, const wchar_t *_S2, size_t _N)
	{wchar_t *_Su1 = _S1;
	if (_S2 < _Su1 && _Su1 < _S2 + _N)
		if (0) ; else for (_Su1 += _N, _S2 += _N; 0 < _N; --_N)
			*--_Su1 = *--_S2;
	else
		if (0) ; else for (; 0 < _N; --_N)
			*_Su1++ = *_S2++;
	return (_S1); }
inline wchar_t *wmemset(wchar_t *_S, wchar_t _C, size_t _N)
	{wchar_t *_Su = _S;
	if (0) ; else for (; 0 < _N; ++_Su, --_N)
		*_Su = _C;
	return (_S); }
}	
inline wchar_t *wmemchr(wchar_t *_S, wchar_t _C, size_t _N)
	{return ((wchar_t *)wmemchr((const wchar_t *)_S, _C, _N)); }
inline wchar_t *wcschr(wchar_t *_S, wchar_t _C)
	{return ((wchar_t *)wcschr((const wchar_t *)_S, _C)); }
inline wchar_t *wcspbrk(wchar_t *_S, const wchar_t *_P)
	{return ((wchar_t *)wcspbrk((const wchar_t *)_S, _P)); }
inline wchar_t *wcsrchr(wchar_t *_S, wchar_t _C)
	{return ((wchar_t *)wcsrchr((const wchar_t *)_S, _C)); }
inline wchar_t *wcsstr(wchar_t *_S, const wchar_t *_P)
	{return ((wchar_t *)wcsstr((const wchar_t *)_S, _P)); }
#line 830 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"


#pragma pack(pop)
#line 834 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"

#line 836 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"













#line 850 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\wchar.h"
#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cwchar"
 


#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cwchar"
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\cwchar"





#line 7 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"
#line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstddef"






























































#line 8 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"


#pragma pack(push,8)
#line 12 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"
namespace std {
                
typedef long streamoff;
const streamoff _BADOFF = -1;
typedef int streamsize;
extern __declspec(dllimport) const fpos_t _Fpz;
                
template<class _St>
        class fpos {
        typedef fpos<_St> _Myt;
public:

        fpos(streamoff _O = 0)
                : _Off(_O), _Fpos(_Fpz)
                {_Lockit _Lk;
                _State = _Stz; }



#line 32 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"
        fpos(_St _S, fpos_t _F)
                : _Off(0), _Fpos(_F), _State(_S) {}
        _St state() const
                {return (_State); }
        void state(_St _S)
                {_State = _S; }
        fpos_t get_fpos_t() const
                {return (_Fpos); }
        operator streamoff() const
                {return (_Off + ((long)(_Fpos))); }
        streamoff operator-(const _Myt& _R) const
                {return ((streamoff)*this - (streamoff)_R); }
        _Myt& operator+=(streamoff _O)
                {_Off += _O;
                return (*this); }
        _Myt& operator-=(streamoff _O)
                {_Off -= _O;
                return (*this); }
        _Myt operator+(streamoff _O) const
                {_Myt _Tmp = *this;
                return (_Tmp += _O); }
        _Myt operator-(streamoff _O) const
                {_Myt _Tmp = *this;
                return (_Tmp -= _O); }
        bool operator==(const _Myt& _R) const
                {return ((streamoff)*this == (streamoff)_R); }
        bool operator!=(const _Myt& _R) const
                {return (!(*this == _R)); }
private:
        static _St _Stz;
        streamoff _Off;
        fpos_t _Fpos;
        _St _State;
        };
template<class _St>
        _St fpos<_St>::_Stz;
typedef fpos<mbstate_t> streampos;
typedef streampos wstreampos;
                
template<class _E>
        struct char_traits {
        typedef _E char_type;
        typedef _E int_type;
        typedef streampos pos_type;
        typedef streamoff off_type;
        typedef mbstate_t state_type;
        static void __cdecl assign(_E& _X, const _E& _Y)
                {_X = _Y; }
        static bool __cdecl eq(const _E& _X, const _E& _Y)
                {return (_X == _Y); }
        static bool __cdecl lt(const _E& _X, const _E& _Y)
                {return (_X < _Y); }
        static int __cdecl compare(const _E *_U, const _E *_V, size_t _N)
                {if (0) ; else for (size_t _I = 0; _I < _N; ++_I, ++_U, ++_V)
                        if (!eq(*_U, *_V))
                                return (lt(*_U, *_V) ? -1 : +1);
                return (0); }
        static size_t __cdecl length(const _E *_U)
                {size_t _N;
                if (0) ; else for (_N = 0; !eq(*_U, _E(0)); ++_U)
                        ++_N;
                return (_N); }
        static _E * __cdecl copy(_E *_U, const _E *_V, size_t _N)
                {_E *_S = _U;
                if (0) ; else for (; 0 < _N; --_N, ++_U, ++_V)
                        assign(*_U, *_V);
                return (_S); }
        static const _E * __cdecl find(const _E *_U, size_t _N,
                const _E& _C)
                {if (0) ; else for (; 0 < _N; --_N, ++_U)
                        if (eq(*_U, _C))
                                return (_U);
                return (0); }
        static _E * __cdecl move(_E *_U, const _E *_V, size_t _N)
                {_E *_Ans = _U;
                if (_V < _U && _U < _V + _N)
                        if (0) ; else for (_U += _N, _V += _N; 0 < _N; --_N)
                                assign(*--_U, *--_V);
                else
                        if (0) ; else for (; 0 < _N; --_N, ++_U, ++_V)
                                assign(*_U, *_V);
                return (_Ans); }
        static _E * __cdecl assign(_E *_U, size_t _N, const _E& _C)
                {_E *_Ans = _U;
                if (0) ; else for (; 0 < _N; --_N, ++_U)
                        assign(*_U, _C);
                return (_Ans); }
        static _E __cdecl to_char_type(const int_type& _C)
                {return (_C); }
        static int_type __cdecl to_int_type(const _E& _C)
                {return (_C); }
        static bool __cdecl eq_int_type(const int_type& _X,
                const int_type& _Y)
                {return (_X == _Y); }
        static int_type __cdecl eof()
                {return ((-1)); }
        static int_type __cdecl not_eof(const int_type& _C)
                {return (_C != eof() ? _C : !eof()); }
        };
                
struct __declspec(dllimport) char_traits<wchar_t> {
        typedef wchar_t _E;
        typedef _E char_type;   
        typedef wint_t int_type;
        typedef streampos pos_type;
        typedef streamoff off_type;
        typedef mbstate_t state_type;
        static void __cdecl assign(_E& _X, const _E& _Y)
                {_X = _Y; }
        static bool __cdecl eq(const _E& _X, const _E& _Y)
                {return (_X == _Y); }
        static bool __cdecl lt(const _E& _X, const _E& _Y)
                {return (_X < _Y); }
        static int __cdecl compare(const _E *_U, const _E *_V, size_t _N)
                {return (wmemcmp(_U, _V, _N)); }
        static size_t __cdecl length(const _E *_U)
                {return (wcslen(_U)); }
        static _E *__cdecl copy(_E *_U, const _E *_V, size_t _N)
                {return (wmemcpy(_U, _V, _N)); }
        static const _E * __cdecl find(const _E *_U, size_t _N,
                const _E& _C)
                {return ((const _E *)wmemchr(_U, _C, _N)); }
        static _E * __cdecl move(_E *_U, const _E *_V, size_t _N)
                {return (wmemmove(_U, _V, _N)); }
        static _E * __cdecl assign(_E *_U, size_t _N, const _E& _C)
                {return (wmemset(_U, _C, _N)); }
        static _E __cdecl to_char_type(const int_type& _C)
                {return (_C); }
        static int_type __cdecl to_int_type(const _E& _C)
                {return (_C); }
        static bool __cdecl eq_int_type(const int_type& _X,
                const int_type& _Y)
                {return (_X == _Y); }
        static int_type __cdecl eof()
                {return ((wint_t)(0xFFFF)); }
        static int_type __cdecl not_eof(const int_type& _C)
                {return (_C != eof() ? _C : !eof()); }
        };
                
struct __declspec(dllimport) char_traits<char> {
        typedef char _E;
        typedef _E char_type;
        typedef int int_type;
        typedef streampos pos_type;
        typedef streamoff off_type;
        typedef mbstate_t state_type;
        static void __cdecl assign(_E& _X, const _E& _Y)
                {_X = _Y; }
        static bool __cdecl eq(const _E& _X, const _E& _Y)
                {return (_X == _Y); }
        static bool __cdecl lt(const _E& _X, const _E& _Y)
                {return (_X < _Y); }
        static int __cdecl compare(const _E *_U, const _E *_V, size_t _N)
                {return (memcmp(_U, _V, _N)); }
        static size_t __cdecl length(const _E *_U)
                {return (strlen(_U)); }
        static _E * __cdecl copy(_E *_U, const _E *_V, size_t _N)
                {return ((_E *)memcpy(_U, _V, _N)); }
        static const _E * __cdecl find(const _E *_U, size_t _N,
                const _E& _C)
                {return ((const _E *)memchr(_U, _C, _N)); }
        static _E * __cdecl move(_E *_U, const _E *_V, size_t _N)
                {return ((_E *)memmove(_U, _V, _N)); }
        static _E * __cdecl assign(_E *_U, size_t _N, const _E& _C)
                {return ((_E *)memset(_U, _C, _N)); }
        static _E __cdecl to_char_type(const int_type& _C)
                {return (_C); }
        static int_type __cdecl to_int_type(const _E& _C)
                {return ((unsigned char)_C); }
        static bool __cdecl eq_int_type(const int_type& _X,
                const int_type& _Y)
                {return (_X == _Y); }
        static int_type __cdecl eof()
                {return ((-1)); }
        static int_type __cdecl not_eof(const int_type& _C)
                {return (_C != eof() ? _C : !eof()); }
        };
                
template<class T>
        class allocator;
class ios_base;
template<class _E, class _Tr = char_traits<_E> >
        class basic_ios;
template<class _E, class _Tr = char_traits<_E> >
        class istreambuf_iterator;
template<class _E, class _Tr = char_traits<_E> >
        class ostreambuf_iterator;
template<class _E, class _Tr = char_traits<_E> >
        class basic_streambuf;
template<class _E, class _Tr = char_traits<_E> >
        class basic_istream;
template<class _E, class _Tr = char_traits<_E> >
        class basic_ostream;
template<class _E, class _Tr = char_traits<_E> >
        class basic_iostream;
template<class _E, class _Tr = char_traits<_E>,
        class _A = allocator<_E> >
        class basic_stringbuf;
template<class _E, class _Tr = char_traits<_E>,
        class _A = allocator<_E> >
        class basic_istringstream;
template<class _E, class _Tr = char_traits<_E>,
        class _A = allocator<_E> >
        class basic_ostringstream;
template<class _E, class _Tr = char_traits<_E>,
        class _A = allocator<_E> >
        class basic_stringstream;
template<class _E, class _Tr = char_traits<_E> >
        class basic_filebuf;
template<class _E, class _Tr = char_traits<_E> >
        class basic_ifstream;
template<class _E, class _Tr = char_traits<_E> >
        class basic_ofstream;
template<class _E, class _Tr = char_traits<_E> >
        class basic_fstream;


template<class _E, class _II >
    class num_get;
template<class _E, class _OI >
    class num_put;

template<class _E>
    class collate;

#pragma warning(disable:4231) 
#pragma warning(disable:4662)

extern template class __declspec(dllimport) basic_ios<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_streambuf<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >;


extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >& __cdecl operator>>(
       basic_istream<char, char_traits<char> >&, char *);
extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >& __cdecl operator>>(
       basic_istream<char, char_traits<char> >&, char&);
extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >& __cdecl operator>>(
       basic_istream<char, char_traits<char> >&, signed char *);
extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >& __cdecl operator>>(
       basic_istream<char, char_traits<char> >&, signed char&);
extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >& __cdecl operator>>(
       basic_istream<char, char_traits<char> >&, unsigned char *);
extern template class __declspec(dllimport) basic_istream<char, char_traits<char> >& __cdecl operator>>(
       basic_istream<char, char_traits<char> >&, unsigned char&);
extern template class __declspec(dllimport) basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
       basic_istream<wchar_t, char_traits<wchar_t> >&, wchar_t *);
extern template class __declspec(dllimport) basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
       basic_istream<wchar_t, char_traits<wchar_t> >&, wchar_t&);
extern template class __declspec(dllimport) basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
       basic_istream<wchar_t, char_traits<wchar_t> >&, signed short *);

extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >& __cdecl operator<<(
       basic_ostream<char, char_traits<char> >&, const char *);
extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >& __cdecl operator<<(
       basic_ostream<char, char_traits<char> >&, char);
extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >& __cdecl operator<<(
       basic_ostream<char, char_traits<char> >&, const signed char *);
extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >& __cdecl operator<<(
       basic_ostream<char, char_traits<char> >&, const signed char);
extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >& __cdecl operator<<(
       basic_ostream<char, char_traits<char> >&, const unsigned char *);
extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >& __cdecl operator<<(
       basic_ostream<char, char_traits<char> >&, const unsigned char);
extern template class __declspec(dllimport) basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
       basic_ostream<wchar_t, char_traits<wchar_t> >&, const wchar_t *);
extern template class __declspec(dllimport) basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
       basic_ostream<wchar_t, char_traits<wchar_t> >&, const wchar_t);
extern template class __declspec(dllimport) basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
       basic_ostream<wchar_t, char_traits<wchar_t> >&, const signed short *);

template<class _E, class _Tr = char_traits<_E>, class _A = allocator<_E> >
    class basic_string;
extern template class __declspec(dllimport)
    basic_string<char, char_traits<char>, allocator<char> > __cdecl operator+(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template class __declspec(dllimport)
    basic_string<char, char_traits<char>, allocator<char> > __cdecl operator+(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template class __declspec(dllimport)
    basic_string<char, char_traits<char>, allocator<char> > __cdecl operator+(
        const char, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template class __declspec(dllimport)
    basic_string<char, char_traits<char>, allocator<char> > __cdecl operator+(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template class __declspec(dllimport)
    basic_string<char, char_traits<char>, allocator<char> > __cdecl operator+(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char);
extern template __declspec(dllimport) bool __cdecl operator==(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator==(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator==(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template __declspec(dllimport) bool __cdecl operator!=(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator!=(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator!=(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template __declspec(dllimport) bool __cdecl operator<(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator<(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator<(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template __declspec(dllimport) bool __cdecl operator>(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator>(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator>(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template __declspec(dllimport) bool __cdecl operator<=(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator<=(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator<=(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template __declspec(dllimport) bool __cdecl operator>=(
        const basic_string<char, char_traits<char>, allocator<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator>=(
        const char *, const basic_string<char, char_traits<char>, allocator<char> >&);
extern template __declspec(dllimport) bool __cdecl operator>=(
        const basic_string<char, char_traits<char>, allocator<char> >&, const char *);
extern template class __declspec(dllimport)
    basic_istream<char, char_traits<char> >& __cdecl operator>>(
        basic_istream<char, char_traits<char> >&,
        basic_string<char, char_traits<char>, allocator<char> >&);
extern template class __declspec(dllimport)
    basic_istream<char, char_traits<char> >& __cdecl getline(
        basic_istream<char, char_traits<char> >&,
        basic_string<char, char_traits<char>, allocator<char> >&);
extern template class __declspec(dllimport)
    basic_istream<char, char_traits<char> >& __cdecl getline(
        basic_istream<char, char_traits<char> >&,
        basic_string<char, char_traits<char>, allocator<char> >&, const char);
extern template class __declspec(dllimport)
    basic_ostream<char, char_traits<char> >& __cdecl operator<<(
        basic_ostream<char, char_traits<char> >&,
        const basic_string<char, char_traits<char>, allocator<char> >&);

extern template class __declspec(dllimport)
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > __cdecl operator+(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template class __declspec(dllimport)
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > __cdecl operator+(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template class __declspec(dllimport)
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > __cdecl operator+(
        const wchar_t, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template class __declspec(dllimport)
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > __cdecl operator+(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template class __declspec(dllimport)
    basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> > __cdecl operator+(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t);
extern template __declspec(dllimport) bool __cdecl operator==(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator==(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator==(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template __declspec(dllimport) bool __cdecl operator!=(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator!=(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator!=(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template __declspec(dllimport) bool __cdecl operator<(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator<(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator<(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template __declspec(dllimport) bool __cdecl operator>(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator>(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator>(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template __declspec(dllimport) bool __cdecl operator<=(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator<=(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator<=(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template __declspec(dllimport) bool __cdecl operator>=(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator>=(
        const wchar_t *, const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template __declspec(dllimport) bool __cdecl operator>=(
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t *);
extern template class __declspec(dllimport)
    basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
        basic_istream<wchar_t, char_traits<wchar_t> >&,
        basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template class __declspec(dllimport)
    basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl getline(
        basic_istream<wchar_t, char_traits<wchar_t> >&,
        basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);
extern template class __declspec(dllimport)
    basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl getline(
        basic_istream<wchar_t, char_traits<wchar_t> >&,
        basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&, const wchar_t);
extern template class __declspec(dllimport)
    basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
        basic_ostream<wchar_t, char_traits<wchar_t> >&,
        const basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >&);

template<class _E>
    class complex;
extern template class __declspec(dllimport) complex<float>& __cdecl operator+=(
		complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float>& __cdecl operator-=(
		complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float>& __cdecl operator*=(
		complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float>& __cdecl operator/=(
		complex<float>&, const complex<float>&);
extern template class __declspec(dllimport)
	basic_istream<char, char_traits<char> >& __cdecl operator>>(
		basic_istream<char, char_traits<char> >&, complex<float>&);
extern template class __declspec(dllimport)
	basic_ostream<char, char_traits<char> >& __cdecl operator<<(
		basic_ostream<char, char_traits<char> >&, const complex<float>&);
extern template class __declspec(dllimport)
	basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
		basic_istream<wchar_t, char_traits<wchar_t> >&, complex<float>&);
extern template class __declspec(dllimport)
	basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
		basic_ostream<wchar_t, char_traits<wchar_t> >&, const complex<float>&);

extern template class __declspec(dllimport) complex<double>& __cdecl operator+=(
		complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double>& __cdecl operator-=(
		complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double>& __cdecl operator*=(
		complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double>& __cdecl operator/=(
		complex<double>&, const complex<double>&);
extern template class __declspec(dllimport)
	basic_istream<char, char_traits<char> >& __cdecl operator>>(
		basic_istream<char, char_traits<char> >&, complex<double>&);
extern template class __declspec(dllimport)
	basic_ostream<char, char_traits<char> >& __cdecl operator<<(
		basic_ostream<char, char_traits<char> >&, const complex<double>&);
extern template class __declspec(dllimport)
	basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
		basic_istream<wchar_t, char_traits<wchar_t> >&, complex<double>&);
extern template class __declspec(dllimport)
	basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
		basic_ostream<wchar_t, char_traits<wchar_t> >&, const complex<double>&);

extern template class __declspec(dllimport) complex<long double>& __cdecl operator+=(
		complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double>& __cdecl operator-=(
		complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double>& __cdecl operator*=(
		complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double>& __cdecl operator/=(
		complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport)
	basic_istream<char, char_traits<char> >& __cdecl operator>>(
		basic_istream<char, char_traits<char> >&, complex<long double>&);
extern template class __declspec(dllimport)
	basic_ostream<char, char_traits<char> >& __cdecl operator<<(
		basic_ostream<char, char_traits<char> >&, const complex<long double>&);
extern template class __declspec(dllimport)
	basic_istream<wchar_t, char_traits<wchar_t> >& __cdecl operator>>(
		basic_istream<wchar_t, char_traits<wchar_t> >&, complex<long double>&);
extern template class __declspec(dllimport)
	basic_ostream<wchar_t, char_traits<wchar_t> >& __cdecl operator<<(
		basic_ostream<wchar_t, char_traits<wchar_t> >&, const complex<long double>&);

extern template __declspec(dllimport) float __cdecl imag(const complex<float>&);
extern template __declspec(dllimport) float __cdecl real(const complex<float>&);
extern template __declspec(dllimport) float __cdecl _Fabs(const complex<float>&, int *);
extern template class __declspec(dllimport) complex<float> __cdecl operator+(
		const complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator+(
		const complex<float>&, const float&);
extern template class __declspec(dllimport) complex<float> __cdecl operator+(
		const float&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator-(
		const complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator-(
		const complex<float>&, const float&);
extern template class __declspec(dllimport)	complex<float> __cdecl operator-(
		const float&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator*(
		const complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator*(
		const complex<float>&, const float&);
extern template class __declspec(dllimport) complex<float> __cdecl operator*(
		const float&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator/(
		const complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator/(
		const complex<float>&, const float&);
extern template class __declspec(dllimport) complex<float> __cdecl operator/(
		const float&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator+(
		const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl operator-(
		const complex<float>&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const complex<float>&, const complex<float>&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const complex<float>&, const float&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const float&, const complex<float>&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const complex<float>&, const complex<float>&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const complex<float>&, const float&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const float&, const complex<float>&);
extern template __declspec(dllimport) float __cdecl abs(const complex<float>&);
extern template __declspec(dllimport) float __cdecl arg(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl conj(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl cos(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl cosh(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl exp(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl log(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl log10(const complex<float>&);
extern template __declspec(dllimport) float __cdecl norm(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl polar(const float&, const float&);
extern template class __declspec(dllimport) complex<float> __cdecl polar(const float&);
extern template class __declspec(dllimport) complex<float> __cdecl pow(
		const complex<float>&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl pow(
		const complex<float>&, const float&);
extern template class __declspec(dllimport) complex<float> __cdecl pow(
		const complex<float>&, int);
extern template class __declspec(dllimport) complex<float> __cdecl pow(
		const float&, const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl sin(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl sinh(const complex<float>&);
extern template class __declspec(dllimport) complex<float> __cdecl sqrt(const complex<float>&);

extern template __declspec(dllimport) double __cdecl imag(const complex<double>&);
extern template __declspec(dllimport) double __cdecl real(const complex<double>&);
extern template __declspec(dllimport) double __cdecl _Fabs(const complex<double>&, int *);
extern template class __declspec(dllimport) complex<double> __cdecl operator+(
		const complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator+(
		const complex<double>&, const double&);
extern template class __declspec(dllimport) complex<double> __cdecl operator+(
		const double&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator-(
		const complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator-(
		const complex<double>&, const double&);
extern template class __declspec(dllimport)	complex<double> __cdecl operator-(
		const double&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator*(
		const complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator*(
		const complex<double>&, const double&);
extern template class __declspec(dllimport) complex<double> __cdecl operator*(
		const double&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator/(
		const complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator/(
		const complex<double>&, const double&);
extern template class __declspec(dllimport) complex<double> __cdecl operator/(
		const double&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator+(
		const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl operator-(
		const complex<double>&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const complex<double>&, const complex<double>&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const complex<double>&, const double&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const double&, const complex<double>&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const complex<double>&, const complex<double>&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const complex<double>&, const double&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const double&, const complex<double>&);
extern template __declspec(dllimport) double __cdecl abs(const complex<double>&);
extern template __declspec(dllimport) double __cdecl arg(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl conj(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl cos(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl cosh(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl exp(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl log(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl log10(const complex<double>&);
extern template __declspec(dllimport) double __cdecl norm(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl polar(const double&, const double&);
extern template class __declspec(dllimport) complex<double> __cdecl polar(const double&);
extern template class __declspec(dllimport) complex<double> __cdecl pow(
		const complex<double>&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl pow(
		const complex<double>&, const double&);
extern template class __declspec(dllimport) complex<double> __cdecl pow(
		const complex<double>&, int);
extern template class __declspec(dllimport) complex<double> __cdecl pow(
		const double&, const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl sin(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl sinh(const complex<double>&);
extern template class __declspec(dllimport) complex<double> __cdecl sqrt(const complex<double>&);

extern template __declspec(dllimport) long double __cdecl imag(const complex<long double>&);
extern template __declspec(dllimport) long double __cdecl real(const complex<long double>&);
extern template __declspec(dllimport) long double __cdecl _Fabs(const complex<long double>&, int *);
extern template class __declspec(dllimport) complex<long double> __cdecl operator+(
		const complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator+(
		const complex<long double>&, const long double&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator+(
		const long double&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator-(
		const complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator-(
		const complex<long double>&, const long double&);
extern template class __declspec(dllimport)	complex<long double> __cdecl operator-(
		const long double&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator*(
		const complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator*(
		const complex<long double>&, const long double&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator*(
		const long double&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator/(
		const complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator/(
		const complex<long double>&, const long double&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator/(
		const long double&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator+(
		const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl operator-(
		const complex<long double>&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const complex<long double>&, const complex<long double>&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const complex<long double>&, const long double&);
extern template __declspec(dllimport) bool __cdecl operator==(
		const long double&, const complex<long double>&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const complex<long double>&, const complex<long double>&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const complex<long double>&, const long double&);
extern template __declspec(dllimport) bool __cdecl operator!=(
		const long double&, const complex<long double>&);
extern template __declspec(dllimport) long double __cdecl abs(const complex<long double>&);
extern template __declspec(dllimport) long double __cdecl arg(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl conj(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl cos(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl cosh(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl exp(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl log(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl log10(const complex<long double>&);
extern template __declspec(dllimport) long double __cdecl norm(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl polar(const long double&, const long double&);
extern template class __declspec(dllimport) complex<long double> __cdecl polar(const long double&);
extern template class __declspec(dllimport) complex<long double> __cdecl pow(
		const complex<long double>&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl pow(
		const complex<long double>&, const long double&);
extern template class __declspec(dllimport) complex<long double> __cdecl pow(
		const complex<long double>&, int);
extern template class __declspec(dllimport) complex<long double> __cdecl pow(
		const long double&, const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl sin(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl sinh(const complex<long double>&);
extern template class __declspec(dllimport) complex<long double> __cdecl sqrt(const complex<long double>&);
#line 718 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"
extern template class __declspec(dllimport) basic_ostream<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_iostream<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_stringbuf<char, char_traits<char>, allocator<char> >;
extern template class __declspec(dllimport) basic_istringstream<char, char_traits<char>, allocator<char> >;
extern template class __declspec(dllimport) basic_ostringstream<char, char_traits<char>, allocator<char> >;
extern template class __declspec(dllimport) basic_stringstream<char, char_traits<char>, allocator<char> >;
extern template class __declspec(dllimport) basic_filebuf<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_ifstream<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_ofstream<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_fstream<char, char_traits<char> >;
extern template class __declspec(dllimport) basic_ios<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_streambuf<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_istream<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_ostream<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_iostream<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_stringbuf<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >;
extern template class __declspec(dllimport) basic_istringstream<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >;
extern template class __declspec(dllimport) basic_ostringstream<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >;
extern template class __declspec(dllimport) basic_stringstream<wchar_t, char_traits<wchar_t>, allocator<wchar_t> >;
extern template class __declspec(dllimport) basic_filebuf<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_ifstream<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_ofstream<wchar_t, char_traits<wchar_t> >;
extern template class __declspec(dllimport) basic_fstream<wchar_t, char_traits<wchar_t> >;

extern template class __declspec(dllimport) num_get<char,
                      istreambuf_iterator<char, char_traits<char> > >;
extern template class __declspec(dllimport) num_get<wchar_t,
                      istreambuf_iterator<wchar_t, char_traits<wchar_t> > >;
extern template class __declspec(dllimport) num_put<char,
                      ostreambuf_iterator<char, char_traits<char> > >;
extern template class __declspec(dllimport) num_put<wchar_t,
                      ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >;

extern template class __declspec(dllimport) collate<char>;
extern template class __declspec(dllimport) collate<wchar_t>;
#line 754 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"

                
typedef basic_ios<char, char_traits<char> > ios;
typedef basic_streambuf<char, char_traits<char> > streambuf;
typedef basic_istream<char, char_traits<char> > istream;
typedef basic_ostream<char, char_traits<char> > ostream;
typedef basic_iostream<char, char_traits<char> > iostream;
typedef basic_stringbuf<char, char_traits<char>,
        allocator<char> > stringbuf;
typedef basic_istringstream<char, char_traits<char>,
        allocator<char> > istringstream;
typedef basic_ostringstream<char, char_traits<char>,
        allocator<char> > ostringstream;
typedef basic_stringstream<char, char_traits<char>,
        allocator<char> > stringstream;
typedef basic_filebuf<char, char_traits<char> > filebuf;
typedef basic_ifstream<char, char_traits<char> > ifstream;
typedef basic_ofstream<char, char_traits<char> > ofstream;
typedef basic_fstream<char, char_traits<char> > fstream;
                
typedef basic_ios<wchar_t, char_traits<wchar_t> > wios;
typedef basic_streambuf<wchar_t, char_traits<wchar_t> >
        wstreambuf;
typedef basic_istream<wchar_t, char_traits<wchar_t> > wistream;
typedef basic_ostream<wchar_t, char_traits<wchar_t> > wostream;
typedef basic_iostream<wchar_t, char_traits<wchar_t> > wiostream;
typedef basic_stringbuf<wchar_t, char_traits<wchar_t>,
        allocator<wchar_t> > wstringbuf;
typedef basic_istringstream<wchar_t, char_traits<wchar_t>,
        allocator<wchar_t> > wistringstream;
typedef basic_ostringstream<wchar_t, char_traits<wchar_t>,
        allocator<wchar_t> > wostringstream;
typedef basic_stringstream<wchar_t, char_traits<wchar_t>,
        allocator<wchar_t> > wstringstream;
typedef basic_filebuf<wchar_t, char_traits<wchar_t> > wfilebuf;
typedef basic_ifstream<wchar_t, char_traits<wchar_t> > wifstream;
typedef basic_ofstream<wchar_t, char_traits<wchar_t> > wofstream;
typedef basic_fstream<wchar_t, char_traits<wchar_t> > wfstream;


typedef num_get<char, istreambuf_iterator<char, char_traits<char> > > numget;
typedef num_get<wchar_t, istreambuf_iterator<wchar_t, char_traits<wchar_t> > >
                                                                                                                                          wnumget;
typedef num_put<char, ostreambuf_iterator<char, char_traits<char> > > numput;
typedef num_put<wchar_t, ostreambuf_iterator<wchar_t, char_traits<wchar_t> > >
                                                                                                                                          wnumput;
typedef collate<char> ncollate;
typedef collate<wchar_t> wcollate;

#pragma warning(default:4662)
#pragma warning(default:4231) 
#line 806 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"

};

#pragma pack(pop)
#line 811 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"

#line 813 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\iosfwd"





#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\utility"


#pragma pack(push,8)
#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\utility"
namespace std {
		
template<class _T1, class _T2> struct pair {
	typedef _T1 first_type;
	typedef _T2 second_type;
	pair()
		: first(_T1()), second(_T2()) {}
	pair(const _T1& _V1, const _T2& _V2)
		: first(_V1), second(_V2) {}
	_T1 first;
	_T2 second;
	};
template<class _T1, class _T2> inline
	bool __cdecl operator==(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (_X.first == _Y.first && _X.second == _Y.second); }
template<class _T1, class _T2> inline
	bool __cdecl operator!=(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (!(_X == _Y)); }
template<class _T1, class _T2> inline
	bool __cdecl operator<(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (_X.first < _Y.first ||
		!(_Y.first < _X.first) && _X.second < _Y.second); }
template<class _T1, class _T2> inline
	bool __cdecl operator>(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (_Y < _X); }
template<class _T1, class _T2> inline
	bool __cdecl operator<=(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (!(_Y < _X)); }
template<class _T1, class _T2> inline
	bool __cdecl operator>=(const pair<_T1, _T2>& _X,
		const pair<_T1, _T2>& _Y)
	{return (!(_X < _Y)); }
template<class _T1, class _T2> inline
	pair<_T1, _T2> __cdecl make_pair(const _T1& _X, const _T2& _Y)
	{return (pair<_T1, _T2>(_X, _Y)); }
		
struct input_iterator_tag {};
struct output_iterator_tag {};
struct forward_iterator_tag
	: public input_iterator_tag {};
struct bidirectional_iterator_tag
	: public forward_iterator_tag {};
struct random_access_iterator_tag
	: public bidirectional_iterator_tag  {};
		
template<class _C, class _Ty, class _D = ptrdiff_t>
	struct iterator {
	typedef _C iterator_category;
	typedef _Ty value_type;
	typedef _D distance_type;
	};
template<class _Ty, class _D>
	struct _Bidit : public iterator<bidirectional_iterator_tag,
		_Ty, _D> {};
template<class _Ty, class _D>
	struct _Ranit : public iterator<random_access_iterator_tag,
		_Ty, _D> {};
		
template<class _It>
	struct iterator_traits {
	typedef _It::iterator_category iterator_category;
	typedef _It::value_type value_type;
	typedef _It::distance_type distance_type;
	};
		
template<class _C, class _Ty, class _D> inline
	_C __cdecl _Iter_cat(const iterator<_C, _Ty, _D>&)
	{_C _X;
	return (_X); }
template<class _Ty> inline
	random_access_iterator_tag __cdecl _Iter_cat(const _Ty *)
	{random_access_iterator_tag _X;
	return (_X); }
		
template<class _II> inline
	ptrdiff_t __cdecl distance(_II _F, _II _L)
	{ptrdiff_t _N = 0;
	_Distance(_F, _L, _N, _Iter_cat(_F));
	return (_N); }
template<class _II, class _D> inline
	void __cdecl _Distance(_II _F, _II _L, _D& _N)
	{_Distance(_F, _L, _N, _Iter_cat(_F)); }
template<class _II, class _D> inline
	void __cdecl _Distance(_II _F, _II _L, _D& _N, input_iterator_tag)
	{if (0) ; else for (; _F != _L; ++_F)
		++_N; }
template<class _II, class _D> inline
	void __cdecl _Distance(_II _F, _II _L, _D& _N, forward_iterator_tag)
	{if (0) ; else for (; _F != _L; ++_F)
		++_N; }
template<class _II, class _D> inline
	void __cdecl _Distance(_II _F, _II _L, _D& _N,
		bidirectional_iterator_tag)
	{if (0) ; else for (; _F != _L; ++_F)
		++_N; }
template<class _RI, class _D> inline
	void __cdecl _Distance(_RI _F, _RI _L, _D& _N,
		random_access_iterator_tag)
	{_N += _L - _F; }
		
template<class _RI,
	class _Ty,
	class _Rt = _Ty&,
	class _Pt = _Ty *,
	class _D = ptrdiff_t>
	class reverse_iterator : public _Ranit<_Ty, _D> {
public:
	typedef reverse_iterator<_RI, _Ty, _Rt, _Pt, _D> _Myt;
	typedef _RI iter_type;
	typedef _Rt reference_type;
	typedef _Pt pointer_type;
	reverse_iterator()
		{}
	explicit reverse_iterator(_RI _X)
		: current(_X) {}
	_RI base() const
		{return (current); }
	_Rt operator*() const
		{return (*(current - 1)); }
	_Pt operator->() const
		{return (&**this); }
	_Myt& operator++()
		{--current;
		return (*this); }
	_Myt operator++(int)
		{_Myt _Tmp = *this;
		--current;
		return (_Tmp); }
	_Myt& operator--()
		{++current;
		return (*this); }
	_Myt operator--(int)
		{_Myt _Tmp = *this;
		++current;
		return (_Tmp); }
	_Myt& operator+=(_D _N)
		{current -= _N;
		return (*this); }
	_Myt operator+(_D _N) const
		{return (_Myt(current - _N)); }
	_Myt& operator-=(_D _N)
		{current += _N;
		return (*this); }
	_Myt operator-(_D _N) const
		{return (_Myt(current + _N)); }
	_Rt operator[](_D _N) const
		{return (*(*this + _N)); }
protected:
	_RI current;
	};
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	bool __cdecl operator==(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (_X.base() == _Y.base()); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	bool __cdecl operator!=(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (!(_X == _Y)); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	bool __cdecl operator<(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (_Y.base() < _X.base()); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	bool __cdecl operator>(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (_Y < _X); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	bool __cdecl operator<=(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (!(_Y < _X)); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	bool __cdecl operator>=(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (!(_X < _Y)); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	_D __cdecl operator-(
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _X,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (_Y.base() - _X.base()); }
template<class _RI, class _Ty, class _Rt, class _Pt,
	class _D> inline
	reverse_iterator<_RI, _Ty, _Rt, _Pt, _D> __cdecl operator+(_D _N,
		const reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>& _Y)
	{return (reverse_iterator<_RI, _Ty, _Rt, _Pt, _D>(
		_Y.base() - _N)); }
		
template<class _E, class _Tr = char_traits<_E> >
	class istreambuf_iterator
		: public iterator<input_iterator_tag, _E, _Tr::off_type> {
public:
	typedef istreambuf_iterator<_E, _Tr> _Myt;
	typedef _E char_type;
	typedef _Tr traits_type;
	typedef _Tr::int_type int_type;
	typedef basic_streambuf<_E, _Tr> streambuf_type;
	typedef basic_istream<_E, _Tr> istream_type;
	istreambuf_iterator(streambuf_type *_Sb = 0) throw ()
		: _Sbuf(_Sb), _Got(_Sb == 0) {}
	istreambuf_iterator(istream_type& _I) throw ()
		: _Sbuf(_I.rdbuf()), _Got(_I.rdbuf() == 0) {}
	const _E& operator*() const
		{if (!_Got)
			((_Myt *)this)->_Peek();
		return (_Val); }
	const _E *operator->() const
		{return (&**this); }
	_Myt& operator++()
		{_Inc();
		return (*this); }
	_Myt operator++(int)
		{if (!_Got)
			_Peek();
		_Myt _Tmp = *this;
		_Inc();
		return (_Tmp); }
	bool equal(const _Myt& _X) const
		{if (!_Got)
			((_Myt *)this)->_Peek();
		if (!_X._Got)
			((_Myt *)&_X)->_Peek();
		return (_Sbuf == 0 && _X._Sbuf == 0
			|| _Sbuf != 0 && _X._Sbuf != 0); }
private:
	void _Inc()
		{if (_Sbuf == 0
			|| _Tr::eq_int_type(_Tr::eof(), _Sbuf->sbumpc()))
			_Sbuf = 0, _Got = true;
		else
			_Got = false; }
	_E _Peek()
		{int_type _C;
		if (_Sbuf == 0
			|| _Tr::eq_int_type(_Tr::eof(), _C = _Sbuf->sgetc()))
			_Sbuf = 0;
		else
			_Val = _Tr::to_char_type(_C);
		_Got = true;
		return (_Val); }
	streambuf_type *_Sbuf;
	bool _Got;
	_E _Val;
	};
template<class _E, class _Tr> inline
	bool __cdecl operator==(const istreambuf_iterator<_E, _Tr>& _X,
		const istreambuf_iterator<_E, _Tr>& _Y)
	{return (_X.equal(_Y)); }
template<class _E, class _Tr> inline
	bool __cdecl operator!=(const istreambuf_iterator<_E, _Tr>& _X,
		const istreambuf_iterator<_E, _Tr>& _Y)
	{return (!(_X == _Y)); }
		
template<class _E, class _Tr = char_traits<_E> >
	class ostreambuf_iterator
		: public iterator<output_iterator_tag, void, void> {
	typedef ostreambuf_iterator<_E, _Tr> _Myt;
public:
	typedef _E char_type;
	typedef _Tr traits_type;
	typedef basic_streambuf<_E, _Tr> streambuf_type;
	typedef basic_ostream<_E, _Tr> ostream_type;
	ostreambuf_iterator(streambuf_type *_Sb) throw ()
		: _Failed(false), _Sbuf(_Sb) {}
	ostreambuf_iterator(ostream_type& _O) throw ()
		: _Failed(false), _Sbuf(_O.rdbuf()) {}
	_Myt& operator=(_E _X)
		{if (_Sbuf == 0
			|| _Tr::eq_int_type(_Tr::eof(), _Sbuf->sputc(_X)))
			_Failed = true;
		return (*this); }
	_Myt& operator*()
		{return (*this); }
	_Myt& operator++()
		{return (*this); }
	_Myt& operator++(int)
		{return (*this); }
	bool failed() const throw ()
		{return (_Failed); }
private:
	bool _Failed;
	streambuf_type *_Sbuf;
	};
		
	namespace rel_ops {
template<class _Ty> inline
	bool __cdecl operator!=(const _Ty& _X, const _Ty& _Y)
	{return (!(_X == _Y)); }
template<class _Ty> inline
	bool __cdecl operator>(const _Ty& _X, const _Ty& _Y)
	{return (_Y < _X); }
template<class _Ty> inline
	bool __cdecl operator<=(const _Ty& _X, const _Ty& _Y)
	{return (!(_Y < _X)); }
template<class _Ty> inline
	bool __cdecl operator>=(const _Ty& _X, const _Ty& _Y)
	{return (!(_X < _Y)); }
	};
};

#pragma pack(pop)
#line 327 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\utility"

#line 329 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\utility"






















#line 11 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"

 
 
 
#line 16 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"
 
 
namespace std {
		
template<class _Ty> inline
	_Ty  *_Allocate(ptrdiff_t _N, _Ty  *)
	{if (_N < 0)
		_N = 0;
	return ((_Ty  *)operator new(
		(size_t)_N * sizeof (_Ty))); }
		
template<class _T1, class _T2> inline
	void _Construct(_T1  *_P, const _T2& _V)
	{new ((void  *)_P) _T1(_V); }
		
template<class _Ty> inline
	void _Destroy(_Ty  *_P)
	{(_P)->~_Ty(); }
inline void _Destroy(char  *_P)
	{}
inline void _Destroy(wchar_t  *_P)
	{}
		
template<class _Ty>
	class allocator {
public:
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;
	typedef _Ty  *pointer;
	typedef const _Ty  *const_pointer;
	typedef _Ty & reference;
	typedef const _Ty & const_reference;
	typedef _Ty value_type;
	pointer address(reference _X) const
		{return (&_X); }
	const_pointer address(const_reference _X) const
		{return (&_X); }
	pointer allocate(size_type _N, const void *)
		{return (_Allocate((difference_type)_N, (pointer)0)); }
	char  *_Charalloc(size_type _N)
		{return (_Allocate((difference_type)_N,
			(char  *)0)); }
	void deallocate(void  *_P, size_type)
		{operator delete(_P); }
	void construct(pointer _P, const _Ty& _V)
		{_Construct(_P, _V); }
	void destroy(pointer _P)
		{_Destroy(_P); }
	size_t max_size() const
		{size_t _N = (size_t)(-1) / sizeof (_Ty);
		return (0 < _N ? _N : 1); }
	};
template<class _Ty, class _U> inline
	bool operator==(const allocator<_Ty>&, const allocator<_U>&)
	{return (true); }
template<class _Ty, class _U> inline
	bool operator!=(const allocator<_Ty>&, const allocator<_U>&)
	{return (false); }
		
class __declspec(dllimport) allocator<void> {
public:
	typedef void _Ty;
	typedef _Ty  *pointer;
	typedef const _Ty  *const_pointer;
	typedef _Ty value_type;
	};
};

#pragma pack(pop)
#line 86 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"

#line 88 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xmemory"






















#line 5 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstring"


#pragma pack(push,8)
#line 9 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xstring"
 #line 1 "C:\\Program Files\\DevStudio\\VC\\INCLUDE\\xutility"