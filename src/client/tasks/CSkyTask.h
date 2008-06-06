/**
 * \file CSkyTask.h
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

#ifndef __CSKYTASK_H__
#define __CSKYTASK_H__

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

/**
 * \class CSkyTask CSkyTask.h "CSkyTask.h"
 * \brief Handles rendering the sky.
 *
 * This handles the sky, displaying clouds, night time.
 */
class CSkyTask : public WWCOMMON::ISingleton<CSkyTask>, public WWCOMMON::ITask {
public:
	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	virtual std::string name() { return "CSkyTask"; }

	void shapeName(std::string shapeName);
	std::string shapeName();
	
	friend class WWCOMMON::ISingleton<CSkyTask>;
protected:
	CSkyTask();
	
private:
	NL3D::UScene			*m_SkyScene;
	NL3D::UInstance			m_SkyMesh;
	NL3D::UCloudScape		*m_CloudScape;

	std::string m_ShapeName;
};

}; // END NAMESPACE WWCLIENT

#endif // __CSKYTASK_H__
