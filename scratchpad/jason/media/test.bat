@echo off
:: Author: Jason Oppel
:: Description: 
:: use this script to replace text in a file(s)
:: usage: ir.bat texttoreplace replacementtext filename
:: you can use wildcards in any of the passed parameters
:: so *.* is valid for "filename" for instance
:: you need a copy of win32 sed (google for Unix Utils)
:: in your path or in the dir you're running this script
:: from.

erase TEMP.TXT > NUL

grep -l %1 %3 > FILES.TXT
for /F %%f in (files.txt) do (sed s/%1/%2/g %%f > TEMP.TXT
	copy %%f %%f.bak > NUL
	copy TEMP.TXT %%f > NUL
	ECHO Modified: %%f)
erase TEMP.TXT > NUL
erase FILES.TXT > NUL