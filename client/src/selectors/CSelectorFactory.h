/**
 * \file CSelectorFactory.h
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

#ifndef __CSELECTORFACTORY_H__
#define __CSELECTORFACTORY_H__

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
#include <wwcommon/ISingleton.h>
#include <wwcommon/general.h>

#include "selectors/ISelector.h"
#include "selectors/CRandomSelector.h"
#include "selectors/CSequentialSelector.h"

//
// Namespaces
//

namespace WWCLIENT {

template<class T>
class CSelectorFactory : public WWCOMMON::ISingleton<CSelectorFactory<T> > {
public:
	typedef ISelector<T>* (*CreateSelector)(void);
	typedef std::map<std::string, CreateSelector> selectorMap;
	
	bool registerSelector(CreateSelector create, std::string name) {
		return m_SelectorMap.insert( typename selectorMap::value_type(name, create)).second;
	};

	ISelector<T>* createSelector(std::string name) {
		typename selectorMap::iterator iter = m_SelectorMap.find(name);
		if( iter == m_SelectorMap.end()) {
			nlwarning("Selector creation failed: %s not found!", name.c_str());
			if(m_DefaultSelector)
				return m_DefaultSelector();
			return NULL;
		}

		return (iter->second)();
	};

	CSelectorFactory() {
		m_DefaultSelector = &(CSequentialSelector<T>::create);
		registerSelector(&(CSequentialSelector<T>::create), "sequential");
		registerSelector(&(CRandomSelector<T>::create), "random");
	};

private:
	selectorMap m_SelectorMap;

	CreateSelector m_DefaultSelector;

};

}; // END NAMESPACE WWCLIENT

#endif // __CSELECTORFACTORY_H__
