#ifndef WW_ENTITY_DEF_ENTRY_H
#define WW_ENTITY_DEF_ENTRY_H

#include <string>
#include <vector>

#include <nel/misc/types_nl.h>
#include <nel/misc/app_context.h>
#include <nel/misc/stream.h>

#include <nel/georges/u_form.h>
#include <nel/georges/u_form_elm.h>

class Entity;

class TEntityProperty {
public:

	virtual void serial(NLMISC::IStream &f) { }

	virtual void build(Entity *) { };
	std::string Type;
};

class TEntityBasicStringProperty : public TEntityProperty {
public:

	TEntityBasicStringProperty() {};
	TEntityBasicStringProperty(std::string stringName, std::string stringValue) : StringName(stringName), StringValue(stringValue) { };

	void serial(NLMISC::IStream &f) {
		if(f.isReading()) {
			f.serial(StringName);
			f.serial(StringValue);
		} else {
			f.serial(StringName);
			f.serial(StringValue);
		}
	}

	std::string StringName;
	std::string StringValue;
};

class TEntityBasicIntegerProperty : public TEntityProperty {
public:

	TEntityBasicIntegerProperty() {};
	TEntityBasicIntegerProperty(std::string integerName, int integerValue) : IntegerName(integerName), IntegerValue(integerValue) { };

	void serial(NLMISC::IStream &f) {
		if(f.isReading()) {
			f.serial(IntegerName);
			f.serial(IntegerValue);
		} else {
			f.serial(IntegerName);
			f.serial(IntegerValue);
		}
	}

	void build(Entity *entity);

	std::string IntegerName;
	int IntegerValue;
};

class TEntityAreaTriggerProperty : public TEntityProperty {
	void serial(NLMISC::IStream &f) {
		if(f.isReading()) {
			f.serial(AreaX);
			f.serial(AreaY);
		} else {
			f.serial(AreaX);
			f.serial(AreaY);
		}
	}

	uint AreaX;
	uint AreaY;
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

		NLGEORGES::UFormElm *properties;
		res=root.getNodeByName(&properties, ".Properties");
		if(res==false || properties==NULL)
			nlinfo("No properties loaded for entity definition type: '%s'", Type.c_str());
		else
			processProperties(properties);

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

	void processProperties(NLGEORGES::UFormElm *properties) {
		uint numProperties;
		properties->getArraySize(numProperties);

		nlinfo("processing %d properties", numProperties);

		for(uint idx=0 ; idx<numProperties ; idx++) {
			TEntityProperty *aProperty = NULL;
			std::string propertyName;

			// Get the current property element.			
			NLGEORGES::UFormElm *property;
			NLGEORGES::UFormElm *propertyHolder;
			NLGEORGES::UFormElm *propertyValue;
			properties->getArrayNode(&property, idx);
			property->getNodeByName(&propertyHolder, ".PropertyValues");
			propertyHolder->getValueByName(propertyName, ".PropertyName");
			propertyHolder->getNodeByName(&propertyValue, ".PropertyValue");

			// Now get the name of the definition that defines this property.
			std::string dfnName;
			propertyValue->getDfnName(dfnName);
			nlinfo("dfn name is %s", dfnName.c_str());
			// Now that we know which definition formats this property
			// we know how to create it and process it.
			if(dfnName == "basic_string.dfn") {
				std::string stringValue;

				propertyValue->getValueByName(stringValue, ".StringValue");
				aProperty = new TEntityBasicStringProperty(propertyName, stringValue);
				aProperty->Type = dfnName;
			} else if(dfnName == "basic_integer.dfn") {
				int integerValue;

				propertyValue->getValueByName(integerValue, ".IntValue");
				aProperty = new TEntityBasicIntegerProperty(propertyName, integerValue);
				aProperty->Type = dfnName;
			} else if(dfnName == "area_trigger_property.dfn") {
				nlinfo("need to implement the area trigger prperty stuff.");
			
			} else {
				/* more to come later */
				nlwarning("Unknown property definition type specified in entity definition '%s'", Type.c_str());
			}

			if(aProperty != NULL) {
				// Add it to the list of properties for this entity definition.
				EntityProperties.push_back(aProperty);
				nlinfo("pushed property back.");
			}
		}
	}

	void serial(NLMISC::IStream &f) {
		if(f.isReading()) {
			f.serial(Type);
			f.serialCont(EntityComponents);
			
		} else {
			f.serial(Type);
			f.serialCont(EntityComponents);
			//f.serialCont(EntityProperties);
		}
	}

	static uint getVersion() { return 1; }
	void removed() { }

	std::string Type;

	typedef std::vector<TEntityProperty *> PropertiesVector;
	PropertiesVector EntityProperties;

	typedef std::vector<TEntityComponent> ComponentsVector;
	ComponentsVector EntityComponents;
};

#endif // WW_ENTITY_DEF_ENTRY_H