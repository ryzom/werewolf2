/**
 * \file CRandomSelector.h
 * \date March 2006
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

#ifndef __CRANDOMSELECTOR_H__
#define __CRANDOMSELECTOR_H__

//
// Standard Includes
//

//
// System Includes
//
#include <cstdlib>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "selectors/ISelector.h"
#include "CClientSimulation.h"

//
// Namespaces
//

namespace WWCLIENT {

template<class T>
class CRandomSelector : public ISelector<T> {
public:
	virtual ~CRandomSelector() {;};

	T* get(std::vector<T>* items) {
		srand((unsigned int)(getSimulation()->time()*1000.0f));
		double r = (double)rand();
		r /= ((double)RAND_MAX)+1.0f;
		unsigned int index = (unsigned int)(r*items->size());
		if(index < items->size()) {
			return &(items->at(index));
		}
		return NULL;
	}

	static ISelector<T>* create() {
		return new CRandomSelector<T>();
	}

protected:
	CRandomSelector() {;};
};

}; // END NAMESPACE WWCLIENT

#endif // __CRANDOMSELECTOR_H__