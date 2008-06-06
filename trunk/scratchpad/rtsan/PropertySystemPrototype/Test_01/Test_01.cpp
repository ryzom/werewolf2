// Test_01.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <wwproperty/CBaseProperty.h>
#include <hash_map>
#include <string>
#include <cstdio>
#include <conio.h>

class PropertyProviderNode {
public:
	WWPROPERTY::IProperty *getProperty(std::string name);
	void setProperty(std::string name, WWPROPERTY::IProperty* prop);

	typedef std::hash_map<std::string, WWPROPERTY::IProperty*> PropertyMap;
private:
	PropertyMap m_PropertyMap;
};

WWPROPERTY::IProperty *PropertyProviderNode::getProperty(std::string name) {
	PropertyProviderNode::PropertyMap::iterator iter = m_PropertyMap.find(name);
	if(iter == m_PropertyMap.end())
		return NULL;
	return iter->second;
}

void PropertyProviderNode::setProperty(std::string name, WWPROPERTY::IProperty* prop) {
	m_PropertyMap[name] = prop;
}

class TestNode : public WWPROPERTY::IDirtyPropagator {
public:
	void setProvider(PropertyProviderNode *provider) { m_Provider = provider; }
	void update();

private:
	void calculate();
	PropertyProviderNode *m_Provider;
};

void TestNode::update() {
	if(isDirty()) {
		calculate();
	}
}

void TestNode::calculate() {
	printf("Calculating test node...\n");
	setClean();
	printf("Done calculating.\n");
}

int _tmain(int argc, _TCHAR* argv[])
{
	PropertyProviderNode provider;

	WWPROPERTY::CBaseProperty<int> prop1("prop1");
	WWPROPERTY::CBaseProperty<float> prop2("prop2");
	WWPROPERTY::CBaseProperty<bool> prop3("prop3");
	provider.setProperty("int", &prop1);
	provider.setProperty("float", &prop2);
	provider.setProperty("bool", &prop3);

	prop1.setValue(0);
	prop2.setValue(0);
	prop3.setValue(false);

	TestNode test;
	test.setProvider(&provider);
	test.makeInputConnection(&prop1);
	test.makeInputConnection(&prop2);
	test.makeInputConnection(&prop3);

	bool run = true;
	while(run) {
		test.update();
		char c = getch();
		switch(c) {
		  case 'f':
			  prop2.setValue(prop2.getValue()+0.1f);
			  break;
		  case 'i':
			  prop1.setValue(prop1.getValue()+1);
			  break;
		  case 'b':
			  prop3.setValue(!prop3.getValue());
			  break;
		  case 'q':
			  run = false;
			  break;
		  default:
			  break;
		}
	}
	return 0;
}

