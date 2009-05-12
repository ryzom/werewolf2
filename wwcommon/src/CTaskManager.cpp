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
#include <nel/misc/command.h>

//
// Werewolf Includes
//
#include "wwcommon/CTaskManager.h"
//#include "main.h"


//
// Namespaces
//
using namespace std;
using namespace NLMISC;

//
// Variables
//

//
// Functions
//

namespace WWCOMMON {

void CTaskManager::execute() {
	
	m_Benchmark=false;
	m_Exit = false;
	CHTimer::startBench();
	CHTimer::endBench();

	while(!m_Exit) {

		if(m_Benchmark && !CHTimer::benching())
			CHTimer::startBench(false, false, false);

		// new frame, updating.
		for(std::list<ITask*>::iterator it = m_OrderSortedTasks.begin(); it != m_OrderSortedTasks.end(); it++) {
			ITask *task = *it;
			if((*it)->m_Execute) {
				(*it)->m_HTimerUpdate.before();
				(*it)->update();
				(*it)->m_HTimerUpdate.after();
			}
		}

		// new frame, rendering.
		for(std::list<ITask*>::iterator it = m_OrderSortedTasks.begin(); it != m_OrderSortedTasks.end(); it++) {
			if((*it)->m_Execute) {
				(*it)->m_HTimerRender.before();
				(*it)->render();
				(*it)->m_HTimerRender.after();
			}
		}
		// end frame.

		// remove extractable tasks from m_Tasks
		for(std::list<ITask*>::iterator it2 = m_Tasks.begin(); it2 != m_Tasks.end(); ) {
			if((*it2)->m_Extract) {
				remove(*(*it2));
				it2 = m_Tasks.begin();
			} else {
				it2++;
			}
		}
	}

	// release tasks.
	for(std::list<ITask*>::reverse_iterator it = m_Tasks.rbegin(); it != m_Tasks.rend(); it++) {
		(*it)->release();
	}

	CHTimer::endBench();
}

void CTaskManager::remove(ITask &task) {
	nlinfo("Removing task %s", task.name().c_str());

	// release and remove task from m_Tasks
	for(std::list<ITask*>::iterator it = m_Tasks.begin(); it != m_Tasks.end();) {
		if(&task == (*it)) {
			std::list<ITask*>::iterator itr = it;
			it++;
			m_Tasks.remove(*itr);
		} else {
			it++;
		}
	}

	// remove task from m_OrderSortedTasks
	for(std::list<ITask*>::iterator it = m_OrderSortedTasks.begin(); it != m_OrderSortedTasks.end();)
	{
		if(&task == (*it)) {
			std::list<ITask*>::iterator itr = it;
			it++;
			m_OrderSortedTasks.remove(*itr);
		} else {
			it++;
		}
	}
	nlinfo("Removed task %s from list", task.name().c_str());
	task.release();
	nlinfo("Removed task %s and release called", task.name().c_str());
}

void CTaskManager::add(ITask &task, sint32 order, bool startNow)
{
	task.m_Order = order;

	std::string n = task.name() + "Update";
	strcpy(task.m_NameUpdate, n.c_str());
	n = task.name() + "Render";
	strcpy(task.m_NameRender, n.c_str());
	task.m_HTimerUpdate.setName(task.m_NameUpdate);
	task.m_HTimerRender.setName(task.m_NameRender);

	task.m_Extract = false;
	task.m_Execute = startNow;

	m_Tasks.push_back(&task);

	if(order == -1) {
		m_OrderSortedTasks.push_back(&task);
	}	else {
		std::list<ITask*>::iterator it;
		for(it = m_OrderSortedTasks.begin(); it != m_OrderSortedTasks.end(); it++) {
			if((*it)->m_Order > order) {
				break;
			}
		}
		m_OrderSortedTasks.insert(it, &task);
	}

	nlinfo("Init during exec %s", task.name().c_str());
	task.init();
	nlinfo("Init during exec %s complete", task.name().c_str());
}

CTaskManager::CTaskManager() {
	;
}

void CTaskManager::benchmark(bool onoff) {
	m_Benchmark=onoff;
	if(m_Benchmark)
		nlinfo("Starting benchmark...");
	else {
		nlinfo("End of benchmark");
		CHTimer::display();
		CHTimer::displayHierarchicalByExecutionPathSorted(InfoLog, CHTimer::TotalTime, true, 64);
	}		
}

bool CTaskManager::benchmark() {
	return m_Benchmark;
}

void CTaskManager::exit() {
	m_Exit = true;
}

}; // END OF NAMESPACE WWCOMMON

//
// Commands
//

/*
NLMISC_COMMAND(displayTasks, "display all task", "")
{
	if(args.size() != 0) return false;

	log.displayNL("There's %d tasks:", WWCOMMON::CTaskManager::instance().m_Tasks.size());
	log.displayNL("Init order call:");
	for(std::list<ITask*>::iterator it = WWCOMMON::CTaskManager::instance().m_Tasks.begin(); it != WWCOMMON::CTaskManager::instance().m_Tasks.end(); it++)
	{
		log.displayNL("  %s %s", (*it)->name().c_str(), ((*it)->m_Execute?"Running":"Stop"));
	}
	log.displayNL("Update order call:");
	for(std::list<ITask*>::iterator it = WWCOMMON::CTaskManager::instance().m_OrderSortedTasks.begin(); it != WWCOMMON::CTaskManager::instance().m_OrderSortedTasks.end(); it++)
	{
		log.displayNL("  %s", (*it)->name().c_str());
		(*it)->render();
	}
	log.displayNL("Release order call:");
	for(std::list<ITask*>::reverse_iterator it = WWCOMMON::CTaskManager::instance().m_Tasks.rbegin(); it != WWCOMMON::CTaskManager::instance().m_Tasks.rend(); it++)
	{
		log.displayNL("  %s", (*it)->name().c_str());
		(*it)->release();
	}

	return true;
} */


