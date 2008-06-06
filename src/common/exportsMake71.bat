@echo off
@rem Do the vsvars32.bat stuff.
@SET VSINSTALLDIR=C:\Program Files\Microsoft Visual Studio .NET 2003\Common7\IDE
@SET VCINSTALLDIR=C:\Program Files\Microsoft Visual Studio .NET 2003
@SET FrameworkDir=C:\WINDOWS\Microsoft.NET\Framework
@SET FrameworkVersion=v1.1.4322
@SET FrameworkSDKDir=C:\Program Files\Microsoft Visual Studio .NET 2003\SDK\v1.1
@if "%VCINSTALLDIR%"=="" set VCINSTALLDIR=%VSINSTALLDIR%
@set DevEnvDir=%VSINSTALLDIR%
@set MSVCDir=%VCINSTALLDIR%\VC7
@echo Setting environment for using Microsoft Visual Studio .NET 2003 tools.
@echo (If you have another version of Visual Studio or Visual C++ installed and wish
@echo to use its tools from the command line, run vcvars32.bat for that version.)
@set PATH=%DevEnvDir%;%MSVCDir%\BIN;%VCINSTALLDIR%\Common7\Tools;%VCINSTALLDIR%\Common7\Tools\bin\prerelease;%VCINSTALLDIR%\Common7\Tools\bin;%FrameworkSDKDir%\bin;%FrameworkDir%\%FrameworkVersion%;%PATH%;
@set INCLUDE=%MSVCDir%\ATLMFC\INCLUDE;%MSVCDir%\INCLUDE;%MSVCDir%\PlatformSDK\include\prerelease;%MSVCDir%\PlatformSDK\include;%FrameworkSDKDir%\include;%INCLUDE%
@set LIB=%MSVCDir%\ATLMFC\LIB;%MSVCDir%\LIB;%MSVCDir%\PlatformSDK\lib\prerelease;%MSVCDir%\PlatformSDK\lib;%FrameworkSDKDir%\lib;%LIB%
pause
"C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\bin\dumpbin.exe" /linkermember:1 ..\..\lib\wwcommon_rd.lib | find "?CreateInstance@?$CFactoryRegistrar" > ..\..\lib\wwcommon_linker.sf1
"C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\bin\dumpbin.exe" /linkermember:1 ..\..\lib\wwcommon_rd.lib | find "?RegisterEventType@?$CGameEventRegistrar" >> ..\..\lib\wwcommon_linker.sf1
pause
cscript exportRegistrar.vbs ..\..\lib\wwcommon_linker.sf1
