/**
 * \file CEventTest.h
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

#ifndef __CEVENT_TEST_H__
#define __CEVENT_TEST_H__

#include "ISource.h"
#include <string>
#include <memory>
#include "CBroadcaster.h"
#include "CFilter.h"
#include "CListener.h"
#include <vector>

class CEventTest {
public:
//	typedef ISource<unsigned int>* uintsource;
//	typedef ISource<float>* floatsource;
	typedef unsigned int uintsource;
	typedef float floatsource;

	CEventTest();
	~CEventTest();

	void setMessageSize(uintsource messageSize);
	void setFps(uintsource fps);
	void setNumBroadcasters(uintsource numBroadcasters);
	void setNumPreListeners(uintsource numPreListeners);
	void setNumHandlers(uintsource numHandlers);
	void setNumPostListeners(uintsource numPostListeners);
	void setNumFilters(uintsource numFilters);
	void setMessageFrequency(floatsource messageFrequency);
	void setDuration(float duration);
	unsigned int getMessageSize();
	unsigned int getFps();
	unsigned int getNumBroadcasters();
	unsigned int getNumPreListeners();
	unsigned int getNumHandlers();
	unsigned int getNumPostListeners();
	unsigned int getNumFilters();
	float getMessageFrequency();
	double getResultFrameTime();
	uint getResultFps();
	double getResultMessageDelay();
	double getResultMessageTime();
	uint getResultTotalMessageCount();
	double getResultTotalMessageTime();
	double getResultMessageFrameTime();
	uint getResultNumMessageFrames();

	void run();
	void reset();
	void clear();

	void setDebug(bool debug) { m_debug = debug; }

	static void init_script_class(const char* typeName);
private:
	uintsource		m_messageSize;
	uintsource		m_fps;
	uintsource		m_numBroadcasters;
	uintsource		m_numPreListeners;
	uintsource		m_numHandlers;
	uintsource		m_numPostListeners;
	uintsource		m_numFilters;
	floatsource		m_messageFrequency;
	float			m_duration;
	bool			m_debug;
	double			m_resultFrameTime;
	uint			m_resultFps;
	double			m_resultMessageDelay;
	double			m_resultMessageTime;
	uint			m_resultTotalMessageCount;
	double			m_resultTotalMessageTime;
	double			m_resultMessageFrameTime;
	uint			m_resultNumMessageFrames;

	std::vector<CBroadcaster*> m_broadcasters;
	std::vector<CListener*> m_handlers;
};

#endif // __CEVENT_TEST_H__
