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

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "tasks/C3DTask.h"
#include "tasks/CConfigTask.h"
#include "tasks/C3DSwapTask.h"
#include "tasks/CInputTask.h"
//#include "tasks/CTimeTask.h"
//#include "tasks/CEntityTask.h"

//#include "interfaces/IEntity.h"

//
// Namespaces
//
using namespace std;
using namespace NL3D;
using namespace NLMISC;

namespace WWCLIENT {

void C3DSwapTask::init() {
	m_SleepTime = CConfigTask::instance().configFile().getVar("Sleep").asInt();
}

void C3DSwapTask::render() {
	C3DTask::instance().driver().swapBuffers();
	
	if(CInputTask::instance().kbPressed(KeyF2)) {
		CBitmap btm;
		C3DTask::instance().driver().getBuffer (btm);
		string filename = CFile::findNewFile ("screenshot.jpg");
		COFile fs (filename);
		btm.writeJPG(fs);
		nlinfo("Screenshot '%s' saved", filename.c_str());
	}

	// TODO henri:everyone Temporary clear until sky task is working
	C3DTask::instance().clear();
	
	if (m_SleepTime >= 0)
		nlSleep (m_SleepTime);
}

void C3DSwapTask::update() {
	;
}

void C3DSwapTask::release() {
	;
}

std::string C3DSwapTask::name() {
	return "C3DSwapTask";
}

}; // END NAMESPACE WWCLIENT
