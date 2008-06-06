// HelloWorld.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/python.hpp>
#include <iostream>
#include <string>
#include <conio.h>

using namespace boost::python::api;
using namespace boost::python;

int _tmain(int argc, _TCHAR* argv[]) {
	Py_Initialize();

	object main_module((
    handle<>(borrowed(PyImport_AddModule("__main__")))));
	object main_namespace = main_module.attr("__dict__");

	handle<> ignored((PyRun_String(
	    "hello = file('hello.txt', 'w')\n"
		"hello.write('Hello world!')\n"
		"hello.close()"
	  , Py_file_input
	  , main_namespace.ptr()
	  , main_namespace.ptr())
	));

	FILE* file = fopen("hello.pyc", "r");
	PyRun_SimpleFile(file, "hello.pyc");
	fclose(file);

	object result((handle<>(
    PyRun_String("5 ** 2"
        , Py_eval_input
        , main_namespace.ptr()
        , main_namespace.ptr()))
	));

	std::cout << extract<double>(result) << std::endl;
	getch();

	Py_Finalize();
	return 0;
}

