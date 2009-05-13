/**
 * \file CCameraThird.h
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

#ifndef __CCAMERATHIRD_H__
#define __CCAMERATHIRD_H__

//
// System Includes
//

//
// NeL Includes
//
#include <nel/3d/u_camera.h>

//
// Werewolf Includes
//
#include "interfaces/IBaseCamera.h"

//
// Namespaces
//

namespace WWCLIENT {

//
// Class
//

/**
 * \class CCameraThird CCameraThird.h "CCameraThird.h"
 * \brief This is a basic third-person camera.
 */
class CCameraThird : public IBaseCamera {
public:
	CCameraThird();
	// this is the control mechanism.
	virtual void init();
	virtual void update();
	virtual void render();
	virtual void release();

	virtual void stop();
	
	virtual std::string name();

	// and some camera stuff.
	virtual float getViewingDistance();
	virtual void setViewingDistance(float dist);
	virtual float getViewAheadDistance();
	virtual void setViewAheadDistance(float dist);
	virtual void rotate(int dp, int dy);
	
protected:
	/**
	 * \brief Represents the actual camera.
	 */
	NL3D::UCamera m_Camera;
	/**
	 * \brief document me.
	 */
	float				m_ViewLagBehind;
	/**
	 * \brief Offset the height of the point we are looking at
	 */
	float				m_ViewHeightOffset;
	/**
	 * \brief document me.
	 */
	float				m_ViewPitch;
	/**
	 * \brief document me.
	 */
	float				m_ViewAheadDistance;
	/**
	 * \brief The position the camera wants to be at
	 */
	NLMISC::CVector		m_EyePos;
	/**
	 * \brief The position the camera wants to look at
	 */
	NLMISC::CVector		m_LookAtPoint;
	/**
	 * \brief This calculates the target of the camera
	 * It calculates and sets m_EyePos and m_LookAtPoint.
	 * Note that his does not actually move the camera.
	 * It just predicts where the camera wants to move.
	 * Movement is handled by the move() method.
	 */
	virtual void		calculateTarget();
	/**
	 * \brief This moves the camera
	 * Try to move to the position previously calculated.
	 * It should be implemented differently by different camera types.
	 * This one just sets the current position to the calculated one.
	 */
	virtual void		move();

	double m_CameraLag;
	float m_ViewYaw;
	float m_YawVelocity;
	float m_PitchVelocity;
	float m_MaxRotSpeed;
	float m_RotBrakingFactor;
	float m_RotAccel;

	double m_LastTime;
};

}; // END NAMESPACE WWCLIENT

#endif // __CCAMERATHIRD_H__
