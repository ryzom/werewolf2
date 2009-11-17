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
#include "wwscript/ScriptBindings/ScriptBinding.h"
#include "wwscript/ScriptBindings/ScriptWwcommonSimulationBindery.h"

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
#include "wwcommon/CPerformer.h"
#include "wwcommon/CSobSpawnEvent.h"
#include "nel/misc/matrix.h"

#include "CScriptedGameEventListener.h"
#include "CScriptedSobEventHandler.h"
#include "CSimulationImpl.h"

class BindScriptSampleObjects : public WWSCRIPT::ScriptBinding {
public:
	bool bindObjects() {
		asIScriptEngine *engine = WWSCRIPT::ScriptManager::getInstance().getEngine();
		int r;
		nlinfo("Binding CSimulationImpl");

		// Register Object Type
		r = engine->RegisterObjectType("CSimulationImpl", sizeof(CSimulationImpl), asOBJ_REF); nlassert(r>=0);

		// Register Behaviors, omit factory behavior so this interface cannot be created.
		r = engine->RegisterObjectBehaviour("CSimulationImpl", asBEHAVE_ADDREF, "void f()", asMETHOD(WWSCRIPT::asRefDummy,addRef), asCALL_THISCALL); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("CSimulationImpl", asBEHAVE_RELEASE, "void f()", asMETHOD(WWSCRIPT::asRefDummy,release), asCALL_THISCALL); nlassert(r>=0);

		// Register Parent Methods
		WWSCRIPT::ScriptWwcommonSimulationBindery::registerIBaseSimulation<CSimulationImpl>("CSimulationImpl");

		// Register object methods.
		r = engine->RegisterObjectMethod("CSimulationImpl", "bool userLogin(uint32 &in,uint32 &in)", asMETHODPR(CSimulationImpl, userLogin, (uint32,uint32), bool), asCALL_THISCALL); nlassert(r>=0);

		// Register inheritance.
		r = engine->RegisterObjectBehaviour("CSimulationImpl", asBEHAVE_IMPLICIT_REF_CAST, "IBaseSimulation@ f()", asFUNCTION((WWSCRIPT::refCast<CSimulationImpl,WWCOMMON::IBaseSimulation>)), asCALL_CDECL_OBJLAST); nlassert(r>=0);
		r = engine->RegisterObjectBehaviour("IBaseSimulation", asBEHAVE_REF_CAST, "CSimulationImpl@ f()", asFUNCTION((WWSCRIPT::refCast<WWCOMMON::IBaseSimulation,CSimulationImpl>)), asCALL_CDECL_OBJLAST); nlassert(r>=0);

		return true;
	}
};
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

	// Create and set up game event listener.
	const WWSCRIPT::ScriptFunction *gameFunc = exampleScr->getFunction("handleGameEvent");
	CScriptedGameEventListener *listener = new CScriptedGameEventListener(gameFunc);

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
			} else if (c == 'm') {
				demonstrateManualScriptCall();
			} else if (c == 'b') {
				demonstrateScriptBinding();
			} else if (c == 'r') {
				// Recompile script.
				nlinfo("Recompiling script.");
				exampleScr->recompileScript();				
			}
		}
	}

	delete listener;
//	delete handler;
	delete gameFunc;
//	delete sobFunc;
}

int main(int argc, char **argv) {
	NLMISC::CApplicationContext myApplicationContext;
	nlinfo("Starting up script sample application. ");
	
	NLMISC::CPath::addSearchPath("data", true, false, NULL);

	// Register game and simulation object events.
	WWCOMMON::registerEvents();
	WWCOMMON::registerCommonFactoryObjects();

	OF_REGISTER(WWCOMMON::CSobFactory, CActor, "sobActor");

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
	// Now add my "client side" bindings.
	WWSCRIPT::ScriptManager::getInstance().addBindingObj(new BindScriptSampleObjects());

	WWSCRIPT::ScriptManager::getInstance().initializeScripts();



	demonstrateScriptedEventListener();
}
