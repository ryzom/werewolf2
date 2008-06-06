/**
 * \file CTimeTask.h
 * \date May 2004
 * \author Matt Raykowski
 * \author Some code adapted from MTP-Target.
 * \todo In CTimeTask::init() find  a cross-platform alternative to getPerformanceTime()
 * \todo In CTimeTask::update() find  a cross-platform alternative to getPerformanceTime()
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

#ifndef __CTIMETASK_H__
#define __CTIMETASK_H__

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/value_smoother.h>

//
// Werewolf Includes
//
#include "ITask.h"

//
// Class
//

/**
 * \class CTimeTask CTimeTask.h "tasks/CTimeTask.h"
 * \brief Time tracking class.
 *
 * This task tracks the current time, time shifts between frames. It
 * currently uses getPerformanceTime() and performs time smoothing.
 */
class CTimeTask : public ISingleton<CTimeTask>, public ITask {
public:
	/**
	 * \brief Initializes all time variables to defaults.
	 *
	 * Upon initialization it sets all of the internal time tracking
	 * variables to the current time in processor ticks.
	 */
	virtual void init();

	/**
	 * \brief Updates the time system.
	 *
	 * Updates the internal time tracking variables to determine the time that
	 * has passed since the last cycle. This method also uses getPerformanceTime()
	 */
	virtual void update();

	virtual void render();	/**< This method, from ITask, does nothing. */
	virtual void release();	/**< This method, from ITask, does nothing. */

	/**
	 * \brief Returns the name of this service.
	 *
	 * @return std::string containing the name of this service, "CTimeTask"
	 */
	virtual std::string name();
	
	/**
	 * \brief The current time, in seconds.
	 *
	 * @return double containing the current time in seconds.
	 */
	double		time() const;

	/**
	 * \brief The time between this frame and the last frame.
	 *
	 * @return double containing the time difference in seconds.
	 */
	double		deltaTime () const;
	
	/**
	 * \brief The average number of frame renders in a second.
	 *
	 * @return deuble containing the average frames per second.
	 */
	double		fps() const;

	/**
	 * \brief The number of seconds to render a frame.
	 *
	 * @return double containing the seconds per frame.
	 */
	double		spf() const;

private:

	NLMISC::CValueSmootherTemplate<double> m_DeltaTimeSmooth;

	double m_Time, m_DeltaTime, m_OldTime;
	double m_FirstTime;
};

#endif // __CTIMETASK_H__
