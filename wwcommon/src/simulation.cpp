//
// Standard Includes
//
#include "stdpch.h"

#include <nel/misc/class_registry.h>

#include "wwcommon/IBaseSimulation.h"

#include "wwcommon/CGameEventRegistrar.h"


// Events we're registering in here (temporary.)
#include "wwcommon/ISobStateChangeEvent.h"
#include "wwcommon/ISobEvent.h"
#include "wwcommon/CSobUnspawnEvent.h"
#include "wwcommon/CSobStateRequestEvent.h"
#include "wwcommon/CSobSpawnEvent.h"
#include "wwcommon/CSobRemoveEvent.h"
#include "wwcommon/CSobAddEvent.h"
#include "wwcommon/IGameEvent.h"
#include "wwcommon/CGameUnspawnRequestEvent.h"
#include "wwcommon/CGameSpawnRequestEvent.h"
#include "wwcommon/CSobStrafeEvent.h"
#include "wwcommon/CSobOrientEvent.h"
#include "wwcommon/CSobMoveEvent.h"
#include "wwcommon/CSobDREvent.h"

// Objects we're registering in here (temporary.)
#include "wwcommon/CStateManager.h"
#include "wwcommon/CSobFactory.h"
#include "wwcommon/CSobHandlerFactory.h"
#include "wwcommon/CMoveBackState.h"
#include "wwcommon/CMoveFwdState.h"
#include "wwcommon/CMoveIdleState.h"
#include "wwcommon/COrientIdleState.h"
#include "wwcommon/COrientTurnLeftState.h"
#include "wwcommon/COrientTurnRightState.h"
#include "wwcommon/CStrafeIdleState.h"
#include "wwcommon/CStrafeLeftState.h"
#include "wwcommon/CStrafeRightState.h"
#include "wwcommon/CPerformer.h"
#include "wwcommon/CRootSob.h"
#include "wwcommon/CClientMotionHandler.h"
#include "wwcommon/CMotionHandler.h"
#include "wwcommon/CRequestHandler.h"


WWCOMMON::IBaseSimulation *Simulation;
WWCOMMON::IBaseSimulation *getSimulation() {
        if(Simulation==NULL) {
		nlinfo("Attempting to create a simulation instance.");
		try
		{
			NLMISC::IClassable *sim = NLMISC::CClassRegistry::create("CSimulationImpl");
			if(sim==NULL)
				nlwarning("Registery creation attempt failed.");
			Simulation = static_cast<WWCOMMON::IBaseSimulation *>(sim);
		}
		catch(NLMISC::EUnregisteredClass &e)
		{
			nlwarning("CSimulationImpl was not registered!!");
		}
        }
	if(Simulation == NULL)
		nlwarning("failed to creation a CSimulationImpl");
        return Simulation;
}

void WWCOMMON::registerEvents() {

	/// Register the event with the class registry.
	
	WWCOMMON_REGISTER_EVENT(ISobStateChangeEvent);	
	WWCOMMON_REGISTER_EVENT(ISobEvent);	
	WWCOMMON_REGISTER_EVENT(CSobUnspawnEvent);
	WWCOMMON_REGISTER_EVENT(CSobStateRequestEvent);
	WWCOMMON_REGISTER_EVENT(CSobSpawnEvent);
	WWCOMMON_REGISTER_EVENT(CSobRemoveEvent);
	WWCOMMON_REGISTER_EVENT(CSobAddEvent);

	WWCOMMON_REGISTER_EVENT(IGameEvent);
	WWCOMMON_REGISTER_EVENT(CGameUnspawnRequestEvent);
	WWCOMMON_REGISTER_EVENT(CGameSpawnRequestEvent);
	WWCOMMON_REGISTER_EVENT(CSobStrafeEvent);
	WWCOMMON_REGISTER_EVENT(CSobOrientEvent);
	WWCOMMON_REGISTER_EVENT(CSobMoveEvent);
	WWCOMMON_REGISTER_EVENT(CSobDREvent);
}

void WWCOMMON::registerCommonFactoryObjects() {
	// Register States
	OF_REGISTER(CStateFactory, CMoveBackState, CStateFactory::ST_MV_BACKWARD);
	OF_REGISTER(CStateFactory, CMoveFwdState, CStateFactory::ST_MV_FORWARD);
	OF_REGISTER(CStateFactory, CMoveIdleState, CStateFactory::ST_MV_IDLE);
	OF_REGISTER(CStateFactory, COrientIdleState, CStateFactory::ST_OR_IDLE);
	OF_REGISTER(CStateFactory, COrientTurnLeftState, CStateFactory::ST_OR_TURNLEFT);
	OF_REGISTER(CStateFactory, COrientTurnRightState, CStateFactory::ST_OR_TURNRIGHT);
	OF_REGISTER(CStateFactory, CStrafeIdleState, CStateFactory::ST_ST_IDLE);
	OF_REGISTER(CStateFactory, CStrafeLeftState, CStateFactory::ST_ST_LEFT);
	OF_REGISTER(CStateFactory, CStrafeRightState, CStateFactory::ST_ST_RIGHT);

	// Register Simulation Objects
	OF_REGISTER(CSobFactory, CPerformer, "sobPerformer");
	OF_REGISTER(CSobFactory, CRootSob, "sobRoot");

	// Register Simulation Object Handlers
	OF_REGISTER(CSobHandlerFactory, CClientMotionHandler, CSobHandlerFactory::H_CLIENT_MOTION);
	OF_REGISTER(CSobHandlerFactory, CMotionHandler, CSobHandlerFactory::H_MOTION);
	OF_REGISTER(CSobHandlerFactory, CRequestHandler, CSobHandlerFactory::H_MOTION_REQUEST);
}
