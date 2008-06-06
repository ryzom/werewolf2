// Author: Henri Kuuste

//
// System Includes
//
#include "stdpch.h"

//
// Werewolf includes
//
#include "CBBClusterModel.h"
#include "Shapes/CBBClusterShape.h"

//
// NeL includes
//
#include <3d/scene.h>
#include <3d/mesh_blender.h>
#include <nel/misc/fast_floor.h>
#include <3d/debug_vb.h>


namespace NL3D {

CBBClusterModel::CBBClusterModel() :
		uv0(0, 1),
		uv1(1, 1),
		uv2(1, 0),
		uv3(0, 0),
		Flags(0)
{
	setIsLightable(true);
	setIsLoadbalancable(true);
	setIsRenderable(true);
	setOrderingLayer(2);
	freezeStaticLightSetup(NULL, 0, 255, NULL);
	transparencySorter.init(1024);
	// TODO: Better mechanism here
	intArray = new uint32[1024];
	for(uint32 i = 0; i < 1024; ++i) {
		intArray[i] = i;
	}
}

CBBClusterModel::~CBBClusterModel() {
	delete[] intArray;
}

void CBBClusterModel::registerBasic() {
	CScene::registerModel(BBClusterModelClassId, TransformShapeId, CBBClusterModel::creator);
}

float CBBClusterModel::getNumTriangles(float distance) {
	CBBClusterShape *shape = NLMISC::safe_cast<CBBClusterShape*> ((IShape*)Shape);
	return shape->getNumTriangles(distance);	
}

void CBBClusterModel::traverseLoadBalancing() {
	H_AUTO(BBCluster_LOADBALANCING);
	CTransformShape::traverseLoadBalancing ();

	CLoadBalancingTrav	&loadTrav = getOwnerScene()->getLoadBalancingTrav();
	if(loadTrav.getLoadPass()==1) {
		CBBClusterShape *shape=NLMISC::safe_cast<CBBClusterShape*> ((IShape*)Shape);

		setTransparency(false);
		setOpacity(false);

		float polygonCount = getNumTrianglesAfterLoadBalancing ();

		Lod0 = 0;
		uint lodCount = shape->m_LodVector.size();
		if (lodCount>1) {
			while ( polygonCount < shape->m_LodVector[Lod0]->EndPolygonCount ) {
				Lod0++;
				if (Lod0 == lodCount-1)
					break;
			}
		}
		
		CBBClusterShape::CBBSlot *slot = shape->m_LodVector[Lod0];

		float distance = (polygonCount - slot->B) / slot->A;

		PolygonCountLod0 = slot->getPolyCount();

		Lod1=0xffffffff;
		CBBClusterShape::CBBSlot *nextSlot = NULL;
		if (Lod0 != lodCount-1)
			nextSlot = shape->m_LodVector[Lod0+1];

		float startBlend;
		if (nextSlot)
			startBlend = slot->DistMax - nextSlot->BlendLength;
		else
			startBlend = slot->DistMax - slot->BlendLength;

		if ( startBlend < distance ) {
			// Alpha factor for main Lod
			BlendFactor = (slot->DistMax - distance) / (slot->DistMax - startBlend);
			if (BlendFactor < 0)
				BlendFactor=0;
			nlassert (BlendFactor<=1);
		
			if (slot->Flags & CBBClusterShape::BlendOut) {
				setTransparency(true);
				Flags |= CBBClusterModel::Lod0Blend;
			} else {
				setTransparency (slot->isTransparent());
				setOpacity (slot->isOpaque());
				Flags&=~CBBClusterModel::Lod0Blend;
			}
			if (nextSlot && (nextSlot->Flags & CBBClusterShape::BlendIn)) {
				PolygonCountLod1=nextSlot->getPolyCount();
				Lod1=Lod0+1;
				setTransparency(true);
			}
		} else {
			setTransparency (slot->isTransparent());
			setOpacity (slot->isOpaque());
			Flags&=~CBBClusterModel::Lod0Blend;
		}
	}
}

void CBBClusterModel::traverseRender() {
	H_AUTO(BBCluster_RENDER);
	CTransformShape::traverseRender();

	invWM = getWorldMatrix().inverted();

	CRenderTrav			&renderTrav = getOwnerScene()->getRenderTrav();
	bool opaquePass = renderTrav.isCurrentPassOpaque();	
	IDriver				*drv  = renderTrav.getDriver();
	nlassert(drv);

	CBBClusterShape *shape = NLMISC::safe_cast<CBBClusterShape*> ((IShape*)Shape);

    drv->setupModelMatrix(getWorldMatrix());

	changeLightSetup(&renderTrav);

	beginRenderTraversal(&renderTrav);

	cam = invWM * renderTrav.CamMatrix;
	if(cam.getPos() != camPos)
		camMoved = true;
	else
		camMoved = false;
	camPos = cam.getPos();
	I = cam.getI().normed();
	J = cam.getJ().normed();
	K = cam.getK().normed();

	if ( (Lod1!=0xffffffff) && (opaquePass==false) ) {
		NL3D::CMeshBlender blender;
		float globalAlpha = 1.f - BlendFactor;
		uint8 globalAlphaInt = (uint8)NLMISC::OptFastFloor(globalAlpha*255);
		blender.prepareRenderForGlobalAlpha(shape->m_Mat, drv, globalAlpha, globalAlphaInt, true);
		renderSlot(Lod1, &renderTrav);
		blender.restoreRender(shape->m_Mat, drv, true);
	}
	if ( (Flags & CBBClusterModel::Lod0Blend) == 0) {
		renderSlot(Lod0, &renderTrav);
	} else {
		nlassert (opaquePass==false);
		NL3D::CMeshBlender blender;
		float globalAlpha = BlendFactor;
		uint8 globalAlphaInt = (uint8)NLMISC::OptFastFloor(globalAlpha*255);
		blender.prepareRenderForGlobalAlpha(shape->m_Mat, drv, globalAlpha, globalAlphaInt, true);
		renderSlot(Lod0, &renderTrav);
		blender.restoreRender(shape->m_Mat, drv, true);
	}

	endRenderTraversal(&renderTrav);
}

void CBBClusterModel::renderSlot(uint slot, CRenderTrav *renderTrav) {
	H_AUTO(BBCluster_SLOT);
	IDriver *drv = renderTrav->getDriver();

	static NL3D::CVertexBuffer	vb;
	static bool setupDone = false;

	if(!setupDone) {
		vb.setVertexFormat(NL3D::CVertexBuffer::PositionFlag | NL3D::CVertexBuffer::NormalFlag | NL3D::CVertexBuffer::PrimaryColorFlag | NL3D::CVertexBuffer::TexCoord0Flag);
		vb.setPreferredMemory(CVertexBuffer::RAMVolatile, false);
		vb.setName("CBBCluster");
		setupDone = true;
	}
	CBBClusterShape *shape = NLMISC::safe_cast<CBBClusterShape*> ((IShape*)Shape);

	CBBClusterShape::CBBSlot *bbslot = shape->m_LodVector[slot];

	vb.setNumVertices (4*bbslot->getNumBBs());
	{
		NL3D::CVertexBufferReadWrite vba;
		vb.lock (vba);

		uint8	*dstPtr= (uint8*)vba.getVertexCoordPointer();
		uint32	colorOfs = vb.getColorOff();
		uint32	uvOfs = vb.getTexCoordOff();
		uint32	nOfs = vb.getNormalOff();
		uint32	vSize= vb.getVertexSize();

		NLMISC::CRGBA color = shape->m_Mat.getColor();

		NLMISC::CVector* pos = bbslot->getBBPositionsArray();
		NLMISC::CVector2f* size = bbslot->getBBSizesArray();
		NLMISC::CVector* axis = bbslot->getBBConstrainAxesArray();

		NLMISC::CVector x, y, z, tx, ty, tz;

		// depth based sorting
		if(isTransparent()) {
			float	OOFar = 1.0f / renderTrav->Far;
			transparencySorter.reset(bbslot->getNumBBs());
			uint32	transparentOtSize = transparencySorter.getSize();
			uint32	transparentOtMax = transparencySorter.getSize()-1;
			uint32	otId;
			float	rPseudoZ;
			
			NLMISC::OptFastFloorBegin();
			for(uint32 i = 0; i < bbslot->getNumBBs(); ++i) {
				rPseudoZ = (pos[i] - camPos).norm();
				// rPseudoZ from 0.0 -> 1.0
				rPseudoZ =  sqrtf( rPseudoZ * OOFar );
				// since norm, we are sure that rPseudoZ>=0
				rPseudoZ = rPseudoZ * transparentOtSize;
				otId = NLMISC::OptFastFloor(rPseudoZ);
				otId = std::min(otId, transparentOtMax);
				// must invert id, because transparent, sort from back to front			
				transparencySorter.insert(transparentOtMax-otId, &intArray[i]);
			}
			NLMISC::OptFastFloorEnd();		
			transparencySorter.begin();
		}

		bool localAlign = false;
		bool adjustForSize = false;
        
		if((bbslot->Alignment == NL3D::CBBClusterShape::eAutoAligned)
			&& (camPos.sqrnorm() < bbslot->CompareDistance)) {
			// View Align
			localAlign = true;
		} else if((bbslot->Alignment == NL3D::CBBClusterShape::eScreenAligned)
			|| (bbslot->Alignment == NL3D::CBBClusterShape::eAutoAligned)) {
			if(!bbslot->useBBSizes()) {
				x = I * bbslot->BBRadius.x;
				z = K * bbslot->BBRadius.y;
			} else {
				adjustForSize = true;
			}
		}

		NLMISC::CVector left, right;
		NLMISC::CQuat rotQ, rotQConj;
		
		static NLMISC::CHTimer beginSlotTime("BBCluster_BEGIN_SLOT_TIME");
		beginSlotTime.before();
		beginSlot(slot, renderTrav);
		beginSlotTime.after();

		for (uint32 j = 0; j < bbslot->getNumBBs(); ++j) {
			uint32 i;
			if(isTransparent()) {
				i = *(transparencySorter.get());
				transparencySorter.next();
			} else {
                i = j;
			}
			static NLMISC::CHTimer singleBBTime("BBCluster_SINGLE_BB_TIME");
			singleBBTime.before();
			tx = x;
			ty = y;
			tz = z;
			if(bbslot->useBBColors())
				color = bbslot->getBBColorsArray()[i];
			if(adjustForSize) {
				tx = I * size[i].x;
				tz = K * size[i].y;
			} else if((bbslot->Alignment == NL3D::CBBClusterShape::eViewAligned) || localAlign) {
				// View aligned or auto aligned and close enough to the camera to be view aligned
				ty = pos[i];
				ty -= camPos;
				ty.normalize();
				tx = ty^K;
				tx.normalize();
				tz = tx^ty;
				tz.normalize();
				if(bbslot->useBBSizes()) {
					tx *= size[i].x;
					tz *= size[i].y;
				} else {
					tx *= bbslot->BBRadius.x;
					tz *= bbslot->BBRadius.y;
				}
			} else if(bbslot->Alignment == NL3D::CBBClusterShape::eAxisConstrained) {
				tz = axis[i];
				tx = J^tz;
				tx.normalize();
				if(bbslot->useBBSizes()) {
					tx *= size[i].x;
					tz *= size[i].y;
				} else {
					tx *= bbslot->BBRadius.x;
					tz *= bbslot->BBRadius.y;
				}
			}

			if(bbslot->useRadialNormals())
				ty = pos[i].normed();
			else
				ty = -J;
			if(bbslot->useBBOrientations()) {
				NLMISC::CQuat xQ(tx.x, tx.y, tx.z, 0);
				NLMISC::CQuat zQ(tz.x, tz.y, tz.z, 0);
				rotQ.setAngleAxis(y, bbslot->getBBOrientationsArray()[i]);
				rotQConj = rotQ.conjugate();
				xQ = rotQ*xQ*rotQConj;
				zQ = rotQ*zQ*rotQConj;
				tx.x = xQ.x; tx.y = xQ.y; tx.z = xQ.z;
				tz.x = zQ.x; tz.y = zQ.y; tz.z = zQ.z;
			}
			singleBBTime.after();

			{
				left = pos[i]; left -= tz;
				right = left; right += tx;
				left -= tx;
//				NLMISC::CVector temp;

				CHECK_VBA_RANGE(vba, dstPtr+0, sizeof(NLMISC::CVector))
					*(CVector*)(dstPtr+0)= left;
				CHECK_VBA_RANGE(vba, dstPtr+uvOfs, sizeof(NLMISC::CUV))
					*(CUV*)(dstPtr+uvOfs)= uv0;
				CHECK_VBA_RANGE(vba, dstPtr+colorOfs, sizeof(NLMISC::CRGBA))
					*(CRGBA*)(dstPtr+colorOfs)= color;
				CHECK_VBA_RANGE(vba, dstPtr+nOfs, sizeof(NLMISC::CVector))
					*(CVector*)(dstPtr+nOfs)= ty;
				dstPtr+= vSize;
//				nlinfo("%i : Point 0 : %.3f %.3f %.3f", i, left.x, left.y, left.z);
//				temp = getWorldMatrix().mulPoint(left);
//				nlinfo("World: %i : Point 0 : %.3f %.3f %.3f", i, temp.x, temp.y, temp.z);

				CHECK_VBA_RANGE(vba, dstPtr+0, sizeof(NLMISC::CVector))
					*(CVector*)(dstPtr+0)= right;
				CHECK_VBA_RANGE(vba, dstPtr+uvOfs, sizeof(NLMISC::CUV))
					*(CUV*)(dstPtr+uvOfs)= uv1;
				CHECK_VBA_RANGE(vba, dstPtr+colorOfs, sizeof(NLMISC::CRGBA))
					*(CRGBA*)(dstPtr+colorOfs)= color;
				CHECK_VBA_RANGE(vba, dstPtr+nOfs, sizeof(NLMISC::CVector))
					*(CVector*)(dstPtr+nOfs)= ty;
				dstPtr+= vSize;
//				nlinfo("%i : Point 1 : %.3f %.3f %.3f", i, right.x, right.y, right.z);
//				temp = getWorldMatrix().mulPoint(right);
//				nlinfo("World: %i : Point 1 : %.3f %.3f %.3f", i, temp.x, temp.y, temp.z);

				left += tz; left += tz;
				right += tz; right += tz;

				CHECK_VBA_RANGE(vba, dstPtr+0, sizeof(NLMISC::CVector))
					*(CVector*)(dstPtr+0)= right;
				CHECK_VBA_RANGE(vba, dstPtr+uvOfs, sizeof(NLMISC::CUV))
					*(CUV*)(dstPtr+uvOfs)= uv2;
				CHECK_VBA_RANGE(vba, dstPtr+colorOfs, sizeof(NLMISC::CRGBA))
					*(CRGBA*)(dstPtr+colorOfs)= color;
				CHECK_VBA_RANGE(vba, dstPtr+nOfs, sizeof(NLMISC::CVector))
					*(CVector*)(dstPtr+nOfs)= ty;
				dstPtr+= vSize;
//				nlinfo("%i : Point 2 : %.3f %.3f %.3f", i, right.x, right.y, right.z);
//				temp = getWorldMatrix().mulPoint(right);
//				nlinfo("World: %i : Point 2 : %.3f %.3f %.3f", i, temp.x, temp.y, temp.z);

				CHECK_VBA_RANGE(vba, dstPtr+0, sizeof(NLMISC::CVector))
					*(CVector*)(dstPtr+0)= left;
				CHECK_VBA_RANGE(vba, dstPtr+uvOfs, sizeof(NLMISC::CUV))
					*(CUV*)(dstPtr+uvOfs)= uv3;
				CHECK_VBA_RANGE(vba, dstPtr+colorOfs, sizeof(NLMISC::CRGBA))
					*(CRGBA*)(dstPtr+colorOfs)= color;
				CHECK_VBA_RANGE(vba, dstPtr+nOfs, sizeof(NLMISC::CVector))
					*(CVector*)(dstPtr+nOfs)= ty;
				dstPtr+= vSize;
//				nlinfo("%i : Point 3 : %.3f %.3f %.3f", i, left.x, left.y, left.z);
//				temp = getWorldMatrix().mulPoint(left);
//				nlinfo("World: %i : Point 3 : %.3f %.3f %.3f", i, temp.x, temp.y, temp.z);
			}
		}
	}
	static NLMISC::CHTimer renderTime("BBCluster_RENDER_TIME");
	renderTime.before();
	static NLMISC::CHTimer beginRenderTime("BBCluster_BEGIN_RENDER_TIME");
	beginRenderTime.before();
	beginRender(renderTrav);
	beginRenderTime.after();
	drv->activeVertexBuffer(vb);
	drv->renderRawQuads(shape->m_Mat, 0, bbslot->getNumBBs());
	endRender(renderTrav);
	endSlot(slot, renderTrav);
	renderTime.after();
}

const CMaterial* CBBClusterModel::getMaterial(uint materialId) const {
	CBBClusterShape *shape = NLMISC::safe_cast<CBBClusterShape*> ((IShape*)Shape);
	return &(shape->m_Mat);
}

CMaterial* CBBClusterModel::getMaterial(uint materialId) {
	CBBClusterShape *shape = NLMISC::safe_cast<CBBClusterShape*> ((IShape*)Shape);
	return &(shape->m_Mat);
}


} // NL3D
