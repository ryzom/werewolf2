/**
 * \file CAnimationClip.h
 * \date May 2004
 * \author Matt Raykowski
 * \author Henri Kuuste
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

#ifndef __CANIMATIONCLIP_H__
#define __CANIMATIONCLIP_H__

//
// System Includes
//
#include <string>

//
// NeL Includes
//
#include <nel/3d/animation_time.h>
#include <nel/3d/u_animation.h>

//
// Werewolf Includes
//

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
 * \class TAnimClipLoader CAnimateTask.h "tasks/CAnimateTask.h"
 * \brief This loads animation clips from packed sheets.
 */
class CAnimationClip {
public:
	/**
	 * \brief Here you read in the form if necessary, storing it in memebers of the TMyLoader class
	 */
	void readGeorges(const NLMISC::CSmartPtr<NLGEORGES::UForm> &form, const std::string &sheetId) {
		NLGEORGES::UFormElm &root = form->getRootNode();
		root.getValueByName(Filename,".Filename");
		root.getValueByName(FriendlyName,".FriendlyName");
		root.getValueByName(Description,".Description");
	}

	/**
	 * \brief Here you write a standard NeL serial for all the member of TMyLoader that need to be packed.
	 */
	void serial(NLMISC::IStream &f) {
		f.serial(Filename);
		f.serial(FriendlyName);
		f.serial(Description);
	}

	/**
	 * \brief This methed returns the version of the packed sheet.
	 * Version 0 - non-existant.
	 * Version 1 - Fileame, FriendlyName, Description.
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

	std::string Filename;
	std::string FriendlyName;
	std::string Description;
	uint ID;
	NL3D::UAnimation* Animation;
};

}; // END NAMESPACE WWCLIENT

#endif // __CANIMATIONCLIP_H__
