/**
 * \file CFESData.cpp
 * \date December 2004
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

//
// System Includes
//
#include <vector>
#include <list>
#include <string>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/net/service.h>
#include <nel/misc/displayer.h>

//
// Werewolf Includes
//
#include "CFESData.h"

//
// Class
//
std::list<CFESData> FESList;
std::map<uint32, NLNET::TServiceId> UserIdSockAssociations;

CFESData::CFESData(NLNET::TServiceId sid) {
	SId=sid;
	NbPendingUsers=0;
	NbEstimatedUser=0;
	NbUser=0;
	State=PatchOnly;
}

CFESData *CFESData::findBestFES(uint &totalNbUsers) {
	totalNbUsers = 0;

	CFESData *best = NULL;

	for(std::list<CFESData>::iterator it=FESList.begin(); it!=FESList.end(); ++it) {
		if((*it).State == AcceptClientOnly) {
			if(best == NULL || best->getUsersCountHeuristic() > (*it).getUsersCountHeuristic())
				best = &(*it);
			totalNbUsers += (*it).NbUser;
		}
	}

	return best;
}

/**
 * Select a frontend in patch mode to open
 * Returns true if a new FES was open, false if no FES could be open
 */
bool CFESData::openNewFES() {
	for(std::list<CFESData>::iterator it=FESList.begin(); it!=FESList.end(); ++it)
		if((*it).State == PatchOnly) {
			nlinfo("openNewFES: ask the FS %d to accept clients", (*it).SId);

			// switch FES to AcceptClientOnly
			(*it).setToAcceptClients();
			return true;
		}
	return false;
}

