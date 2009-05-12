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
#include <vector>

//
// NeL Includes
//
#include <nel/pacs/u_retriever_bank.h>
#include <nel/pacs/u_global_retriever.h>
#include <nel/pacs/u_move_container.h>
#include <nel/pacs/u_move_primitive.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance_group.h>
#include <nel/3d/u_visual_collision_manager.h>
#include <nel/3d/u_instance.h>
#include <nel/misc/vectord.h>

#include <3d/landscape_model.h>
#include <3d/landscape.h>
#include <3d/landscape_user.h>
//
// Werewolf Includes
//
//#include "CEntityTask.h"
#include "C3DTask.h"
#include "CConfigTask.h"
#include "CLandscapeTask.h"
#include "CPacsTask.h"
#include "CTimeTask.h"
#include "CClientSimulation.h"
#include "CActorProxy.h"
#include <wwcommon/CMotionController.h>
#include <wwcommon/CPolynomialMotionController.h>

//
// Namespaces
//
using namespace std;
using namespace NLMISC;
using namespace NL3D;
using namespace NLPACS;

namespace WWCLIENT {

void	CPacsTask::init() {
	// init the global retriever and the retriever bank
	//m_RetrieverBank = URetrieverBank::createRetrieverBank(CConfigTask::instance().configFile().getVar("RetrieverBankName").asString().c_str());
	//m_GlobalRetriever = UGlobalRetriever::createGlobalRetriever(CConfigTask::instance().configFile().getVar("GlobalRetrieverName").asString().c_str(), m_RetrieverBank);

	// create the move primitive
	//m_MoveContainer = UMoveContainer::createMoveContainer(m_GlobalRetriever, 100, 100, 6.0);

	// grab the landscape.
	//NL3D::CLandscapeUser *lu = static_cast<NL3D::CLandscapeUser*>(&CLandscapeTask::instance().getLandscape());

	// create a visual collision manager
	m_VisualCollisionManager = C3DTask::instance().scene().createVisualCollisionManager();
	m_VisualCollisionManager->setLandscape(&CLandscapeTask::instance().getLandscape());

	// TODO henri:everyone THIS IS BAD!!!
	CActorProxy* actor = ((CClientSimulation*)getSimulation())->getSelfSob();
	WWCOMMON::CMotionController* controller = new WWCOMMON::CMotionController(actor);
	controller->setVisualCollisionEntity(m_VisualCollisionManager);
	actor->addController(controller);


	// create a move primite for each instance in the instance group
	//uint	i, j;
	//for (j=0; j<CLandscapeTask::instance().getInstanceGroups().size(); ++j) {
	//	for (i=0; i<CLandscapeTask::instance().getInstanceGroups()[j]->getNumInstance(); ++i) {
	//		// setup the radius of each mesh in the instance group
	//		std::string	name = CLandscapeTask::instance().getInstanceGroups()[j]->getShapeName(i);
	//		float rad;
//
//			// this sets the radius of the bounding cylinder based on the
//			// name of the shape. this is hacky.
//			/**
//			 * \todo Make this more dynamic.
//			 */
//			     if (strlwr(name) == "pi_po_igloo_a")		rad = 4.5f;
//			else if (strlwr(name) == "pi_po_snowman_a")		rad = 1.0f;
//			else if (strlwr(name) == "pi_po_pinetree_a")	rad = 2.0f;
//			else if (strlwr(name) == "pi_po_tree_a")		rad = 2.0f;
//			else if (strlwr(name) == "pi_po_pingoo_stat_a")	rad = 1.0f;
//			else if (strlwr(name) == "pi_po_gnu_stat_a")	rad = 1.0f;
//			else {
//				rad = 2.0f;
//				nlwarning ("Instance name '%s' doesn't have a good radius for collision", name.c_str());
//			}
//
//			NLPACS::UMovePrimitive	*primitive = m_MoveContainer->addCollisionablePrimitive(0, 1);
//			primitive->insertInWorldImage(0);
//			primitive->setPrimitiveType(UMovePrimitive::_2DOrientedCylinder);
//			primitive->setRadius(rad);
//			primitive->setHeight(6.0f);
//			primitive->setObstacle(true);
//			primitive->setReactionType(UMovePrimitive::DoNothing);
//			primitive->setTriggerType(UMovePrimitive::NotATrigger);
//			primitive->setCollisionMask(OtherCollisionBit+SelfCollisionBit+SnowballCollisionBit);
//			primitive->setOcclusionMask(StaticCollisionBit);
			
//			NLMISC::CVector	pos = CLandscapeTask::instance().getInstanceGroups()[j]->getInstancePos(i);
//			primitive->setGlobalPosition(CVectorD(pos.x, pos.y, pos.z-1.5f), 0);
//			m_InstancesMovePrimitives.push_back(primitive);
//		}
//	}
}

void CPacsTask::release() {
	// create a move primite for each instance in the instance group
//	uint i;
//	for(i=0; i<m_InstancesMovePrimitives.size(); ++i)
//		m_MoveContainer->removePrimitive(m_InstancesMovePrimitives[i]);
//
//	m_InstancesMovePrimitives.clear();

	// delete all allocated objects
//	UGlobalRetriever::deleteGlobalRetriever(m_GlobalRetriever);
//	URetrieverBank::deleteRetrieverBank(m_RetrieverBank);
//	UMoveContainer::deleteMoveContainer(m_MoveContainer);

	// delete the visual collision manager
	C3DTask::instance().scene().deleteVisualCollisionManager(m_VisualCollisionManager);
}

//UMoveContainer &CPacsTask::getMoveContainer() {
//	return *m_MoveContainer;
//}

UVisualCollisionManager &CPacsTask::getVisualCollisionManager() {
	return *m_VisualCollisionManager;
}

//NLPACS::UGlobalRetriever &CPacsTask::getGlobalRetriever() {
//	return *m_GlobalRetriever;
//}

void CPacsTask::update() {
//	m_MoveContainer->evalCollision(CTimeTask::instance().deltaTime(),0);
}

void CPacsTask::render() {
	;
}

std::string CPacsTask::name() {
	return "CPacsTask";
}

}; // END NAMESPACE WWCLIENT
