$ !
$ !set verify
$ !
$ ! Create an option file for the executable or sharable image
$ ! 
$ ! P1 = Makefile Directory
$ ! P2 = Default directory
$ ! P3 = Options filename
$ ! P4 = Value of SHARE_COPY
$ ! P5 = Value of VMS_VECTOR
$ ! P6 = Value of OBJECTS_IN_OPTIONFILE
$ ! P7 = Value of MAKEFILE_NAME
$ ! 
$  end_loop2 = "no"
$  end_loop4 = "no"
$  First_lib = "no"
$  using_makefile2 = "no"
$ ! 
$  MyOut :== "!"
$ ! MyOut :== "Write Sys$output"
$ ! 
$  'MyOut "%VMSCROPT - P1 = ''P1'"
$  'MyOut "%VMSCROPT - P2 = ''P2'"
$  'MyOut "%VMSCROPT - P3 = ''P3'"
$  'MyOut "%VMSCROPT - P4 = ''P4'"
$  'MyOut "%VMSCROPT - P5 = ''P5'"
$  'MyOut "%VMSCROPT - P6 = ''P6'"
$  'MyOut "%VMSCROPT - P7 = ''P7'"
$ ! 
$  if P7 .eqs. ""
$  then
$   mfile = "''P2'" + "Makefile.;"
$  else
$   mfile = "''P2'" + "''P7'"
$  endif
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
$  if P6 .eqs. ""
$  Then
$    Goto Get_libraries
$  Endif
$    'MyOut "%VMSCROPT - OBJECTS_IN_OPTIONFILE is defined!"
$    Write sys$output "%VMSCROPT - OBJECTS_IN_OPTIONFILE is defined!"
$ ! 
$ ! Write the object filename list to the option file.
$ ! 
$ ! Open the makefile
$ ! 
$  Open/read/error=makefile_openerror1 make_file 'mfile
$ ! 
$makefile_loop1:
$ ! 
$ ! read a record from tha makefile
$ ! 
$  Read/end=makefile_loop2_exit/error=makefile_readerror make_file mfrec
$ ! 
$ ! Looking for the start of the sources list.
$ ! 
$  objopt = f$locate("SOURCES =", mfrec)
$ ! 
$ ! Find it?  No, try the next record
$ ! 
$  if (objopt .eq. f$length(objopt)) then goto makefile_loop1
$makefile_loop1_exit:
$ ! 
$ ! Found it! Read the next record
$ ! 
$makefile_loop2:
$ ! 
$  Read/end=makefile_loop4_exit/error=makefile_readerror make_file mfrec
$ ! 
$ ! Locate the line contination character
$ ! 
$  bslash = f$locate("\", mfrec)
$ ! 
$ ! Is it there?
$ ! 
$  if (bslash .eq. f$length(mfrec))
$  then
$    tmp_objnam = mfrec
$    end_loop2 = "yes"
$  else
$    tmp_objnam = f$extract(0, (bslash - 1), mfrec)
$  endif
$ ! 
$ ! Remove ALL whitespace from the string
$ ! 
$  ttmp_objnam = f$edit(tmp_objnam, "COLLAPSE, TRIM")
$ ! 
$ ! Find ".cpp" in the string
$ ! 
$  cpploc = f$locate(".cpp", ttmp_objnam)
$ ! 
$ ! Find it?  No, continue.
$ ! 
$  if (cpploc .eq. f$length(mfrec)) then goto makefile_loop2_exit
$ ! 
$ ! copy the filename excluding the ".cpp"
$ ! 
$  tttmp_objnam = f$extract(0, cpploc, ttmp_objnam)
$ ! 
$ ! Add ".obj" to the end
$ ! 
$  objnam = "''tttmp_objnam'" + ".obj"
$ ! 
$ ! Write the filename to the option file.
$ ! 
$  write/error=optfile_writeerror optfile "''objnam'"
$ ! 
$ ! Keep looking for more source filenames
$ ! 
$  if (end_loop2 .eqs. "no"
$  then
$   goto makefile_loop2
$  endif
$makefile_loop2_exit:
$ ! 
$ ! Close the makefile
$ ! 
$  close make_file
$ ! 
$Get_libraries:
$ ! 
$ ! Reopen the makefile
$ ! 
$ ! 
$  'MyOut "%VMSCROPT - mfile = ''mfile'"
$ !
$  Open/read/error=makefile_openerror2 make_file 'mfile
$ ! 
$ ! Every option file starts with the following
$ ! 
$  libnam = f$trnlnm("PEGASUS_VMSHOMEA") + "obj.platforms.vms]vms_crtl_init.obj"
$ ! 
$ ! Write it to the option file
$ ! 
$  write/error=optfile_writeerror optfile "''libnam'"
$ ! 
$makefile_loop3:
$ ! 
$ ! read the next record
$ ! 
$  Read/end=makefile_loop4_exit/error=makefile_readerror make_file mfrec
$ ! 
$ ! Looking for "LIBRARIES =" in a record
$ ! 
$  'Myout "mfrec = ''mfrec'"
$ ! 
$  libs1 = f$locate("LIBRARIES =", mfrec)
$  libs2 = f$locate("LIBRARIES=", mfrec)
$  libs3 = f$locate("/libraries.mak", mfrec)
$ ! 
$ ! Find it!  No, try again
$ ! 
$  if ((libs1 .eq. f$length(mfrec)) .and. (libs2 .eq. f$length(mfrec)) .and. (libs3 .eq. f$length(mfrec)))
$   then
$    goto makefile_loop3
$  endif
$ ! 
$makefile_loop3_exit:
$ ! 
$ ! Found a libraries entry or pointer to another file.
$ ! 
$  If (libs3 .ne. f$length(mfrec))
$   then
$ ! 
$ ! Found a pointer to another file
$ ! 
$    Back1 = f$locate("../", mfrec)
$    Back2 = f$locate("../../", mfrec)
$    Back3 = f$locate("../../../", mfrec)
$    Back4 = f$locate("../../../../", mfrec)
$ !
$    if ((back1 .eq. f$length(mfrec)) .and. (back2 .eq. f$length(mfrec)) .and. (back3 .eq. f$length(mfrec)) .and. (back4 .eq. f$length(mfrec)))
$     then
$ ! 
$ ! Something is wrong!
$ ! 
$      Goto run_error2
$    endif
$ ! 
$    FoundBack = "no"
$    If (back4 .ne. f$length(mfrec))
$     Then
$      FoundBack = "yes"
$      BackStr = "[----]"
$      Goto makefile_loop4c
$    endif
$ ! 
$    If (back3 .ne. f$length(mfrec))
$     Then
$      FoundBack = "yes"
$      BackStr = "[---]"
$      Goto makefile_loop4c
$    endif
$ ! 
$    If (back2 .ne. f$length(mfrec))
$     Then
$      FoundBack = "yes"
$      BackStr = "[--]"
$      Goto makefile_loop4c
$    endif
$ ! 
$    If (back1 .ne. f$length(mfrec))
$     Then
$      FoundBack = "yes"
$      BackStr = "[-]"
$      Goto makefile_loop4c
$    endif
$ ! 
$    if (FoundBack .eqs. "no")
$     then
$ ! 
$ ! Something is wrong!
$ ! 
$      goto run_error2
$    endif
$ !
$makefile_loop4c:
$ ! 
$ ! 
$ ! Close the makefile
$ ! 
$    close make_file
$ ! 
$ ! Reopen the new makefile
$ ! 
$    mfile1 = "''BackStr'" + "libraries.mak"
$    Open/read/error=makefile2_openerror make_file 'mfile1
$ !
$makefile_loop4:
$ ! 
$ ! read the next record
$ ! 
$    Read/end=makefile_loop4_exit/error=makefile_readerror make_file mfrec
$ ! 
$ ! Looking for "LIBRARIES =" in a record
$ ! 
$    'Myout "mfrec = ''mfrec'"
$ ! 
$    libs1 = f$locate("LIBRARIES =", mfrec)
$    libs2 = f$locate("LIBRARIES=", mfrec)
$ ! 
$ ! Find it!  No, try again
$ ! 
$    if ((libs1 .eq. f$length(mfrec)) .and. (libs2 .eq. f$length(mfrec)))
$     then
$      goto makefile_loop4
$    endif
$ ! 
$  endif
$ ! 
$  First_lib = "yes"
$makefile_loop5:
$ ! 
$makefile_loop4a:
$ ! 
$ ! Found a line with 'LIBRARIES ='
$ ! 
$makefile_loop4d:
$ ! 
$ ! Read the next record.
$ ! 
$  Read/end=makefile_loop4_exit/error=makefile_readerror make_file mfrec
$ ! 
$ ! End of the libraries list?  Yes, continue
$ ! 
$  if (mfrec .eqs. "") then goto makefile_loop4_exit
$ ! 
$ ! No. Search for the line continuation character
$ ! 
$  bslash = f$locate("\", mfrec)
$ ! 
$ ! Is it there?
$ ! 
$  if (bslash .eq. f$length(mfrec))
$  then
$    tmp_libnam = mfrec
$    end_loop4 = "yes"
$  else
$    tmp_libnam = f$extract(0, (bslash - 1), mfrec)
$  endif
$ ! 
$ ! Remove ALL whitespace from string
$ ! 
$  ttmp_libnam = f$edit(tmp_libnam, "COLLAPSE, TRIM")
$ ! 
$ ! Create the library file entry
$ ! 
$  if (first_lib .eqs. "yes")
$  then
$    libnam = f$trnlnm("PEGASUS_VMSHOMEA") + "lib]" + "lib" + "''ttmp_libnam'" + "/lib"
$    first_lib = "no"
$  else
$    libnam = "lib" + "''ttmp_libnam'" + "/lib"
$  endif
$ ! 
$ ! Write it too the option file
$ ! 
$  write/error=optfile_writeerror optfile "''libnam'"
$ ! 
$ ! Try again
$ ! 
$  if (end_loop4 .eqs. "no")
$  then
$   goto makefile_loop4d
$  endif
$makefile_loop4_exit:
$ ! 
$ ! Close the makefile
$ ! 
$  close make_file
$ ! 
$  ssllibdir = f$trnlnm("PEGASUS_OPENSSLLIB")
$  if (ssllibdir .EQS. "") then goto No_ssl
$ ! 
$  ssllib1 = "pegasus_openssllib:libssl32/lib"
$  ssllib2 = "libcrypto32/lib"
$  write/error=optfile_writeerror optfile "''ssllib1'"
$  write/error=optfile_writeerror optfile "''ssllib2'"
$No_ssl:
$ ! 
$ ! Looking for "SHARE_COPY" in a record
$ ! 
$  if P4 .eqs. ""
$  Then
$    Exit
$  Endif
$ ! 
$  'MyOut "%VMSCROPT - SHARE_COPY defined"
$  tmp_vecsym = "PegasusCreateProvider"
$ ! 
$ ! 
$ ! Looking for "VMS_VECTOR =" in a record
$ ! 
$ ! Reopen the makefile
$ ! 
$  Open/read/error=makefile_openerror4 make_file 'mfile
$ ! 
$ ! read the next record
$ ! 
$makefile_loop9:
$  Read/end=makefile_loop9_exit2/error=makefile_readerror make_file mfrec
$ ! 
$ ! Looking for "VMS_VECTOR =" in a record
$ ! 
$  vmsvec = f$locate("VMS_VECTOR =", mfrec)
$ ! 
$ ! Find it!  No, try again
$ ! 
$  if (vmsvec .eq. f$length(mfrec)) then goto makefile_loop9
$makefile_loop9_exit1:
$ ! 
$ ! point to the start of the program name
$ ! 
$  vmsvec = vmsvec + 12
$ ! 
$ ! Extract the vector symbol from the record
$ ! 
$  tmp_vecsym = f$extract(vmsvec, (f$length(mfrec) - vmsvec), mfrec)
$makefile_loop9_exit2:
$  'MyOut "%VMSCROPT -  tmp_vecsym = ''tmp_vecsym'"
$ ! 
$ ! Remove ALL whitespace from string
$ ! 
$  ttmp_vecsym = f$edit(tmp_vecsym, "COLLAPSE, TRIM")
$  'MyOut "%VMSCROPT -  ttmp_vecsym = ''ttmp_vecsym'"
$  vecsym = "SYMBOL_VECTOR=(" + "''ttmp_vecsym'" + "=PROCEDURE)"
$  'MyOut "%VMSCROPT -  vecsym = ''vecsym'"
$ ! 
$ ! Write it too the option file
$ ! 
$  write/error=optfile_writeerror optfile "''vecsym'"
$ ! 
$ ! Close the makefile.
$ ! 
$  Close make_file
$ ! 
$ ! Close the options file.
$ ! 
$  Close optfile
$makefile_loop9_exit3:
$ ! 
$  Exit
$ !
$optfile_eof:
$ exit
$ ! 
$makefile_openerror1:
$    Write sys$output "%VMSCROPT - Can't find makefile! 1 - ''mfile'"
$ ! 
$  Exit
$ ! 
$makefile_openerror2:
$    Write sys$output "%VMSCROPT - Can't find makefile! 2 - ''mfile'"
$ ! 
$  Exit
$ ! 
$makefile_openerror3:
$    Write sys$output "%VMSCROPT - Can't find makefile! 3 - ''mfile'"
$ ! 
$  Exit
$ ! 
$makefile_openerror4:
$    Write sys$output "%VMSCROPT - Can't find makefile! 4 - ''mfile'"
$ ! 
$  Exit
$ ! 
$makefile_readerror:
$    Write sys$output "%VMSCROPT - Error reading makefile! - ''mfile'"
$ ! 
$  Exit
$ ! 
$makefile2_openerror:
$    Write sys$output "%VMSCROPT - Can't find makefile! - ''mfile1'"
$    Show Default
$ ! 
$  Exit
$ ! 
$makefile_readerror1:
$    Write sys$output "%VMSCROPT - Error reading makefile! - ''mfile1'"
$ ! 
$  Exit
$ ! 
$optfile_openerror:
$    Write sys$output "%VMSCROPT - Can't open options file! - ''Optname'"
$ ! 
$  Exit
$ ! 
$optfile_writeerror:
$    Write sys$output "%VMSCROPT - Error writing options file! - ''Optname'"
$ ! 
$  Exit
$ !
$run_error1:
$ ! 
$  Write sys$output "%VMSCROPT - Can't find option filename!"
$  Exit
$ ! 
$run_error2:
$ ! 
$  Write Sys$output "%VMSCROPT - Makefile.; libraries.mak line is not right!"
$  Close 'mfile
$  Close 'Optname
$  Exit
$ ! 
$run_error1:
$ ! 
$  Exit
$ ! 
$run_error1:
$ ! 
$  Exit
$ ! 
$run_error3:
$ ! 
$  Exit
$ ! 
$run_error0:
$ ! 
$  Exit
$ ! 
$ ! 
$ ! 
$ exit
$ !
