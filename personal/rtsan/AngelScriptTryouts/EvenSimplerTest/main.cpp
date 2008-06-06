#include <iostream>
#include <string>
#include "../SimpleTest/ScriptManager.h"
#include "../SimpleTest/ScriptFunctionInstance.h"

int main(int argc, char **argv) {
	std::string progName(argv[0]);

	Script* s = ScriptManager::instance().loadScript("hello_world.xml", "test");
	ScriptFunctionInstance* hello = s->getFunction("hello")->getInstance();
	ScriptVariable *whoArg = hello->getArg("who");
	if(whoArg && whoArg->getType() == ScriptArg::STRING) {
		std::cout << "The argument is a string :)" << std::endl;
		whoArg->setValue((void*)&progName);
	}
	hello->execute();
	std::string *ret;
	hello->getRetVal()->getValue((void**)&ret);
	std::cout << *ret << std::endl;

	delete hello;
}