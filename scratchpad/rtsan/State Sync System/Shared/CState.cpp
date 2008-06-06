/**
 * \file CState.cpp
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

//
// Standard Includes
//

//
// System Includes
//
#include <cstdio>

//
// NeL Includes
//

//
// Werewolf Includes
//	
#include "CState.h"

//
// Namespaces
//

CState::CState() : m_Id(1) {
	// does nothing
}

CState::~CState() {
	m_StateMap.clear();
}

void CState::move() {
	printf("Moving...\n");
}

void CState::setId(uint32 id) {
	m_Id = id;
}

CPCState::CPCState() {
	m_StateMap[(uint32)Idle] = "Idling";
	m_StateMap[(uint32)Walk] = "Walking";
	m_StateMap[(uint32)Run] = "Running";
}

CPCState::~CPCState() {
	// does nothing
}

void CPCState::move() {
	printf("Moving like a PC... %s\n", m_StateMap.find(m_Id)->second.c_str());
}