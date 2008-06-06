/**
* \file CNeLViewport.cpp
* \date October 2007
* \author Henri Kuuste
*/

/* Copyright, 2007 Ironic Entertainment
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

#include <CNeLViewport.h>

#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MRenderingInfo.h>
#include <maya/MFnCamera.h>
#include <maya/MItDag.h>
#include <maya/MMatrix.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnMesh.h>
#include <maya/MAngle.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MQuaternion.h>
#include <maya/MBoundingBox.h>
#include <maya/MFnTransform.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MHardwareRenderer.h>
#include <stdio.h>
#include <maya/MIOStream.h>
#include <maya/MDrawTraversal.h>
#include <maya/MGeometryManager.h>
#include <maya/MGeometry.h>
#include <maya/MGeometryData.h>
#include <maya/MGeometryPrimitive.h>
#include <maya/MHwTextureManager.h>
#include <maya/MImageFileInfo.h>
#include <maya/MFnSingleIndexedComponent.h>
#include <nel/3d/u_driver.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_text_context.h>
#include <3d/driver_user.h> 
#include <3d/driver.h>
#include <nel/misc/bitmap.h> 
#include <nel/3d/u_material.h>
#include <nel/misc/file.h>
#include <nel/misc/path.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_camera.h>
#include <nel/misc/vector.h>
#include <3d/scene_user.h> 
#include <3d/scene.h> 
#include <3d/render_trav.h> 
#include <GL/gl.h>

CNeLViewport::CNeLViewport() : MViewportRenderer("NeLViewport") {
	fUIName.set( "Plugin NeL Viewport");
	fRenderingOverride = MViewportRenderer::kOverrideAllDrawing;
	m_API = MViewportRenderer::kOpenGL;
	m_Version = 2.0f;
}

CNeLViewport::~CNeLViewport() {
	uninitialize();
}

MStatus	CNeLViewport::initialize() {
	m_Driver = NL3D::UDriver::createDriver(0, false);
	NL3D::IDriver *driver = ((NL3D::CDriverUser*)m_Driver)->getDriver();
	m_width = 1000;
	m_height = 1000;
	driver->setDisplay(NULL, NL3D::GfxMode(1000, 1000, 32, true, true), true);
	m_Scene = m_Driver->createScene(false);
	m_Driver->setSwapVBLInterval(0);
	NL3D::UCamera Camera = m_Scene->getCam();
	Camera.setTransformMode (NL3D::UTransformable::DirectMatrix);
	NLMISC::CPath::addSearchPath("C:\\Temp\\Werewolf", true, false);
	return MStatus::kSuccess;
}

MStatus	CNeLViewport::uninitialize() {
	m_Driver->deleteScene(m_Scene);
	m_Driver->release();
	delete m_Driver;
	return MStatus::kSuccess;
}

void displayMatrix(const float *mat) {
	for(int i = 0; i < 16; i += 4) {
		printf("% 2.2f % 2.2f % 2.2f % 2.2f\n", mat[i], mat[i+1], mat[i+2], mat[i+3]);
	}
}

MStatus	CNeLViewport::render(const MRenderingInfo &renderInfo) {
	//printf("Render using (%s : %s) renderer\n", fName.asChar(), fUIName.asChar());

	if (renderInfo.renderingAPI() == MViewportRenderer::kOpenGL) {
		const MRenderTarget & target = renderInfo.renderTarget();
		const MDagPath &cameraPath = renderInfo.cameraPath();
		if (cameraPath.isValid()) {
			if(m_width != target.width() || m_height != target.height()) {
				NL3D::IDriver *driver = ((NL3D::CDriverUser*)m_Driver)->getDriver();
				// TODO Figure out a better way to resize/recreate the pbuffer. Recreating the display messes everything up.
				// An offscreen buffer resize is probably needed in the opengl driver.
				driver->setDisplay(NULL, NL3D::GfxMode(target.width(), target.height(), 32, true, true), true);
				m_width = target.width();
				m_height = target.height();
				m_Scene->enableLightingSystem(true);
			}
			MStatus status;
			MFnCamera camera (cameraPath, &status);
			if ( !status ) {
				status.perror("MFnCamera constructor");
			}
			else
			{
				MPoint eyePoint = camera.eyePoint( MSpace::kWorld );
				MPoint lookAtPt	= camera.centerOfInterestPoint( MSpace::kWorld );
				double horizontalFieldOfView = MAngle(camera.horizontalFieldOfView()).asRadians();
				double nearClippingPlane = camera.nearClippingPlane();
				double farClippingPlane = camera.farClippingPlane();

				m_Scene->getCam().setPerspective((float)horizontalFieldOfView,
					(float)target.width()/(float)target.height(),
					(float)nearClippingPlane,
					(float)farClippingPlane);
				m_Scene->getCam().lookAt(NLMISC::CVector((float)eyePoint.x, (float)eyePoint.y, (float)-eyePoint.z),
					NLMISC::CVector((float)lookAtPt.x, (float)lookAtPt.y, (float)lookAtPt.z));
				NLMISC::CMatrix camMat(m_Scene->getCam().getMatrix());
				NLMISC::CMatrix model;
				model.setRot(model.getI(), model.getJ(), NLMISC::CVector(0, 0, -1));
				model *= camMat;
				m_Scene->getCam().setMatrix(model);
				m_Scene->setSunDirection(m_Scene->getCam().getMatrix().getJ());
				m_Scene->setAmbientGlobal(NLMISC::CRGBA(128, 128, 128));
			}
			renderToTarget( renderInfo );
			NLMISC::CBitmap bitmap;
			m_Driver->getBuffer(bitmap);
			MImage image;
			// image.create(bitmap.getHeight(), bitmap.getWidth());
			//		printf("Rendered at (%u x %u) %u\n", bitmap.getHeight(), bitmap.getWidth(), bitmap.getPixels().size());
			//		std::string filename = NLMISC::CFile::findNewFile ("screenshot.tga");
			//		NLMISC::COFile fs (filename);
			//		bitmap.writeTGA (fs,24,true);
			
			// hack - it seems to want to do BGRA no matter what I do
			// TODO fix this...somehow
			unsigned char* data = new unsigned char[bitmap.getPixels().size()];
			unsigned char* pixels = (unsigned char*)bitmap.getPixels().getPtr();
			unsigned int size = bitmap.getPixels().size();
			for(unsigned int i = 0; i < size; i += 4) {
				data[i] = pixels[i+2];
				data[i+1] = pixels[i+1];
				data[i+2] = pixels[i];
				data[i+3] = pixels[i+3];
			}
			image.setPixels(data, bitmap.getWidth(), bitmap.getHeight());
			image.setRGBA(true);
			target.writeColorBuffer(image, 0, 0);
			delete[] data;
		} else {
			printf("Camera not valid\n");
		}
	}
	return MStatus::kSuccess;
}

bool CNeLViewport::nativelySupports( MViewportRenderer::RenderingAPI api, float version ) {
	return (api == m_API);
}

MStatus CNeLViewport::getTransforms(MDagPath path, double scale[3], MQuaternion &rot, MVector &trans, MSpace::Space spc, bool rhs) {
	MStatus status;
	MObject transformObject = path.transform();
   	MDagPath transformPath(path);
	while(transformPath.length() > 0) {
		if(transformPath.node() == transformObject)
			break;
		else
			transformPath.pop();
	}
	MFnTransform fTransform(transformPath);

	MMatrix mworld = path.inclusiveMatrix();
	MTransformationMatrix world(mworld);
	if ( world.getScale(scale, spc) == MStatus::kFailure) {
		MGlobal::displayError("MFnTransform::getScale");
		return MStatus::kFailure;
	}
	if( fTransform.getRotation(rot, spc) == MStatus::kFailure) {
		MGlobal::displayError("MFnTransform::getRotation(quat)");
		return MStatus::kFailure;
	}
	trans = fTransform.translation(spc, &status) + fTransform.rotatePivotTranslation(spc);
	if(status == MStatus::kFailure) {
		MGlobal::displayError("MFnTransform::translation");
		return MStatus::kFailure;
	}
	trans += fTransform.rotatePivot(MSpace::kTransform, &status) - fTransform.scalePivot(MSpace::kTransform);
	if(status == MStatus::kFailure) {
		MGlobal::displayError("MFnTransform::rotatePivot");
		return MStatus::kFailure;
	}
	return MStatus::kSuccess;
}


bool CNeLViewport::override( MViewportRenderer::RenderingOverride override ) {
	return (override == fRenderingOverride);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Rendering methods
////////////////////////////////////////////////////////////////////////////////////////////////////

bool CNeLViewport::renderToTarget( const MRenderingInfo &renderInfo ) {
	const MRenderTarget &renderTarget = renderInfo.renderTarget();
	const MDagPath &cameraPath = renderInfo.cameraPath();
	NL3D::IDriver *driver = ((NL3D::CDriverUser*)m_Driver)->getDriver();
	m_Driver->activate();
	m_Driver->clearRGBABuffer(NLMISC::CRGBA(128, 128, 128));
	m_Driver->clearZBuffer();
	m_Scene->render();

	NL3D::CScene &scene = ((NL3D::CSceneUser*)m_Scene)->getScene();
	NL3D::CRenderTrav &renderTrav = scene.getRenderTrav();
	renderTrav.setupDriverCamera();
	renderTrav.resetLightSetup();

	NLMISC::CMatrix model;
//	model.setRot(model.getI(), model.getJ(), NLMISC::CVector(0, 0, -1));
	driver->setupModelMatrix(model);

	// View axis
	NL3D::CMaterial cmat1;
	NL3D::UMaterial mat1(&cmat1);
	cmat1.initUnlit();
	cmat1.setColor(NLMISC::CRGBA::Red);
	m_Driver->drawLine(NLMISC::CLine(NLMISC::CVector(0, 0, 0), NLMISC::CVector(2, 0, 0)), mat1);
	cmat1.setColor(NLMISC::CRGBA::Green);
	m_Driver->drawLine(NLMISC::CLine(NLMISC::CVector(0, 0, 0), NLMISC::CVector(0, 2, 0)), mat1);
	cmat1.setColor(NLMISC::CRGBA::Blue);
	m_Driver->drawLine(NLMISC::CLine(NLMISC::CVector(0, 0, 0), NLMISC::CVector(0, 0, 2)), mat1);

	// Render some bounding boxes
	MDrawTraversal *trav = NULL;
	trav = new MDrawTraversal;
	trav->enableFiltering(false);
	trav->setFrustum(cameraPath, renderTarget.width(), renderTarget.height());
	trav->traverse();

	unsigned int numItems = trav->numberOfItems();
	unsigned int i;
	for (i=0; i<numItems; i++)
	{
		MDagPath path;
		trav->itemPath(i, path);
		if (path.isValid())
		{
			if(path.hasFn(MFn::kMesh)) {
				if (trav->itemHasStatus( i, MDrawTraversal::kActiveItem )) {
					::glColor3f( 1.0f, 1.0f, 1.0f );
				} else {
					::glColor3f( 0.286f, 0.706f, 1.0f );
				}
				MFnDagNode dagNode(path);
				MBoundingBox box = dagNode.boundingBox();
				drawBounds( path, box );
			}
		}
	}


	m_Driver->swapBuffers();

	return true;
}

void
CNeLViewport::drawBounds( const MDagPath &dagPath, const MBoundingBox &box) {
	MMatrix  matrix = dagPath.inclusiveMatrix();
	MPoint	minPt = box.min();
	MPoint	maxPt = box.max();

	double bottomLeftFront[3] =		{ minPt.x, minPt.y, minPt.z };
	double topLeftFront[3] = 		{ minPt.x, maxPt.y, minPt.z };
	double bottomRightFront[3] =	{ maxPt.x, minPt.y, minPt.z };
	double topRightFront[3] =		{ maxPt.x, maxPt.y, minPt.z };
	double bottomLeftBack[3] =		{ minPt.x, minPt.y, maxPt.z };
	double topLeftBack[3] =			{ minPt.x, maxPt.y, maxPt.z };
	double bottomRightBack[3] =		{ maxPt.x, minPt.y, maxPt.z };
	double topRightBack[3] =		{ maxPt.x, maxPt.y, maxPt.z };
	::glMatrixMode( GL_MODELVIEW );
	::glPushMatrix();
	::glMultMatrixd( &(matrix.matrix[0][0]) );

	::glBegin(GL_LINE_STRIP);
	::glVertex3dv( bottomLeftFront );
	::glVertex3dv( bottomLeftBack );
	::glVertex3dv( topLeftBack );
	::glVertex3dv( topLeftFront );
	::glVertex3dv( bottomLeftFront );
	::glVertex3dv( bottomRightFront );
	::glVertex3dv( bottomRightBack);
	::glVertex3dv( topRightBack );
	::glVertex3dv( topRightFront );
	::glVertex3dv( bottomRightFront );
	::glEnd();

	::glBegin(GL_LINES);
	::glVertex3dv(bottomLeftBack);
	::glVertex3dv(bottomRightBack);

	::glVertex3dv(topLeftBack);
	::glVertex3dv(topRightBack);

	::glVertex3dv(topLeftFront);
	::glVertex3dv(topRightFront);
	::glEnd();	

	::glPopMatrix();
}