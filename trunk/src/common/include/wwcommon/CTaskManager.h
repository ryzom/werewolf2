/**
 * \file CTaskManager.h
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

#ifndef __CTASKMANAGER_H__
#define __CTASKMANAGER_H__

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "ISingleton.h"
#include "ITask.h"

//
// Class
//
//struct commands_displayTasksClass;

namespace WWCOMMON {

/**
 * \brief The Task Manager Class.
 *
 * This class handles calling of tasks based on priority. It is the core runtime loop.
 * \todo Add cross platform project options for turning off benchmarking timers.
 */
class CTaskManager : public ISingleton<CTaskManager>
{
public:

	/**
	 * \brief The main event loop.
	 *
   * As long as m_Exit is false, each iteration of the loop four 
	 * internal loops are processed:
	 *	- OrderSortedTasks searched for m_Execute tasks and updated.
	 *		-# Task m_HTimerUpdate.before()
	 *		-# Task update()
	 *		-# Task m_HTimerUpdate.before()
	 *	- OrderSortedTasks searched for m_Execute tasks and updated.
	 *		-# Task m_HTimerRender.before()
	 *		-# Task render()
	 *		-# Task HTimerRender.after()
	 *	- Tasks searched for m_Extract tasks and removed from the list.
	 *	- Tasks iterated and released()
	 *
	 * @see remove()
	 * @see m_Tasks
	 * @see m_OrderSortedTasks
	 * @see m_Exit
	 */
	void execute();

	/**
	 * \brief Adds a task into the task manager.
	 *
	 * When called this sets the appropriate names on the tasks members, flags the
	 * task as not extractable and toggles execution. Once the task has been prepared 
	 * it is pushed to the back of the m_Tasks list. If the \a order is called with -1
	 * the task is immediately inserted into the bottom of the ordered list. Otherwise
	 * the list is iterated until a task with a higher priority is found and then it is
	 * inserted. Finally the task's init() method is called.
	 * \param task Any object ITask object to be added to the loop.
	 * \param order The order of execution, if none specified -1 is assumed.
	 * \param startNow Initialize immediately or on first polling. Defaults to true.
	 */
	void add(ITask &task, sint32 order = -1, bool startNow = true);

	/**
	 * \brief Removes a task from the task manager.
	 *
	 * This method will remove task from m_Tasks and m_OrderSortedTasks and 
	 * call it's release method.
	 *
	 * \param task A reference to an ITask object.
	 */
	void remove(ITask &task);

	/**
	 * \brief Flags the task manager to exit the game.
	 *
	 * This method is called to stop the task manager from processing
	 * and exiting the event loop. When called it sets m_Exit to true.
	 * \see m_Exit
	 */
	void exit();

	/**
	 * \brief The Task Manager Constructor.
	 *
	 * This presently does nothing.
	 */
	CTaskManager();

	/**
	 * \brief Turn benchmarking on and off.
	 */
	void benchmark(bool onoff);

	/**
	 * \brief Get the status of benchmarking.
	 */
	bool benchmark();

private:

	/**
	 * \brief List of all tasks.
	 *
	 * This is a standard list of all tasks registered with
	 * CTaskManager. It is un-ordered by the task priority.
	 */
	std::list<ITask*>	m_Tasks;
	/**
	 * \brief List of ordered tasks.
	 *
	 * This is a standard list of all tasks registered with
	 * the CTaskManager and ordered by the task priority.
	 */
	std::list<ITask*>	m_OrderSortedTasks;

	/**
	 * \brief Flagged true to exit.
	 *
	 * When set to true execute() will stop processing and end the game.
	 */
	bool m_Exit;

	/**
	 * \brief Benchmarking flag.
	 *
	 * Determines whether or not benchmarking information is displayed.
	 */
	bool m_Benchmark;

	//friend struct commands_displayTasksClass;
};

}; // END OF NAMESPACE WWCOMMON

#endif
