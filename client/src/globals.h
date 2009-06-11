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

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

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
#ifdef HAVE_CONFIG_H
#include	"config.h"
#endif // HAVE_CONFIG_H

//
// Class
//

//
// Global Variables
//
static const float GScale = 0.01f;
extern bool FollowEntity;
//extern std::string Cookie;
//extern std::string FSAddr;

//
// Global Defines
//
#define WW_RELEASE_VERSION_NUMBER	"0.1"
#define WW_RELEASE_VERSION_NAME		"Gunnolf"

#endif // __GLOBALS_H__
