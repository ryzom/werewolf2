#include <iostream>
#include "../SimpleTest/ScriptManager.h"
#include "../SimpleTest/ScriptFunctionInstance.h"

int main(int argc, char **argv) {
	Script* s = ScriptManager::instance().loadScript("script.xml", "test");
	ScriptFunctionInstance* test = s->getFunction("test")->getInstance();
	test->execute();
	std::cout << "Function \"void test()\" executed!" << std::endl;
	delete test;
}