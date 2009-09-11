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
#include "zlib.h"

//
// NeL Includes
//
#include <nel/misc/file.h>
#include <nel/misc/path.h>
#include <nel/3d/mesh.h>
#include <nel/3d/shape.h>
#include <nel/3d/texture.h>
#include <nel/3d/material.h>
#include <nel/3d/register_3d.h>
#include <nel/3d/texture_file.h>
#include <nel/3d/texture_multi_file.h>

//
// Werewolf Includes
//
#include "CConfigTask.h"
#include "CTimeTask.h"
#include "CResourceTask.h"

//
// Namespaces
//
using namespace std;
using namespace NL3D;
using namespace NLMISC;

namespace WWCLIENT {

void CResourceTask::init() {
	CPath::remapExtension("dds", "tga", true);
	m_CacheDirectory = CPath::standardizePath(CConfigTask::instance().configFile().getVar("CacheDirectory").asString());

	if(!CFile::isDirectory(m_CacheDirectory)) {
		if(!CFile::createDirectory(m_CacheDirectory)) {
			nlerror("Couldn't create \"%s\" directory", m_CacheDirectory.c_str());
		}
	}

	CConfigFile::CVar &v = CConfigTask::instance().configFile().getVar("Path");
	for (int i = 0; i < v.size(); i++) {
		nlinfo("Added search path: %s",v.asString(i).c_str());
		CPath::addSearchPath (v.asString(i), true, false);
	}
	CRCCheckTimes.clear();
	preloadMeshes();
	preloadEMD();
}

void CResourceTask::refresh(const string &filename) {
	string fn = CFile::getFilename(filename);
	filename2LastCRCCheckTime::iterator it = CRCCheckTimes.find(fn);
	while(it!=CRCCheckTimes.end()) {
		CRCCheckTimes.erase(it);		
		it = CRCCheckTimes.find(fn);
	}
}

std::string CResourceTask::get(const std::string &filename) {
	std::string unk;
	
	if(filename.empty())
		return unk;

	std::string ext = CFile::getExtension(filename);

	std::string fn = NLMISC::CFile::getFilename(filename);
	nlinfo("get filename: %s", fn.c_str());
	std::string path = CPath::lookup(fn, false, false);
	std::string fns = CFile::getFilename(path);

	double currentTime = CTimeTask::instance().time();
	float updatePercent = 0;
	nlinfo("CResourceManager get(%s)",filename.c_str());

	if(!path.empty()) {
		loadChildren(path);
		// we already have the file on local
		return path;
	} else {
		nlinfo("holy crap, empty path!");
	}

	nlinfo("loaded2");
}

void CResourceTask::loadChildren(const std::string &filename) {
	string ext = CFile::getExtension(filename);
	if(ext == "shape") {
		// need to get texture inside the shape
		NL3D::registerSerial3d();

		CShapeStream ss;
		NLMISC::CIFile i(CPath::lookup(filename, false).c_str());
		i.serial(ss);
		i.close();

		CMesh *m = (CMesh*)ss.getShapePointer();
		uint nbm = m->getNbMaterial();
		for(uint i = 0; i < nbm; i++) {
			CMaterial &mat = m->getMaterial(i);
			for(uint j = 0; j < IDRV_MAT_MAXTEXTURES; j++) {
				ITexture *t = mat.getTexture(j);
				if(t) {
					CTextureFile *tf = dynamic_cast<CTextureFile *>(t);
					if(tf) {
						get(tf->getFileName());
					} else {
						CTextureMultiFile *tmf = dynamic_cast<CTextureMultiFile *>(t);
						if(tmf) {
							for(uint t = 0; t < tmf->getNumFileName(); t++)
								get(tmf->getFileName(t));
						}
					}
				}
			}
		}
	}
}

void CResourceTask::update() {
	;
}

void CResourceTask::render() { 
	;
}

void CResourceTask::release() {
	;
}

std::string CResourceTask::name() {
	return "CResourceTask";
}

void CResourceTask::preloadMeshes() {
	std::map<std::string, TMeshLoader> container;

	::loadForm("mesh", "data/meshes.packed_sheets", container, true, false);
	std::map<std::string, TMeshLoader>::iterator iter(container.begin()), last(container.end());
	for(; iter != last; ++iter) {
		std::string shapeName = get(iter->second.Filename);
		nlinfo("Loaded mesh %s -> %s", iter->second.FriendlyName.c_str(), shapeName.c_str()); 
		m_MeshMap[iter->second.FriendlyName] = shapeName;
		nlinfo("Loaded mesh %s -> %s", iter->second.FriendlyName.c_str(), shapeName.c_str()); 
	}
}

std::string CResourceTask::getMesh(std::string name) {
	return m_MeshMap.find(name)->second;
}

void CResourceTask::preloadEMD() {
	std::map<std::string, CEntityMedia> container;

	::loadForm("emd", "data/emd.packed_sheets", container, true, false);
	std::map<std::string, CEntityMedia>::iterator iter(container.begin()), last(container.end());
	for(; iter != last; ++iter) {
		m_EMDMap[iter->second.Name] = iter->second;
		nlinfo("Loaded emd %s", iter->second.Name.c_str()); 
	}
}

CEntityMedia CResourceTask::getEMD(std::string name) {
	return m_EMDMap.find(name)->second;
}

}; // END NAMESPACE WWCLIENT
