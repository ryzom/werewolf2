// Author: Henri Kuuste

//
// System Includes
//
#include "stdpch.h"
#include <algorithm>

//
// Werewolf includes
//
#include "CBBClusterShape.h"
#include "Models/CBBClusterModel.h"

//
// NeL Includes
//
#include <3d/scene.h>
#include <3d/mesh_blender.h>
#include <nel/misc/fast_floor.h>
#include <3d/debug_vb.h>

namespace NL3D {

	CBBClusterShape::CBBClusterShape() : m_bBBoxClean(false) {
		setDistMax(1000.f);
	}
	
	CBBClusterShape::~CBBClusterShape() {
		clear();
	}

	void CBBClusterShape::serial(NLMISC::IStream &f) throw(NLMISC::EStream) {
		sint ver = f.serialVersion(1);
		f.serial(_DistMax);
		f.serial(m_Mat);
		f.serial(Radius);
		f.serialContPolyPtr(m_LodVector);

        if (f.isReading())
			compileRunTime();
	}

	NL3D::CTransformShape *CBBClusterShape::createInstance(NL3D::CScene &scene) {
		NL3D::CBBClusterModel *m = NLMISC::safe_cast<NL3D::CBBClusterModel *>(scene.createModel(BBClusterModelClassId));
		m->Shape = this;
		return m;
	}

	bool CBBClusterShape::clip(const std::vector<NLMISC::CPlane> &pyramid, const NLMISC::CMatrix &worldMatrix) {
		// IMPLEMENT ME
		for (uint k = 0; k < pyramid.size(); ++k) {
			if (! m_BBox.clipBack(pyramid[k] * worldMatrix))
				return false;
		}
		return true;
	}

	void CBBClusterShape::getAABBox(NLMISC::CAABBox &bbox) const {
		// IMPLEMENT ME
		bbox = m_BBox;
	}

	float CBBClusterShape::getNumTriangles (float distance) {
		uint lodCount = m_LodVector.size();
		
		if (lodCount > 0) {
			uint i=0;		
			while (m_LodVector[i]->DistMax < distance) {
				if (i == lodCount-1)
					break;
				i++;
			}
			const CBBSlot *slot = m_LodVector[i];
			return slot->A * distance + slot->B;
		}
		return 0;
	}

	void CBBClusterShape::flushTextures (NL3D::IDriver &driver, uint selectedTexture) {
		// IMPLEMENT ME
	}

	void CBBClusterShape::build(CBBClusterBuild *build) {
		clear ();

		m_Mat = build->Mat;
		Radius = build->Radius;

		int numLods = build->Lods.size();
		m_LodVector.resize(numLods);
		
		CBBClusterBuild::CBBBuildSlot	*pBS;
		CBBSlot							*pS;

		for (uint slot = 0; slot < numLods; slot++) {
			pBS = build->Lods[slot];
			pS = newSlot();

			if(pBS->Flags & UseBBSizes) {
				float largestX = 0;
				float largestY = 0;
				for(int i = 0; i < pBS->BBSizes.size(); i++) {
					NLMISC::CVector2f size = pBS->BBSizes[i];
					if(size.x > largestX)
						largestX = size.x;
					if(size.y > largestY)
						largestY = size.y;
				}
				pS->BBRadius		= NLMISC::CVector2f(largestX, largestY);
			} else {
				pS->BBRadius		= pBS->BBRadius;
			}
			pS->CompareDistance = (Radius*Radius)+ std::max(pS->BBRadius.x, pS->BBRadius.y);
			pS->DistMax		= pBS->DistMax;
			pS->BlendLength	= pBS->BlendLength;
			pS->Flags		= pBS->Flags;
			pS->Alignment	= pBS->Alignment;

			pS->setBBPositions(pBS->BBPositions);
			if(pBS->Flags & UseBBColors)
				pS->setBBColors(pBS->BBColors);
			if(pBS->Flags & UseBBSizes)
				pS->setBBSizes(pBS->BBSizes);
			if(pBS->Flags & UseBBOrientations)
				pS->setBBOrientations(pBS->BBOrientations);
			if(pBS->Alignment == eAxisConstrained)
				pS->setBBConstrainAxes(pBS->BBConstrainAxes);

			m_LodVector[slot] = pS;
		}

		for (int i = numLods-1; i > 0; i--)
		for (int j = 0; j < i; j++)	{
			// Bad sort ?
			if (m_LodVector[j]->DistMax > m_LodVector[j+1]->DistMax) {
				CBBSlot *tmp = m_LodVector[j];
				m_LodVector[j] = m_LodVector[j+1];
				m_LodVector[j+1] = tmp;
			}
		}

		for (uint k = 0; k < numLods; k++) {
			float startDist;
			if (k == 0)
				startDist = 0;
			else 
				startDist = m_LodVector[k-1]->DistMax;

			float startPolyCount;
			startPolyCount = m_LodVector[k]->getPolyCount();

			float endDist = m_LodVector[k]->DistMax;

			if (k == numLods-1)
				m_LodVector[k]->EndPolygonCount = startPolyCount/2;
			else 
				m_LodVector[k]->EndPolygonCount = m_LodVector[k+1]->getPolyCount();
            
			if (endDist == startDist)
				m_LodVector[k]->A = 0;
			else
				m_LodVector[k]->A = (m_LodVector[k]->EndPolygonCount - startPolyCount)/(endDist-startDist);

			m_LodVector[k]->B = m_LodVector[k]->EndPolygonCount - m_LodVector[k]->A*endDist;
		}

		compileRunTime();
	}

	void CBBClusterShape::clear() {
		std::vector<CBBSlot*>::iterator it = m_LodVector.begin();
		for(; it < m_LodVector.end(); it++)
			delete *it;
		m_LodVector.clear();
	}

	void CBBClusterShape::compileDistMax() {
		// Last element
		if(m_LodVector.empty())
			IShape::_DistMax= -1;
		else
			IShape::_DistMax= m_LodVector.back()->DistMax;
	}


	void CBBClusterShape::compileRunTime() {
		if(!m_bBBoxClean) {
			m_bBBoxClean = true;
			m_BBox.setHalfSize(NLMISC::CVector(Radius, Radius, Radius));
			m_BBox.setCenter(NLMISC::CVector::Null);
		}

		compileDistMax();

		// Might do collision mesh generation here or something
	}

	void CBBClusterShape::CBBSlot::serial(NLMISC::IStream &f) throw(NLMISC::EStream) {
		sint ver = f.serialVersion(1);
		f.serial(DistMax);
		f.serial (A);
		f.serial (B);
		f.serial (EndPolygonCount);
		f.serial(BlendLength);
		f.serial(Flags);
		f.serial(m_numBBs);
		f.serial(CompareDistance);
		f.serial(BBRadius);
		f.serialEnum(Alignment);

		std::vector<NLMISC::CVector> BBVec;
		if(!f.isReading())
			getBBPositions(BBVec);
		f.serialCont(BBVec);
		if(f.isReading())
			setBBPositions(BBVec);

		if(Flags & UseBBColors) {
			std::vector<NLMISC::CRGBA> BBColorVec;
			if(!f.isReading())
				getBBColors(BBColorVec);
			f.serialCont(BBColorVec);
			if(f.isReading())
				setBBColors(BBColorVec);
		}

		if(Flags & UseBBSizes) {
			std::vector<NLMISC::CVector2f> BBSizeVec;
			if(!f.isReading())
				getBBSizes(BBSizeVec);
			f.serialCont(BBSizeVec);
			if(f.isReading())
				setBBSizes(BBSizeVec);
		}

		if(Flags & UseBBOrientations) {
			std::vector<float> BBOrientVec;
			if(!f.isReading())
				getBBOrientations(BBOrientVec);
			f.serialCont(BBOrientVec);
			if(f.isReading())
				setBBOrientations(BBOrientVec);
		}

		if(Alignment == eAxisConstrained) {
			std::vector<NLMISC::CVector> BBConstraintVec;
			if(!f.isReading())
				getBBConstrainAxes(BBConstraintVec);
			f.serialCont(BBConstraintVec);
			if(f.isReading())
				setBBConstrainAxes(BBConstraintVec);
		}
	}

	CBBClusterShape::CBBSlot::CBBSlot() {
		m_numBBs = 0;
		Flags = 0;
		DistMax = 0;
		BlendLength = 0;
		m_BBPositions = NULL;
		m_BBColors = NULL;
		m_BBSizes = NULL;
		m_BBOrientations = NULL;
		m_BBConstrainAxes = NULL;
	}

	CBBClusterShape::CBBSlot::~CBBSlot() {
		clearAll();
	}

	void CBBClusterShape::CBBSlot::clearBBPositions() {
		if(m_BBPositions) {
			delete m_BBPositions;
			m_BBPositions = NULL;
		}
	}

	void CBBClusterShape::CBBSlot::clearBBColors() {
		if(m_BBColors) {
			delete m_BBColors;
			m_BBColors = NULL;
		}
	}

	void CBBClusterShape::CBBSlot::clearBBSizes() {
		if(m_BBSizes) {
			delete m_BBSizes;
			m_BBSizes = NULL;
		}
	}

	void CBBClusterShape::CBBSlot::clearBBOrientations() {
		if(m_BBOrientations) {
			delete m_BBOrientations;
			m_BBOrientations = NULL;
		}
	}

	void CBBClusterShape::CBBSlot::clearBBConstrainAxes() {
		if(m_BBConstrainAxes) {
			delete m_BBConstrainAxes;
			m_BBConstrainAxes = NULL;
		}
	}

	void CBBClusterShape::CBBSlot::clearAll() {
		clearBBPositions();
		clearBBSizes();
		clearBBColors();
		clearBBOrientations();
		clearBBConstrainAxes();
	}


	void CBBClusterShape::CBBSlot::setBBPositions(std::vector<NLMISC::CVector> &posVec) {
		m_numBBs = posVec.size();
		clearBBPositions();
		m_BBPositions = new NLMISC::CVector[m_numBBs];
		for(int i = 0; i < m_numBBs; i++) {
			m_BBPositions[i] = posVec[i];
		}
	}

	void CBBClusterShape::CBBSlot::getBBPositions(std::vector<NLMISC::CVector> &v) {
		v.clear();
		v.resize(m_numBBs);
		for(int i = 0; i < m_numBBs; i++) {
			v[i] = m_BBPositions[i];
		}
	}

	void CBBClusterShape::CBBSlot::setBBColors(std::vector<NLMISC::CRGBA> &colVec) {
		if(colVec.size() < m_numBBs)
			colVec.resize(m_numBBs, CRGBA(0, 0, 0));
		clearBBColors();
		m_BBColors = new NLMISC::CRGBA[m_numBBs];
		for(int i = 0; i < m_numBBs; i++) {
			m_BBColors[i] = colVec[i];
		}
	}

	void CBBClusterShape::CBBSlot::getBBColors(std::vector<NLMISC::CRGBA> &v) {
		v.clear();
		v.resize(m_numBBs);
		for(int i = 0; i < m_numBBs; i++) {
			v[i] = m_BBColors[i];
		}
	}

	void CBBClusterShape::CBBSlot::setBBSizes(std::vector<NLMISC::CVector2f> &sizeVec) {
		if(sizeVec.size() < m_numBBs)
			sizeVec.resize(m_numBBs, NLMISC::CVector2f(1, 1));
		clearBBSizes();
		m_BBSizes = new NLMISC::CVector2f[m_numBBs];
		for(int i = 0; i < m_numBBs; i++) {
			m_BBSizes[i] = sizeVec[i];
		}
	}

	void CBBClusterShape::CBBSlot::getBBSizes(std::vector<NLMISC::CVector2f> &v) {
		v.clear();
		v.resize(m_numBBs);
		for(int i = 0; i < m_numBBs; i++) {
			v[i] = m_BBSizes[i];
		}
	}

	void CBBClusterShape::CBBSlot::setBBOrientations(std::vector<float> &orientVec) {
		if(orientVec.size() < m_numBBs)
			orientVec.resize(m_numBBs, 0.f);
		clearBBOrientations();
		m_BBOrientations = new float[m_numBBs];
		for(int i = 0; i < m_numBBs; i++) {
			m_BBOrientations[i] = orientVec[i];
		}
	}

	void CBBClusterShape::CBBSlot::getBBOrientations(std::vector<float> &v) {
		v.clear();
		v.resize(m_numBBs);
		for(int i = 0; i < m_numBBs; i++) {
			v[i] = m_BBOrientations[i];
		}
	}

	void CBBClusterShape::CBBSlot::setBBConstrainAxes(std::vector<NLMISC::CVector> &constraintVec) {
		if(constraintVec.size() < m_numBBs)
			constraintVec.resize(m_numBBs, NLMISC::CVector(0, 1, 0));
		clearBBConstrainAxes();
		m_BBConstrainAxes = new NLMISC::CVector[m_numBBs];
		for(int i = 0; i < m_numBBs; i++) {
			m_BBConstrainAxes[i] = constraintVec[i];
		}
	}

	void CBBClusterShape::CBBSlot::getBBConstrainAxes(std::vector<NLMISC::CVector> &v) {
		v.clear();
		v.resize(m_numBBs);
		for(int i = 0; i < m_numBBs; i++) {
			v[i] = m_BBConstrainAxes[i];
		}
	}

	bool CBBClusterShape::CBBSlot::useBBColors() {
		return (Flags & UseBBColors) && m_BBColors;
	}

	bool CBBClusterShape::CBBSlot::useBBSizes() {
		return (Flags & UseBBSizes) && m_BBSizes;
	}

	bool CBBClusterShape::CBBSlot::useBBOrientations() {
		return (Flags & UseBBOrientations) && m_BBOrientations;
	}

	bool CBBClusterShape::CBBSlot::useBBConstrainAxes() {
		return (Alignment == eAxisConstrained) && m_BBConstrainAxes;
	}

	CBBClusterShape::CBBClusterBuild::CBBClusterBuild() {
		// does nothing
	}

	CBBClusterShape::CBBClusterBuild::~CBBClusterBuild() {
		clear();
	}

	void CBBClusterShape::CBBClusterBuild::clear() {
		std::vector<CBBBuildSlot*>::iterator it = Lods.begin();
		for(; it < Lods.end(); it++)
			delete *it;
		Lods.clear();
	}

	void CBBClusterShape::CBBClusterBuild::addLod(CBBBuildSlot *slot) {
		Lods.push_back(slot);
	}
} // NL3D