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
// Standard Includes
//
#include "stdpch.h"

//
// System Includes
//
#ifdef NL_OS_WINDOWS
#	include <windows.h>
#	undef min
#	undef max
	HINSTANCE ghInstance = 0;
#endif

#include <string>

//#include "wwcommon/stdpragma.h"

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "CWWTask.h"
#include "main.h"
#include "wwcommon/CTaskManager.h"

//
// Namespaces
//
using namespace std;
using namespace NLMISC;

//
// Global Variables
//
bool DisplayDebug = false;


uint MainThreadId = 0;
uint NetworkThreadId = 0;


#ifdef NL_OS_WINDOWS
int WINAPI WinMain( HINSTANCE hInstance, 
									 HINSTANCE hPrevInstance, 
									 LPSTR lpCmdLine, 
									 int nCmdShow ) {
	ghInstance = hInstance;
#else
int main(int argc, char **argv)
{
#endif

	// record the main thread's ID for all to see.
	MainThreadId = getThreadId();
	// add the main task
	WWCOMMON::CTaskManager::instance().add(WWCLIENT::CWWTask::instance(), 70);

	// start the execution
	WWCOMMON::CTaskManager::instance().execute();

	// return
	return EXIT_SUCCESS;
}



