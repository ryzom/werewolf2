#include <iostream>
#include "../SimpleTest/ScriptManager.h"
#include "../SimpleTest/ScriptFunctionInstance.h"

void echo(std::string text) {
	std::cout << text;
}

int main(int argc, char **argv) {

	asIScriptEngine* engine = ScriptManager::instance().getEngine();

	engine->RegisterGlobalFunction("void echo(string text)", asFUNCTION(echo), asCALL_CDECL);

	std::string greeting1("Greets from instance #1!");
	std::string greeting2("Howdy from instance #2!");

	Script* s = ScriptManager::instance().loadScript("script.xml", "test");
	ScriptFunctionInstance* greet1 = s->getFunction("greet")->getInstance();
	greet1->getArg("greeting")->setValue((void*)&greeting1);
	greet1->execute();

	ScriptFunctionInstance* greet2 = s->getFunction("greet")->getInstance();
	greet2->getArg("greeting")->setValue((void*)&greeting2);
	greet2->execute();

	// it should remember the args
	greet1->execute();

	delete greet1;
}