/**
 * \file CAnimateTask.h
 * \date May 2004
 * \author Matt Raykowski
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

#ifndef __CANIMATETASK_H__
#define __CANIMATETASK_H__

//
// System Includes
//
#include <string>

//
// NeL Includes
//
#include <nel/3d/u_animation_set.h>
#include <nel/3d/u_play_list_manager.h>
#include <nel/georges/u_form_loader.h>
#include <nel/georges/u_form.h>
#include <nel/georges/u_form_elm.h>
#include <nel/georges/load_form.h>

//
// Werewolf Includes
//
#include "CAnimationClip.h"

#include <wwcommon/general.h>
#include <wwcommon/ITask.h>
#include <wwcommon/ISingleton.h>

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
 * \class CAnimateTask CAnimateTask.h "tasks/CAnimateTask.h"
 * \brief This task manages animation clips and playlists.
 */
class CAnimateTask : public WWCOMMON::ISingleton<CAnimateTask>, public WWCOMMON::ITask {
public:
	/**
	 * \brief Initializes the animation system.
	 *
	 * This initializes the animation subsystem. It loads up the "animationclips" variable, which
	 * tells it which other variables to load into the animation clip list.
	 */
	void init();
	void update();
	void render();
	void release();
	std::string name(); /**< Returns the name of this task. */
	NL3D::UPlayListManager* getPlayListManager();
	NL3D::UAnimationSet* getAnimationSet();
	CAnimationClip* getClip(std::string name);

protected:
	typedef CHashMap<std::string, CAnimationClip> clipMap;
	clipMap m_AnimationClips;
	NL3D::UAnimationSet *m_AnimationSet;
	NL3D::UPlayListManager *m_PlayListManager; 
};

}; // END NAMESPACE WWCLIENT

#endif // __CANIMATETASK_H__
