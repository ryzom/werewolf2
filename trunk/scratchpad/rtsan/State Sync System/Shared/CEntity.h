/**
 * \file CEntity.h
 * \date February 2005
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

#ifndef __SSS_CENTITY_H__
#define __SSS_CENTITY_H__

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/vector.h>

//
// Werewolf Includes
//	

//
// Namespaces
//

class CEntity {
public:
	CEntity();
	CEntity(uint32 id);
	~CEntity();
	
	virtual void update();
	virtual void render();

	uint32 state();
	void setState(uint32 state);

	uint32 id();
	void setId(uint32 id);

	static CEntity *Create();

	enum eMotionState {
		EMS_IDLE	=	0,
		EMS_WALKING	=	1,
		EMS_RUNNING	=	2,
	};

protected:
	NLMISC::CVector	m_position;
	float			m_orientation;
	uint32			m_state;
	uint32			m_id;
};

#endif // __SSS_CENTITY_H__