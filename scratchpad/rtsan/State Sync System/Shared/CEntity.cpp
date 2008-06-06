/**
 * \file CEntity.cpp
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

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "CEntity.h"
#include "CEntityFactory.h"

//
// Namespaces
//

namespace {
	bool registered=CEntityFactory::instance().registerEntity("default",CEntity::Create);
}

CEntity::CEntity() {
	// does nothing
}

CEntity::CEntity(uint32 id) : m_id(id) {
	// does nothing
}

CEntity::~CEntity() {
	// does nothing
}

uint32 CEntity::id() {
	return m_id;
}

void CEntity::setId(uint32 id) {
	m_id = id;
}

uint32 CEntity::state() {
	return m_state;
}

void CEntity::setState(uint32 state) {
	m_state = state;
}

void CEntity::render() {
}

void CEntity::update() {
}

CEntity *CEntity::Create() {
	return new CEntity();
}