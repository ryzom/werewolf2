// Hello.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <python.h>

int _tmain(int argc, _TCHAR* argv[])
{
	Py_Initialize();

	FILE* file = fopen("hello.py", "r");
	PyRun_SimpleFile(file, "hello.py");
	fclose(file);
	getch();

	Py_Finalize();
	return 0;
}

