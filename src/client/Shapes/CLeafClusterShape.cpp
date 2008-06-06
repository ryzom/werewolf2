// Author: Henri Kuuste

//
// System Includes
//
#include "stdpch.h"

//
// Werewolf includes
//
#include "CLeafClusterShape.h"
#include "Models/CLeafClusterModel.h"

//
// NeL Includes
//
#include <3d/scene.h>

namespace NL3D {

CLeafClusterShape::CLeafClusterShape() {
}

CLeafClusterShape::~CLeafClusterShape() {
}

void CLeafClusterShape::serial(NLMISC::IStream &f) throw(NLMISC::EStream) {
	CBBClusterShape::serial(f);
}

NL3D::CTransformShape *CLeafClusterShape::createInstance(NL3D::CScene &scene) {
	NL3D::CLeafClusterModel *m = NLMISC::safe_cast<NL3D::CLeafClusterModel *>(scene.createModel(LeafClusterModelClassId));
	m->Shape = this;
	return m;
}

void CLeafClusterShape::build(CBBClusterBuild *build) {
	for(int i = 0; i < build->Lods.size(); i++) {
		build->Lods[i]->Flags |= CBBClusterShape::UseRadialNormals;
	}
	CLeafClusterBuild *pBuild = dynamic_cast<CLeafClusterBuild *>(build);
	CBBClusterShape::build(build);

	int numLods = build->Lods.size();

	CLeafClusterBuild::CLeafBuildSlot	*pBS;
	CLeafSlot							*pS;

	for(int i = 0; i < numLods; i++) {
		pS = dynamic_cast<CLeafSlot *>(m_LodVector[i]);
		pBS = dynamic_cast<CLeafClusterBuild::CLeafBuildSlot *>(build->Lods[i]);

		pS->Translucency = pBS->Translucency;
		pS->useFaceNormal = pBS->useFaceNormal;
	}
}

CLeafClusterShape::CLeafSlot::CLeafSlot() {
	useFaceNormal = 0;
}

CLeafClusterShape::CLeafSlot::~CLeafSlot() {
}

void CLeafClusterShape::CLeafSlot::clearAll() {
	CBBSlot::clearAll();
}

bool CLeafClusterShape::CLeafSlot::useBBCastShadows() {
	return (Flags & UseBBCastShadows);
}

void CLeafClusterShape::CLeafSlot::serial(NLMISC::IStream &f) throw(NLMISC::EStream) {
	CBBSlot::serial(f);
	f.serial(useFaceNormal);
	f.serial(Translucency);
}

} // NL3D