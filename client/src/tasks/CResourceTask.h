/**
 * \file CResourceTask.h
 * \date May 2004
 * \author Matt Raykowski
 * \author Some code adapted from MTP-Target.
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

#ifndef __CRESOURCETASK_H__
#define __CRESOURCETASK_H__

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/mem_stream.h>
#include <nel/misc/sha1.h>
#include <nel/georges/u_form_loader.h>
#include <nel/georges/u_form.h>
#include <nel/georges/u_form_elm.h>
#include <nel/georges/load_form.h>

//
// Werewolf Includes
//
#include "wwcommon/ITask.h"
#include "wwcommon/ISingleton.h"
#include "entities/CEntityMedia.h"

//
// Class
//

namespace WWCLIENT {

class TMeshLoader {
public:
	/**
	 * \brief Here you read in the form if necessary, storing it in memebers of the TMyLoader class
	 */
	void readGeorges(const NLMISC::CSmartPtr<NLGEORGES::UForm> &form, const std::string &sheetId) {
		NLGEORGES::UFormElm &root = form->getRootNode();
		root.getValueByName(Filename,".Filename");
		root.getValueByName(FriendlyName,".FriendlyName");
		root.getValueByName(Description,".Description");
	}

	/**
	 * \brief Here you write a standard NeL serial for all the member of TMyLoader that need to be packed.
	 */
	void serial(NLMISC::IStream &f) {
		if(f.isReading()) {
			f.serial(Filename);
			f.serial(FriendlyName);
			f.serial(Description);
		} else {
			f.serial(Filename);
			f.serial(FriendlyName);
			f.serial(Description);
		}
	}
	/**
	 * \brief This methed returns the version of the packed sheet.
	 * Version 0 - non-existant.
	 * Version 1 - Fileame, FriendlyName, Description.
	 *
	 * \return uint This must always return an integer. Unsigned is probably best.
	 */
	static uint getVersion() {
		return 1;
	}

	/**
	 * \brief Here you can write custom code for when the packed sheet loader needs to remove an old sheet.
	 * \note This is rarely used.
	 */
	void removed() {
		;
	}

	std::string Filename;
	std::string FriendlyName;
	std::string Description;
};

typedef std::map<std::string, double> filename2LastCRCCheckTime;
typedef std::map<std::string, std::string> shapeMap;
typedef std::map<std::string, CEntityMedia> entityMediaMap;

class CResourceTask : public WWCOMMON::ISingleton<CResourceTask>, public WWCOMMON::ITask {
public:

	/**
	 * \brief Initializes the Resource Task.
	 *
	 * Initializes the Resource Task system. This includes remapping the
	 * dds/tga extensions, checking the validity of the cache directory,
	 * resetting the CRCCheckTimes, and adding the appropriate search paths.
	 */
	virtual void init();
	virtual void update();	/**< This method, from ITask, does nothing. */
	virtual void render();	/**< This method, from ITask, does nothing. */
	virtual void release();	/**< This method, from ITask, does nothing. */
	
	/**
	 * \brief Returns the Task name.
	 *
	 * From ITask, this method returns CResourceTask, the name of the
	 * Task.
	 * \return std::string containing the name of the task, CResourceTask.
	 */
	virtual std::string name();

	std::string get(const std::string &filename);

	/**
	 * \brief Forces a refresh of the defined resource.
	 *
	 * When called the file path for filename is fetched and all occurances
	 * of that resource in CRCCheckTimes is erased. Next call to get() will
	 * force a recheck of the specified resource.
	 * \param filename A std::string object containing the name of the resource.
	 */
	void refresh(const  std::string &filename);

	/**
	 * \brief Loads a shape and it's child components.
	 *
	 * This method is called to load a specified shape and all of
	 * it's child components. First it registers and all serializable
	 * 3D classes and then converts the file into a CShapeStream. This
	 * is immediately converted to a CMesh and then then Mesh is looped
	 * through to load all materials. This method only loads .shape files.
	 * Loading materials happens in this order, for each material assigned:
	 *	- From the mesh, copy the CMaterial information.
	 *	- Using CMaterial copy the Texture assigned.
	 *	- Using the Texture determine the filename of the texture.
	 *	- If it's a single texture call get() and load the texture.
	 *		- If it's a multi texture, copy the CTextureMultiFile
	 *		- Finally iterate through the CTextureMultiFile and call get() for each.
	 * @param filename std::string containing the path to the file/mesh to be loaded.
	 * @see get()
	 */
	void loadChildren(const std::string &filename);
	filename2LastCRCCheckTime CRCCheckTimes;
	void preloadMeshes();
	std::string getMesh(std::string name);
	void preloadEMD();
	CEntityMedia getEMD(std::string name);

	/**
	 * \brief Return the entity media map.
	 *
	 * \todo Change this to return a pointer, when the emd map grows copying the map will suck.
	 */
	entityMediaMap getEntityMediaMap() { return m_EMDMap; }
private:
	std::string			m_CacheDirectory;
	bool						m_CRCUpToDate;
	shapeMap			m_MeshMap;
	entityMediaMap		m_EMDMap;
};

}; // END NAMESPACE WWCLIENT

#endif // __CRESOURCETASK_H__
