// Author: Henri Kuuste

#ifndef __CLEAFCLUSTERMODEL_H__
#define __CLEAFCLUSTERMODEL_H__

//
// NeL Includes
//
#include <3d/vertex_program.h>

//
// Werewolf includes
//
#include "CBBClusterModel.h"
#include "Shapes\CLeafClusterShape.h"

namespace NL3D {

class CLeafClusterModel : public CBBClusterModel {
public:
	static	void			registerBasic();

protected:
	CLeafClusterModel();
	~CLeafClusterModel();

	static	std::auto_ptr<NL3D::CVertexProgram>	m_VP;

	virtual void			beginRenderTraversal(NL3D::CRenderTrav *renderTrav);
	virtual void			endRenderTraversal(NL3D::CRenderTrav *renderTrav);

	virtual void			beginSlot(uint slot, NL3D::CRenderTrav *renderTrav);
	virtual void			endSlot(uint slot, NL3D::CRenderTrav *renderTrav);

	virtual void			beginRender(NL3D::CRenderTrav *renderTrav);
	virtual void			endRender(NL3D::CRenderTrav *renderTrav);
	void					clearCastShadows();
	void					recieveCastShadow(NLMISC::CVector center, float radius, bool first);
	void					setupCastShadows();
	bool					useCastShadows();

private:
	static CTransform		*creator() { return new CLeafClusterModel; }

	float *					m_BBCastShadows;
	int						m_numCastShadows;
	CLeafClusterShape::CLeafSlot *m_Lod0Slot;
	CLeafClusterShape::CLeafSlot *m_Lod1Slot;

	friend class CLeafClusterShape;
};

// ray and sphere intersection test
bool raySphereIntersect(NLMISC::CVector start, NLMISC::CVector end, NLMISC::CVector center, float radius, float &disc);

} // NL3D

#endif // __CLEAFCLUSTERMODEL_H__
