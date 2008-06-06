/**
* \file CEventTest.cpp
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
#include "CEventTest.h"
#include "CLocalSimulation.h"
#include "CTestEvent.h"
#include <boost/python.hpp>
#include <wwcommon/CGameEventServer.h>
#include <nel/misc/value_smoother.h>

CEventTest::CEventTest() :
m_messageSize(4), m_fps(30),
m_messageFrequency(100.0f), m_duration(1), m_debug(false)
{
	setNumBroadcasters(1);
	setNumHandlers(1);
	setNumPreListeners(1);
	setNumPostListeners(1);
	setNumFilters(1);
	getSimulation()->init();
	getSimulation()->updateTime();
}

CEventTest::~CEventTest() {
	clear();
}

void CEventTest::setMessageSize(uintsource messageSize) {
	m_messageSize = messageSize;
}

void CEventTest::setFps(uintsource fps) {
	m_fps = fps;
}

void CEventTest::setNumBroadcasters(uintsource numBroadcasters) {
	m_numBroadcasters = numBroadcasters;
}

void CEventTest::setNumPreListeners(uintsource numPreListeners) {
	m_numPreListeners = numPreListeners;
}

void CEventTest::setNumHandlers(uintsource numHandlers) {
	m_numHandlers = numHandlers;
}

void CEventTest::setNumPostListeners(uintsource numPostListeners) {
	m_numPostListeners = numPostListeners;
}

void CEventTest::setNumFilters(uintsource numFilters) {
	m_numFilters = numFilters;
}

void CEventTest::setMessageFrequency(floatsource messageFrequency) {
	m_messageFrequency = messageFrequency;
}

void CEventTest::setDuration(float duration) {
	m_duration = duration;
}

unsigned int CEventTest::getMessageSize() {
	return m_messageSize;
}

unsigned int CEventTest::getFps() {
	return m_fps;
}

unsigned int CEventTest::getNumBroadcasters() {
	return m_numBroadcasters;
}

unsigned int CEventTest::getNumPreListeners() {
	return m_numPreListeners;
}

unsigned int CEventTest::getNumHandlers() {
	return m_numHandlers;
}

unsigned int CEventTest::getNumPostListeners() {
	return m_numPostListeners;
}

unsigned int CEventTest::getNumFilters() {
	return m_numFilters;
}

float CEventTest::getMessageFrequency() {
	return m_messageFrequency;
}

double CEventTest::getResultFrameTime() {
	return m_resultFrameTime;
}

double CEventTest::getResultMessageDelay() {
	return m_resultMessageDelay;
}

uint CEventTest::getResultFps() {
	return m_resultFps;
}

uint CEventTest::getResultTotalMessageCount() {
	return m_resultTotalMessageCount;
}

double CEventTest::getResultTotalMessageTime() {
	return m_resultTotalMessageTime;
}

double CEventTest::getResultMessageFrameTime() {
	return m_resultMessageFrameTime;
}

double CEventTest::getResultMessageTime() {
	return m_resultMessageTime;
}

uint CEventTest::getResultNumMessageFrames() {
	return m_resultNumMessageFrames;
}

void CEventTest::clear() {
	for(uint i = 0; i < m_broadcasters.size(); ++i) {
		if(m_broadcasters[i] != NULL)
			delete m_broadcasters[i];
	}
	m_broadcasters.clear();

	for(uint i = 0; i < m_handlers.size(); ++i) {
		if(m_handlers[i] != NULL)
			delete m_handlers[i];
	}
	m_handlers.clear();
}

void CEventTest::reset() {
	clear();
	m_broadcasters.resize(getNumBroadcasters());
	for(uint i = 0; i < m_broadcasters.size(); ++i) {
		if(m_broadcasters[i] == NULL) {
			m_broadcasters[i] = new CBroadcaster();
		}
	}
	m_handlers.resize(getNumHandlers());
	for(uint i = 0; i < m_handlers.size(); ++i) {
		if(m_handlers[i] == NULL) {
			m_handlers[i] = new CListener(WWCOMMON::CGameEventServer::EVENT);
		}
	}
}
void CEventTest::run() {
	reset();
	if(m_debug) {
		printf("Running event simulation for %.2f seconds with parameters:\n", m_duration);
		printf("\tMessage size:        %d bytes\n", getMessageSize());
		CTestEvent evt;
		evt.createContent(getMessageSize());
		printf("\tReal message size:   %d bytes\n", evt.getRealEventSize());
		printf("\tEvent class size:    %d bytes\n", evt.getEventClassSize());
		printf("\tMessage frequency:   %.1f Hz\n", getMessageFrequency());
		printf("\tFPS:                 %d\n", getFps());
		printf("\t# broadcasters:      %d\n", getNumBroadcasters());
		printf("\t# listeners:         %d\n", getNumPreListeners() + getNumFilters() + getNumHandlers() + getNumPostListeners());
		printf("\t# pre-listeners:     %d\n", getNumPreListeners());
		printf("\t# filters:           %d\n", getNumFilters());
		printf("\t# handlers:          %d\n", getNumHandlers());
		printf("\t# post-listeners:    %d\n", getNumPostListeners());
	}
	getSimulation()->updateTime();
	double timeBefore = getSimulation()->time();
	double timeAfter = timeBefore;
	double period = 1.0/(double)getFps();
	double messagePeriod = 1.0/(double)getMessageFrequency();
	double frameStart, frameEnd;
	NLMISC::CValueSmootherTemplate<double> fpsCounter;
	fpsCounter.init(getFps()*2);
	NLMISC::CValueSmootherTemplate<double> messageFrameTime;
	messageFrameTime.init(getMessageFrequency()*2);
	double restOfMessageTime = 0;
	m_resultTotalMessageCount = 0;
	m_resultTotalMessageTime = 0;
	m_resultNumMessageFrames = 0;
	while((timeAfter - timeBefore) < m_duration) {
		frameStart = NLMISC::CTime::ticksToSecond(NLMISC::CTime::getPerformanceTime());

		bool messageFrame = false;
		// SIMULATION FRAME START
		double restOfMessageTime = NLMISC::CTime::ticksToSecond(NLMISC::CTime::getPerformanceTime());
		for(uint i = 0; i < m_broadcasters.size(); ++i) {
				if(m_broadcasters[i]->sendMessage(messagePeriod, getMessageSize())) {
					messageFrame = true;
					++m_resultTotalMessageCount;
				}

		}
		WWCOMMON::CGameEventServer::instance().processEventQueue();
		m_resultTotalMessageTime += NLMISC::CTime::ticksToSecond(NLMISC::CTime::getPerformanceTime()) - restOfMessageTime;
		if(messageFrame) {
			++m_resultNumMessageFrames;
		}
		getSimulation()->updateTime();
		timeAfter = getSimulation()->time();
		// SIMULATION FRAME END

		frameEnd = NLMISC::CTime::ticksToSecond(NLMISC::CTime::getPerformanceTime());
/*		if((frameEnd - frameStart) > period) {
			printf("Given FPS can not be achieved. Stopping simulation!\n");
			break;
		}
*/
		while((frameEnd - frameStart) < period) {
			frameEnd = NLMISC::CTime::ticksToSecond(NLMISC::CTime::getPerformanceTime());
		}
		fpsCounter.addValue(frameEnd - frameStart);
		if(messageFrame) {
			messageFrameTime.addValue(frameEnd - frameStart);
		}
	}
	m_resultFrameTime = fpsCounter.getSmoothValue();
	m_resultFps = (uint)std::ceil(1.0/fpsCounter.getSmoothValue());
	m_resultMessageTime = m_resultTotalMessageTime/m_resultTotalMessageCount;
	m_resultMessageFrameTime = messageFrameTime.getSmoothValue();
	double handlerDelay = 0;
	for(uint i = 0; i < m_handlers.size(); ++i) {
		handlerDelay += m_handlers[i]->getObservedDelay();
	}
	handlerDelay = handlerDelay/(double)m_handlers.size();
	m_resultMessageDelay = handlerDelay;
	if(m_debug) {
		printf("\n===========================================\n\n");
		printf("Results:\n");
		printf("\tActual FPS:          %d\n", m_resultFps);
		printf("\tHandler delay:       %G\n", handlerDelay);
		printf("\n===========================================\n");
	}
}

void CEventTest::init_script_class(const char* typeName) {
	using namespace boost::python;
	using namespace boost::python::api;
	class_<CEventTest> c(typeName, init<>());
	c.def("setMessageSize", &CEventTest::setMessageSize);
	c.def("setFps", &CEventTest::setFps);
	c.def("setNumBroadcasters", &CEventTest::setNumBroadcasters);
	c.def("setNumPreListeners", &CEventTest::setNumPreListeners);
	c.def("setNumHandlers", &CEventTest::setNumHandlers);
	c.def("setNumPostListeners", &CEventTest::setNumPostListeners);
	c.def("setNumFilters", &CEventTest::setNumFilters);
	c.def("setMessageFrequency", &CEventTest::setMessageFrequency);
	c.def("getMessageSize", &CEventTest::getMessageSize);
	c.def("getFps", &CEventTest::getFps);
	c.def("getNumBroadcasters", &CEventTest::getNumBroadcasters);
	c.def("getNumPreListeners", &CEventTest::getNumPreListeners);
	c.def("getNumHandlers", &CEventTest::getNumHandlers);
	c.def("getNumPostListeners", &CEventTest::getNumPostListeners);
	c.def("getNumFilters", &CEventTest::getNumFilters);
	c.def("getMessageFrequency", &CEventTest::getMessageFrequency);
	c.def("setDuration", &CEventTest::setDuration);
	c.def("run", &CEventTest::run);
	c.def("setDebug", &CEventTest::setDebug);
	c.def("getResultFps", &CEventTest::getResultFps);
	c.def("getResultFrameTime", &CEventTest::getResultFrameTime);
	c.def("getResultMessageDelay", &CEventTest::getResultMessageDelay);
	c.def("getResultMessageTime", &CEventTest::getResultMessageTime);
	c.def("getResultTotalMessageCount", &CEventTest::getResultTotalMessageCount);
	c.def("getResultTotalMessageTime", &CEventTest::getResultTotalMessageTime);
	c.def("getResultMessageFrameTime", &CEventTest::getResultMessageFrameTime);
	c.def("getResultNumMessageFrames", &CEventTest::getResultNumMessageFrames);
}



