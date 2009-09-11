/**
 * \file CServerSimulation.cpp
 * \date May 2005
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
// Standard Includes
//
#include "stdpch.h"

//
// System Includes
//
#include <string>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/path.h>

//
// Werewolf Includes
//
#include "CUserPaths.h"

//
// Namespaces
//

namespace WWCLIENT {

std::string CUserPaths::getHomeDir() {
#if defined(NL_OS_WINDOWS)
	std::string homeDir;
	// Returns something like:
	// C:\\Documents and Settings\\myuser\\Application Data\\
	TCHAR szAppData[MAX_PATH];
	SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, 0, szAppData);

	homeDir.assign(szAppData);
	return homeDir;
#elif defined(NL_OS_UNIX)
	char *home = getenv("HOME");
	if(home == NULL || home[0] == '\0') {
		nlerror ("No home dir");
	}
	return std::string(home) + "/";
#elif defined(NL_OS_MAC)
	nlerror ("TODO: Mac home dir");
	return "";
#endif // NL_OS_WINDOWS
}

std::string CUserPaths::getConfigDir() {
#if defined(NL_OS_WINDOWS)
	return getHomeDir();
#elif defined(NL_OS_UNIX)
	char *xdgConfigHome = getenv("XDG_CONFIG_HOME");
	std::string configDir;
	if(xdgConfigHome == NULL || xdgConfigHome[0] == '\0') {
		configDir = getHomeDir() + ".config/";
		if(!NLMISC::CFile::isDirectory(configDir)) {
			if(!NLMISC::CFile::createDirectoryTree(configDir)) {
				nlerror("Unable to create configuration directory: %s", configDir.c_str());
			}
		}
	} else {
		configDir = std::string(xdgConfigHome) + "/";
	}
	return configDir;
#elif defined(NL_OS_MAC)
	nlerror ("TODO: Mac config dir");
	return "";
#endif // NL_OS_WINDOWS
}

std::string CUserPaths::getWerewolfConfigDir() {
	std::string werewolfConfigDir = getConfigDir() + "werewolf/";
	if(!NLMISC::CFile::isDirectory(werewolfConfigDir)) {
		if(!NLMISC::CFile::createDirectoryTree(werewolfConfigDir)) {
			nlerror("Unable to create configuration directory: %s", werewolfConfigDir.c_str());
		}
	}
	return werewolfConfigDir;
}

}; // END NAMESPACE WWCLIENT
