#ifndef WW_ENTITY_DEF_MANAGER_H
#define WW_ENTITY_DEF_MANAGER_H

#include <nel/misc/types_nl.h>
#include <nel/misc/app_context.h>
#include <nel/misc/debug.h>
#include <nel/misc/path.h>
#include <nel/misc/sheet_id.h>

#include <nel/georges/u_form.h>
#include <nel/georges/u_form_elm.h>
#include <nel/georges/u_form_loader.h>
#include <nel/georges/load_form.h>

#include "entity_def_entry.h"

class Entity;

class EntityDefManager {
	EntityDefManager() { };
	virtual ~EntityDefManager() { };

	NLMISC_SAFE_SINGLETON_DECL(EntityDefManager);
public:
	void initializeDefinitions();

	void build(Entity *entity, std::string entity_def);

protected:
	// Contains all of the sheets loaded.
	typedef std::map<std::string, TEntityDefEntry> TEntityDefinitions;
	TEntityDefinitions EntityDefinitions;
};

#endif // WW_ENTITY_H