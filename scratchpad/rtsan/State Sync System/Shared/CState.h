/**
 * \file CState.h
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

#ifndef __SSS_CSTATE_H__
#define __SSS_CSTATE_H__

//
// Standard Includes
//

//
// System Includes
//
#include <map>
#include <string>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>

//
// Werewolf Includes
//	

//
// Namespaces
//

class CState {
public:
	CState();
	~CState();
	
	virtual void move();
	void setId(uint32 id);

protected:
	// atm just a string map to see what's going on,
	// should be a more meaningful structure attached to each Id tho.
	// something that would contain the actual movement code.
	// So for example a script or something.
	std::map<uint32, std::string> m_StateMap;
	uint32 m_Id;

};

class CPCState : public CState {
public:
	CPCState();
	~CPCState();

	enum TId {
		Idle		= 0x00000001,
		Walk		= 0x00000002,
		Run			= 0x00000004
	};

	virtual void move();
};

#endif // __SSS_CSTATE_H__
