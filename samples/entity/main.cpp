#include <iostream>

#include <nel/misc/app_context.h>
#include <nel/misc/path.h>
#include <nel/misc/sheet_id.h>

#include <nel/georges/u_form.h>
#include <nel/georges/u_form_elm.h>
#include <nel/georges/u_form_loader.h>
#include <nel/georges/load_form.h>

// Sheet/Form Loader
#include "entity_def_entry.h"

// Actual system.
#include "entity.h"
#include "component.h"
#include "component_container.h"
#include "component_factory.h"


void main() {
	std::cout << "Test!" << std::endl;

	NLMISC::CApplicationContext context;

	NLMISC::CPath::addSearchPath("data", true, false);

	// Register property classes.
	NLMISC_REGISTER_CLASS(TEntityProperty);
	NLMISC_REGISTER_CLASS(TEntityBasicStringProperty);

	// Contains all of the sheets loaded.
	typedef std::map<std::string, TEntityDefEntry> TEntityDefinitions;
	TEntityDefinitions EntityDefinitions;

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
		
		//Script *script=new Script(first->second);
		//m_scripts[script->getName().c_str()] = script;
	}
}