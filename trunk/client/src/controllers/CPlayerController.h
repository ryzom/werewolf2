/**
 * \file CPlayerController.h
 * \date July 2004
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

#ifndef __CPLAYERCONTROLLER_H__
#define __CPLAYERCONTROLLER_H__

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/event_listener.h>
#include <nel/misc/matrix.h>

#include <nel/3d/u_driver.h>
#include <nel/3d/frustum.h>
#include <nel/3d/u_3d_mouse_listener.h>

//
// Werewolf Includes
//
//#include "wwcommon/CStateManager.h"

//
// Class
//

namespace WWCLIENT {

/**
 * \class CPlayerController CPlayerController.h "controllers/CPlayerController.h"
 * \brief This controller handles player-level input response.
 */
class CPlayerController : public NLMISC::IEventListener, public NL3D::U3dMouseListener {
public:
	CPlayerController();
	~CPlayerController();

	void	update();
	void	reset();

	// NL3D::U3dMouseListener required methods
	void setMatrix (const NLMISC::CMatrix &matrix) {
		m_Matrix=matrix;	
	}
	void setFrustrum(const NL3D::CFrustum &frustrum) {
		m_Frustrum=frustrum;
	}
	void setViewport(const NL3D::CViewport &viewport) {
		m_Viewport=viewport;
	}
	void setHotSpot(const NLMISC::CVector &hotSpot) {
		m_HotSpot=hotSpot;
	}
	void setMouseMode(NL3D::U3dMouseListener::TMouseMode mouseMode) {}
	void setSpeed (float speed) {
		m_Speed=speed;
	}
	const NLMISC::CMatrix &getViewMatrix();
	NLMISC::CVector getHotSpot() const {
		return m_HotSpot;
	}
	NLMISC::IEventListener &getEventListenerInterface() {
		return static_cast<NLMISC::IEventListener &>(*this);
	}

private:
	virtual void operator ()(const NLMISC::CEvent &event);

	float m_VirtualFrameTime;
	bool m_FirstFrame;
	float m_MissedTime;

	// NL3D::U3dMouseListener stuff.
	NLMISC::CMatrix m_Matrix;
	NL3D::CFrustum m_Frustrum;
	NL3D::CViewport m_Viewport;
	NLMISC::CVector m_HotSpot;
	float m_Speed;
};

}; // END NAMESPACE WWCLIENT

#endif // __CPLAYERCONTROLLER_H__
