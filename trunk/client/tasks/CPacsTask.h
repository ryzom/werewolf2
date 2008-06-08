/**
 * \file CPacsTask.h
 * \date May 2004
 * \author Matt Raykowski
 * \author Some code adapted from Snowballs, Nevrax France.
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

#ifndef __CPACSTASK_H__
#define __CPACSTASK_H__

//
// System Includes
//

//
// NeL Includes
//
#include <nel/pacs/u_retriever_bank.h>
#include <nel/pacs/u_global_retriever.h>
#include <nel/pacs/u_move_container.h>
#include <nel/3d/u_visual_collision_manager.h>

//
// Werewolf Includes
//
#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"

//
// Namespaces
//

namespace WWCLIENT {

//
// Class
//

class CPacsTask : public WWCOMMON::ISingleton<CPacsTask>, public WWCOMMON::ITask {
public:
	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();
	virtual std::string name();
	NLPACS::UMoveContainer &getMoveContainer();
	NL3D::UVisualCollisionManager  &getVisualCollisionManager();
	NLPACS::UGlobalRetriever					&getGlobalRetriever();

	enum {
		SelfCollisionBit = 1,
		OtherCollisionBit = 2,
		SnowballCollisionBit = 4,
		StaticCollisionBit = 8
	};

private:
	NL3D::UVisualCollisionManager	*m_VisualCollisionManager;
};

}; // END NAMESPACE WWCLIENT

#endif // __CPACSTASK_H__
