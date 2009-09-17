/**
 * \file CAnimationController.h
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

#ifndef __CANIMATIONCONTROLLER_H__
#define __CANIMATIONCONTROLLER_H__

//
// Standard Includes
//

//
// System Includes
//
#include <vector>
#include <utility>

//
// NeL Includes
//

//
// Werewolf Includes
//	
#include "entities/IMediaController.h"
#include <wwcommon/IDirtyPropagator.h>
#include "entities/CEntityMedia.h"
#include <wwcommon/CStateManager.h>

//
// Namespaces
//

namespace WWCLIENT {

class CAnimationController : public IMediaController {
public:
	typedef std::pair<const char*, const char*> AnimDesc;
	typedef std::pair<uint32, AnimDesc> Animation;
	typedef std::vector<Animation> AnimationList;
	typedef std::pair<uint32, AnimationList> AnimationLayer;
	typedef std::vector<AnimationLayer> AnimationLayerList;

	CAnimationController(const char* name, CEntityMedia* emd, AnimationLayerList& list) 
		: m_Name(name), m_EMD(emd), m_Owner(emd->getOwner()), m_Layers(list) {
		;
	}

	void init() {
		AnimationLayerList::iterator iter = m_Layers.begin();
		for( ; iter != m_Layers.end(); ++iter ) {
			makeInputConnection(&(m_Owner->getStateLayer((*iter).first)->ActiveState));
		}
	}

	void update() {
		if(isDirty()) {
			nldebug("Updating %s", m_Name);
			AnimationLayerList::iterator iter = m_Layers.begin();
			for( ; iter != m_Layers.end(); ++iter ) {
				WWCOMMON::ISimulationState* state = m_Owner->getState((*iter).first);
				AnimationList::iterator aiter = (*iter).second.begin();
				for( ; aiter != (*iter).second.end(); ++aiter) {
					if(state && state->getStateId() == (*aiter).first) {
						m_EMD->playAnimation((*aiter).second.first, (*aiter).second.second);
						break;
					}
				}
			}
			setClean();
		}
	}

	const char* getControllerName() {
		return m_Name;
	}
private:
	const char* m_Name;
	CEntityMedia* m_EMD;
	AnimationLayerList m_Layers;
	WWCOMMON::ISimulationObj* m_Owner;
};

}; // END NAMESPACE WWCLIENT

#endif // __CANIMATIONCONTROLLER_H__
