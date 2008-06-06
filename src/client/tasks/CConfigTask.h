/**
 * \file CConfigTask.h
 * \date May 2004
 * \author Matt Raykowski
 * \author Some code adapted from MTP-Target.
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

#ifndef __CCONFIGTASK_H__
#define __CCONFIGTASK_H__

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"

//
// Class
//

namespace WWCLIENT {

class CConfigTask : public WWCOMMON::ISingleton<CConfigTask>, public WWCOMMON::ITask {
public:
	/**
	 * \brief Loads up the config file, creates anything necessary.
	 *
	 * When called this checks to see if "werewolf.cfg" exists. If it does
	 * not exist create the file and add:
	 * RootConfigFilename = "werewolf_default.cfg"
	 *
	 * @see m_ConfigFile
	 */
	virtual void init();
	/**
	 * \brief Calls CConfigFile to check for changes to the actual files.
	 *
	 * Calls CConfigFile to check for changes to the actual files. Updates the
	 * variables internal to m_ConfigFile.
	 *
	 * @see m_ConfigFile
	 */
	virtual void update();
	virtual void render();		/**< This method, part of ITask, does nothing. */
	virtual void release();		/**< This method, part of ITask, does nothing. */

	virtual std::string name(); /**< Sends the name of this task back. */

	/**
	 * \brief Get the NeL config file.
	 * \return NLMISC::CConfigFile containing all of the loaded config information.
	 */
	NLMISC::CConfigFile &configFile();

	/**
	 * \brief Adds additional files to the NeL config management.
	 * \param configFile : a string containing the filename to load.
	 */
	void addFile(std::string configFile);

	/**
	 * \brief Checks to see if the file has been loaded.
	 * \param configFile : a string containing the file to verify.
	 * \return bool whether the file has been loaded.
	 */
	bool checkFile(std::string configFile);

private:
	/**
	 * \brief Contains our config file and hooks to update it.
	 *
	 * This contains a NeL config file object. This lets us pull variables from
	 * the file and get notified when they're updated.
	 */
	NLMISC::CConfigFile m_ConfigFile;

	/**
	 * \brief List of additional configuration files
	 */
	std::vector< std::string > m_FileNames;

};

}; // END NAMESPACE WWCLIENT

#endif // __CCONFIGTASK_H__
