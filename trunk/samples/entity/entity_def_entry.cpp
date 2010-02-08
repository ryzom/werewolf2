#include "entity_def_entry.h"

#include "entity.h"

void TEntityBasicIntegerProperty::build(Entity *entity) {
	if(entity->HasProperty(IntegerName))
		entity->GetProperty<int>(IntegerName) = IntegerValue;
	else
		entity->AddProperty<int>(IntegerName, IntegerValue);
}