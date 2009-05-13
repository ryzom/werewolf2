
/* Copyright, 2004 Werewolf
 *
 * This file is part of Werewolf.
 * Werewolf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * Werewolf is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Werewolf; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

//
// System Includes
//
#include "stdpch.h"
#include "CEGUI.h"

//
// NeL Includes
//
#include <nel/3d/u_light.h>
#include <nel/3d/u_point_light.h>
#include <nel/3d/instance_group_user.h>
#include <nel/3d/scene_group.h>
#include <nel/3d/scene_user.h>
#include <nel/3d/point_light_model.h>

//
// Werewolf Includes
//
#include "tasks/CLandscapeTask.h"
#include "tasks/C3DTask.h"
#include "tasks/CConfigTask.h"
//#include "tasks/CEntityTask.h"
#include "tasks/CIntroTask.h"
#include "tasks/CGuiTask.h"
//#include "interfaces/IEntity.h"

#include "CClientSimulation.h"


//
// Namespaces
//

namespace WWCLIENT {

void CLandscapeTask::updateProgressBar(float prog1, float prog2, std::string message) {
	C3DTask::instance().clear();
	CEGUI::DefaultWindow *wnd=static_cast<CEGUI::DefaultWindow*>(CEGUI::WindowManager::getSingleton().getWindow("LandscapeProgress/Message"));
	wnd->setText(message);
	CEGUI::ProgressBar *pbar=(CEGUI::ProgressBar *)CEGUI::WindowManager::getSingleton().getWindow("LandscacpeProgress/ProgBar");
	pbar->setProgress(prog1);
	CGuiTask::instance().render();
	C3DTask::instance().driver().swapBuffers();
}

void CLandscapeTask::init() {
	CLandCallback progresscb;		// used for showing the progress of loading zones.
	NLMISC::CConfigFile::CVar cv;	// temporary holder for clean input from the log files.

	// show the loading progress window.
	CEGUI::FrameWindow *wnd=(CEGUI::FrameWindow *)CEGUI::WindowManager::getSingleton().getWindow("LandscapeProgress");
	wnd->show();

	// get the zones loading list for the debug window.
	//m_DebugZonesList=(CEGUI::MultiLineEditbox *)CEGUI::WindowManager::getSingleton().getWindow("GameTask/DebugZones");

	// and set a default message.
	updateProgressBar(0.0f, 0.0f, "Beginning to load the landscape...");

	// nab a copy of the config file just to make things quicker.
	CConfigFile ConfigFile=CConfigTask::instance().configFile();
	
	/**
	 * Gather config information.
	 */

	// color of the landscape shadow
	cv=ConfigFile.getVar("LandscapeDiffuseColor");
	NLMISC::CRGBA landscapeDiffuse(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	cv=ConfigFile.getVar("LandscapeAmbientColor");
	NLMISC::CRGBA landscapeAmbient(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	float landscapeMultiplyFactor=ConfigFile.getVar("LandscapeMultiplyFactor").asFloat();

	// set up the fog.
	cv=ConfigFile.getVar("FogColor");
	NLMISC::CRGBA fogColor(cv.asInt(0), cv.asInt(1), cv.asInt(2) );
	float fogStart=ConfigFile.getVar("FogStart").asFloat();
	float fogEnd=ConfigFile.getVar("FogEnd").asFloat();
	bool fogEnable=(ConfigFile.getVar("FogEnd").asInt() == 1);

	// landscape tiles and thresholds
	float landscapeTileNear=ConfigFile.getVar("LandscapeTileNear").asFloat();
	float landscapeThreshold=ConfigFile.getVar("LandscapeThreshold").asFloat();

	// tile bank information.
	std::string bankName=ConfigFile.getVar("BankName").asString();
	std::string farBankName=ConfigFile.getVar("FarBankName").asString();

	// get the loading point.
	//cv=ConfigFile.getVar("StartPoint");
	nlinfo("Loading landscape at: %f / %f / %f", getClientSimulation()->getSelfSob()->getInterpolator()->getLastKey().getPosition().x,
		getClientSimulation()->getSelfSob()->getInterpolator()->getLastKey().getPosition().y, 
		getClientSimulation()->getSelfSob()->getInterpolator()->getLastKey().getPosition().z);
	NLMISC::CVector landscapeLoadPoint=getClientSimulation()->getSelfSob()->getInterpolator()->getLastKey().getPosition();
	m_ZoneRadius=ConfigFile.getVar("LandscapeLoadRadius").asFloat();

	// sun / lighting information.
	cv=ConfigFile.getVar("SunAmbientColor");
	NLMISC::CRGBA landscapeSunAmbient(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	cv=ConfigFile.getVar("SunDiffuseColor");
	NLMISC::CRGBA landscapeSunDiffuse(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	cv=ConfigFile.getVar("SunSpecularColor");
	NLMISC::CRGBA landscapeSunSpecular(cv.asInt(0), cv.asInt(1), cv.asInt(2));
	cv=ConfigFile.getVar("SunDirection");
	m_SunDirection.set(cv.asFloat(0), cv.asFloat(1), cv.asFloat(2));


	/**
	 * Start setting up the landscape.
	 */
	updateProgressBar(0.1f, 0.1f, "Creating the landscape...");
	m_Landscape=C3DTask::instance().scene().createLandscape();
	m_Landscape->enableAdditive(true);
	m_Landscape->setRefineCenterAuto(false);
	nlinfo("Setting landscape tile near: %.1f", landscapeTileNear);
	m_Landscape->setTileNear(landscapeTileNear);
	m_Landscape->setThreshold(landscapeThreshold);
	
	/*
	 * Load the banks.
	 */
	// progress bar alter here.
	updateProgressBar(0.2f, 0.2f, "Loading the tile banks...");
	m_Landscape->loadBankFiles(CPath::lookup(bankName), CPath::lookup(farBankName));

	/*
	 * Load the zones.
	 */
	// progress bar alter here.
	updateProgressBar( 0.4f, 0.4f, "Loading the landscape...");
	progresscb.setProgressBar((CEGUI::ProgressBar *)CEGUI::WindowManager::getSingleton().getWindow("LandscacpeProgress/ProgBar"));
	m_Landscape->setZonePath (ConfigFile.getVar("DataPath").asString() + "zones/");
	m_Landscape->refreshAllZonesAround(landscapeLoadPoint, m_ZoneRadius, m_ZonesAdded, m_ZonesRemoved, progresscb);

	/*
	 * Load the instance groups.
	 */
	updateProgressBar( 0.6f, 0.6f, "Loading instance groups...");
	CConfigFile::CVar igv = ConfigFile.getVar("InstanceGroups");
	for (sint32 i = 0; i < igv.size (); i++) {
		UInstanceGroup *inst = UInstanceGroup::createInstanceGroup (igv.asString (i));
		if (inst == NULL) {
			nlwarning ("Instance group '%s' not found", igv.asString (i).c_str ());
		} else {
			NL3D::CInstanceGroupUser *iu = NLMISC::safe_cast<NL3D::CInstanceGroupUser*>(inst);
			NL3D::CInstanceGroup ig = iu->getInternalIG();
			//int numLights = ig.getNumPointLights();
			//for(int i = 0; i < numLights; i++) {
			//	NL3D::UPointLight *up = &(C3DTask::instance().scene().createPointLight());
			//	up->getObjectPtr()->PointLight = NL3D::CPointLight(static_cast<NL3D::CPointLight>(ig.getPointLightNamed(i)));
			//	up->setPos( up->getObjectPtr()->PointLight.getPosition() );
			//}
			inst->addToScene (C3DTask::instance().scene());
			m_InstanceGroups.push_back (inst);
		}
	}

	// set up the sun.
	updateProgressBar(0.8f, 0.8f, "Setting up lighting...");
	m_Landscape->setupStaticLight(landscapeDiffuse, landscapeAmbient, landscapeMultiplyFactor);
	m_Landscape->enableReceiveShadowMap(true);

	C3DTask::instance().scene().setSunDirection(m_SunDirection);
	C3DTask::instance().scene().setSunAmbient(landscapeSunAmbient);
	C3DTask::instance().scene().setSunDiffuse(landscapeSunDiffuse);
	C3DTask::instance().scene().setSunSpecular(landscapeSunSpecular);

	// enable fog
	updateProgressBar(0.9f, 0.9f, "The world is foggier today...");
	C3DTask::instance().driver().setupFog(fogStart,fogEnd,fogColor);
	C3DTask::instance().driver().enableFog(fogEnable);

	wnd->hide();
}

void CLandscapeTask::update() {
	// get the entity and make sure everything is valid.
	//IEntity *ent=&CEntityTask::instance().getSelf();
	CActorProxy *actor=getClientSimulation()->getSelfSob();
	if(actor!=NULL) {
		// store the position
		std::string added, removed;
		NLMISC::CVector pos=actor->getPosition();
		if(pos==NLMISC::CVector::Null) {
			return;
		}

		// refresh the landscape.
		m_Landscape->refreshZonesAround(pos,m_ZoneRadius, added, removed);
		m_Landscape->setRefineCenterUser(pos);
	} else {
		nlinfo("Cannot refresh zone, no entity position.");
	}
}

void CLandscapeTask::render() {
	;
}

void CLandscapeTask::release() {
	;
}

void CLandscapeTask::reset() {
	;
}

void CLandscapeTask::error(std::string reason) {
	;
}
	
std::string CLandscapeTask::name() {
	return "CLandscapeTask";
}

ULandscape &CLandscapeTask::getLandscape() {
	return *m_Landscape;
}

InstanceGroupVector &CLandscapeTask::getInstanceGroups() {
	return m_InstanceGroups;
}

CVector CLandscapeTask::getSunDirection() {
	return m_SunDirection;
}

}; // END NAMESPACE WWCLIENT
