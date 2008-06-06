/*
 * This file contain the Component Prototype Frontend Service.
 */

/*
 * Copyright, 2007 Matt Raykowski
 *
 * This file is part of NEVRAX SNOWBALLS.
 * NEVRAX SNOWBALLS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * NEVRAX SNOWBALLS is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with NEVRAX SNOWBALLS; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

// This include is mandatory to use NeL. It include NeL types.
#include <nel/misc/types_nl.h>
#include <nel/misc/vector.h>

#include <utility>

#include "frontend_service.h"

/**
 * Contains all callbacks from client
 */
NLNET::TCallbackItem ClientCallbackArray[] =
{
//        { "ADD_ENTITY",    cbAddClient      },
//        { "ENTITY_POS",    cbPosClient      },
//        { "CHAT",          cbChatClient     },
//        { "REMOVE_ENTITY", cbRemoveClient   },
//        { "SNOWBALL",      cbSnowballClient },
};


/**
 * Contains *all* callbacks from the shard
 */
NLNET::TUnifiedCallbackItem CallbackArray[] =
{
//        { "CHAT",                       cbChatService           },
//        { "ADD_ENTITY",         cbAddService            },
//        { "ENTITY_POS",         cbPosService            },
//        { "REMOVE_ENTITY",      cbRemoveService         },
//        { "SNOWBALL",           cbSnowballService       },
//        { "HIT",                        cbHitService            },
};

// Initialisation
void CFrontendService::init() {
	// Create the server where the client must connect into
	// In a real game, it should be an UDP server with specific protocol to manage packet lost and so on.
	m_Clients = new NLNET::CCallbackServer();
	nlassert (m_Clients != 0);

	// Set the callbacks for that connection (comming from the Chat service)
	m_Clients->addCallbackArray (ClientCallbackArray, sizeof(ClientCallbackArray)/sizeof(ClientCallbackArray[0]));

	// Set the callbacks for the client disconnection of the Frontend
	m_Clients->setDisconnectionCallback(onDisconnectClient, 0);
	
	// Set the callbacks for the client connection to the frontend.
	m_Clients->setConnectionCallback(onConnectionClient, 0);

	// Set listening port and initialize.
	m_Clients->init (37000);

	// Connect the frontend to the login system
	//CLoginServer::init( *Clients, onConnectionClient);

	/*
	 * Set the callback function when the Chat service reconnect to the
	 * frontend
	 */
	//CUnifiedNetwork::getInstance ()->setServiceUpCallback ("CHAT", onReconnectChat, 0);

	/*
	 * Set the callback function when the Chat service disconnect from
	 * frontend
	 */
	//CUnifiedNetwork::getInstance ()->setServiceDownCallback ("CHAT", onDisconnectChat, 0);

	/*
	 * Set the callback function when the Position service reconnect to the
	 * frontend
	 */
	//CUnifiedNetwork::getInstance ()->setServiceUpCallback ("POS", onReconnectPosition, 0);

	/*
	 * Set the callback function when the Position service disconnect from
	 * frontend
	 */
	//CUnifiedNetwork::getInstance ()->setServiceDownCallback ("POS", onDisconnectPosition, 0);
}

bool CFrontendService::update ()
{
	// Manage messages from clients
	m_Clients->update ();

	// we want to continue
	return true;
}

/****************************************************************************   
 * SNOWBALLS FRONTEND SERVICE MAIN Function
 *
 * This call create a main function for a service:
 *
 *    - based on the "CFrontEndService" class
 *    - having the short name "FS"
 *    - having the long name "frontend_service"
 *    - listening on the port "0" (dynamically determined)
 *    - and shard callback set to "CallbackArray"
 *
 ****************************************************************************/
using namespace NLMISC;
using namespace NLNET;
NLNET_SERVICE_MAIN(CFrontendService, "FS", "frontend_service", 0, CallbackArray, FS_CONFIG, FS_LOGS)


/* end of file */
