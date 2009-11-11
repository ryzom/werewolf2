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
#include "wwcommon/ISobHandler.h"
#include "wwcommon/IBaseSimulation.h"
#include "wwcommon/IGameEvent.h"
#include "wwcommon/ISobEvent.h"
#include "wwcommon/CGameSpawnRequestEvent.h"
#include "wwcommon/CGameUnspawnRequestEvent.h"
#include "wwcommon/CSobMoveEvent.h"
#include "wwcommon/CSobOrientEvent.h"
#include "wwcommon/CSobStrafeEvent.h"
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
		// Get a function instance to execute.
		WWSCRIPT::ScriptFunctionInstance *inst = m_ScriptFunction->getInstance();

		// Pass the event object to the script.
		inst->getArg("gameEvent")->setValue((WWCOMMON::IGameEvent*)event);
		inst->execute();
		
		// Delete the instance once we're finished.
		delete inst;

		return true;
	}
private:
	const WWSCRIPT::ScriptFunction *m_ScriptFunction;
	std::string m_scriptName;
	std::string m_functionName;
};

class CScriptedSobEventHandler : public WWCOMMON::ISobHandler {
public:
	CScriptedSobEventHandler(const WWSCRIPT::ScriptFunction *function) {
		nlinfo("**** Initializing scripted game event listener ****");
		// Save the function.
		if(function == NULL) {
			nlwarning("Invalid function passed to scripted listener.");
		} 	
		m_ScriptFunction = function;
		
		setPriority(0);
	}

	CScriptedSobEventHandler(std::string scriptName, std::string functionName) {
		nlinfo("**** Initializing scripted game event listener ****");

		const WWSCRIPT::Script *script = WWSCRIPT::ScriptManager::getInstance().getScript(scriptName.c_str());
		if(!script) {
			nlerror("Failed to retrieve: %s", scriptName.c_str());
			return;
		}
		m_ScriptFunction = script->getFunction(functionName.c_str());
	
		// Save the function.
		if(m_ScriptFunction == NULL) {
			nlwarning("Invalid function passed to scripted listener.");
		}
		setPriority(0);
	}

	void addHandledEvent(uint32 eventid) {
		m_HandledEvents.push_back(eventid);
	}


	bool handleSobEvent(NLMISC::CSmartPtr<WWCOMMON::ISobEvent> event, WWCOMMON::ISimulationObj* subject) {
		// Get a function instance to execute.
		WWSCRIPT::ScriptFunctionInstance *inst = m_ScriptFunction->getInstance();

		// Pass the event object to the script.
		inst->getArg("sobEvent")->setValue((WWCOMMON::ISobEvent*)event);
		inst->getArg("sobSubject")->setValue((WWCOMMON::ISimulationObj*)subject);
		inst->execute();
		
		// Delete the instance once we're finished.
		delete inst;

		return true;
	}

	eventList *getEventList() {
		return &m_HandledEvents;
	}
private:
	const WWSCRIPT::ScriptFunction *m_ScriptFunction;
	eventList m_HandledEvents;
};

void demonstrateScriptedEventListener() {
	nlinfo("*** Executing a script event. ***");
	// Retrieve a copy of our simulation implementation.
	CSimulationImpl *simulation = dynamic_cast<CSimulationImpl *>(getSimulation());

	// Create and add the scripted event listener.
	const WWSCRIPT::Script *exampleScr = WWSCRIPT::ScriptManager::getInstance().getScript("ExampleScript");
	if(!exampleScr) {
		nlerror("Failed to retrieve: ExampleScript");
		return;
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
			} else if (c == 'r') {
				// Recompile script.
				nlinfo("Recompiling script.");
				exampleScr->recompileScript();				
			}
		}
	}
}

void demonstrateScriptedSobHandler() {
	nlinfo("*** Executing a script sob event. ***");
	// Retrieve a copy of our simulation implementation.
	CSimulationImpl *simulation = dynamic_cast<CSimulationImpl *>(getSimulation());

	// Create and add the scripted event listener.
	const WWSCRIPT::Script *exampleScr = WWSCRIPT::ScriptManager::getInstance().getScript("ExampleScript");
	if(!exampleScr) {
		nlerror("Failed to retrieve: ExampleScript");
		return;
	}
	const WWSCRIPT::ScriptFunction *func = exampleScr->getFunction("handleSobEvent");
	CScriptedSobEventHandler *handler = new CScriptedSobEventHandler(func);
	handler->addHandledEvent(EVENT_ID(CSobMoveEvent));

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
				//WWCOMMON::CGameSpawnRequestEvent *spawnEvt = new WWCOMMON::CGameSpawnRequestEvent();
				//spawnEvt->CharacterID=3;
				//WWCOMMON::CGameEventServer::instance().postEvent(spawnEvt);
			} else if (c == 'r') {
				// Recompile script.
				nlinfo("Recompiling script.");
				exampleScr->recompileScript();				
			}
		}
	}
}

void demonstrateManualScriptCall() {
	nlinfo("*** Executing a script manually. ***");

	// The vector we'll use.
	NLMISC::CVector *testVec = new NLMISC::CVector(1.1f, 1.2f, 1.3f);
	const WWSCRIPT::Script *exampleScr = WWSCRIPT::ScriptManager::getInstance().getScript("ExampleScript");
	if(!exampleScr) {
		nlerror("Failed to retrieve: ExampleScript");
		return;
	}
	const WWSCRIPT::ScriptFunction *func = exampleScr->getFunction("testVectors");
	WWSCRIPT::ScriptFunctionInstance *inst = func->getInstance();
	inst->getArg("testVec")->setValue(testVec);
	inst->execute();

	// DONE WITH THIS FUNCTION
	delete inst;
	delete testVec;
}

void demonstrateScriptBinding() {
	// Create a vector to bind.
	NLMISC::CVector *vector = new NLMISC::CVector(1.1f,1.2f,1.3f);

	// Create the "LOCAL" property map.
	WWSCRIPT::PropertyMap local;
	// Register the vector as a property.
	local.registerProperty(new SCRIPT_PROPERTY(NLMISC::CVector*, vector, "testVec"));
	// Register the property map with the manager.
	WWSCRIPT::PropertyManager::instance().setPropertyMap("LOCAL", &local);

	const WWSCRIPT::Script *exampleScr = WWSCRIPT::ScriptManager::getInstance().getScript("ExampleScript");
	if(!exampleScr) {
		nlerror("Failed to retrieve: ExampleScript");
		return;
	}

	// Retrieve the function "testVectors"
	const WWSCRIPT::ScriptFunction *func = exampleScr->getFunction("testVectors");

	// Retrieve an instance of the testVectors function for execution.
	WWSCRIPT::ScriptFunctionInstance *inst = func->getInstance();

	// Tell the function instance to bind its arguments.
	inst->setBoundArgs();

	// Execute.
	inst->execute();

	// Change the vector property:
	vector->set(3.1f, 3.2f, 3.3f);

	// Then execute again.
	inst->setBoundArgs();
	inst->execute();

	// Clean up.
	delete inst;
	delete vector;
}

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

	demonstrateManualScriptCall();
	demonstrateScriptBinding();
	demonstrateScriptedEventListener();
	demonstrateScriptedSobHandler();
}
