#include <iostream>
//#include <angelscript.h>
#include "wwscript/ScriptEngine/ScriptManager.h"
#include "wwscript/ScriptEngine/ScriptVariable.h"
#include "wwscript/ScriptEngine/ScriptFunctionInstance.h"
#include "wwscript/GlobalProperty/PropertyManager.h"
#include "wwscript/GlobalProperty/PropertyMap.h"
#include "wwscript/GlobalProperty/ConstantIntProperty.h"
#include "wwscript/GlobalProperty/PropertyTemplate.h"

#include "nel/misc/matrix.h"

class A {
public:
    int Foo;
};


int main(int argc, char **argv) {
	NLMISC::CApplicationContext myApplicationContext;
	nlinfo("Starting up script sample application. ");
	
	int numLights = 2;
	int numShadowCasters = 1;
	int numIntersectors = 1;
	NLMISC::CMatrix *matrix = new NLMISC::CMatrix();
	NLMISC::CVector *vector = new NLMISC::CVector();
	vector->x = 1.1f;
	vector->y = 1.2f;
	vector->z = 1.3f;

	// Register properties to maps
	WWSCRIPT::PropertyMap engine;
	engine.registerProperty(new WWSCRIPT::ConstantIntProperty(&numLights, 1, "NumberOfLights"));
	WWSCRIPT::PropertyMap local;
	local.registerProperty(new WWSCRIPT::ConstantIntProperty(&numShadowCasters, 1, "NumberOfShadowCasters"));
	local.registerProperty(new WWSCRIPT::ConstantIntProperty(&numIntersectors, 1, "NumberOfIntersectors"));
	local.registerProperty(new SCRIPT_PROPERTY(NLMISC::CMatrix*, matrix, "matrix"));
	local.registerProperty(new SCRIPT_PROPERTY(NLMISC::CVector*, vector, "testVec"));
	
	WWSCRIPT::PropertyManager::instance().setPropertyMap("ENGINE", &engine);
	WWSCRIPT::PropertyManager::instance().setPropertyMap("LOCAL", &local);

	NLMISC::CPath::addSearchPath("data", true, false);
	WWSCRIPT::ScriptManager::getInstance().initialize();
	WWSCRIPT::ScriptManager::getInstance().initializeScripts();

	const WWSCRIPT::Script *exampleScr = WWSCRIPT::ScriptManager::getInstance().getScript("ExampleScript");
	if(!exampleScr) {
		nlerror("Failed to retrieve: ExampleScript");
		return -1;
	}

	// Do some preparation before execution
	//const WWSCRIPT::Script* shader = WWSCRIPT::ScriptManager::getInstance().getScript("ShaderScript");
	const WWSCRIPT::ScriptFunction *func = exampleScr->getFunction("getFoobar");
	const WWSCRIPT::ScriptFunction *testFunc = exampleScr->getFunction("testVectors");
	//const WWSCRIPT::ScriptFunction *func = shader->getFunction("getShader");
	if(!testFunc) {
		nlerror("Function getShader not found\n");
		return -1;
	}

	nlinfo("*** Executing ShaderScript.getShader - Initial Execution ***");
	WWSCRIPT::ScriptFunctionInstance *inst = func->getInstance();
	// SET ARGUMENTS AUTOMATICALLY
	inst->setBoundArgs();
	// EXECUTE
	inst->execute();
	// GET THE RETURN VALUE
	//CScriptString* ret;
	//inst->getRetVal()->getValue((void**)&ret); // - pointer to a pointer
	//std::cout << "************ FIST TEST ************" << std::endl << ret->buffer << std::endl;
	//std::cout << "***********************************\n\n" << std::endl;

	nlinfo("*** Executing ExampleScript.testVector ***");
	WWSCRIPT::ScriptFunctionInstance *inst2 = testFunc->getInstance();
	// CHANGE SOMETHING AND TRY AGAIN
	//numShadowCasters = 0;
	//numIntersectors = 0;
	inst2->setBoundArgs();
	inst2->execute();
	//inst->getRetVal()->getValue((void**)&ret);
	//std::cout << "*********** SECOND TEST ***********" << std::endl << ret->buffer << std::endl;
	std::cout << "***********************************\n\n" << std::endl;

	//nlinfo("*** Executing ShaderScript.getShader - Execution No-Binding ***");
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
