/* Copyright, 2009 Werewolf
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

#ifndef __CUSERPATH_H__
#define __CUSERPATH_H__

//
// System Includes
//
#include <string>

//
// NeL Includes
//

//
// Werewolf Includes
//

//
// Class
//

namespace WWCLIENT {

class CUserPaths {
public:
	static std::string getHomeDir();
	static std::string getConfigDir();
	static std::string getWerewolfConfigDir();
};

}; // END NAMESPACE WWCLIENT

#endif // __CUSERPATH_H__
