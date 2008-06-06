/**
 * \file CGameTask.cpp
 * \date June 2004
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

//
// System Includes
//
#include "stdpch.h"
#include "CEGUI.h"

//
// NeL Includes
//
#include <nel/net/callback_client.h>

//
// Werewolf Includes
//
#include "CGameTask.h"

#include "tasks/C3DTask.h"
#include "tasks/CLandscapeTask.h"
#include "tasks/CConfigTask.h"
#include "tasks/CSkyTask.h"
#include "tasks/CAnimateTask.h"
#include "tasks/CPacsTask.h"
#include "tasks/CInputTask.h"
#include "tasks/CIntroTask.h"
#include "tasks/CCameraTask.h"
#include "tasks/CTimeTask.h"
#include "tasks/CNetworkTask.h"

#include "wwcommon/CTaskManager.h"

#include "wwcommon/ITask.h"

#include "CClientSimulation.h"

//
// Namespaces
//
using namespace std;
using namespace NL3D;
using namespace NLMISC;

namespace WWCLIENT {

bool handleChatLine(const CEGUI::EventArgs &e) {
	std::string chatmsg="";

	chatmsg=CEGUI::WindowManager::getSingleton().getWindow("GameTask/Chatbox/ChatEDT")->getText().c_str();
	CEGUI::WindowManager::getSingleton().getWindow("GameTask/Chatbox/ChatEDT")->setText("");

	CGameTask::instance().parseConsole(chatmsg);

	return true;
}

void CGameTask::parseConsole(std::string chatmsg) {
	int idx=chatmsg.find("/");
	if(idx != 0)
		CNetworkTask::instance().sendChat(chatmsg);
	else
		nlinfo("A slash was found at: %d", idx);
}

void CGameTask::init() {
	nlinfo("Start game with start position: %d / %d / %d",getClientSimulation()->getSelfSob()->getPosition().x, 
		getClientSimulation()->getSelfSob()->getPosition().y, 
		getClientSimulation()->getSelfSob()->getPosition().z);

	WWCOMMON::CTaskManager::instance().add(CLandscapeTask::instance(), 55);
//	WWCOMMON::CTaskManager::instance().add(CSkyTask::instance(), 61);
	WWCOMMON::CTaskManager::instance().add(CPacsTask::instance(), 65);
	WWCOMMON::CTaskManager::instance().add(CCameraTask::instance(),75);
	
	if(CNetworkTask::instance().getState()==CNetworkTask::eOnline) {
		CEGUI::WindowManager::getSingleton().getWindow("GameTask/Chatbox/ChatEDT")->
			subscribeEvent(CEGUI::Editbox::EventTextAccepted, handleChatLine);
		CEGUI::WindowManager::getSingleton().getWindow("GameTask/Chatbox")->show();
	}

	// show the menu we're interested in.
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::FrameWindow *wnd=(CEGUI::FrameWindow *)winMgr.getWindow("GameTask/DebugWindow");
	wnd->show();
	m_FpsCounter=(CEGUI::StaticText *)winMgr.getWindow("GameTask/DebugWindow/FPS");
	m_Location=(CEGUI::StaticText *)winMgr.getWindow("GameTask/DebugWindow/Position");
	m_Zone=(CEGUI::StaticText *)winMgr.getWindow("GameTask/DebugWindow/Zone");
}

void CGameTask::update() {
	std::string debug="";
	
	// update the debug window.
	debug="FPS: " + NLMISC::toString(CTimeTask::instance().fps());
	m_FpsCounter->setText(debug.c_str());
	
	// update the entity location.
	CActorProxy *actor=getClientSimulation()->getSelfSob();
	if(actor!=NULL) {
		NLMISC::CVector pos=actor->getPosition();
		if(pos==NLMISC::CVector::Null) {
			return;
		}
		debug="Pos: "+NLMISC::toString(pos.x)+","+NLMISC::toString(pos.y)+","+NLMISC::toString(pos.z);
		m_Location->setText(debug.c_str());

		debug="Zone: "+CLandscapeTask::instance().getLandscape().getZoneName(pos);
		m_Zone->setText(debug.c_str());
	}

	// process mouse/keyboard input
	m_Controller.update();
}

void CGameTask::render() {
	// since we have a sky, clearing the color doesn't do much
	if(C3DTask::instance().driver().getPolygonMode() == 0) {
		C3DTask::instance().driver().clearZBuffer();
	} else {
		C3DTask::instance().clear();
	}
}

void CGameTask::release() {
	;
}

void CGameTask::stop() {
	//ITask::stop();
	CLandscapeTask::instance().stop();
	CSkyTask::instance().stop();
	CPacsTask::instance().stop();
	CAnimateTask::instance().stop();
	//CHudTask::instance().stop();
	//CScoreTask::instance().stop();
	//CChatTask::instance().stop();
}

void CGameTask::recvChat(std::string chatmsg) {
	CEGUI::Listbox *listbox=(CEGUI::Listbox *)CEGUI::WindowManager::getSingleton().getWindow("GameTask/Chatbox/ChatList");
	listbox->addItem(new IntroListItem(chatmsg));
}

}; // END NAMESPACE WWCLIENT
