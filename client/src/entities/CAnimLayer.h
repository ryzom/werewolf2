/**
 * \file CAnimLayer.h
 * \date February 2006
 * \author Henri Kuuste
 */

/* Copyright, 2006 Werewolf
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

#ifndef __CANIMLAYER_H__
#define __CANIMLAYER_H__

//
// Standard Includes
//
#include <hash_map>
#include <string>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/georges/u_form_loader.h>
#include <nel/georges/u_form.h>
#include <nel/georges/u_form_elm.h>

//
// Werewolf Includes
//	
#include "entities/CAnimGroup.h"
#include <wwcommon/general.h>
#include "entities/IControllableMedia.h"

//
// Namespaces
//

namespace WWCLIENT {

class CEntityMedia;

class CAnimLayer : public IControllableMedia {
public:
	typedef std::hash_map<std::string, CAnimGroup, std::hash<std::string> > animGroupMap;

	CAnimLayer();
	/**
	* \brief Here you read in the form if necessary, storing it in memebers of the TMyLoader class
	*/
	CAnimLayer(NLGEORGES::UFormElm *root);

	/**
	* \brief Here you write a standard NeL serial for all the member of TMyLoader that need to be packed.
	*/
	void serial(NLMISC::IStream &f);

	CEntityMedia* getEMD();
	void setEMD(CEntityMedia* emd);

	void play(std::string group);

	void update();
	void updateAnimations();

	CAnimGroup* getAnimGroup(std::string& name);

	std::string AnimLayerName;

private:
	animGroupMap m_AnimationGroups;
	CAnimGroup* m_Playing;
	CEntityMedia* m_EMD;
	uint8 m_Slots[2];
	uint m_CurrentSlotIndex;
	bool m_HasSlots;
};

}; // END NAMESPACE WWCLIENT

#endif // __CANIMLAYER_H__
