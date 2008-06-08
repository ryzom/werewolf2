// Author: Henri Kuuste

#ifndef __CBBCLUSTERMODEL_H__
#define __CBBCLUSTERMODEL_H__

//
// NeL Includes
//
#include <3d/transform_shape.h>
#include <3d/vertex_buffer.h>
#include <3d/ordering_table.h>

//
// Werewolf includes
//

namespace NL3D {

class CBBClusterShape;

class CBBClusterModel : public CTransformShape {
public:
	enum {
		Lod0Blend		=	0x1,
	};

	static	void			registerBasic();

	virtual float			getNumTriangles (float distance);
	virtual void			traverseLoadBalancing();
	virtual void			traverseRender();

	virtual uint			getNumMaterial() const { return 1; };
	virtual const CMaterial* getMaterial(uint materialId) const;
	virtual CMaterial*		getMaterial(uint materialId);

protected:
	CBBClusterModel();
	~CBBClusterModel();

	// things that derived classes can implement to add to the render process
	virtual void			beginRenderTraversal(NL3D::CRenderTrav *renderTrav) { /* Nothing here */ };
	virtual void			endRenderTraversal(NL3D::CRenderTrav *renderTrav) { /* Nothing here */ };

	virtual void			beginSlot(uint slot, NL3D::CRenderTrav *renderTrav) { /* Nothing here */ };
	virtual void			endSlot(uint slot, NL3D::CRenderTrav *renderTrav) { /* Nothing here */ };

	virtual void			beginRender(NL3D::CRenderTrav *renderTrav) { /* Nothing here */ };
	virtual void			endRender(NL3D::CRenderTrav *renderTrav) { /* Nothing here */ };

	void					renderSlot(uint slot, NL3D::CRenderTrav *renderTrav);

	uint					Lod0;
	uint					Lod1;

	uint					Flags;
	float					PolygonCountLod0;
	float					PolygonCountLod1;
	float					BlendFactor;

	NLMISC::CUV				uv0, uv1, uv2, uv3;
	NLMISC::CMatrix			cam;
	NLMISC::CVector			camPos;
	bool					camMoved;
	NLMISC::CVector			I, J, K;
	uint32					*intArray;

	NLMISC::CMatrix			invWM;
	NL3D::COrderingTable<uint32> transparencySorter;
	friend class CBBClusterShape;

private:
	static CTransform		*creator() { return new CBBClusterModel; }
};

} // NL3D

#endif // __CBBCLUSTERMODEL_H__
