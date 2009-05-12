/**
 * \file CBackgroundTask.h
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

#ifndef __CBACKGROUNDTASK_H__
#define __CBACKGROUNDTASK_H__

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"

//
// Class
//

namespace WWCLIENT {

class CBackgroundTask : public WWCOMMON::ISingleton<CBackgroundTask>, public WWCOMMON::ITask {
public:
	virtual void init();
	virtual void update();
	virtual void render();

	/**
	 * \brief Releases the textures when this task is no longer needed.
	 *
	 * \todo Fix instance stuff if particles are resurrected.
	 */
	virtual void release();

	virtual std::string name();
	
private:
	NL3D::UTextureFile	*m_Background;
	NL3D::UMaterial			*m_MatBackground;
};

}; // END NAMESPACE WWCLIENT

#endif // __CBACKGROUNDTASK_H__
