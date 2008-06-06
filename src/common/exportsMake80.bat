@echo off
call "%VS80COMNTOOLS%vsvars32.bat"

@set MSSdk="C:\Program Files\Microsoft Platform SDK"
@set MSVCDir=%VCINSTALLDIR%
@echo Setting environment for using Microsoft Visual Studio .NET 2003 tools.
@echo (If you have another version of Visual Studio or Visual C++ installed and wish
@echo to use its tools from the command line, run vcvars32.bat for that version.)
@set PATH=%DevEnvDir%;%MSVCDir%\BIN;%VS80COMNTOOLS%;%VS80COMNTOOLS%\bin;%FrameworkSDKDir%\bin;%FrameworkDir%\%FrameworkVersion%;%PATH%;
@set INCLUDE=%MSSdk%\INCLUDE;%MSSdk%\INCLUDE\mfc;%MSSdk%\include\atl;%FrameworkSDKDir%\include;%INCLUDE%
@set LIB=%MSSdk%\LIB;%FrameworkSDKDir%\lib;%LIB%
pause
"dumpbin.exe" /linkermember:1 ..\..\lib\wwcommon_rd.lib | find "?CreateInstance@?$CFactoryRegistrar" > ..\..\lib\wwcommon_linker.sf1
pause
cscript exportRegistrar.vbs ..\..\lib\wwcommon_linker.sf1
