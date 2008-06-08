/**
 * \file CCameraTask.cpp
 * \date October 2004
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
#include "stdpch.h"

//
// NeL Includes
//
#include <nel/misc/smart_ptr.h>

//
// Werewolf Includes
//
#include "CCameraTask.h"
#include "interfaces/IBaseCamera.h"
#include "CCameraThird.h"

//
// Namespaces
//

namespace WWCLIENT {

//
// Class
//


void CCameraTask::init() {
	// lets create a third person camera by default.
	CCameraThird *camthird=new CCameraThird();
	m_ActiveCamera=dynamic_cast<IBaseCamera *>(camthird);
	m_ActiveCamera->init();
}

void CCameraTask::update() {
	m_ActiveCamera->update();
}

void CCameraTask::render() {
	m_ActiveCamera->render();
}

void CCameraTask::release() {
	m_ActiveCamera->release();
}

void CCameraTask::stop() {
	m_ActiveCamera->stop();
}

IBaseCamera &CCameraTask::getActiveCamera() {
	return *m_ActiveCamera;
}

}; // END NAMESPACE WWCLIENT
