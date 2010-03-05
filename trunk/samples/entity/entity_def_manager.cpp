#include "entity_def_manager.h"

#include "entity.h"

NLMISC_SAFE_SINGLETON_IMPL(EntityDefManager)

void EntityDefManager::initializeDefinitions() {
	// Whether or not to scan the files and update the packed sheets. We'll default to true.
	bool updatePackedSheet = true;

	std::string path = NLMISC::CPath::lookup("entity_def.packed_sheets", false);
	if(path.empty())
		path = "data/entity_def.packed_sheets";

	::loadForm("entity_def", path, EntityDefinitions, true, false);

	// This is just to demonstrate that we did infact load these.
	TEntityDefinitions::iterator first(EntityDefinitions.begin()), last(EntityDefinitions.end());
	for(; first != last; ++first) {
		TEntityDefEntry entry = first->second;
		nlinfo("handling entity definition type: '%s' with %d components and %d properties", 
			entry.Type.c_str(), entry.EntityComponents.size(), entry.EntityProperties.size());
	}
}

void EntityDefManager::build(Entity *entity, std::string entity_def) {
	TEntityDefEntry definition;
	bool foundDefinition=false;
	// Look up the definition that will be the basis for this entity.
	TEntityDefinitions::iterator first(EntityDefinitions.begin()), last(EntityDefinitions.end());
	for(; first != last; ++first) {
		definition = first->second;
		if(definition.Type == entity_def) {
			foundDefinition=true;
			break;
		}
	}

	// Make sure we found a definition.
	if(!foundDefinition)
		throw NLMISC::Exception("Unable to find entity definition '"+entity_def+"'");
	
	// Next cycle through the components and add them.
	TEntityDefEntry::ComponentsVector::iterator compItr = definition.EntityComponents.begin();
	while(compItr != definition.EntityComponents.end()) {
		TEntityComponent component = (*compItr);
		
		entity->AddComponent(component.ComponentName);
		compItr++;
	}

	// Next we'll begin updating components.
	TEntityDefEntry::PropertiesVector::iterator propItr = definition.EntityProperties.begin();
	while(propItr != definition.EntityProperties.end()) {
		TEntityProperty *prop = (*propItr);

		prop->build(entity);
		propItr++;
	}
}
