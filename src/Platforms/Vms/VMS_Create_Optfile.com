$ ! VMS_Create_Optfile.com
$  env = f$parse(f$environment("PROCEDURE"),,,"NAME")
$  v=f$verify(0+f$integer(f$trnlnm("''env'"+"_VERIFY")))
$  Goto Run_Begin
$ !
$ ! Use the above "env" symbol to enable "set verify" -
$ !     $define VMS_CREATE_OPTFILE_VERIFY 1	
$ !
$ !
$ !
$ ! Create an option file for the executable or sharable image
$ ! 
$ ! P1 = Makefile Directory
$ ! P2 = Default directory   ! [Not used?]
$ ! P3 = Options filename
$ ! P4 = Value of LIBRARIES
$ ! P5 = Value of SHARE_COPY
$ ! P6 = Value of VMS_VECTOR
$ ! P7 = Objects directory
$ ! P8 = Value of SOURCES when OBJECTS_IN_OPTIONFILE set
$ ! 
$Run_Begin:
$ !
$!  sho sym pegasus*
$!  sho log pegasus*
$!  sho sym/local *
$  Prev_lib = ""
$  First_lib = "yes"
$  First_obj = "yes"
$  use_static = "''f$trnlnm("PEGASUS_USE_STATIC_LIBRARIES")'"
$  if f$edit("''use_static'", "LOWERCASE") .eqs. "false"
$  then
$    want_dllexport = "yes"
$  else
$    want_dllexport = ""
$  endif
$ !
$ sav_ver = f$environment("VERIFY_PROCEDURE")
$ if sav_ver
$ then
$   MyOut :== "Write Sys$output"
$ else
$   MyOut :== "!"
$ endif
$ ! 
$  'MyOut "%VMSCROPT - P1 = ''P1'"
$  'MyOut "%VMSCROPT - P2 = ''P2'"
$  'MyOut "%VMSCROPT - P3 = ''P3'"
$  'MyOut "%VMSCROPT - P4 = ''P4'"
$  'MyOut "%VMSCROPT - P5 = ''P5'"
$  'MyOut "%VMSCROPT - P6 = ''P6'"
$  'MyOut "%VMSCROPT - P7 = ''P7'"
$  'MyOut "%VMSCROPT - P8 = ''P8'"
$ ! 
$ !
$ ! Use the program name to open a new .opt file
$ ! 
$  if P3 .eqs. ""
$  Then
$    Goto run_error1
$  Endif
$ ! 
$  optname =  "''P1'" + "''P3'" + ".opt"
$ ! 
$  'MyOut "%VMSCROPT - Optname = ''Optname'"
$ !
$  Open/write/error=optfile_openerror optfile 'Optname
$ ! 
$ ! Look for "OBJECTS_IN_OPTIONFILE".
$ ! 
$  if P8 .eqs. ""
$  Then
$    Goto Get_libraries
$  Endif
$    'MyOut "%VMSCROPT - OBJECTS_IN_OPTIONFILE is defined!"
$    Write sys$output "%VMSCROPT - OBJECTS_IN_OPTIONFILE is defined!"
$ ! 
$ ! Write the object filename list to the option file.
$ ! 
$  Objnum = 0
$ !
$Get_objects_loop:
$ ! 
$ ! Make the list of sources/objects
$ ! 
$  PegObj = f$element(Objnum, " ", "''P8'")
$ ! 
$  if ((PegObj .eqs. " ") .or. (PegObj .eqs. ""))
$  then
$    Goto Get_objects_loop_exit
$  endif
$ ! 
$ ! Create the object file entry
$ ! 
$  if (first_obj .eqs. "yes")
$   then
$    objnam = "''P7'" + "''PegObj'" + ".obj"
$    first_obj = "no"
$  else
$    objnam = "''PegObj'" + ".obj"
$  endif
$ ! 
$  Objnum = Objnum + 1
$ ! 
$ ! Write it too the option file
$ ! 
$  write/error=optfile_writeerror optfile "''objnam'"
$ ! 
$ ! Try again
$ ! 
$  Goto Get_objects_loop
$ !
$Get_objects_loop_exit:
$ ! 
$Get_libraries:
$ ! 
$ ! Every option file starts with the following
$ ! 
$  libnam = f$trnlnm("PEGASUS_VMSHOMEA") + "obj.platforms.vms]vms_crtl_init.obj"
$ ! 
$ ! Write it to the option file
$ ! 
$  write/error=optfile_writeerror optfile "''libnam'"
$ ! 
$  Libnum = 0
$  prev_libtype = ""
$ !
$makefile_loop3:
$ ! 
$ ! Make the list of libraries
$ ! 
$  PegLib = f$element(Libnum, " ", "''P4'")
$ ! 
$  if ((PegLib .eqs. " ") .or. (PegLib .eqs. ""))
$  then
$    Goto makefile_loop3_exit
$  endif
$ ! 
$ ! If .exe exists, use the shareable instead of the .olb.
$  is_dll = ""
$  if "''want_dllexport'" .nes. "" 
$  then 
$    dllname = -
        f$trnlnm("PEGASUS_VMSHOMEA") + "bin]" + "lib" + "''PegLib'" + ".exe"
$    foundit = f$search(dllname)
$    if "''foundit'" .nes. ""
$    then
$      is_dll = "yes"
$    endif
$  endif
$ !
$ ! Create the library file entry
$ ! 
$  if "''is_dll'" .nes. ""
$  then
$    subdir = "bin"
$    libtype = "share"
$!    define/log/job lib'PegLib' 'dllname'
$  else
$    subdir = "lib"
$    libtype = "lib"
$  endif
$  !
$  if (prev_libtype .nes. libtype)
$   then
$    libnam = f$trnlnm("PEGASUS_VMSHOMEA") + "''subdir']" + -
                       "lib" + "''PegLib'" + "/''libtype'"
$    first_lib = "no"
$  else
$    libnam = "lib" + "''PegLib'" + "/''libtype'"
$  endif
$  prev_libtype = libtype
$ ! 
$  Libnum = Libnum + 1
$ ! 
$ ! Write it too the option file
$ ! 
$  write/error=optfile_writeerror optfile "''libnam'"
$ ! 
$ ! Try again
$ ! 
$  Goto makefile_loop3
$ !
$makefile_loop3_exit:
$ !
$ ! example: define PEGASUS_VMS_LINKER_GSMATCH "LEQUAL,2,5400"
$  gsmatch = f$trnlnm("PEGASUS_VMS_LINKER_GSMATCH")
$  if "''gsmatch'" .nes. ""
$  then
$    write/error=optfile_writeerror optfile "GSMATCH=''gsmatch'"
$  endif
$ !
$No_gsmatch:
$ !
$ ! example: define PEGASUS_VMS_LINKER_IDENT "2.5400"
$  ident = f$trnlnm("PEGASUS_VMS_LINKER_IDENT")
$  if "''ident'" .nes. ""
$  then
$    write/error=optfile_writeerror optfile "IDENTIFICATION=''ident'"
$  endif
$ !
$No_ident:
$ ! 
$ ! Add the SSL libraries
$ ! 
$  ssllibdir = f$trnlnm("PEGASUS_OPENSSLLIB")
$  if (ssllibdir .EQS. "") then goto No_ssl
$ ! 
$!  ssllib1 = "pegasus_openssllib:libssl32/lib"
$!  ssllib2 = "libcrypto32/lib"
$  ssllib1 = "pegasus_openssllib:ssl$libssl_shr32/share"
$  ssllib2 = "ssl$libcrypto_shr32/share"
$  write/error=optfile_writeerror optfile "''ssllib1'"
$  write/error=optfile_writeerror optfile "''ssllib2'"
$ !
$No_ssl:
$ !
$  zlibdir = f$trnlnm("libz")
$  if (zlibdir .EQS. "") then goto No_zlib
$ !
$  zlib1 = "libz:libz/lib"
$  write/error=optfile_writeerror optfile "''zlib1'"
$ !
$No_zlib:
$ ! 
$ ! Looking for "SHARE_COPY"
$ ! 
$  if P5 .eqs. ""
$  Then
$    Goto run_exit
$  Endif
$ ! 
$  'MyOut "%VMSCROPT - SHARE_COPY defined"
$  tmp_vecsym = "PegasusCreateProvider"
$ ! 
$VmsVec:
$ ! 
$ ! Looking for "VMS_VECTOR"
$ ! of compiler generated .opt file for the dll_export case.
$ ! 
$  if "''want_dllexport'" .eqs. "" 
$  Then
$    if P6 .eqs. ""
$    Then
$      vecsym = "SYMBOL_VECTOR=(" + "''tmp_vecsym'" + "=PROCEDURE)"
$    Else
$      'MyOut "%VMSCROPT - VMS_VECTOR defined"
$      vecsym = "SYMBOL_VECTOR=(" + "''P6'" + "=PROCEDURE)"
$    Endif
$  Endif
$ ! 
$Write_vmsvec:
$ ! 
$  'MyOut "%VMSCROPT -  vecsym = ''vecsym'"
$ ! 
$ ! Write it to the option file
$ ! 
$  write/error=optfile_writeerror optfile "''vecsym'"
$ !
$Done_vmsvec:
$ ! 
$ ! Close the options file.
$ ! 
$  Close optfile
$ !
$makefile_loop9_exit3:
$ ! 
$  Goto run_exit
$ !
$optfile_eof:
$ Goto run_exit
$ ! 
$makefile_openerror1:
$ Write sys$output "%VMSCROPT - Can't find makefile! 1 - ''mfile'"
$ ! 
$ Goto run_exit
$ ! 
$makefile_openerror2:
$ Write sys$output "%VMSCROPT - Can't find makefile! 2 - ''mfile'"
$ ! 
$ Goto run_exit
$ ! 
$makefile_openerror3:
$ Write sys$output "%VMSCROPT - Can't find makefile! 3 - ''mfile'"
$ ! 
$ Goto run_exit
$ ! 
$makefile_openerror4:
$ Write sys$output "%VMSCROPT - Can't find makefile! 4 - ''mfile'"
$ ! 
$ Goto run_exit
$ ! 
$makefile_readerror:
$ Write sys$output "%VMSCROPT - Error reading makefile! - ''mfile'"
$ ! 
$ Goto run_exit
$ ! 
$makefile2_openerror:
$ Write sys$output "%VMSCROPT - Can't find makefile! - ''mfile1'"
$ Show Default
$ ! 
$ Goto run_exit
$ ! 
$makefile_readerror1:
$ Write sys$output "%VMSCROPT - Error reading makefile! - ''mfile1'"
$ ! 
$ Goto run_exit
$ ! 
$optfile_openerror:
$ Write sys$output "%VMSCROPT - Can't open options file! - ''Optname'"
$ ! 
$ Goto run_exit
$ ! 
$optfile_writeerror:
$ Write sys$output "%VMSCROPT - Error writing options file! - ''Optname'"
$ ! 
$ Goto run_exit
$ !
$run_error1:
$ ! 
$ Write sys$output "%VMSCROPT - Can't find option filename!"
$ Goto run_exit
$ ! 
$run_error2:
$ ! 
$ Write Sys$output "%VMSCROPT - Makefile.; libraries.mak line is not right!"
$ Close 'mfile
$ Close 'Optname
$ Goto run_exit
$ ! 
$run_error1:
$ ! 
$ Goto run_exit
$ ! 
$run_error1:
$ ! 
$ Goto run_exit
$ ! 
$run_error3:
$ ! 
$ Goto run_exit
$ ! 
$run_error0:
$ ! 
$ Goto run_exit
$ ! 
$ ! 
$ ! 
$run_exit:
$ Exit 1+0*f$verify(v)
