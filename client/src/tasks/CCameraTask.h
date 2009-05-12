/**
 * \file CCameraTask.h
 * \date October 2004
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

#ifndef __CCAMERATASK_H__
#define __CCAMERATASK_H__

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/smart_ptr.h>

//
// Werewolf Includes
//
#include "interfaces/IBaseCamera.h"

#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"

//
// Namespaces
//

namespace WWCLIENT {

//
// Class
//

/**
 * \class CCameraTask CCameraTask.h "tasks\CCameraTask.h"
 * \brief This controls the various camera types.
 */
class CCameraTask : public WWCOMMON::ISingleton<CCameraTask>, public WWCOMMON::ITask {
public:

	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	virtual void stop();
	
	virtual std::string name() { return "CCameraTask"; }
	virtual IBaseCamera &getActiveCamera();
	
private:
	IBaseCamera *m_ActiveCamera;
};

}; // END NAMESPACE WWCLIENT

#endif // __CCAMERATASK_H__
