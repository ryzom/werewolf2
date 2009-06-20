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
#include "tasks/CNetworkTask.h"
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

//std::string FSAddr, Cookie;


#ifdef NL_OS_WINDOWS
HINSTANCE ghInstance;

int WINAPI WinMain( HINSTANCE hInstance,
									 HINSTANCE hPrevInstance,
									 LPSTR lpCmdLine,
									 int nCmdShow ) {
	ghInstance = hInstance;
#else
int main(int argc, char **argv)
{
#endif
    std::string fsaddr, cookie;
	// Retrieve the FSAddr and Cookie, if possible.
#ifdef NL_OS_WINDOWS

	// extract the 2 first param (argv[1] and argv[2]) it must be cookie and addr

	std::string cmd = lpCmdLine;
	int pos1 = cmd.find_first_not_of (' ');
	int pos2;
	if (pos1 != string::npos)
	{
		pos2 = cmd.find (' ', pos1);
		if(pos2 != string::npos)
		{
			cookie = cmd.substr (pos1, pos2-pos1);

			pos1 = cmd.find_first_not_of (' ', pos2);
			if (pos1 != string::npos)
			{
				pos2 = cmd.find (' ', pos1);
				if(pos2 == string::npos)
				{
					fsaddr = cmd.substr (pos1);
				}
				else if (pos1 != pos2)
				{
					fsaddr = cmd.substr (pos1, pos2-pos1);
				}
			}
		}
	}

#else

	if (argc>=3)
	{
		cookie = argv[1];
		fsaddr = argv[2];
	}

#endif
    nlinfo("*** Starting up Werewolf! ***");

    if(!cookie.empty() && !fsaddr.empty()) {
        // We'll need to make sure that the network task has the cookie and FS address.
        nlinfo("Received cookie: %s and frontend address: %s", cookie.c_str(), fsaddr.c_str());
        WWCLIENT::CNetworkTask::instance().setLoginCookie(cookie);
        WWCLIENT::CNetworkTask::instance().setShardIp(fsaddr);
    }

	// record the main thread's ID for all to see.
	MainThreadId = getThreadId();
	// add the main task
	WWCOMMON::CTaskManager::instance().add(WWCLIENT::CWWTask::instance(), 70);

	// start the execution
	WWCOMMON::CTaskManager::instance().execute();

	// return
	return EXIT_SUCCESS;
}



