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
#include "stdpch.h"
#include <sstream>

//
// NeL Includes
//
#include <nel/misc/file.h>
#include <nel/misc/path.h>

//
// Werewolf Includes
//
#include "CConfigTask.h"
#include "CUserPaths.h"

//
// Namespaces
//
using namespace std;
using namespace NLMISC;

namespace WWCLIENT {

void CConfigTask::init() {
	std::string ConfigFileName = CUserPaths::getWerewolfConfigDir() + "werewolf.cfg";

	// try to load the config. if the file doesn't exist create it
	// and point it to the defaults.
	try {
		if(!NLMISC::CFile::fileExists(ConfigFileName)) {
			FILE *fp = fopen(ConfigFileName.c_str(), "wt");
			nlassert(fp);
			std::stringstream strstr;
			strstr << "RootConfigFilename = \"";
			strstr << ETC_DIR;
			strstr << "werewolf_default.cfg\";\n";
			fprintf(fp,strstr.str().c_str());
			fclose(fp);
		}
		m_ConfigFile.load (ConfigFileName);
		m_FileNames.push_back(ConfigFileName);
	}	catch (NLMISC::EConfigFile &e) {
		nlerror(e.what());
	}

	/**
	 * @todo In CConfigFile::init() move the search path crap to the ResourceManager
	 */
	CConfigFile::CVar &v = CConfigTask::instance().configFile().getVar("Path");
	for (int i = 0; i < v.size(); i++)
		CPath::addSearchPath (v.asString(i), true, false);
}

void CConfigTask::update() {
	CConfigFile::checkConfigFiles();
}

std::string CConfigTask::name() {
	return "CConfigTask";
}

void CConfigTask::addFile(std::string configFile) {
	checkFile(configFile);
	try {
		if(!NLMISC::CFile::fileExists(configFile)) {
			nlinfo("ConfigTask::addFile attempted to load a config file that doesn't exist.");
			return;
		}
		m_ConfigFile.load(configFile);
		m_FileNames.push_back(configFile);
	} catch(NLMISC::EConfigFile &e) {
		nlerror(e.what());
	}
}

bool CConfigTask::checkFile(std::string configFile) {
	std::vector< std::string >::iterator it=m_FileNames.begin();
	while(it!=m_FileNames.end()) {
		if( (*it)==configFile )
			return true;
		it++;
	}
	return false;
}

NLMISC::CConfigFile &CConfigTask::configFile() {
	nlassert(m_ConfigFile.loaded());
	return m_ConfigFile;
}

void CConfigTask::render() {
	;
}

void CConfigTask::release() {
	;
}

}; // END NAMESPACE WWCLIENT
