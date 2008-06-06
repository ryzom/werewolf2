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

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//	

//
// Namespaces
//

/**
 * \brief Shard information.
 */
struct CShard {
	CShard() { ShardNbPlayers = 0; }
	CShard(const std::string &name, uint8 nbp, uint32 sid) {
		ShardName=name;
		ShardNbPlayers=nbp;
		ShardId=sid;
	}
	std::string ShardName;
	uint8		ShardNbPlayers;
	uint32		ShardId;
};

