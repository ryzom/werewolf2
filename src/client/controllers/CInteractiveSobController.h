/**
 * \file CInteractiveSobController.h
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

#ifndef __CINTERACTIVESOBCONTROLLER_H__
#define __CINTERACTIVESOBCONTROLLER_H__

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "CActorProxy.h"
#include <wwcommon/CStateManager.h>
#include <wwcommon/ISobController.h>

//
// Class
//

namespace WWCLIENT {

class CInteractiveSobController : public WWCOMMON::ISobController {
public:
	CInteractiveSobController(CActorProxy* sob);
	virtual ~CInteractiveSobController();

	void	update();
	const char* getControllerName();

private:
	CActorProxy *m_Subject;
	static const char* m_Name;

	void doMove(int fwd_back);
	void doStrafe(int strafe);
	void doOrient(int orient);

	int m_fwd_back;
	int m_strafe;
	int m_orient;
};

}; // END NAMESPACE WWCLIENT

#endif // __CINTERACTIVESOBCONTROLLER_H__
