//
// Standard Includes
//
#include "stdpch.h"

#include <nel/misc/class_registry.h>

#include "wwcommon/IBaseSimulation.h"

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

