#include <nel/misc/class_registry.h>

#include "wwcommon/IBaseSimulation.h"

WWCOMMON::IBaseSimulation *Simulation;
WWCOMMON::IBaseSimulation *getSimulation() {
        if(Simulation==NULL) {
		try
		{
			Simulation = dynamic_cast<WWCOMMON::IBaseSimulation *>(NLMISC::CClassRegistry::create("CSimulationImpl"));
		}
		catch(NLMISC::EUnregisteredClass &e)
		{
			nlwarning("CSimulationImpl was not registered!!");
		}
        }
        return Simulation;
}

