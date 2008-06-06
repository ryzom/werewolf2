// SimpleExtension.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/python.hpp>
#include <pythonrun.h>
#include <iostream>
#include <string>
#include <conio.h>
#include "ISingleton.h"

using namespace boost::python::api;
using namespace boost::python;

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

char const* greet() {
   return "hello, world";
}

class TestClass {
public:
	TestClass() : f(0) { ; }
	float f;

	int getI() { return m_i; }
	void setI(int i) { m_i = i; }
private:
	int m_i;
};

TestClass g_t;

TestClass* getGT() {
	return &g_t;
}

class Manager : public ISingleton<Manager> {
public:
	Manager() { m_tc.setI(20); }

	TestClass& getTC() { return m_tc; }
private:
	TestClass m_tc;
};

void init_module_test() {
    def("greet", greet);
	class_<TestClass>("TestClass")
		.def_readwrite("f", &TestClass::f)
		.add_property("i", &TestClass::getI, &TestClass::setI);
	def("getGT", getGT, return_value_policy<reference_existing_object>());
	class_<Manager> managerWrapper("Manager");

	managerWrapper
		.def("instance", &Manager::instance, return_value_policy<reference_existing_object>())
		.staticmethod("instance")
		.def("getTC", &Manager::getTC, return_value_policy<reference_existing_object>());

	managerWrapper
		.add_property("tc", managerWrapper.attr("getTC"));
}

void inittest() { 
	boost::python::detail::init_module("test", &init_module_test);
	PyImport_AddModule("test");
}

int _tmain(int argc, _TCHAR* argv[])
{
	Console c;
	c.configure();
	inittest();

	g_t.f = 22.22f;
	g_t.setI(34);

	while(c.update()) {
	}
	return 0;
}

