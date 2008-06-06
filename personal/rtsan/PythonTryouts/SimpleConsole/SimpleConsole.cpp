// SimpleConsole.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/python.hpp>
#include <pythonrun.h>
#include <iostream>
#include <string>
#include <conio.h>

using namespace boost::python::api;
using namespace boost::python;

object main_module;
object main_namespace;

class Console {
public:
	Console();
	~Console();

	void configure();
	bool update();

	void execCommand(std::string &str);

private:
	object* main_module;
	object main_namespace;
};

Console::Console() {
	Py_Initialize();
}

Console::~Console() {
	delete main_module;
	Py_Finalize();
}

void Console::configure() {
	main_module = new object((
    handle<>(borrowed(PyImport_AddModule("__main__")))));
	main_namespace = main_module->attr("__dict__");
}

bool Console::update() {
	std::string input;
	input.resize(256);

	std::cout << "> ";
	std::cin.getline(&input[0], 256);

	// Trim unused characters
	input.resize(strlen(input.c_str()));

	// Interpret the command
	if( input == "quit" )
		return false;
	else if( input == "exit" )
		return false;
	else
		execCommand(input);

	return true;
}

void Console::execCommand(std::string &s) {
	try {
//		s = "str(" + s + ")";
		object result((handle<>(PyRun_String(s.c_str(),
			Py_single_input,
			main_namespace.ptr(),
			main_namespace.ptr()))));

//		std::string res_cmd = call_method<std::string>(result.ptr(),"__repr__");
//		std::cout << res_cmd << std::endl;
	} catch(error_already_set) {
		PyErr_Print();
	}
}

int _tmain(int argc, _TCHAR* argv[]) {
	Console c;
	c.configure();

	while(c.update()) {
	}

	return 0;
}

