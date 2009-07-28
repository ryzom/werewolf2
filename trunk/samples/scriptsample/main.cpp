#include <iostream>
//#include <angelscript.h>
#include "wwscript/ScriptEngine/ScriptManager.h"
//#include "wwscript/ScriptEngine/ScriptFunctionInstance.h"
//#include "wwscript/ScriptEngine/scriptstring.h"
//#include "wwscript/GlobalProperty/PropertyManager.h"
//#include "wwscript/GlobalProperty/PropertyMap.h"
//#include "wwscript/GlobalProperty/ConstantIntProperty.h"
//#include "wwcommon/CGameEventServer.h"

int main(int argc, char **argv) {
	NLMISC::CApplicationContext myApplicationContext;
	nlinfo("Starting up script sample application. ");
	
	int numLights = 2;
	int numShadowCasters = 1;
	int numIntersectors = 1;

	// Register properties to maps
	//WWSCRIPT::PropertyMap engine;
	//engine.registerProperty(new WWSCRIPT::ConstantIntProperty(&numLights, 1, "ENGINE.NumberOfLights"));
	//WWSCRIPT::PropertyMap local;
	//local.registerProperty(new WWSCRIPT::ConstantIntProperty(&numShadowCasters, 1, "LOCAL.NumberOfShadowCasters"));
	//local.registerProperty(new WWSCRIPT::ConstantIntProperty(&numIntersectors, 1, "LOCAL.NumberOfIntersectors"));
	
	//WWSCRIPT::PropertyManager::instance().setPropertyMap("ENGINE", &engine);
	//WWSCRIPT::PropertyManager::instance().setPropertyMap("LOCAL", &local);

	//WWSCRIPT::Script* shader = WWSCRIPT::ScriptManager::instance().loadScript("leaf_shader.xml", "gpu");
	NLMISC::CPath::addSearchPath("data", true, false);
	nlinfo("Starting to initialize scripts.");
	WWSCRIPT::ScriptManager::getInstance().initialize();
	WWSCRIPT::ScriptManager::getInstance().initializeScripts();
	
	nlinfo("Done initializing scripts.");
	//const WWSCRIPT::Script *shader = WWSCRIPT::ScriptManager::instance().getScript("gpu");

	// Do some preparation before execution
	//const WWSCRIPT::ScriptFunction *func = shader->getFunction("getShader");
	//if(!func) {
	//	std::cout << "Function getShader not found\n" << std::endl;
	//	return -1;
	//}

	//WWSCRIPT::ScriptFunctionInstance *inst = func->getInstance();
	// SET ARGUMENTS AUTOMATICALLY
	//inst->setBoundArgs();
	// EXECUTE
	//inst->execute();
	// GET THE RETURN VALUE
	//asCScriptString* ret;
	//inst->getRetVal()->getValue((void**)&ret); // <- pointer to a pointer
	//std::cout << "************ FIST TEST ************" << std::endl << ret->buffer << std::endl;
	//std::cout << "***********************************\n\n" << std::endl;

	// CHANGE SOMETHING AND TRY AGAIN
	//numShadowCasters = 0;
	//numIntersectors = 0;
	//inst->setBoundArgs();
	//inst->execute();
	//inst->getRetVal()->getValue((void**)&ret);
	//std::cout << "*********** SECOND TEST ***********" << std::endl << ret->buffer << std::endl;
	//std::cout << "***********************************\n\n" << std::endl;

	// SET ARGUMENTS MANUALLY
	//inst->getArg("numLights")->setValue((int)1); // make sure the constant is interpreted as the type the variable is supposed to be - if you call getValue(uint) on an int variable - it will return false and do nothing (might change in the future)
	//inst->getArg("numShadowCasters")->setValue((int)1);
	//inst->getArg("numIntersectors")->setValue(numIntersectors);
	//inst->execute();
	//inst->getRetVal()->getValue((void**)&ret);
	//std::cout << "*********** THIRD TEST ************" << std::endl << ret->buffer << std::endl;
	//std::cout << "***********************************\n\n" << std::endl;

	// DONE WITH THIS FUNCTION
	//delete inst;

	return 0;
}