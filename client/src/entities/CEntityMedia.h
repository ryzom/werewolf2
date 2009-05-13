/**
* \file CEntityMedia.h
* \date May 2004
* \author Matt Raykowski
* \author Some code adapted from MTP-Target.
*/

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

#ifndef __CENTITYMEDIA_H__
#define __CENTITYMEDIA_H__

//
// Standard Includes
//
#include <vector>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/georges/u_form_loader.h>
#include <nel/georges/u_form.h>
#include <nel/georges/u_form_elm.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_play_list.h>

//
// Werewolf Includes
//
#include "entities/CAnimLayer.h"
#include "entities/CMeshGroup.h"
#include "entities/CSkeleton.h"
#include <wwcommon/general.h>
#include "CSequenceManager.h"
#include "entities/IControllableMedia.h"
#include <wwcommon/ISimulationObj.h>

//
// Namespaces
//

namespace WWCLIENT {

//
// Class References
//

//
// Class
//

/**
* \class CEntityMedia CEntityMedia.h "entities/CEntityMedia.h"
* \brief This represents the media data for a specific entity description
*/
class CEntityMedia : public IControllableMedia {
public:
	CEntityMedia();
	~CEntityMedia();

	typedef CHashMap<std::string, CSkeleton > skeletonMap;
	typedef CHashMap<std::string, CAnimLayer > animLayerMap;
	typedef CHashMap<std::string, CMeshGroup > meshGroupMap;

	void readGeorges(const NLMISC::CSmartPtr<NLGEORGES::UForm> &form, const std::string &sheetId);
	void initInstance(WWCOMMON::ISimulationObj* sob);

	/**
	* \brief Here you write a standard NeL serial for all the member of TMyLoader that need to be packed.
	*/
	void serial(NLMISC::IStream &f);

	/**
	* \brief This methed returns the version of the packed sheet.
	* Version 0 - non-existant.
	* Version 1 - Basic datas.
	*
	* \return uint This must always return an integer. Unsigned is probably best.
	*/
	static uint getVersion() {
		return 1;
	}

	/**
	* \brief Here you can write custom code for when the packed sheet loader needs to remove an old sheet.
	* \note This is rarely used.
	*/
	void removed() {
		;
	}

	void update();
	void updateAnimations();
	void updateMeshes();
	void updateSkeletons();

	void showMesh(std::string group, std::string mesh);
	void hideMesh(std::string group);
	void transform(NLMISC::CMatrix& matrix);
	NL3D::UTransform* getTransform();

	void playAnimation(std::string layer, std::string group);
	CSequenceManager<uint8>* getSlotManager();;

	WWCOMMON::ISimulationObj* getOwner() {
		return m_Owner;
	}

	CAnimLayer* getAnimLayer(std::string& name);
	CMeshGroup* getMeshGroup(std::string& name);
	CSkeleton* getSkeleton(std::string& name);

	std::string Name;
	std::string Description;
	std::string EMT;
	NL3D::UPlayList* PlayList;
protected:
	skeletonMap m_Skeletons;
	meshGroupMap m_MeshGroups;
	animLayerMap m_AnimationLayers;
	CSequenceManager<uint8>* m_SlotManager;
	NL3D::UTransform m_Transform;
	WWCOMMON::ISimulationObj* m_Owner;
};

}; // END NAMESPACE WWCLIENT

#endif // __CENTITYMEDIA_H__
