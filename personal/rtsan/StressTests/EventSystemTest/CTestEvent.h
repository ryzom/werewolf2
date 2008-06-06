/**
 * \file CTestEvent.h
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

#ifndef __CTESTEVENT_H__
#define __CTESTEVENT_H__

#include <wwcommon/IGameEvent.h>
#include <nel/misc/time_nl.h>

class CTestEvent : public WWCOMMON::IGameEvent {
public:
	static const uint16 CTestEventID = IGameEventID + 100;
	virtual std::string	getClassName() {return "EVTT";}
	static	NLMISC::IClassable *creator() {return new CTestEvent();}

	CTestEvent();
	virtual ~CTestEvent();

	virtual void serial(NLMISC::IStream &f);

	uint getRealEventSize();
	uint getEventClassSize();

	void createContent(uint size);

//~ Instance fields ****************************************
	uint8* content;
	uint contentSize;
	double sendTime;

protected:
	/// Register this object with the class registry.
	WWCOMMON_SETUP_EVENT(CTestEvent);
};

#endif // __CTESTEVENT_H__
