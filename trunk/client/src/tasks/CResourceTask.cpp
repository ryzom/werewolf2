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
	if(ext == "shape") unk = CPath::lookup("unknown.shape", false);
	else if(ext == "skel") unk = CPath::lookup("unknown.skel", false);
	else if(ext == "anim") unk = CPath::lookup("uknown.anim", false);
	else if(ext == "lua") unk = CPath::lookup("unknown.lua", false);
	else if(ext == "tga") unk = CPath::lookup("unknown.tga", false);
	else if(ext == "dds") unk = CPath::lookup("unknown.tga", false);
	else if(ext == "png") unk = CPath::lookup("unknown.tga", false);
	else if(ext == "ps") unk = CPath::lookup("unknown.ps", false);
	else if(ext == "wav") unk = CPath::lookup("unkown.wav", false);
	else {
		nlwarning("Extension not managed : %s(%s)",ext.c_str(),filename.c_str());
	}

	std::string fn = NLMISC::CFile::getFilename(filename);

	std::string path = CPath::lookup(fn, false, false);
	std::string fns = CFile::getFilename(path);

	double currentTime = CTimeTask::instance().time();
	float updatePercent = 0;
	//CSmartPtr<CGuiFrame> mainFrame = NULL;

	//nlinfo("CResourceManager get(%s)",filename.c_str());

	if(!path.empty()) {
		//nlinfo("!path.empty()");
		m_CRCUpToDate = true;
//		if(CNetworkTask::instance().connected()) {
			//nlinfo("connected");
//			double lastCheckTime = 0;
//			filename2LastCRCCheckTime::iterator it = CRCCheckTimes.find(fn);
//			if(it!=CRCCheckTimes.end())
//				lastCheckTime = (*it).second;
//			if( it==CRCCheckTimes.end() && CConfigTask::instance().configFile().getVar("CRCCheck").asInt())
//			{
//				guiSPG<CGuiXml> xml = CGuiXmlManager::instance().Load("checking.xml");
	//			mainFrame = (CGuiFrame *)xml->get("checkingFrame");
		//		guiSPG<CGuiText>  checkingMessage = (CGuiText *)xml->get("checkingMessage");
//				checkingMessage->text = "Please wait while checking : ";
//				guiSPG<CGuiText>  checkingFilename = (CGuiText *)xml->get("checkingFilename");
//				checkingFilename->text = fn;
//				CGuiObjectManager::instance().objects.push_back(mainFrame);

	//			CHashKey hashKey = getSHA1(path);
		//		CNetMessage msgout(CNetMessage::RequestCRCKey);
//				msgout.serial(fns);
//				msgout.serial(hashKey);
//				CNetworkTask::instance().send(msgout);
				
//				CRCReceived = false;
//				bool messageReceived;
//				while(waitNetworkMessage(CRCReceived,messageReceived));
//				if(!messageReceived)
//					return unk;
//				if(it!=CRCCheckTimes.end())
//					CRCCheckTimes.erase(it);
//				CRCCheckTimes.insert(filename2LastCRCCheckTime::value_type(fn,currentTime));
//			}
//		}

		if(m_CRCUpToDate) {
			//nlinfo("CRCUpToDate");
			loadChildren(path);
			// we already have the file on local
			return path;
		}
	} else {
		nlinfo("holy crap, empty path!");
	}

//	if(!CNetworkTask::instance().connected())
//	{
		//nlinfo("!connected");
		// we can't download the file
//		return unk;
//	}
	
//	guiSPG<CGuiXml> xml = CGuiXmlManager::instance().Load("updating.xml");
//	mainFrame = (CGuiFrame *)xml->get("updatingFrame");
//	guiSPG<CGuiText>  updatingMessage = (CGuiText *)xml->get("updatingMessage");
//	updatingMessage->text = "Please wait while dowloading : ";
//	guiSPG<CGuiText>  updatingFilename = (CGuiText *)xml->get("updatingFilename");
//	updatingFilename->text = fn;
//	guiSPG<CGuiProgressBar>  updatingProgressBar = (CGuiProgressBar *)xml->get("updatingProgressBar");
//	updatingProgressBar->ptrValue(&updatePercent);
//	CGuiObjectManager::instance().objects.push_back(mainFrame);		
	
	
//	Eof = false;
//	uint32 part = 0;
//	FileSize = 0;
	
//	string dlfn = CacheDirectory + fn;
//	string packedfn = dlfn + ".gz";

//	if(updatingMessage)
//		updatingMessage->text = "Please wait while dowloading : ";
	
//	while(!Eof)
//	{
//		Received = false;
//		CNetMessage msgout(CNetMessage::RequestDownload);
//		msgout.serial(fn);
//		msgout.serial(part);
//		CNetworkTask::instance().send(msgout);

//		string str = toString("Please wait while downloading '%s' part %d", fn.c_str(), part);

//		bool messageReceived;
//		while(waitNetworkMessage(Received,messageReceived));
	//	if(!messageReceived)
//		{
//			CFile::deleteFile(packedfn);
//			exit(1);
//		}

//		if(ReceivedError)
//		{
//			CFile::deleteFile(packedfn);
//			return unk;
//		}

//		nlinfo("Receive an answer of a download request block size %s %d eof %d", Reason.c_str(), Buffer.size(), Eof);

//		FILE *fp = fopen(packedfn.c_str(), "ab");
//		if(!fp)
//		{
//			nlwarning("Couldn't open file '%s'", packedfn.c_str());
//			return unk;
//		}
//		if(fwrite(&*Buffer.begin(), 1, Buffer.size(), fp) != Buffer.size())
//		{
//			nlwarning("Couldn't write file '%s'", packedfn.c_str());
//			fclose(fp);
//			return unk;
//		}
//		fclose(fp);

//		part += Buffer.size();
//		if(FileSize)
//			updatePercent = ((float)part) / FileSize;
//	}

//	string destfn = CacheDirectory + ReceivedFilename;
//	nlinfo("Received the whole file '%s'", destfn.c_str());

//	vector<uint8> buf;
//	buf.resize(8000);
//	uint8 *ptr = &(*(buf.begin()));
	
//	{
//		FILE *fp = fopen(destfn.c_str(), "wb");
//		gzFile gzfp = gzopen(packedfn.c_str(), "rb");
//		while (!gzeof(gzfp)) 
//		{
//			uint32 res = gzread(gzfp, ptr, 8000);
//			fwrite(ptr,1,res,fp);
//		}
//		fclose(fp);
//		gzclose(gzfp);
//		CFile::deleteFile(packedfn);		
//	}
	
//	CPath::addSearchFile(destfn);

//	loadChildren(destfn);
//	CRCCheckTimes.insert(filename2LastCRCCheckTime::value_type(fn,currentTime));
				
	// need to download the file
//	return destfn;
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
