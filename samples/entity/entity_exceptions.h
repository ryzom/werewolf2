#ifndef WW_ENTITY_EXCEPTIONS_H
#define WW_ENTITY_EXCEPTIONS_H

#include <nel/misc/types_nl.h>

class EEntityNotFound {
};

class EComponentNotFound : public NLMISC::Exception {
public:
	EComponentNotFound() : NLMISC::Exception("Component not found") { }
	EComponentNotFound(const std::string &str) : NLMISC::Exception( str ) {}
};

#endif // WW_ENTITY_EXCEPTIONS_H