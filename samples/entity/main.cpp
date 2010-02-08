#include <iostream>

#include <nel/misc/app_context.h>
#include <nel/misc/path.h>


// Sheet/Form Loader
#include "entity_def_manager.h"

// Actual system.
#include "entity.h"
#include "component.h"
#include "component_container.h"
#include "component_factory.h"

#include "component_health.h"

void main() {
	NLMISC::CApplicationContext context;

	NLMISC::CPath::addSearchPath("data", true, false);

	// Register all of the component classes.
	ComponentFactory::getInstance().RegisterComponent("Health", ComponentHealth::Create);

	// Load the forms/sheets for the entity definitions.
	EntityDefManager::getInstance().initializeDefinitions();
	
	Entity *ent = new Entity();
	EntityDefManager::getInstance().build(ent, "laurelin_shopkeeper");
	
	nlinfo("Current Max Health is: %d", ent->GetProperty<int>("MaxHealth").Get());

	ent->GetProperty<int>("MaxHealth") = 100;

	nlinfo("Current Max Health is: %d", ent->GetProperty<int>("MaxHealth").Get());
}