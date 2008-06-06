/**
 * \file callbacks.h
 * \date December 2004
 * \author Matt Raykowski
 *
 * This file contains exports for callback functions and the callback arrays 
 * for the login service.
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

//
// System Includes
//
#include <string>

#include <math.h>
#include <stdio.h>
#include <ctype.h>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>
#include <nel/net/service.h>
#include <nel/misc/log.h>
#include <nel/misc/displayer.h>
#include <nel/misc/window_displayer.h>
#include <nel/misc/path.h>
#include <nel/net/login_cookie.h>
#include <nel/misc/md5.h>

//
// Werewolf Includes
//
#include "CLoginService.h"
#include "callbacks.h"

// needs to be after the nel includes for socket related stuff.
#include <mysql.h>

//
// Class
//

/**
 * \brief Validate the login name for length and characters.
 */
std::string validateLoginName(const std::string &login) {
	if(login.empty())
		return "Empty login name.";
	else if(login.size()>20)
		return "Login name too large.";

	// checking to see if there are any invalid characters
	for(uint i=0;i<login.size();i++) {
		if( !isprint(login[i]) || !isalnum(login[i]) && login[i] != '_' && 
			login[i] != '-' && login[i] != '.' && login[i] != '[' && login[i] != ']') {
				return NLMISC::toString("Invalid login name, character '%c' is not allowed.",login[i]);
			}
	}
	return "";
}

/**
 * \brief Compares two passwords.
 */
std::string validatePassword(const std::string &pass1, const std::string &pass2) {
	if(pass1.empty() || pass2.empty())
		return "Bad password, it is empty.";

	if(pass1.size()>32)
		return "Password too large.";

	NLMISC::CHashKeyMD5 clPass, srPass;
	if( !clPass.fromString(pass1) ) {
		nlwarning("Client Password not in MD5!");
		return "Client password not MD5 crypted.";
	}

	if( !srPass.fromString(pass1) ) {
		nlwarning("Server Password not in MD5!");
		return "Server password not crypted correctly.";
	}


	return (pass1!=pass2) ? "" : "Incorrect username or password.";
}

/**
 * \brief Generates a shard refusal message.
 */
void refuseShard(uint16 sid, const char *format, ...) {
	std::string reason;
	NLMISC_CONVERT_VARGS(reason, format, NLMISC::MaxCStringSize);
	nlwarning(reason.c_str());
	NLNET::CMessage msgout("FAILED");
	msgout.serial(reason);
	NLNET::CUnifiedNetwork::getInstance()->send(sid, msgout);
}

/*****
 * Welcome Service callbacks
 *****/

/**
 * \brief Login Service is receiving a connection from a shard.
 *
 * A Welcome Service has connected to this Login Service.
 */
void cbWSConnection(const std::string &serviceName, uint16 sid, void *arg) {
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;
	NLNET::TSockId from;
	NLNET::CCallbackNetBase *cnb = NLNET::CUnifiedNetwork::getInstance()->getNetBase(sid, from);
	const NLNET::CInetAddress &ia = cnb->hostAddress (from);
	nldebug("new potential shard: %s", ia.asString ().c_str());

	// see if this shard is authroized.
	reason = sqlQuery("select * from shards where WSAddr='"+ia.ipAddress()+"'", nbrow, row, result);
	if(!reason.empty()) return;

	// if there are no matches, boot it out, it's not ours.
	if(nbrow == 0) {
		refuseShard(sid, "Bad shard identification, the shard (WSAddr %s) is not in the database and can't be added", ia.ipAddress ().c_str ());
		return;
	}
}

/**
 * \brief Login Service lost connection with a shard.
 *
 * This Login Service couldn't contact a Welcome Service anymore.
 */
void cbWSDisconnection (const std::string &serviceName, uint16 sid, void *arg) {
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;
	NLNET::TSockId from;
	NLNET::CCallbackNetBase *cnb = NLNET::CUnifiedNetwork::getInstance()->getNetBase(sid, from);
	const NLNET::CInetAddress &ia = cnb->hostAddress (from);
	nldebug("shard disconnection: %s", ia.asString().c_str());

	reason = sqlQuery("select * from shards where internalid="+NLMISC::toString(sid), nbrow, row, result);
	if(!reason.empty()) return;

	if(nbrow != 1) {
		nlwarning("Disconnection of a shard that have %hu entries online", nbrow);
		return;
	}

	// shard disconnected
	nlinfo("ShardId %s with IP '%s' is offline!", row[0], ia.asString().c_str());
	nlinfo("*** ShardId %3s NbPlayers %3s -> %3d", row[0], row[2], 0);

	sqlQuery("update shards set online=0, internalid=0, plrcount=0 where shardid="+std::string(row[0]), nbrow, row, result);

	// put users connected on this shard offline
	sqlQuery("update users set State='Offline', shardid=-1, cookie='' where shardid="+std::string(row[0]), nbrow, row, result);
}

/**
 * \brief Verrify a new shard's identity and add it to the database.
 */
void cbWSIdentification(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid) {
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;
	NLNET::TSockId from;
	NLNET::CCallbackNetBase *cnb = NLNET::CUnifiedNetwork::getInstance ()->getNetBase (sid, from);
	const NLNET::CInetAddress &ia = cnb->hostAddress (from);

	// get the shard id and player count.
	sint32 shardId;
	msgin.serial(shardId);
	sint32 nbplayers;
	msgin.serial(nbplayers);
	nldebug("Shard identification, It says to be ShardId %d with %Ld players, let check that!", shardId, nbplayers);

	// if there are any errors or no results, refuse it.
	reason = sqlQuery("select * from shards where shardid="+NLMISC::toString(shardId), nbrow, row, result);
	if(!reason.empty()) { refuseShard(sid, reason.c_str()); return; }

	if(nbrow == 0) {
		refuseShard(sid, "Bad shard identification, The shard %d is not in the database and can't be added", shardId);
		return;
	} else if(nbrow == 1) {
		// check that the ip is ok
		NLNET::CInetAddress iadb;
		iadb.setNameAndPort (row[1]);
		nlinfo ("check %s with %s (%s)", ia.ipAddress ().c_str(), iadb.ipAddress().c_str(), row[1]);
		if (ia.ipAddress() != iadb.ipAddress()) {
			// good shard id but from a bad computer address
			refuseShard(sid, "Bad shard identification, ShardId %d should come from '%s' and come from '%s'", shardId, row[1], ia.ipAddress ().c_str());
			return;
		}

		if(row[4] == "1") {
			// the shard is already online, disconnect the old one and set the new one
			refuseShard(atoi(row[7]), "A new shard connects with the same IP/ShardId, you were replaced");
		} else {
			// we're going to need a new hook to the db.
			CLoginService *loginService=(CLoginService *)gServiceInstance;
			MYSQL *db=loginService->getDatabase();

			std::string query = "update shards set online=1 where shardid="+NLMISC::toString(shardId);
			sint ret = mysql_query(db, query.c_str());
			if (ret != 0) {
				refuseShard (sid, "mysql_query (%s) failed: %s", query.c_str(),  mysql_error(db));
				return;
			}
		}

		MYSQL_RES *result2;
		MYSQL_ROW row2;
		sint32 nbrow2;
		reason = sqlQuery("update shards set online=1, plrcount="+NLMISC::toString(nbplayers)+", internalid="+NLMISC::toString(sid)+" where shardid="+NLMISC::toString(shardId), nbrow2, row2, result2);
		if(!reason.empty()) { refuseShard(sid, reason.c_str()); return; }

		// ok, the shard is identified correctly
		nlinfo("ShardId %d name '%s' wsaddr '%s' with ip '%s' is online!", shardId, row[3], row[1], ia.ipAddress().c_str());

		for(sint32 i = 0; i < nbplayers; i++) {
			sint32 uid;
			msgin.serial(uid);
			sqlQuery("update users set State='Online', shardid="+NLMISC::toString(shardId)+", cookie='' where userid="+NLMISC::toString(uid), nbrow2, row2, result2);
		}

		return;
	} else {
		refuseShard(sid, "mysql problem, There's more than 1 shard with the shardId %d in the database", shardId);
		return;
	}
	
	nlstop;
}

/**
 * \brief Recieve a SCS message from the Welcome Service.
 *
 * S10: receive "SCS" message from WS
 */
void cbWSShardChooseShard(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid) {
	std::string reason, cookie;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;

	// Get the callback server so we can message the client.
	CLoginService *loginService=(CLoginService *)gServiceInstance;
	NLNET::CCallbackNetBase *cnb = loginService->getClientServer();

	// input the message and start creating the message to relay to the client.
	NLNET::CMessage msgout("SCS");
	try {
		msgin.serial(reason);
	} catch(NLMISC::Exception &e) {
		nlwarning("WS \"SCS\" call failed to provide 'reason': %s", e.what());
		reason="";
	}

	try {
		msgin.serial(cookie);
	} catch(NLMISC::Exception &e) {
		nlwarning("WS \"SCS\" call failed to provide 'cookie': %s", e.what());
	}
	msgout.serial(reason);
	

	// if there was no reason, welcome service accepted the client.
	if (reason.empty()) {
		std::string addr;
		msgout.serial(cookie);	// relay the cookie.
		try {
			msgin.serial(addr);
			msgout.serial(addr);	// and the address of the new FES
		} catch(NLMISC::Exception &e) {
			nlerror("Unable to parse SCS message: %s", e.what());
		}
		
	}

	
	reason = sqlQuery("select * from users where cookie='"+cookie+"'", nbrow, row, result);
	if(!reason.empty()) return;
	if(nbrow != 1) {
		nlwarning("cookie %s is not associated to anybody", cookie.c_str());
		return;
	}

	NLNET::CLoginCookie lc;
	lc.setFromString(cookie);
	cnb->send(msgout, (NLNET::TSockId)lc.getUserAddr());
}

/**
 * \brief A client disconnection or connection notice was sent by a WS.
 *
 * S16: Receive "CC" message from WS
 */
void cbWSClientConnected(NLNET::CMessage &msgin, const std::string &serviceName, uint16 sid) {
	// a WS tells me that a player is connected or disconnected
	uint32 Id;
	uint8 con;
	msgin.serial(Id);
	msgin.serial(con);	// con=1 means a client is connected on the shard, 0 means a client disconnected

	if(con)
		nlinfo("Received a validation that client %u is connected on the frontend", Id);
	else
		nlinfo("Received a validation that client %u is disconnected on the frontend", Id);
	
	std::string reason;
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;

	reason = sqlQuery("select * from users where userid="+NLMISC::toString(Id), nbrow, row, result);
	if(!reason.empty()) return;

	if(nbrow == 0) {
		nlwarning ("Id %d doesn't exist", Id);
		gOutput->displayNL("###: %3d UId doesn't exist", Id);
		return;
	} else if(nbrow > 1) {
		nlerror("Id %d have more than one entry!!!", Id);
		return;
	}

	// row[4] = State
	if(con == 1 && std::string(row[4]) != std::string("Waiting")) {
		nlwarning("Id %d is not waiting but is %s", Id, row[4]);
		gOutput->displayNL("###: %3d User isn't waiting, his state is '%s'", Id, row[4]);
		return;
	} else if(con == 0 && std::string(row[4]) != std::string("Online")) {
		nlwarning("Id %d wasn't connected on a shard but is %s", Id, row[4]);
		gOutput->displayNL("###: %3d User wasn't connected on a shard, his state is '%s'", Id, row[4]);
		return;
	}

	MYSQL_RES *result2;
	MYSQL_ROW row2;
	sint32 nbrow2;

	if (con == 1) {
		// new client on the shard
		reason = sqlQuery("update users set state='Online', cookie='' where userid="+NLMISC::toString(Id), nbrow2, row2, result2);
		if(!reason.empty()) return;

		reason = sqlQuery("update shards set plrcount=plrcount+1 where shardid="+std::string(row[3]), nbrow2, row2, result2);
		if(!reason.empty()) return;

		nldebug("Id %d is connected on the shard", Id);
		gOutput->displayNL("###: %3d User connected to the shard (%s)", Id, row[3]);
	} else {
		// client removed from the shard (true is for potential other client with the same id that wait for a connection)

		reason = sqlQuery("update users set state='Offline', shardid=-1, cookie='' where userid="+NLMISC::toString(Id), nbrow2, row2, result2);
		if(!reason.empty()) return;

		reason = sqlQuery("update shards set plrcount=plrcount-1 where shardid="+std::string(row[3]), nbrow2, row2, result2);
		if(!reason.empty()) return;
			
		nldebug("Id %d is disconnected from the shard", Id);
		gOutput->displayNL("###: %3d User disconnected from the shard (%s)", Id, row[3]);
	}
}

/*****
 * Client Connection callbacks
 *****/

/**
 * \brief Accept a new connection and permit it to only verify login and pass.
 */
void cbClientConnection(NLNET::TSockId from, void *arg) {
	CLoginService *loginService=(CLoginService *)gServiceInstance;
	NLNET::CCallbackNetBase *cnb = loginService->getClientServer();

	const NLNET::CInetAddress &ia = cnb->hostAddress(from);
	nldebug("Client connection: %s", ia.asString ().c_str());
	gOutput->displayNL("CCC: Connection from %s", ia.asString ().c_str());
	cnb->authorizeOnly("VLP", from);
}

/**
 * \brief Disconnects users and updates the database.
 */
void cbClientDisconnection(NLNET::TSockId from, void *arg) {
	CLoginService *loginService=(CLoginService *)gServiceInstance;
	NLNET::CCallbackNetBase *cnb = loginService->getClientServer();

	const NLNET::CInetAddress &ia = cnb->hostAddress (from);
	nldebug("new client disconnection: %s", ia.asString ().c_str ());

	std::string reason;	
	MYSQL_RES *result;
	MYSQL_ROW row;
	sint32 nbrow;

	// make sure there are users to disconnect.
	reason = sqlQuery("select * from users where state!='Offline'", nbrow, row, result);
	if(!reason.empty()) return;
		
	if(nbrow == 0) {
		return;
	}
	
	while(row != 0)	{
		NLNET::CLoginCookie lc;
		std::string str = row[6];	// cookie
		if(!str.empty()) {
			lc.setFromString(str);
			if(lc.getUserAddr() == (uint32)from) {
				// got it, if he is not in waiting state, it s not normal, remove all
				if(row[4] != std::string("Waiting"))
					sqlQuery("update users set state='Offline', shardid=-1, cookie='' where userid="+std::string(row[0]), nbrow, row, result);
				return;
			}
		}
		row = mysql_fetch_row(result);
	}
}

/**
 * \brief Called by the client to verify their login and password.
 *
 * S03: check the validity of the client login/password and send "VLP" message to client
 */
void cbClientVerifyLoginPassword(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	// reason is empty if everything goes right or contains the reason of the failure
	std::string reason="";
	sint32 uid = -1;
	std::string login, password;//, cpassword;
	msgin.serial (login);
	msgin.serial (password);

	breakable {
		MYSQL_RES *result;
		MYSQL_ROW row;
		sint32 nbrow;
		std::string OS, Proc, Mem, Gfx;

		/*
		 * validate the login name provided.
		 */
		reason = validateLoginName(login);
		if(!reason.empty()) break;
		
		reason = sqlQuery("select * from users where username='"+login+"'", nbrow, row, result);
		if(!reason.empty()) break;

		if(nbrow == 0) {
			reason = NLMISC::toString("Login '%s' doesn't exist and can't be added", login.c_str());
			break;
		}

		// save the userid
		uid = atoi(row[0]);

		// log the address.
		const NLNET::CInetAddress &ia = netbase.hostAddress(from);
		gOutput->displayNL("***: %3d Login '%s' Ip '%s'", uid, login.c_str(), ia.asString().c_str());

		// get some more info out of the message.
		msgin.serial (OS);
		msgin.serial (Proc);
		msgin.serial (Mem);
		msgin.serial (Gfx);
		
		// and log it for records.
		if(!OS.empty()) gOutput->displayNL("OS : %3d %s", uid, OS.c_str());
		if(!Proc.empty()) gOutput->displayNL("PRC: %3d %s", uid, Proc.c_str());
		if(!Mem.empty()) gOutput->displayNL("MEM: %3d %s", uid, Mem.c_str());
		if(!Gfx.empty()) gOutput->displayNL("GFX: %3d %s", uid, Gfx.c_str());

		reason = validatePassword(password, row[2]);
		if(!reason.empty()) break;

		NLNET::CLoginCookie c;
		c.set((uint32)from, rand(), uid);

		// turn this user on.
		reason = sqlQuery("update users set State='Authorized', cookie='"+c.setToString()+"' where userid="+NLMISC::toString(uid), nbrow, row, result);
		if(!reason.empty()) break;

		// and grab a list of online shards.
		reason = sqlQuery("select * from shards where online=1", nbrow, row, result);
		if(!reason.empty()) break;

		// Send success message
		gOutput->displayNL("---: %3d Ok", uid);
		NLNET::CMessage msgout("VLP");
		msgout.serial(reason);
		msgout.serial(nbrow);	// the number of shards being sent.
		
		// send address and name of all online shards
		while(row != 0) {
			// serial the name of the shard
			std::string shardname = row[3];
			uint8 nbplayers = atoi(row[2]);
			uint32 sid = atoi(row[0]);
			msgout.serial(shardname, nbplayers, sid);
			row = mysql_fetch_row(result);
		}

		/*
		 * send the message to the client and authorize them
		 * to choose a shard.
		 */
		netbase.send (msgout, from);
		netbase.authorizeOnly("CS", from);

		return;
	}

	// Manage error
	gOutput->displayNL("---: %3d %s", uid, reason.c_str());
	NLNET::CMessage msgout("VLP");
	msgout.serial(reason);
	netbase.send(msgout, from);
	// FIX: On GNU/Linux, when we disconnect now, sometime the other side doesn't receive the message sent just before.
	//      So it is the other side to disconnect
	//		netbase.disconnect (from);
}

/**
 * \brief Process a CS message received from the client.
 *
 * S06: receive "CS" message from client
 */
void cbClientChooseShard(NLNET::CMessage &msgin, NLNET::TSockId from, NLNET::CCallbackNetBase &netbase) {
	std::string reason;

	breakable {
		MYSQL_RES *result;
		MYSQL_ROW row;
		sint32 nbrow;

		// Get a list of all authorized users waiting for a shard.
		reason = sqlQuery("select * from users where State='Authorized'", nbrow, row, result);
		if(!reason.empty()) break;
		if(nbrow == 0) {
			reason = "You are not authorized to select a shard";
			break;
		}

		// Go through the list and find the user that this connection is for.
		bool ok = false;
		while(row != 0) {
			NLNET::CLoginCookie lc;
			lc.setFromString(row[6]);
			if(lc.getUserAddr() == (uint32)from) {
				ok = true;
				break;
			}
			row = mysql_fetch_row(result);
		}
	
		// If no user was found that was authorized, reject the user.
		if(!ok) {
			reason = "You are not authorized to select a shard";
			break;
		}

		// it is ok, so we find the wanted shard
		uint32 shardid;
		msgin.serial(shardid);

		MYSQL_RES *result2;
		MYSQL_ROW row2;
		sint32 nbrow2;
		reason = sqlQuery("select * from shards where shardid="+NLMISC::toString(shardid), nbrow2, row2, result2);
		if(!reason.empty()) break;

		// if there were no results, the shard doesn't exist.
		if(nbrow2 == 0) {
			reason = "This shard is not available";
			break;
		}

		uint16 sid = atoi(row2[7]);
		reason = sqlQuery("update users set State='Waiting', shardid="+NLMISC::toString(shardid)+" where userid="+row[0], nbrow2, row2, result2);
		if(!reason.empty()) break;
		
		// werewolf doesn't have a score.
		std::string cookie = row[6];
		std::string name = row[1];
		NLNET::CMessage msgout("CS");
		msgout.serial(cookie);
		msgout.serial(name);

		nlinfo("Sending cookie and user: %s, %s",name.c_str(), cookie.c_str());
		// Send CS to the WS.
		NLNET::CUnifiedNetwork::getInstance()->send(sid, msgout);

		return;
	}

	// Send an error to the client using SCS.
	NLNET::CMessage msgout("SCS");
	msgout.serial(reason);
	netbase.send(msgout, from);
	// FIX: On GNU/Linux, when we disconnect now, sometime the other side doesn't receive the message sent just before.
	//      So it's the other side to disconnect
	//			netbase.disconnect (from);
}
