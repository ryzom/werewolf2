/**
 * \file ITask.h
 * \date May 2004
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

#ifndef __ITASK_H__
#define __ITASK_H__

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/hierarchical_timer.h>

//
// Werewolf Includes
//

//
// Class
//

namespace WWCOMMON {


/**
 * \interface ITask ITask.h "interfaces/ITask.h"
 * \brief Task Interface Class.
 *
 * This is the interface used when designing new tasks that are manageable 
 * by the CTaskManager class.
 */
class ITask {
public:
	/**
	 * \brief ITask Constructor.
	 *
	 * Sets m_Extract and m_Execute values and preps the task to be started.
	 * @see m_Extract
	 * @see m_Execute
	 */
	ITask() {
		m_Extract = false;
		m_Execute = true;
	}

	/**
	 * \brief Pure virtual init method.
	 *
	 * This pure virtual method is to be implemented by the specific task.
	 */
	virtual void init() = 0;
	/**
	 * \brief Pure virtual update method.
	 *
	 * This pure virtual method is to be implemented by the specific task.
	 */
	virtual void update() = 0;
	/**
	 * \brief Pure virtual render method.
	 *
	 * This pure virtual method is to be implemented by the specific task.
	 */	
	virtual void render() = 0;
	/**
	 * \brief Pure virtual release method.
	 *
	 * This pure virtual method is to be implemented by the specific task.
	 */
	virtual void release() = 0;
	/**
	 * \brief Pure virtual name method.
	 *
	 * This pure virtual method is to be implemented by the specific task.
	 */
	virtual std::string name() = 0;

	/**
	 * \brief Stops the task.
	 *
	 * This method is called when something needs the implementing task to
	 * stop processing. It sets m_Execute to false by default.
	 * @see m_Execute
	 */
	virtual void stop()	{
		nlinfo("Task %s is now stopped", name().c_str());
		m_Execute = false;
	}

	/**
	 * \brief Restart the task.
	 *
	 * This method is called when something needs the implementing task to
	 * restart processing. By default it only sets m_Execute to true.
	 * @see m_Execute
	 */
	virtual void restart()	{
		nlinfo("Task %s is restarted", name().c_str());
		m_Execute = true;
	}

	/**
	 * \brief Remove the task.
	 *
	 * This method is called when something needs the implementing task to
	 * to be removed from the task manager. By default it sets m_Extract to
	 * true and m_Execute to false.
	 * @see m_Extract
	 * @see m_Execute
	 */
	virtual void remove()	{
		nlinfo("Task %s need to be remove", name().c_str());
		m_Extract = true;
		m_Execute = false;
	}

private:
	/**
	 * \brief Task priority.
	 *
	 * This member defines the task's priority in the CTaskManager queue.
	 */
	sint32 m_Order;

	/**
	 * \brief TODO Doc This.
	 *
	 * No documentation on this private member yet.
	 */
	NLMISC::CHTimer	m_HTimerUpdate;
	/**
	 * \brief TODO Doc This.
	 *
	 * No documentation on this private member yet.
	 */
	NLMISC::CHTimer	m_HTimerRender;
	/**
	 * \brief TODO Doc This.
	 *
	 * No documentation on this private member yet.
	 */
	char m_NameUpdate[256];
	/**
	 * \brief TODO Doc This.
	 *
	 * No documentation on this private member yet.
	 */
	char m_NameRender[256];

	bool m_Extract; /**< Set to true when CTaskManager is to extract it from service. */
	bool m_Execute; /**< Set to true when running, false when stopping. */

	friend class CTaskManager;
	friend struct commands_displayTasksClass;
};

}; // END OF NAMESPACE WWCOMMON

#endif // end __ITASK_H__
