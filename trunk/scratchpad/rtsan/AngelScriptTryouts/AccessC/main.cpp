#include <iostream>
#include "../SimpleTest/ScriptManager.h"
#include "../SimpleTest/ScriptFunctionInstance.h"

void echo(std::string text) {
	std::cout << text;
}

int main(int argc, char **argv) {

	asIScriptEngine* engine = ScriptManager::instance().getEngine();

	std::string greeting("I'm a test application to show off access of C++ variables and functions!\n");

	engine->RegisterGlobalProperty("string greeting", &greeting);
	engine->RegisterGlobalFunction("void echo(string text)", asFUNCTION(echo), asCALL_CDECL);

	Script* s = ScriptManager::instance().loadScript("script.xml", "test");
	ScriptFunctionInstance* test = s->getFunction("hello")->getInstance();
	test->execute();
	
	delete test;
}