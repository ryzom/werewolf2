#include <iostream>
#include <angelscript.h>
#include "ScriptManager.h"
#include "ScriptFunctionInstance.h"
#include "PropertyManager.h"
#include "ConstantIntProperty.h"

int main(int argc, char **argv) {
	int numLights = 2;
	int numShadowCasters = 1;
	int numIntersectors = 1;

	// Register properties to maps
	PropertyMap engine;
	engine.registerProperty(new ConstantIntProperty(&numLights, 1, "ENGINE.NumberOfLights"));
	PropertyMap local;
	local.registerProperty(new ConstantIntProperty(&numShadowCasters, 1, "LOCAL.NumberOfShadowCasters"));
	local.registerProperty(new ConstantIntProperty(&numIntersectors, 1, "LOCAL.NumberOfIntersectors"));
	
	PropertyManager::instance().setPropertyMap("ENGINE", &engine);
	PropertyManager::instance().setPropertyMap("LOCAL", &local);

	Script* shader = ScriptManager::instance().loadScript("leaf_shader.xml", "gpu");

	// Do some preparation before execution
	const ScriptFunction *func = shader->getFunction("getShader");
	if(!func) {
		std::cout << "Function getShader not found\n" << std::endl;
		return -1;
	}

	ScriptFunctionInstance *inst = func->getInstance();
	// SET ARGUMENTS AUTOMATICALLY
	inst->setBoundArgs();
	// EXECUTE
	inst->execute();
	// GET THE RETURN VALUE
	asCScriptString* ret;
	inst->getRetVal()->getValue((void**)&ret); // <- pointer to a pointer
	std::cout << "************ FIST TEST ************" << std::endl << ret->buffer << std::endl;
	std::cout << "***********************************\n\n" << std::endl;

	// CHANGE SOMETHING AND TRY AGAIN
	numShadowCasters = 0;
	numIntersectors = 0;
	inst->setBoundArgs();
	inst->execute();
	inst->getRetVal()->getValue((void**)&ret);
	std::cout << "*********** SECOND TEST ***********" << std::endl << ret->buffer << std::endl;
	std::cout << "***********************************\n\n" << std::endl;

	// SET ARGUMENTS MANUALLY
	inst->getArg("numLights")->setValue((int)1); // make sure the constant is interpreted as the type the variable is supposed to be - if you call getValue(uint) on an int variable - it will return false and do nothing (might change in the future)
	inst->getArg("numShadowCasters")->setValue((int)1);
	inst->getArg("numIntersectors")->setValue(numIntersectors);
	inst->execute();
	inst->getRetVal()->getValue((void**)&ret);
	std::cout << "*********** THIRD TEST ************" << std::endl << ret->buffer << std::endl;
	std::cout << "***********************************\n\n" << std::endl;

	// DONE WITH THIS FUNCTION
	delete inst;

	return 0;
}