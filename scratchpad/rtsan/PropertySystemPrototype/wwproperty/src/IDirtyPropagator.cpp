/**
 * \file IDirtyPropagator.cpp
 * \date June 2006
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
#include "wwproperty/IDirtyPropagator.h"

//
// Namespaces
//

namespace WWPROPERTY {

IDirtyPropagator::IDirtyPropagator() : m_Dirty(true), m_Inputs(NULL), m_Outputs(NULL) {
}

IDirtyPropagator::~IDirtyPropagator() {
	breakAllConnections();
	
	if(m_Inputs)
		delete m_Inputs;

	if(m_Outputs)
		delete m_Outputs;
}

bool IDirtyPropagator::isDirty() {
	return m_Dirty;
}

void IDirtyPropagator::setClean() {
	m_Dirty = false;
}

void IDirtyPropagator::setDirty() {
	// if already dirty - stop propagation. This also stops loop propagation.
	if(m_Dirty)
		return;

	m_Dirty = true;
	propagateDirtyState();
}

void IDirtyPropagator::propagateDirtyState() {
	if(m_Outputs) {
		IDirtyPropagator::TConList::iterator iter = m_Outputs->begin();
		for( ; iter != m_Outputs->end(); ++iter ) {
			(*iter)->setDirty();
		}
	}
}

bool IDirtyPropagator::makeInputConnection(IDirtyPropagator *input) {
	// can not connect to yourself
	// TODO henri:henri do loop checks here as well
	if(input == this)
		return false;

	if(!m_Inputs)
		m_Inputs = new IDirtyPropagator::TConList;

	if(m_Inputs && input->makeOutputConnection(this)) {
		m_Inputs->push_back(input);
		return true;
	}

	return false;
}

bool IDirtyPropagator::makeOutputConnection(IDirtyPropagator *output) {
	// can not connect to yourself...just in case - should be checked by the caller already tho.
	if(output == this)
		return false;

	if(!m_Outputs)
		m_Outputs = new IDirtyPropagator::TConList;

	if(m_Outputs) {
		m_Outputs->push_back(output);
		return true;
	}

	return false;
}

bool IDirtyPropagator::breakInputConnection(IDirtyPropagator *input) {
	killInputConnection(input);
	input->killOutputConnection(this);
	return false;
}

bool IDirtyPropagator::breakOutputConnection(IDirtyPropagator *output) {
	killOutputConnection(output);
	output->killInputConnection(this);
	return false;
}

void IDirtyPropagator::killInputConnection(IDirtyPropagator::TConnection con) {
	if(m_Inputs)
		m_Inputs->remove(con);
}

void IDirtyPropagator::killOutputConnection(IDirtyPropagator::TConnection con) {
	if(m_Outputs)
		m_Outputs->remove(con);
}

void IDirtyPropagator::breakAllConnections() {
	if(m_Inputs) {
		while(m_Inputs->begin() != m_Inputs->end()) {
			m_Inputs->front()->killOutputConnection(this);
			m_Inputs->pop_front();
		}
	}
	if(m_Outputs) {
		while(m_Outputs->begin() != m_Outputs->end()) {
			m_Outputs->front()->killInputConnection(this);
			m_Outputs->pop_front();
		}
	}
}

}; // END OF NAMESPACE WWPROPERTY
