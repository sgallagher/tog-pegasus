@echo off
jdaux %1 %2

if errorlevel 2 goto jdlast
if errorlevel 1 goto done

:chdir
    c:\temp\jdtmp.bat
    goto done	
:jdlast
    c:\temp\jdlast.bat
    goto done
:done
