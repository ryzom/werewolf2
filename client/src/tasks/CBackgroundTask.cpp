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
#include <nel/3d/u_driver.h>
#include <nel/3d/frustum.h>
#include <nel/3d/u_material.h>
#include <nel/3d/u_texture.h>

#include <nel/3d/texture_user.h>

//
// Werewolf Includes
//
#include "CBackgroundTask.h"
#include "C3DTask.h"
#include "CTimeTask.h"
#include "CResourceTask.h"

//
// Namespaces
//
using namespace std;
using namespace NL3D;
using namespace NLMISC;

namespace WWCLIENT {

void CBackgroundTask::init() {
	string res;
	res = CResourceTask::instance().get("ww_background.tga");
	m_Background = C3DTask::instance().driver().createTextureFile (res);
	nlassert(m_Background);
}

std::string CBackgroundTask::name() {
	return "CBackgroundTask";
}

void CBackgroundTask::update() {
	;
}

void CBackgroundTask::render() {
	float sWidth=C3DTask::instance().getScreenWidth();
	float sHeight=C3DTask::instance().getScreenHeight();
	C3DTask::instance().driver().setMatrixMode2D11 ();
	C3DTask::instance().driver().setMatrixMode2D (CFrustum(0,sWidth,0,sHeight,-1,1,false));
	C3DTask::instance().driver().drawBitmap(0.0f,0.0f,sWidth,sHeight,*(UTexture*)m_Background);
}

void CBackgroundTask::release() {
	if(m_Background) {
		C3DTask::instance().driver().deleteTextureFile(m_Background);
		m_Background = 0;
	}
}

}; // END NAMESPACE WWCLIENT
