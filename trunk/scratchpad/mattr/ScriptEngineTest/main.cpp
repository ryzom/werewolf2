#include "nel/misc/types_nl.h"
#include "nel/misc/path.h"

#include "ScriptEngine/ScriptManager.h"

int main(int argc, char **argv) {
	NLMISC::CApplicationContext myApplicationContext;

	nlinfo("Starting Script Engine Test.");

	NLMISC::CPath::addSearchPath("data/");

	WWSCRIPT::ScriptManager::instance().initializeScripts();

	WWSCRIPT::ScriptManager::instance().getEngine()->ExecuteString("ExampleScript", "getFoobar()");
}
