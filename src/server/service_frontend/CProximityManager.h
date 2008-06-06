/**
 * \file CProximityManager.h
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

#ifndef __CPROXIMITYMANAGER_H__
#define __CPROXIMITYMANAGER_H__

//
// Standard Includes
//

//
// System Includes
//
#include <vector>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include <wwcommon/ISingleton.h>
#include <wwcommon/CPerformer.h>
#include <wwcommon/CRangeGrid2D.h>

//
// Namespaces
//


// TODO henri:henri currently a dummy implementation. Implement decently.
class CProximityManager : public WWCOMMON::ISingleton<CProximityManager> {
public:
	CProximityManager();
	~CProximityManager();

	typedef WWCOMMON::CRangeGrid2D<WWCOMMON::CPerformer*> sobGrid;

	// Dealing with proximity.
	void addSob(WWCOMMON::CPerformer* sob);
	void removeSob(WWCOMMON::CPerformer* sob);
	sobGrid::List getSobsInRange(WWCOMMON::CPerformer* sob);
	
	// Dealing with proximity updates during motion.
	void registerSobForUpdate(WWCOMMON::CPerformer* sob);
	void update();
	double getUpdateFrequency(WWCOMMON::CPerformer* sob);

	sobGrid::Position getSobPosition(WWCOMMON::CPerformer* sob);
	sobGrid::Position getStoredSobPosition(WWCOMMON::CPerformer* sob);

	void postAddEvents(WWCOMMON::CPerformer* sob, sobGrid::List* list);
	void postRemoveEvents(WWCOMMON::CPerformer* sob, sobGrid::List* list);

private:
	typedef std::set<WWCOMMON::CPerformer*> sobList;
	sobList m_SobsToUpdate;

	sobGrid	*m_Grid;
	sobGrid::Position m_Origin;
	float m_ProximityRange;
};

#endif // __CPROXIMITYMANAGER_H__
