/**
 * \file CFESData.h
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

#ifndef __CFESDATA_H__
#define __CFESDATA_H__

//
// System Includes
//
#include <vector>
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

//
// Class
//
class CFESData {
public:
	CFESData(NLNET::TServiceId sid);

	NLNET::TServiceId	SId;				// Connection to the front end
	uint32 NbPendingUsers;		// Number of not yet connected users (but rooted to this frontend)
	uint32 NbEstimatedUser;	// Number of user that already routed to this FES. This number could be different with the NbUser if
									// some users are not yet connected on the FES (used to equilibrate connection to all front end).
									// This number *never* decrease, it's just to fairly distribute user.
	uint32 NbUser;				// Number of user currently connected on this front end

	enum TFESState {
		PatchOnly,
		AcceptClientOnly
	};
	TFESState	State;				// State of frontend (patching/accepting clients)
	std::string	PatchAddress;		// Address of frontend patching server

	uint32 getUsersCountHeuristic() const {
		//return NbEstimatedUser;
		return NbUser + NbPendingUsers;
	}

	void setToAcceptClients() {
		if(State == AcceptClientOnly)
			return;

		// tell FS to accept client
		State = AcceptClientOnly;
		//NLNET::CMessage msgOpenFES("FS_ACCEPT");
		//NLNET::CUnifiedNetwork::getInstance()->send(SId, msgOpenFES);

		// report state to LS
		bool dummy;
		reportStateToLS(dummy, true);
	}

	void reportStateToLS(bool& reportPatching, bool alive = true) {
		// report to LS
		bool patching = (State == PatchOnly);
		if (alive && patching)
			reportPatching = true;

		//NLNET::CMessage msgout("REPORT_FS_STATE");
		//msgout.serial(SId);
		//msgout.serial(alive);
		//msgout.serial(patching);
		//msgout.serial(PatchAddress);
		//NLNET::CUnifiedNetwork::getInstance()->send("LS", msgout);
	}

	/********
	 * The following functions are global methods for manipulating FES data.
	 ********/

	/**
	 * \brief Finds the best Front End Service available for a new user.
	 *
	 * Find the best front end service for a new connecting user (return NULL if there is no suitable FES).
	 * Additionally, calculate totalNbUsers.
	 */
	static CFESData *CFESData::findBestFES(uint &totalNbUsers);

	/**
	 * \brief Selects a front-end in patch mode to open.
	 *
	 * Select a frontend in patch mode to open
	 * Returns true if a new FES was open, false if no FES could be open
	 */
	static bool openNewFES();
};

#endif

