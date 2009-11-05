#include <iostream>

#include <nel/misc/types_nl.h>

#ifdef NL_OS_WINDOWS
#include <conio.h>
#define KEY_ESC		27
#define KEY_ENTER	13
#endif // NL_OS_WINDOWS

//#include <angelscript.h>
#include "wwscript/ScriptEngine/ScriptManager.h"
#include "wwscript/ScriptEngine/ScriptVariable.h"
#include "wwscript/ScriptEngine/ScriptFunctionInstance.h"
#include "wwscript/GlobalProperty/PropertyManager.h"
#include "wwscript/GlobalProperty/PropertyMap.h"
#include "wwscript/GlobalProperty/ConstantIntProperty.h"
#include "wwscript/GlobalProperty/PropertyTemplate.h"

#include "wwcommon/CGameEventRegistrar.h"
#include "wwcommon/CFactoryRegistrar.h"
#include "wwcommon/CBaseProperty.h"
//#include "wwcommon/ISimulationObj.h"
#include "wwcommon/IBaseSimulation.h"
#include "wwcommon/IGameEvent.h"
#include "wwcommon/CGameSpawnRequestEvent.h"
#include "wwcommon/CGameUnspawnRequestEvent.h"

#include "nel/misc/matrix.h"

class A {
public:
    int Foo;
};

// Make a fake simulation implementation.
class CSimulationImpl : public WWCOMMON::IBaseSimulation {
public:
	NLMISC_DECLARE_CLASS(CSimulationImpl);
	bool attachUser(uint32 uid, uint32 sobid) { return true; };
	void detachUser(uint32 uid, uint32 sobid) { };
	void init() {
		std::string retBankName, globRetBank;
		retBankName="snowballs.rbank";
		globRetBank="snowballs.gr";
		// pacs stuff.
		m_RetrieverBank = NLPACS::URetrieverBank::createRetrieverBank(retBankName.c_str());
		if(m_RetrieverBank == NULL)
			nlinfo("what the fuck bitch.");
		m_GlobalRetriever = NLPACS::UGlobalRetriever::createGlobalRetriever(globRetBank.c_str(), m_RetrieverBank);

		nlinfo("Create Move Container.");
		m_MoveContainer = NLPACS::UMoveContainer::createMoveContainer(m_GlobalRetriever, 100, 100, 6.0);
		WWCOMMON::IBaseSimulation::init();
		WWCOMMON::CGameEventServer::instance().setDeltaMultiplier(0.0f);
	}

	void update() {
		// Process the basic functions.
		WWCOMMON::IBaseSimulation::update();
	}

};

class CScriptedGameEventListener : public WWCOMMON::IGameEventListener {
public:
	CScriptedGameEventListener(const WWSCRIPT::ScriptFunction *function) {
		if(function == NULL) {
			nlwarning("Invalid function passed to scripted listener.");
		} else {
			nlinfo("**** Initializing scripted game event listener ****");
			m_ScriptFunction = function;
			WWCOMMON::CGameEventServer::instance().addListener(this,WWCOMMON::CGameEventServer::ALL_TYPES);
		}
	}

	virtual bool observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event) { return true; };
	virtual bool observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event) { return true; };

	virtual bool observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
		WWSCRIPT::ScriptFunctionInstance *inst = m_ScriptFunction->getInstance();

		inst->getArg("gameEvent")->setValue((WWCOMMON::IGameEvent*)event);
		inst->execute();
		return true;
	}

	
private:
	const WWSCRIPT::ScriptFunction *m_ScriptFunction;
};

int main(int argc, char **argv) {
	NLMISC::CApplicationContext myApplicationContext;
	nlinfo("Starting up script sample application. ");
	
	NLMISC::CPath::addSearchPath("data", true, false, NULL);

	// Register game and simulation object events.
	WWCOMMON::registerEvents();
	WWCOMMON::registerCommonFactoryObjects();

	// Register our mock simulation implementation.
	try	{
		NLMISC_REGISTER_CLASS(CSimulationImpl);
	} catch(NLMISC::ERegisteredClass &e) {
		nlwarning("CSimulationImpl is already registered.");
	}

	// Retrieve a copy of our simulation implementation.
	CSimulationImpl *simulation = dynamic_cast<CSimulationImpl *>(getSimulation());
	simulation->init();

	// Now initialize the scripting system.
	WWSCRIPT::ScriptManager::getInstance().initialize();
	WWSCRIPT::ScriptManager::getInstance().initializeScripts();

	// Configure property manager.
	//WWSCRIPT::PropertyMap local;
	//WWSCRIPT::PropertyManager::instance().setPropertyMap("LOCAL", &local);

	// Create and add the scripted event listener.
	const WWSCRIPT::Script *exampleScr = WWSCRIPT::ScriptManager::getInstance().getScript("ExampleScript");
	if(!exampleScr) {
		nlerror("Failed to retrieve: ExampleScript");
		return -1;
	}
	const WWSCRIPT::ScriptFunction *func = exampleScr->getFunction("handleGameEvent");
	CScriptedGameEventListener *listener = new CScriptedGameEventListener(func);

	bool running = true;
	while(running) {
		simulation->updateTime();
		simulation->update();
		int c;
		if(kbhit()) {
			c = getch();
			if (c == KEY_ESC) {
				nlinfo("Escape pressed, ending script sample!");
				running=false; // FINSIH
			} else if (c == KEY_ENTER) {
				nlinfo("Enter pressed, submit an event to the server.");
				WWCOMMON::CGameSpawnRequestEvent *spawnEvt = new WWCOMMON::CGameSpawnRequestEvent();
				spawnEvt->CharacterID=3;
				WWCOMMON::CGameEventServer::instance().postEvent(spawnEvt);
			}
		}
	}
}

int whatevs() {
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
