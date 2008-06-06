/**
 * \file IBaseCamera.h
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

#ifndef __IBASECAMERA_H__
#define __IBASECAMERA_H__

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//

//
// Namespaces
//

namespace WWCLIENT {

//
// Class
//

/**
 * \interface IBaseCamera IBaseCamera.h "interfaces\IBaseCamera.h"
 * \brief This is the base-class for all camera types.
 */
class IBaseCamera {
public:
	// this is the control mechanism.
	virtual void init()=0;
	virtual void update()=0;
	virtual void render()=0;
	virtual void release()=0;

	virtual void stop()=0;
	
	virtual std::string name()=0;

	// and some camera stuff.
	virtual float getViewingDistance()=0;
	virtual void setViewingDistance(float dist)=0;
	virtual float getViewAheadDistance()=0;
	virtual void setViewAheadDistance(float dist)=0;
	virtual void rotate(int dp, int dy)=0;
};

}; // END NAMESPACE WWCLIENT

#endif // __IBASECAMERA_H__
