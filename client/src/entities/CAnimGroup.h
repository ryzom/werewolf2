/**
 * \file CAnimGroup.h
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

#ifndef __CANIMGROUP_H__
#define __CANIMGROUP_H__

//
// Standard Includes
//
#include <vector>
#include <map>

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
#include "CAnimationClip.h"
#include "selectors/ISelector.h"
#include "selectors/ISelectable.h"
#include "entities/IControllableMedia.h"
#include <wwcommon/general.h>

//
// Namespaces
//

namespace WWCLIENT {

class CEntityMedia;

/**
* \class CAnimGroup CEntityMeda.h "entities/CEntityMedia.h"
* \brief This loads animation groups from packed sheets.
*/
class CAnimGroup : public IControllableMedia {
public:

	CAnimGroup();
	/**
	* \brief Here you read in the form if necessary, storing it in memebers of the TMyLoader class
	*/
	CAnimGroup(NLGEORGES::UFormElm *root);

	virtual ~CAnimGroup();

	/**
	* \brief Here you write a standard NeL serial for all the member of TMyLoader that need to be packed.
	*/
	void serial(NLMISC::IStream &f);

	class CAnimClip : public ISelectable {
	public:
		CAnimClip();

		CAnimClip(NLGEORGES::UFormElm *root);

		std::string FriendlyClipName;
		uint Priority;

		unsigned int getPriority();

		void serial(NLMISC::IStream &f);
		CAnimationClip* getClip();
	private:
		CAnimationClip* m_Clip;
	};

	void play(uint8 slot);
	CAnimationClip* getClip();

	CEntityMedia* getEMD();
	void setEMD(CEntityMedia* emd);

	void update();

	std::string AnimGroupName;
	NL3D::UPlayList::TWrapMode WrapMode;
	uint AnimCycles;
	std::string Selector;
	std::vector<CAnimClip> AnimClipList;
	bool Playing;
private:
	ISelector<CAnimClip>* m_Selector;
	CEntityMedia* m_EMD;
};

}; // END NAMESPACE WWCLIENT

#endif // __CANIMGROUP_H__
