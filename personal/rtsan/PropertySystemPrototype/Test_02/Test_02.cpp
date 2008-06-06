// Test_02.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <cstdio>
#include <conio.h>
#include <stdexcept>

#include "SOB.h"
#include "Sob3DView.h"
#include "CharacterData.h"
#include <boost/python.hpp>
#include "CScriptEngine.h"
#include "CScriptConsole.h"
#include "MediaScriptController.h"
#include "hash_map_item.h"
#include <wwproperty/IProperty.h>

BOOST_PYTHON_MODULE(ww) {
	WWPROPERTY::IDirtyPropagatorWrap::init_script_class();
	WWPROPERTY::IProperty::init_script_class();
	IViewWrap::init_script_class();
	IMediaControllerWrap::init_script_class();
	IDataContainerWrap::init_script_class();
	SOB::init_script_class();
	EMD::init_script_class();
	MeshGroup::init_script_class();
	Mesh::init_script_class();
	Sob3DView::init_script_class();
	CharacterData::init_script_class();
	MediaScriptController<Mesh>::init_script_class("MeshScriptController");
	MediaScriptController<MeshGroup>::init_script_class("MeshGroupScriptController");
	MediaScriptController<EMD>::init_script_class("EMDScriptController");

	hash_map_item<const char*, WWPROPERTY::IProperty*, std::hash<const char*>, streqpred>::init_script_class("PropertyMap");
}

int _tmain(int argc, _TCHAR* argv[])
{
	CScriptEngine::instance().init();
	if (PyImport_AppendInittab("ww", initww) == -1)
		throw std::runtime_error("Failed to add ww to the interpreter's builtin modules");
	CScriptEngine::instance().execCommand("import ww");

	SOB sob;

	sob.addDataContainer(new CharacterData(&sob));
	
	EMD* emd = new EMD(&sob);
	MeshGroup* body = new MeshGroup("body", emd);
	Mesh* normalBody = new Mesh("normal", emd);
	normalBody->addController(new MediaScriptController<Mesh>("fatMorpher", normalBody,"fat_morph_init.py","fat_morph_update.py"));

	body->addMesh(normalBody);
	emd->addMeshGroup(body);

	sob.addView(new Sob3DView(&sob, emd));

	CharacterData* charData = dynamic_cast<CharacterData*>(sob.getDataContainer("Character"));

	CScriptEngine::instance().getMainNamespace()["sob"] = boost::python::ptr(&sob);

	while(CScriptConsole::instance().update()) {
		sob.update();
	}
	return 0;
}

