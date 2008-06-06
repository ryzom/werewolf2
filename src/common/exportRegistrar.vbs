' get the command-line arguments. 0 should be the lib name.
Dim oFile
oFile = WScript.Arguments.Item(0)
	
Dim sFileLine

Set oFS = CreateObject("Scripting.FileSystemObject")
Set oFS2 = CreateObject("Scripting.FileSystemObject")
Set oFS3 = CreateObject("Scripting.FileSystemObject")

Set oHeaderFile = oFS3.CreateTextFile("..\common\include\wwcommon\stdpragma.h",True)

'WScript.Echo oFile

If oFS.FileExists(oFile) Then

'	Set MyCsv =     MyFso.OpenTextFile("c:\\perfname.csv", 1)
	Set oTextStream = oFS.OpenTextFile(oFile,1)
	
	' Cycle through the lines
	Do Until oTextStream.AtEndOfStream
		arrLine = Split(LTrim(oTextStream.ReadLine), " ")
		sPosition = arrLine(0)
		sSymbol = arrLine(1)
		
		oHeaderFile.WriteLine "#pragma comment(linker, ""/include:" & sSymbol & """)"
		'WScript.Echo "#pragma comment(linker, ""/include:" & sSymbol & """)"
		
	Loop
	
	oHeaderFile.Close
Else
	WScript.Echo "ERROR! Unable to open export symbol dump."
End If
