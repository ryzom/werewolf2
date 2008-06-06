/**
 * \file CTestEvent.cpp
 * \date November 2006
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

#include "stdafx.h"
#include "CTestEvent.h"

/// Register the event with the class registry.
WWCOMMON_REGISTER_EVENT(CTestEvent);

CTestEvent::CTestEvent() : IGameEvent(), contentSize(0), content(NULL), sendTime(0) {
	m_ID = CTestEvent::CTestEventID;
}

CTestEvent::~CTestEvent() {
	delete[] content;
}

void CTestEvent::serial(NLMISC::IStream &f) {
	IGameEvent::serial(f);

	f.serial(contentSize);
	f.serialBuffer(content, contentSize);
	f.serial(sendTime);
}

void CTestEvent::createContent(uint size) {
	if(content != NULL)
		delete[] content;
	contentSize = 0;
	if(getRealEventSize() > size)
		size = 0;
	else
		size = size - getRealEventSize();
	
	content = new uint8[size];
	contentSize = size;
}

uint CTestEvent::getRealEventSize() {
	return sizeof(uint16) + sizeof(uint8) + contentSize + getClassName().length() + 1 + sizeof(double);
}

uint CTestEvent::getEventClassSize() {
	return sizeof(CTestEvent) + contentSize;
}