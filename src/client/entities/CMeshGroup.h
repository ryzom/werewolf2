/**
 * \file CMeshGroup.h
 * \date February 2006
 * \author Henri Kuuste
 */

/* Copyright, 2006 Werewolf
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

#ifndef __CMESHGROUP_H__
#define __CMESHGROUP_H__

//
// Standard Includes
//
#include <map>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/georges/u_form_loader.h>
#include <nel/georges/u_form.h>
#include <nel/georges/u_form_elm.h>
#include <nel/3d/u_instance.h>

//
// Werewolf Includes
//
#include "entities/CSkeleton.h"
#include "entities/IControllableMedia.h"
#include <wwcommon/general.h>

//
// Namespaces
//

namespace WWCLIENT {

class CEntityMedia;

class CMeshGroup : public IControllableMedia {
public:

	//~ Nested classes *****************************************

	class CMesh : public IControllableMedia {
	public:
		//~ Constructors/Destructors *******************************
													
		CMesh();
		CMesh(NLGEORGES::UFormElm *root);

		~CMesh();

		//~ Methods ************************************************

		void serial(NLMISC::IStream &f);
		bool hasInstance();
		bool hasSkeleton();

		void hide();
		bool show();
		NL3D::UInstance* getInstance();
		NL3D::USkeleton* getSkeleton();
		CEntityMedia* getEMD();
		void setEMD(CEntityMedia* emd);

		void update();

		//~ Instance fields ****************************************

		std::string FriendlyName;
		std::string SkeletonName;
		std::string MeshName;
		CSkeleton* skeleton;

	private:
		//~ Instance fields ****************************************

		NL3D::UInstance m_instance;
		CEntityMedia* m_EMD;
	};

	typedef std::map<std::string, CMesh> meshMap;
	
	//~ Constructors/Destructors *******************************
	
	CMeshGroup();
	/**
	* \brief Here you read in the form if necessary, storing it in memebers of the TMyLoader class
	*/
	CMeshGroup(NLGEORGES::UFormElm *root);

	//~ Methods ************************************************

	/**
	* \brief Here you write a standard NeL serial for all the member of TMyLoader that need to be packed.
	*/
	void serial(NLMISC::IStream &f);

	void showMesh(std::string mesh);
	void hideMesh();
	void show();
	void hide();
	bool isVisible();

	/**
	* \brief Returns the trasform node of the current mesh.
	*/
	NL3D::UTransform* getTransform();
	/**
	* \brief Returns the current mesh.
	*/
	CMesh* getMesh();
	CEntityMedia* getEMD();
	void setEMD(CEntityMedia* emd);
	CMesh* getMesh(std::string& name);

	void addMesh(CMesh& mesh);

	void update();
	void updateMeshes();

	//~ Instance fields ****************************************

	std::string MeshGroupName;

private:
	//~ Instance fields ****************************************

	meshMap m_Meshes;
	CMesh* m_displayedMesh;
	CEntityMedia* m_EMD;

	//~ Methods ************************************************
	
	void setMesh(CMesh* mesh);

};

}; // END NAMESPACE WWCLIENT

#endif // __CMESHGROUP_H__
