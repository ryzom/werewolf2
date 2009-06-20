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
