#include "component_health.h"

bool ComponentHealth::handleEvent(EntityEvent event, Entity *subject) {
	nlinfo("Retrieving health: %d", subject->GetProperty<int>("MaxHealth").Get());
	return true;
}