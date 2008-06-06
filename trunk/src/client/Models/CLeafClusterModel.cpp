//
// System Includes
//
#include "stdpch.h"
#include <string>

//
// Werewolf includes
//
#include "CLeafClusterModel.h"
#include "Shapes/CLeafClusterShape.h"

//
// NeL includes
//
#include <3d/scene.h>

namespace NL3D {

std::auto_ptr<NL3D::CVertexProgram>	CLeafClusterModel::m_VP;

/*
static const char*	LeafClusterVPCodeStart=
"!!VP1.0										\n\
  MOV R6, v[0];									\n\
";

static const char* LeafClusterVPCodeEnd=
"												\n\
  # c[0..3] == MODELVIEW_PROJECTION				\n\
  DP4 o[HPOS].x, c[0], v[0];					\n\
  DP4 o[HPOS].y, c[1], v[0];					\n\
  DP4 o[HPOS].z, c[2], v[0];					\n\
  DP4 o[HPOS].w, c[3], v[0];					\n\
												\n\
  #MUL o[COL0], o[COL0], v[3];					\n\
  MOV o[TEX0], v[8];							\n\
												\n\
  END											\n\
";
*/

static const char* LeafClusterVPCodeStatic=
"!!VP1.0																				\n\
	# Calculate normal																	\n\
	MUL R6, c[4].wwww, v[0];															\n\
	MAD R6, c[4].zzzz, v[2], R6;														\n\
	# normalize normal																	\n\
	DP3	R6.w, R6, R6;																	\n\
	RSQ	R6.w, R6.w;																		\n\
	MUL	R6, R6, R6.w;																	\n\
																						\n\
	# Global Ambient.																	\n\
	MOV	R2, c[6];																		\n\
																						\n\
	# Diffuse Sun																		\n\
	DP3	R0.x, R6, c[11];			# R0.x= normal*-lightDir							\n\
	LIT	R0.y, R0.xxxx;				# R0.y= R0.x clamped								\n\
	MAD R0.y, R0.y, c[4].yyyy, c[4].xxxx; # Translucency								\n\
	MAD	R2, R0.y, c[7], R2;			# R2= summed vertex color from light				\n\
	MUL R2, R2, v[3];				# Add object color									\n\
	# output to o[COL0] only, replacing alpha with material alpha.						\n\
	MAD	o[COL0], R2, c[15].zzzx, c[15].xxxw;											\n\
																						\n\
	# c[0..3] == MODELVIEW_PROJECTION													\n\
	DP4 o[HPOS].x, c[0], v[0];															\n\
	DP4 o[HPOS].y, c[1], v[0];															\n\
	DP4 o[HPOS].z, c[2], v[0];															\n\
	DP4 o[HPOS].w, c[3], v[0];															\n\
																						\n\
	DP4	o[FOGC].x, c[5], v[0];															\n\
																						\n\
	MOV o[TEX0], v[8];																	\n\
																						\n\
	END																					\n\
";

CLeafClusterModel::CLeafClusterModel() {
	static bool VPSetupDone = false;
	if(!VPSetupDone) {
//		std::string vpCode = std::string(LeafClusterVPCodeStart)
//						   + CRenderTrav::getLightVPFragment(0, 5, false, true)
//						   + std::string(LeafClusterVPCodeEnd);
		std::string vpCode(LeafClusterVPCodeStatic);
		m_VP = std::auto_ptr<NL3D::CVertexProgram>(new NL3D::CVertexProgram(vpCode.c_str()));
		VPSetupDone = true;
	}
	m_BBCastShadows = NULL;
	m_Lod0Slot = NULL;
	m_Lod1Slot = NULL;
	m_numCastShadows = 0;
}

CLeafClusterModel::~CLeafClusterModel() {
	clearCastShadows();
}

void CLeafClusterModel::registerBasic() {
	CScene::registerModel(LeafClusterModelClassId, TransformShapeId, CLeafClusterModel::creator);
}

void CLeafClusterModel::beginRenderTraversal(NL3D::CRenderTrav *renderTrav) {
	NL3D::IDriver *drv = renderTrav->getDriver();
	drv->setConstantMatrix(0, NL3D::IDriver::ModelViewProjection, NL3D::IDriver::Identity);

	CLeafClusterShape *shape = NLMISC::safe_cast<CLeafClusterShape*> ((IShape*)Shape);

	renderTrav->beginVPLightSetup(6, false, invWM);
	renderTrav->changeVPLightSetupMaterial(shape->m_Mat,false);
}

void CLeafClusterModel::endRenderTraversal(NL3D::CRenderTrav *renderTrav) {
	// does nothing
}

void CLeafClusterModel::beginSlot(uint slot, NL3D::CRenderTrav *renderTrav) {
	NL3D::IDriver *drv = renderTrav->getDriver();

	CLeafClusterShape *shape = NLMISC::safe_cast<CLeafClusterShape*> ((IShape*)Shape);
	CLeafClusterShape::CLeafSlot *bbslot = dynamic_cast<CLeafClusterShape::CLeafSlot *>(shape->m_LodVector[slot]);

	drv->setConstant(4, bbslot->Translucency, (float)1.0 - bbslot->Translucency, bbslot->useFaceNormal, (float)1.0 - bbslot->useFaceNormal);
	drv->setConstantFog(5);
}

void CLeafClusterModel::endSlot(uint slot, NL3D::CRenderTrav *renderTrav) {
	// does nothing
}

void CLeafClusterModel::beginRender(NL3D::CRenderTrav *renderTrav) {
	NL3D::IDriver *drv = renderTrav->getDriver();
	drv->activeVertexProgram(m_VP.get());
}

void CLeafClusterModel::endRender(NL3D::CRenderTrav *renderTrav) {
	NL3D::IDriver *drv = renderTrav->getDriver();
	drv->activeVertexProgram(NULL);
}

void CLeafClusterModel::clearCastShadows() {
	if(m_BBCastShadows) {
		delete m_BBCastShadows;
		m_BBCastShadows = NULL;
	}
}

void CLeafClusterModel::recieveCastShadow(NLMISC::CVector center, float radius, bool first) {
	if(!m_BBCastShadows)
		return;

	NLMISC::CVector light = getOwnerScene()->getRenderTrav().getSunDirection();
	light.normalize();
	NLMISC::CVector *pos;
	int numLod0BBs;
	if(m_Lod0Slot) {
		numLod0BBs = m_Lod0Slot->getNumBBs();
		if(m_numCastShadows < numLod0BBs)
			return;
		pos = m_Lod0Slot->getBBPositionsArray();
		for(int i = 0; i < numLod0BBs; i++) {
			float disc;
			if(first)
				m_BBCastShadows[i] = 1.f;
			if(raySphereIntersect(pos[i], light, center, radius, disc)) {
				m_BBCastShadows[i] *= 1 - disc;
			}
		}
	}
	if(m_Lod1Slot) {
		int numBBs = m_Lod1Slot->getNumBBs();
		pos = m_Lod1Slot->getBBPositionsArray();
		int j = 0;
		int i = 0;
		if(m_Lod0Slot) {
			i = numLod0BBs;
			numBBs += numLod0BBs;
		}
		if(m_numCastShadows < numBBs)
			return;
		for(; i < numBBs; i++, j++) {
			float disc;
			if(first)
				m_BBCastShadows[i] = 1.f;
			if(raySphereIntersect(pos[j], light, center, radius, disc)) {
				m_BBCastShadows[i] *= 1 - disc;
			}
		}
	}
}

void CLeafClusterModel::setupCastShadows() {
	CLeafClusterShape *shape = NLMISC::safe_cast<CLeafClusterShape*> ((IShape*)Shape);
	int size = 0;
	m_Lod0Slot = dynamic_cast<CLeafClusterShape::CLeafSlot *>(shape->m_LodVector[Lod0]);
	if(m_Lod0Slot->useBBCastShadows())
        size = m_Lod0Slot->getNumBBs();
	else
		m_Lod0Slot = NULL;
	if(Lod1 != 0xffffffff) {
		m_Lod1Slot = dynamic_cast<CLeafClusterShape::CLeafSlot *>(shape->m_LodVector[Lod1]);
		if(m_Lod1Slot->useBBCastShadows())
			size += m_Lod1Slot->getNumBBs();
		else {
			m_Lod1Slot = NULL;
			if(!m_Lod0Slot)
				return;
		}
	}
	if(m_numCastShadows < size)
		clearCastShadows();
	if(!m_BBCastShadows) {
		m_BBCastShadows = new float[size];
		m_numCastShadows = size;
	}
}

bool CLeafClusterModel::useCastShadows() {
	CLeafClusterShape *shape = NLMISC::safe_cast<CLeafClusterShape*> ((IShape*)Shape);
	CLeafClusterShape::CLeafSlot *Lod0Slot = dynamic_cast<CLeafClusterShape::CLeafSlot *>(shape->m_LodVector[Lod0]);
	CLeafClusterShape::CLeafSlot *Lod1Slot = NULL;
	if(Lod1 != 0xffffffff)
		Lod1Slot = dynamic_cast<CLeafClusterShape::CLeafSlot *>(shape->m_LodVector[Lod1]);
	return Lod0Slot->useBBCastShadows() || ((Lod1Slot) && Lod1Slot->useBBCastShadows());
}

// ray and sphere intersection test
bool raySphereIntersect(NLMISC::CVector pos, NLMISC::CVector direction, NLMISC::CVector center, float radius, float &disc) {
	direction.normalize();
	pos = center - pos;
	disc = direction * pos;
	if(disc < 0.0f)
		return false;
	disc = radius*radius - (pos.sqrnorm() - disc*disc);
	if(disc < 0.0f)
		return false;
	return true;
}

} // NL3D
