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

/*
 * TODO
 *
 * * Entity Aliases
 * * Component-to-Entity-Links (e.g. use alias so that the area trigger component can link to the gate entity.)
 * * Components as sob event handlers (receive events for their sobs.)
 * * "Generate" a world from a list of entities and property values
 * ** Expand this to pull entites and properties from a DB using Soci.
 *
 */
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

	EntityEvent event;
	Component *component = ent->GetComponent("Health");
	component->handleEvent(event, ent);
}