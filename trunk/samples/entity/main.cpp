#include <iostream>

#include <nel/misc/app_context.h>
#include <nel/misc/path.h>
#include <nel/misc/sheet_id.h>

#include <nel/georges/u_form.h>
#include <nel/georges/u_form_elm.h>
#include <nel/georges/u_form_loader.h>
#include <nel/georges/load_form.h>

class TEntityProperty {
public:
	void serial(NLMISC::IStream &f) {
		// Nothing here yet.
	}
};

class TEntityComponent {
public:
	std::string ComponentName;
	std::string ComponentScript;

	void serial(NLMISC::IStream &f) {
		f.serial(ComponentName);
		f.serial(ComponentScript);
	}
};

class TEntityDefEntry {
public:
	void readGeorges(const NLMISC::CSmartPtr<NLGEORGES::UForm> &form, const std::string &sheetId) {
		NLGEORGES::UFormElm &root = form->getRootNode();
		root.getValueByName(Type,".Type");

		// Process the Components.
		NLGEORGES::UFormElm *components;
		bool res=root.getNodeByName(&components, ".Components");
		if(res==false || components==NULL)
			nlinfo("No components loaded for entity definition type: '%s'", Type.c_str());
		else
			processComponents(components);


	}

	void processComponents(NLGEORGES::UFormElm *components) {
		uint numComponents;
		components->getArraySize(numComponents);

		for(uint idx=0 ; idx<numComponents ; idx++) {
			TEntityComponent aComponent;
			std::string componentName;
			std::string componentScript;
			bool result;

			NLGEORGES::UFormElm *component;
			components->getArrayNode(&component, idx);

			// Retrieve the component name
			component->getValueByName(componentName, ".ComponentName");
			aComponent.ComponentName=componentName;
			// Retrieve the Script Name, if applicable.
			result=component->getValueByName(componentScript, ".ComponentScript");
			if(result)
				aComponent.ComponentScript=componentScript;

			EntityComponents.push_back(aComponent);
		}
	}

	void serial(NLMISC::IStream &f) {
		if(f.isReading()) {
			f.serial(Type);
			f.serialCont(EntityComponents);
			f.serialCont(EntityProperties);
		} else {
			f.serial(Type);
			f.serialCont(EntityComponents);
			f.serialCont(EntityProperties);
		}
	}

	static uint getVersion() { return 1; }
	void removed() { }

	std::string Type;

	typedef std::vector<TEntityProperty> PropertiesVector;
	PropertiesVector EntityProperties;

	typedef std::vector<TEntityComponent> ComponentsVector;
	ComponentsVector EntityComponents;
};


void main() {
	std::cout << "Test!" << std::endl;

	NLMISC::CApplicationContext context;

	NLMISC::CPath::addSearchPath("data", true, false);

	// Contains all of the sheets loaded.
	typedef std::map<std::string, TEntityDefEntry> TEntityDefinitions;
	TEntityDefinitions EntityDefinitions;

	// Whether or not to scan the files and update the packed sheets. We'll default to true.
	bool updatePackedSheet = true;

	std::string path = NLMISC::CPath::lookup("entity_def.packed_sheets", false);
	if(path.empty())
		path = "data/entity_def.packed_sheets";

	::loadForm("entity_def", path, EntityDefinitions, true, false);

	// Now go throught the list of scripts and create them.
	TEntityDefinitions::iterator first(EntityDefinitions.begin()), last(EntityDefinitions.end());
	for(; first != last; ++first) {
		TEntityDefEntry entry = first->second;
		nlinfo("handling entity definition type: '%s' with %d components and %d properties", 
			entry.Type.c_str(), entry.EntityComponents.size(), entry.EntityProperties.size());
		
		//Script *script=new Script(first->second);
		//m_scripts[script->getName().c_str()] = script;
	}
}