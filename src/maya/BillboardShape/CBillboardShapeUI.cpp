// Author: Henri Kuuste

// Maya includes
#include <maya/MPoint.h>
#include <maya/MDagPath.h>
#include <maya/MMaterial.h>
#include <maya/MFnCamera.h>
#include <maya/MMatrix.h>
#include <maya/MSelectionMask.h>
#include <maya/MQuaternion.h>
#include <maya/MLightLinks.h>
#include <maya/MFnLight.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MItDag.h>

// Local includes
#include "CBillboardShapeUI.h"

// Global includes
#include <iostream>

// Globals
extern CGcontext shaderContext;
cgProgMap		vMap;
cgProgMap		fMap;


#define LEAD_COLOR			18	// green
#define ACTIVE_COLOR			15	// white
#define ACTIVE_AFFECTED_COLOR		8	// purple
#define DORMANT_COLOR			4	// blue
#define HILITE_COLOR			17	// pale blue

/////////////////////////////////////////////////////////////////////
// Shader constat code
/////////////////////////////////////////////////////////////////////
static const char* rayTraceCode =
"bool raySphereIntersect(out float disc, float3 start, float3 end, float3 center, float radsqr) { \n\
	bool intersect = true;														\n\
	start = center - start;														\n\
	disc = dot(end, start);														\n\
	float sqrlength = dot(start, start);										\n\
	if(disc < 0.0) {															\n\
		if(sqrlength < radsqr) {												\n\
			float tdisc = -disc;												\n\
			disc = radsqr - (sqrlength - tdisc*tdisc);							\n\
			disc = disc * (radsqr - (tdisc*tdisc));								\n\
			disc = disc / (radsqr*radsqr);										\n\
			intersect = true;													\n\
		} else {																\n\
			intersect = false;													\n\
		}																		\n\
	} else {																	\n\
		disc = radsqr - (sqrlength - disc*disc);								\n\
		if(disc < 0.0) {														\n\
			intersect = false;													\n\
		}																		\n\
		disc = disc / radsqr;													\n\
	}																			\n\
	return intersect;															\n\
}																				\n";

static const char* shadowCode =
"	float disc = 1;																\n\
	if(raySphereIntersect(disc, pos, lightPos, center[i], radius[i])) {			\n\
		disc = saturate(1 - (disc + (1-penumbra)));								\n\
		if(penumbra != 0 && disc != 0)											\n\
			disc = disc / penumbra;												\n\
		shadow = shadow * disc;													\n\
	}																			\n\
	}																			\n\
	shadow = shadow * rtranslucency + translucency;								\n\
	return shadow;																\n\
}																				\n";

static const char* vProgramStart =
"struct appin {								\n\
	float4 Position	:POSITION;				\n\
	float4 Normal	:NORMAL;				\n\
	float4 tex0		:TEXCOORD0;				\n\
};											\n\
struct vertout {							\n\
	float4 Color0	:COLOR0;				\n\
	float4 HPosition:POSITION;				\n\
	float4 tex0		:TEXCOORD0;				\n\
};											\n\
vertout main(appin IN,						\n\
			uniform float4x4 ModelViewProj,	\n\
			uniform float4x4 ModelViewIT,	\n\
			uniform float translucency,		\n\
			uniform float useFaceNormal,	\n\
			uniform float3 clusterPosition,	\n\
			uniform float penumbra,			\n\
			uniform float3 color,			\n";

static const char* vProgramMiddle =
"{												\n\
    vertout OUT;								\n\
	float3 normal = ((1 - useFaceNormal)*(IN.Position.xyz - clusterPosition) + (useFaceNormal * IN.Normal.xyz)); \n\
	normal = normalize(normal);					\n\
	float rtranslucency = (1 - translucency);	\n\
	float3 DiffuseColor = float3(0.0, 0.0, 0.0);\n";

static const char* vProgramEnd = 
"		float NdotL = dot(light, normal);		\n\
		NdotL = NdotL * rtranslucency + translucency;\n\
		DiffuseColor = DiffuseColor + LightColor[i] * shadow * saturate(NdotL);\n\
	}													\n\
	OUT.Color0 = float4(color * DiffuseColor, 1);		\n\
	OUT.HPosition = mul(ModelViewProj, IN.Position);	\n\
	OUT.tex0.xy = IN.tex0.xy;							\n\
	return OUT;											\n\
}														\n";

/////////////////////////////////////////////////////////////////////
// UI IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

CBillboardShapeUI::CBillboardShapeUI() {
	// TODO: should also test for GL_EXT_secondary_color here
    if (cgGLIsProfileSupported(CG_PROFILE_ARBVP1)) {
        vProfile = CG_PROFILE_ARBVP1;
    } else if (cgGLIsProfileSupported(CG_PROFILE_VP20)) {
        vProfile = CG_PROFILE_VP20;
    } else {
        printf("Vertex programming extensions (GL_ARB_vertex_program or "
			"GL_NV_vertex_program) not supported :(\n");
    }

}

CBillboardShapeUI::~CBillboardShapeUI() {
	cgProgMap::iterator iter = vMap.begin();
	for( ;iter != vMap.end(); iter++) {
		cgDestroyProgram(iter->second);
	}
}

void* CBillboardShapeUI::creator()
{
	return new CBillboardShapeUI();
}

/* override */
void CBillboardShapeUI::getDrawRequests( const MDrawInfo & info, bool /*objectAndActiveOnly*/, MDrawRequestQueue & queue ) {
	// The draw data is used to pass geometry through the 
	// draw queue. The data should hold all the information
	// needed to draw the shape.
	//
	MDrawData data;
	MDrawRequest request = info.getPrototype( *this );
	CBillboardShape* shapeNode = (CBillboardShape*)surfaceShape();
	CBillboardGeom* geom = shapeNode->geometry();
	getDrawData( geom, data );
	request.setDrawData( data );

	// Use display status to determine what color to draw the object
	//
	switch ( info.displayStyle() )
	{		 
		case M3dView::kWireFrame :
			getDrawRequestsWireframe( request, info );
			queue.add( request );
			break;
		
		case M3dView::kGouraudShaded :
			request.setToken( kDrawSmoothShaded );
			getDrawRequestsShaded( request, info, queue, data );
			queue.add( request );
			break;
		
		case M3dView::kFlatShaded :
			request.setToken( kDrawFlatShaded );
 			getDrawRequestsShaded( request, info, queue, data );
			queue.add( request );
			break;
		default:	
			break;
	}
}

/* override */
void CBillboardShapeUI::draw( const MDrawRequest & request, M3dView & view ) const { 	
	MDrawData data = request.drawData();
	CBillboardGeom * geom = (CBillboardGeom*)data.geometry();
	short token = request.token();
	bool drawTexture = false;
	MDagPath path = request.multiPath();
	MMatrix inverseWorld = path.inclusiveMatrixInverse();

	view.beginGL(); 

	if ( (token == kDrawSmoothShaded) || (token == kDrawFlatShaded) ) {
#if		defined(SGI) || defined(MESA)
		glEnable( GL_POLYGON_OFFSET_EXT );
#else
		glEnable( GL_POLYGON_OFFSET_FILL );
#endif
		// Set up the material
		//
		MMaterial material = request.material();
		material.setMaterial( request.multiPath(), /*request.isTransparent()*/ false );

		// Enable texturing
		//
		drawTexture = material.materialIsTextured();
		if ( drawTexture ) glEnable(GL_TEXTURE_2D);

		// Apply the texture to the current view
		//
		if ( drawTexture ) {
			material.applyTexture( view, data );
		}
	}


	glPushAttrib( GL_ENABLE_BIT );
	if ( (token == kDrawSmoothShaded) || (token == kDrawFlatShaded) ) {
		// temporary hacky solution
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5);
		glDisable(GL_BLEND);
	}

	switch( token )
	{
		case kDrawWireframe :
		case kDrawWireframeOnShaded :
			if ( drawTexture ) glDisable(GL_TEXTURE_2D);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			break;

		case kDrawSmoothShaded :
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;

		case kDrawFlatShaded :
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
	}

	MVector x, y, z;
	MPoint pos(0, 0, 0);
	createBillboard(inverseWorld, view, geom, x, y, z);
	MPointArray quad;
	createQuad(quad, x, y, z, pos);
	MPoint center = geom->clusterCenter * inverseWorld;

	MColor color;
	if(geom->useBBColor) {
		color = geom->color;
	} else {
		color = geom->globalColor;
	}

	boolean vProgramActive = false;
	if ( geom->isLeaf && ((token == kDrawSmoothShaded) || (token == kDrawFlatShaded)) ) {
		CBillboardShapeUI* nonConstThis = const_cast <CBillboardShapeUI*> (this);
		CGprogram prog = nonConstThis->composeVertexProgram(view, request, inverseWorld);
		if(prog != NULL) {
			CGparameter translucency = cgGetNamedParameter(prog, "translucency");
			CGparameter penumbra = cgGetNamedParameter(prog, "penumbra");
			CGparameter useFaceNormal = cgGetNamedParameter(prog, "useFaceNormal");
			CGparameter clusterPosition = cgGetNamedParameter(prog, "clusterPosition");
			CGparameter colorParam = cgGetNamedParameter(prog, "color");
			cgGLSetParameter1f(translucency, geom->translucency);
			cgGLSetParameter1f(useFaceNormal, geom->faceNormal);
			cgGLSetParameter3f(clusterPosition, center.x, center.y, center.z);
			cgGLSetParameter3f(colorParam, color.r, color.g, color.b);
			cgGLSetParameter1f(penumbra, geom->penumbra);

			cgGLBindProgram(prog);
			cgGLEnableProfile(vProfile);
			vProgramActive = true;
		}
	}
	glBegin(GL_QUADS);
		glNormal3f((GLfloat)y.x, (GLfloat)y.y, (GLfloat)y.z);
		glTexCoord2f(0, 0); glVertex3f((GLfloat)quad[0].x, (GLfloat)quad[0].y, (GLfloat)quad[0].z);
		glTexCoord2f(1, 0); glVertex3f((GLfloat)quad[1].x, (GLfloat)quad[1].y, (GLfloat)quad[1].z);
		glTexCoord2f(1, 1); glVertex3f((GLfloat)quad[2].x, (GLfloat)quad[2].y, (GLfloat)quad[2].z);
		glTexCoord2f(0, 1); glVertex3f((GLfloat)quad[3].x, (GLfloat)quad[3].y, (GLfloat)quad[3].z);
	glEnd();

	if(vProgramActive) {
		cgGLDisableProfile(vProfile);
	}
	// Turn off texture mode
	//
	glPopAttrib();

	if ( drawTexture ) glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);

	view.endGL(); 
}

/* override */
bool CBillboardShapeUI::select( MSelectInfo &selectInfo, MSelectionList &selectionList, MPointArray &worldSpaceSelectPts ) const {
	MVector x, y, z;
	M3dView view = selectInfo.view();
	CBillboardGeom *geom = ((CBillboardShape*)surfaceShape())->geometry();
	MDagPath path = selectInfo.selectPath();
	MMatrix inverseWorld = path.inclusiveMatrixInverse();
	
	createBillboard(inverseWorld, view, geom, x, y, z);
	MPointArray quad;
	createQuad(quad, x, y, z, MPoint(0, 0, 0));
	MPoint rayOrigin;
	MVector rayDirection;
	selectInfo.getLocalRay(rayOrigin, rayDirection);
	MPoint xformedPt;
	if(intersectQuadrilateralRay(quad, rayOrigin, rayDirection, xformedPt)) {
		xformedPt *= path.inclusiveMatrix();
        MSelectionMask priorityMask( MSelectionMask::kSelectObjectsMask );
		MSelectionList item;
		item.add( selectInfo.selectPath() );
		selectInfo.addSelection( item, xformedPt, selectionList, worldSpaceSelectPts, priorityMask, false );
		return true;
	}
	return false;
}

void CBillboardShapeUI::getDrawRequestsWireframe( MDrawRequest& request, const MDrawInfo& info ) {
	request.setToken( kDrawWireframe );

	M3dView::DisplayStatus displayStatus = info.displayStatus();
	M3dView::ColorTable activeColorTable = M3dView::kActiveColors;
	M3dView::ColorTable dormantColorTable = M3dView::kDormantColors;
	switch ( displayStatus )
	{
		case M3dView::kLead :
			request.setColor( LEAD_COLOR, activeColorTable );
			break;
		case M3dView::kActive :
			request.setColor( ACTIVE_COLOR, activeColorTable );
			break;
		case M3dView::kActiveAffected :
			request.setColor( ACTIVE_AFFECTED_COLOR, activeColorTable );
			break;
		case M3dView::kDormant :
			request.setColor( DORMANT_COLOR, dormantColorTable );
			break;
		case M3dView::kHilite :
			request.setColor( HILITE_COLOR, activeColorTable );
			break;
		default:	
			break;
	}
}

void CBillboardShapeUI::getDrawRequestsShaded( MDrawRequest& request, const MDrawInfo& info, MDrawRequestQueue& queue, MDrawData& data ) {
	// Need to get the material info
	//
	MDagPath path = info.multiPath();	// path to your dag object 
	M3dView view = info.view();; 		// view to draw to
	MMaterial material = MPxSurfaceShapeUI::material( path );
	M3dView::DisplayStatus displayStatus = info.displayStatus();

	// Evaluate the material and if necessary, the texture.
	//
	if ( ! material.evaluateMaterial( view, path ) ) {
		std::cerr << "Couldnt evaluate\n";
	}

	bool drawTexture = true;
	if ( drawTexture && material.materialIsTextured() ) {
		material.evaluateTexture( data );
	}

	request.setMaterial( material );

	bool materialTransparent = false;
	material.getHasTransparency( materialTransparent );
	if ( materialTransparent ) {
		request.setIsTransparent( true );
	}
	
	// create a draw request for wireframe on shaded if
	// necessary.
	//
	if ( (displayStatus == M3dView::kActive) ||
		 (displayStatus == M3dView::kLead) ||
		 (displayStatus == M3dView::kHilite) )
	{
		MDrawRequest wireRequest = info.getPrototype( *this );
		wireRequest.setDrawData( data );
		getDrawRequestsWireframe( wireRequest, info );
		wireRequest.setToken( kDrawWireframeOnShaded );
		wireRequest.setDisplayStyle( M3dView::kWireFrame );
		queue.add( wireRequest );
	}
}

CGprogram CBillboardShapeUI::composeVertexProgram(M3dView & view, const MDrawRequest& request, MMatrix &inverseWorld) {
	MDrawData data = request.drawData();
	CBillboardGeom * geom = (CBillboardGeom*)data.geometry();

	M3dView::LightingMode mode;
	view.getLightingMode(mode);
	if(mode == M3dView::kLightDefault) {
		numLights = 1;
	} else {
		view.getLightCount(numLights, false);
	}

	if(numLights == 0) {
		return NULL;
	}

	unsigned int numShadowCasters;
	if(geom->useCastShadows) {
		numShadowCasters = std::min(geom->casterPositions.length(), geom->casterSizes.length());
	} else {
		numShadowCasters = 0;
	}

	unsigned int code = numLights*100 + numShadowCasters;

	cgProgMap::iterator iter = vMap.find(code);
	CGprogram prog;
	if(iter == vMap.end()) {
		// Stupid hardcoded crap - NEED THE SHADER SYSTEM!
		MString vShad;
		if(numShadowCasters > 0) {
			vShad += rayTraceCode;
			vShad += "float getShadow(float3 pos, float3 lightPos, float translucency, float rtranslucency, float penumbra,";
			vShad += MString("float3 center[") + numShadowCasters + "],";
			vShad += MString("float radius[") + numShadowCasters + "]) {\n";
			vShad += "float shadow = 1;\n";
			vShad += MString("for(int i = 0; i < ") + numShadowCasters + "; i++) {\n";
			vShad += shadowCode;
		}
		vShad += vProgramStart;
		vShad += MString("uniform float3 LightVector[") + numLights + "],\n";
		vShad += MString("uniform float3 LightColor[") + numLights + "]";
		if(numShadowCasters > 0) {
			vShad += MString(",\nuniform float3 CasterPositions[") + numShadowCasters + "],\n";
			vShad += MString("uniform float CasterRadii[") + numShadowCasters + "]";
		}
		vShad += ")\n";
		vShad += vProgramMiddle;
		vShad += MString("for(int i = 0; i < ") + numLights + "; i++) {\n";
		vShad += "float3 light = LightVector[i] - IN.Position.xyz;\n";
		vShad += "light = normalize(light);\n";

		if(numShadowCasters > 0)
			vShad += "float shadow = getShadow(IN.Position.xyz, light, translucency, rtranslucency, penumbra, CasterPositions, CasterRadii);\n";
		else
			vShad += "float shadow = 1;\n";

		vShad += vProgramEnd;
		printf("Compiling shader for %i lights and %i casters\n", numLights, numShadowCasters);
		prog = cgCreateProgram(shaderContext, CG_SOURCE, vShad.asChar(), vProfile, "main", NULL);
		cgGLLoadProgram(prog);
		vMap[code] = prog;
	} else {
		prog = iter->second;
	}

	CGparameter modelViewProj = cgGetNamedParameter(prog, "ModelViewProj");
	CGparameter modelViewIT = cgGetNamedParameter(prog, "ModelViewIT");
	cgGLSetStateMatrixParameter(modelViewProj, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
	cgGLSetStateMatrixParameter(modelViewIT, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);

	CGparameter lightVector = cgGetNamedParameter(prog, "LightVector");
	CGparameter lightColor = cgGetNamedParameter(prog, "LightColor");

	if(mode == M3dView::kLightDefault) {
		MDagPath camera;
		view.getCamera(camera);
		MTransformationMatrix mat(camera.inclusiveMatrix());
		MVector pos = mat.translation(MSpace::kWorld) * inverseWorld;
		cgGLSetParameter3f(cgGetArrayParameter(lightVector, 0), pos.x, pos.y, pos.z);
		cgGLSetParameter3f(cgGetArrayParameter(lightColor, 0), 1.0, 1.0, 1.0);
	} else {
		for(unsigned int i = 0; i < numLights; i++) {
			MDagPath lightPath;
			view.getLightPath(i, lightPath);
			MFnLight light(lightPath);
			MColor color = light.color();
			MTransformationMatrix mat(lightPath.inclusiveMatrix());
			MVector pos = mat.translation(MSpace::kWorld) * inverseWorld;
			cgGLSetParameter3f(cgGetArrayParameter(lightVector, i), pos.x, pos.y, pos.z);
			cgGLSetParameter3f(cgGetArrayParameter(lightColor, i), color.r, color.g, color.b);
		}
	}
	if(numShadowCasters > 0) {
		CGparameter casterPositions = cgGetNamedParameter(prog, "CasterPositions");
		CGparameter casterRadii = cgGetNamedParameter(prog, "CasterRadii");
		for(unsigned int i = 0; i < numShadowCasters; i++) {
			MPoint pos = geom->casterPositions[i] * inverseWorld;
			cgGLSetParameter3f(cgGetArrayParameter(casterPositions, i), pos.x, pos.y, pos.z);
			cgGLSetParameter1f(cgGetArrayParameter(casterRadii, i), geom->casterSizes[i]*geom->casterSizes[i]);
		}
	}
	return prog;
}	

CGprogram CBillboardShapeUI::composeFragmentProgram(){
	// Implement someday? Think about it...per pixel shadows..mmmmm
	return NULL;
}

void CBillboardShapeUI::createBillboard(MMatrix &inverseWorld, M3dView &view, CBillboardGeom *geom, MVector &x, MVector &y, MVector &z) const {
	MDagPath camera;
	view.getCamera(camera);
	MFnCamera fnCamera(camera);

	MVector up = fnCamera.upDirection(MSpace::kWorld);
	up.normalize();
	MVector right = fnCamera.rightDirection(MSpace::kWorld);
	right.normalize();
	MVector normal = fnCamera.viewDirection(MSpace::kWorld);
	normal.normalize();
	MVector camPos(fnCamera.eyePoint(MSpace::kWorld));
	
	up *= inverseWorld;
	right *= inverseWorld;
	normal *= inverseWorld;
	normal = normal*(-1);
	camPos *= inverseWorld;

	MPoint center = geom->clusterCenter * inverseWorld;

	bool localAlign = false;
	bool adjustForSize = false;
        
	y = normal;
	if(geom->alignType == kScreenAlign) {
		if(!geom->useBBSize) {
			x = right * geom->globalRadiusX;
			z = up * geom->globalRadiusY;
		} else {
			adjustForSize = true;
		}
	} else if(geom->alignType == kAutoAlign) {
		if(camPos.length() < (geom->clusterRadius + std::max(geom->globalRadiusX, geom->globalRadiusY))) {
			localAlign = true;
		} else {
			if(!geom->useBBSize) {
				x = right * geom->globalRadiusX;
				z = up * geom->globalRadiusY;
			} else {
				adjustForSize = true;
			}
		}
	}

	if(adjustForSize) {
		x = right * geom->radiusX;
		z = up * geom->radiusY;
	} else if((geom->alignType == kViewAlign) || localAlign) {
		y = -camPos;
		y.normalize();
		x = y^up;
		x.normalize();
		z = x^y;
		z.normalize();
		if(geom->useBBSize) {
			x *= geom->radiusX;
			z *= geom->radiusY;
		} else {
			x *= geom->globalRadiusX;
			z *= geom->globalRadiusY;
		}
	} else if(geom->alignType == kAxisConstrain) {
		z = geom->constrainAxis;
		x = normal^z;
		x.normalize();
		if(geom->useBBSize) {
			x *= geom->radiusX;
			z *= geom->radiusY;
		} else {
			x *= geom->globalRadiusX;
			z *= geom->globalRadiusY;
		}
		y = x^z;
		y.normalize();
	}

	if(geom->useOrientation) {
		MQuaternion xQ(x.x, x.y, x.z, 0);
		MQuaternion zQ(z.x, z.y, z.z, 0);
		MQuaternion rotQ(geom->orientation, y);
		MQuaternion rotQConj(rotQ.conjugate());
		xQ = rotQ*xQ*rotQConj;
		zQ = rotQ*zQ*rotQConj;
		x.x = xQ.x; x.y = xQ.y; x.z = xQ.z;
		z.x = zQ.x; z.y = zQ.y; z.z = zQ.z;
	}

	if(geom->useRadialNormals) {
		y = (-1)*center;
		y.normalize();
	}
}
void CBillboardShapeUI::createQuad(MPointArray &quad, MVector &x, MVector &y, MVector &z, MPoint pos) const {
		MVector l(pos);  l = l - z; 
		MVector r(l); r = r + x; 
		l = l - x;
		MPoint v1(l); quad.append(v1);
		MPoint v2(r); quad.append(v2);
		l = l + z;  l = l + z;
		r = r + z;  r = r + z;
		MPoint v3(r); quad.append(v3);
		MPoint v4(l); quad.append(v4);
}
bool CBillboardShapeUI::intersectQuadrilateralRay(const MPointArray &quad, MPoint origin, MVector direction, MPoint &intersection) const {
  static const double eps = 10e-6;
  
  // Reject rays that are parallel to Q, and rays that intersect the plane
  // of Q either on the left of the line V00V01 or below the line V00V10.  
  
  MVector E_01 = quad[1] - quad[0];
  MVector E_03 = quad[3] - quad[0];
  MVector P = direction^E_03;
  double det = E_01*P;
  if ((det<0?-det:det) < eps) return false;
  MVector T = origin - quad[0];
  double alpha = (T*P) / det;
  if (alpha < 0.0) return false;
  MVector Q = T^E_01;
  double beta = (direction*Q) / det;
  if (beta < 0.0) return false; 
    
  if ((alpha + beta) > 1.0)
  {
    // Reject rays that that intersect the plane of Q either on
    // the right of the line V11V10 or above the line V11V00.    
    
    MVector E_23 = quad[1] - quad[2];
    MVector E_21 = quad[3] - quad[2];
    MVector P_prime = direction^E_21;
    double det_prime = E_23*P_prime;
	if ((det_prime<0?-det_prime:det_prime) < eps) return false;
    MVector T_prime = origin - quad[2];
    double alpha_prime = (T_prime*P_prime) / det_prime;
    if (alpha_prime < 0.0) return false;
    MVector Q_prime = T_prime^E_23;
    double beta_prime = (direction*Q_prime) / det_prime;
    if (beta_prime < 0.0) return false;    
  }
  
  // Compute the ray parameter of the intersection point, and
  // reject the ray if it does not hit Q.
  
  double t = (E_03*Q) / det;
  if (t < 0.0) return false;

  intersection = origin + (t * direction);
  
  return true;
}