// Local includes
#include "general.h"
#include "CArgs.h"

// Maya includes
#include <maya/MFnDagNode.h>
#include <maya/MFnBlendShapeDeformer.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MObjectArray.h>
#include <maya/MStatus.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MGlobal.h>
#include <maya/MStringArray.h>
#include <maya/MFnMesh.h>
#include <maya/MIntArray.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnTransform.h>
#include <maya/MMatrix.h>
#include <maya/MFnReflectShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnBlinnShader.h>
#include <3d/texture_file.h>

NLMISC::CRGBA mColor2CRGBA(const MColor &c) {
	return NLMISC::CRGBA((uint8)(c.r * 255), (uint8)(c.g * 255), (uint8)(c.b * 255), (uint8)(c.a * 255));
}

std::vector<MObject> findBlendShapes(MObject &mesh) {
	// probably an ugly hack
	MItDependencyNodes it(MFn::kBlendShape); 
	MFnDagNode n(mesh);
	std::vector<MObject> vec;
	while(!it.isDone()) {
		MFnBlendShapeDeformer fn(it.item());
		MObjectArray affected;
		fn.getBaseObjects(affected);
		for(unsigned int i=0; i<affected.length(); ++i) {
				MFnDagNode a(affected[i]);
				if(affected[i] == mesh) {
					vec.push_back(fn.object());
				}
		}
		it.next(); 
	}
	return vec;
}

MString getMorphName(MObject &mesh) {
	MFnDagNode target(mesh);
	MDagPath path;
	target.getPath(path);
	MStatus status;
	MFnDagNode transform(path.transform(), &status);
	MString name = target.name();
	if(status)
		name = transform.name();
	return name;
}

MObject findShader(const MObject& setNode) {

	MFnDependencyNode fnNode(setNode);
	MPlug shaderPlug = fnNode.findPlug("surfaceShader");
			
	if (!shaderPlug.isNull()) {			
		MPlugArray connectedPlugs;

		MStatus status;
		shaderPlug.connectedTo(connectedPlugs, true, false, &status);
		if (MStatus::kFailure == status) {
			MGlobal::displayError("MPlug::connectedTo");
			return MObject::kNullObj;
		}

		if (1 != connectedPlugs.length()) {
			MGlobal::displayError("Error getting shader");
		} else {
			return connectedPlugs[0].node();
		}
	}
	
	return MObject::kNullObj;
}

MObject	findTexture (const MObject& shaderNode, const MString& channel) {
	
	MFnDependencyNode fnNode(shaderNode);
	MPlug channelPlug = fnNode.findPlug(channel);

	if(!channelPlug.isNull()) {
		MPlugArray connectedPlugs;
		MStatus status;
		channelPlug.connectedTo (connectedPlugs, true, false, &status);
		if(MStatus::kFailure == status) {
			MGlobal::displayError("MPlug::connectedTo");
			return MObject::kNullObj;
		}
		if(1 != connectedPlugs.length()) {
			if(CArgs::instance().verboseMode)
				printf("\t\tNo %s texture on %s\n", channel.asChar(), fnNode.name().asChar());
		} else {
			return connectedPlugs[0].node();
		}
	}
	return MObject::kNullObj;
}

bool isLOD(MDagPath path) {
	MFnDagNode node(path.transform());
	MPlug visPlug = node.findPlug("lodVisibility");
	MPlugArray conPlugs;
	visPlug.connectedTo(conPlugs, true, false);
	if(conPlugs.length() > 0) {
		if(conPlugs[0].node().hasFn(MFn::kLodGroup)) {
			return true;
		}
	}
	return false;
}

bool isBB(MDagPath path) {
	MFnDagNode node(path.transform());
	MStatus stat;
	node.findPlug("Billboard", &stat);
	if(stat == MStatus::kSuccess) {
		return true;
	}
	return false;
}

bool isBBCluster(MDagPath path) {
	MFnDagNode node(path);
	MStatus stat;
	node.findPlug("BBCluster", &stat);
	if(stat == MStatus::kSuccess) {
		return true;
	}
	return false;
}

bool isBBClusterSlot(MDagPath path) {
	MFnDagNode node(path);
	MStatus stat;
	node.findPlug("BBSlot", &stat);
	if(stat == MStatus::kSuccess) {
		return true;
	}
	return false;
}

bool isLeafCluster(MDagPath path) {
	MFnDagNode node(path);
	MStatus stat;
	node.findPlug("LeafCluster", &stat);
	if(stat == MStatus::kSuccess) {
		return true;
	}
	return false;
}

bool isLeafClusterSlot(MDagPath path) {
	MFnDagNode node(path);
	MStatus stat;
	node.findPlug("LeafSlot", &stat);
	if(stat == MStatus::kSuccess) {
		return true;
	}
	return false;
}

bool isCollisionMesh(MDagPath path) {
	MFnDagNode node(path.transform());
	MStatus stat;
	node.findPlug("NelCollisionMesh", &stat);
	if(stat == MStatus::kSuccess) {
		return true;
	}
	return false;
}

bool isVeg(MDagPath path) {
	MFnDagNode node(path.transform());
	MStatus stat;
	node.findPlug("NelVeg", &stat);
	if(stat == MStatus::kSuccess) {
		return true;
	}
	return false;
}

bool getColorAttribute(MString name, MObject object, MColor &c) {
	MFnDependencyNode node(object);
	if(!node.findPlug(name + "R").getValue(c.r))
		return false;
	if(!node.findPlug(name + "G").getValue(c.g))
		return false;
	if(!node.findPlug(name + "B").getValue(c.b))
		return false;
	if(!node.findPlug(name + "Alpha").getValue(c.a))
		c.a = 1.0f;
	return true;
}

MString dag2FilePath(MString path) {
	MString filePath("");
	MStringArray pathComponents;
	path.split('|', pathComponents);
	for(unsigned int i = 0; i < pathComponents.length(); i++) {
		if(i != 0)
			filePath += "_";
		filePath += pathComponents[i];
	}
	return filePath;
}

bool isWater(MDagPath path) {
	MFnMesh mesh(path);
	MObjectArray shaders;
	MIntArray shaderIndices;
	if (MStatus::kFailure == mesh.getConnectedShaders(path.instanceNumber(), shaders, shaderIndices)) {
		MGlobal::displayError("MFnMesh::getConnectedShaders"); 
		return false;
	}
	for(unsigned int i=0; i < shaders.length(); i++) {
		MObject surfaceShader = findShader(shaders[i]);
		if(surfaceShader != MObject::kNullObj) {
			MFnLambertShader shaderNode(surfaceShader);
			bool isWater = false;
			shaderNode.findPlug("NeLWater").getValue(isWater);
			return isWater;
		}
	}
	return false;
}

MDagPath getTransformPath(MDagPath path, MStatus &status) {
	MObject transformObject = path.transform();
	MDagPath transformPath(path);
	status = MStatus::kFailure;
	while(transformPath.length() > 0) {
		if(transformPath.node() == transformObject) {
			status = MStatus::kSuccess;
			break;
		} else
			transformPath.pop();
	}
	return transformPath;
}

MStatus getTransforms(MDagPath path,
				   double scale[3],
				   MQuaternion &rot,
				   MEulerRotation &rotE,
				   MVector &trans,
				   MPoint &pivot,
				   MSpace::Space spc) {
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

	MTransformationMatrix world = path.inclusiveMatrix();
	if ( world.getScale(scale, spc) == MStatus::kFailure) {
		MGlobal::displayError("MFnTransform::getScale");
		return MStatus::kFailure;
	}
	if( fTransform.getRotation(rot, spc) == MStatus::kFailure) {
		MGlobal::displayError("MFnTransform::getRotation(quat)");
		return MStatus::kFailure;
	}
	if( fTransform.getRotation(rotE) == MStatus::kFailure) {
		MGlobal::displayError("MFnTransform::getRotation(euler)");
		return MStatus::kFailure;
	}
	pivot = fTransform.scalePivot(MSpace::kTransform, &status);
	if(status == MStatus::kFailure) {
		MGlobal::displayError("MFnTransform::scalePivot");
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

bool isVisible(MFnDagNode & fnDag, MStatus& status)  {
	if(fnDag.isIntermediateObject())
		return false;

	MPlug visPlug = fnDag.findPlug("visibility", &status);
	if (MStatus::kFailure == status) {
		MGlobal::displayError("MPlug::findPlug");
		return false;
	} else {
		bool visible;
		status = visPlug.getValue(visible);
		if (MStatus::kFailure == status) {
			MGlobal::displayError("MPlug::getValue");
		}
		return visible;
	}
}
MStatus getMaterial(NL3D::CMaterial *mat, MFnMesh &mesh, MObject &shader) {
	bool bLit = true;
	float shininess = 0;
	MStatus status;
	MColor e, a, d, s, t;

	bool doubleSided = false;
	MObject sidedAttr = mesh.attribute("doubleSided", &status);
	if(status) {
		MPlug doubleSidedPlug(mesh.object(), sidedAttr);
		if(doubleSidedPlug.getValue(doubleSided)) {
			//something broken here - so setting to opposite bc Maya defaults to doublesided
			//it is likely that NeL reverses normals when set to doublesided. Have to look into that.
//			doubleSided = !doubleSided;
		}
	}


	mat->initLighted();
	mat->setDoubleSided(doubleSided);
	MObject surfaceShader = findShader(shader);
	if(surfaceShader != MObject::kNullObj) {
		MFnLambertShader shaderNode(surfaceShader);

		bool isNeLShader = false;
		shaderNode.findPlug("NeL").getValue(isNeLShader);

		if(CArgs::instance().verboseMode)
			printf("\tMaterial: %s\n", shaderNode.name().asChar());
		if(surfaceShader.hasFn(MFn::kReflect)) {
			MFnReflectShader shader(surfaceShader);
			e = shader.incandescence();
			MPlug channelPlug = shaderNode.findPlug("color");
			if(channelPlug.isConnected() && isNeLShader) {
				d = MColor(1, 1, 1, 1);
			} else {
				d = shader.color();
			}
			s = shader.specularColor();
			a = shader.ambientColor();
			t = shader.transparency();
			if(surfaceShader.hasFn(MFn::kPhong)) {
				MFnPhongShader phong(surfaceShader);
				shininess=phong.cosPower();
			} else if(surfaceShader.hasFn(MFn::kBlinn)) {
				MFnBlinnShader blinn(surfaceShader);
				shininess=(float)(-log(blinn.eccentricity()))*40.f; // an empiric solution
				s *= blinn.specularRollOff();
			} else {
				shininess = 50;
			}
		} else {
			e = shaderNode.incandescence();
			MPlug channelPlug = shaderNode.findPlug("color");
			if(channelPlug.isConnected() && isNeLShader) {
				d = MColor(1, 1, 1, 1);
			} else {
				d = shaderNode.color();
			}
			s = MColor(0, 0, 0, 0);
			a = shaderNode.ambientColor();
			t = shaderNode.transparency();
			shininess = 0;
		}
		if(isNeLShader) {
			if(CArgs::instance().verboseMode)
				printf("\t\tIs NeL shader\n");
			getNelShader(mat, shaderNode, bLit);
		} else {
			MObject texture = findTexture(surfaceShader, "color");
			if(texture.hasFn(MFn::kFileTexture) && !(mat->getShader()&(NL3D::CMaterial::UserColor|NL3D::CMaterial::LightMap))) {
				MFnDependencyNode image(texture);
				MObject fileAttr = image.attribute ("fileTextureName");
				MPlug textureFileNamePlug(image.object(), fileAttr);
				MString textureFileName;
				if(textureFileNamePlug.getValue(textureFileName)) {
					MGlobal::executeCommand("nelExportTexture \"" + CArgs::instance().filePath
						+ CArgs::instance().locTextures
						+ "\" \"" + textureFileName + "\";", true); 
					int substrI;
					substrI = textureFileName.rindex('\\');
					if(substrI<0)
						substrI = textureFileName.rindex('/');

					if(substrI>0)
						textureFileName = textureFileName.substring(substrI+1, textureFileName.length()-1);
					NL3D::ITexture *pTex;
					pTex = new NL3D::CTextureFile(textureFileName.asChar());
					mat->setTexture(0, pTex);
					if(mat->getShader() == NL3D::CMaterial::Normal) {
						mat->setTexCoordGen (0, false);
						mat->setTexCoordGenMode(0, NL3D::CMaterial::TexCoordGenReflect);
						mat->texEnvOpRGB(0, NL3D::CMaterial::Modulate);
						mat->texEnvArg0RGB(0, NL3D::CMaterial::Diffuse, NL3D::CMaterial::SrcColor);
						mat->texEnvArg1RGB(0, NL3D::CMaterial::Texture, NL3D::CMaterial::SrcColor);
						mat->texEnvOpAlpha(0, NL3D::CMaterial::Modulate);
						mat->texEnvArg0Alpha(0, NL3D::CMaterial::Diffuse, NL3D::CMaterial::SrcAlpha);
						mat->texEnvArg1Alpha(0, NL3D::CMaterial::Texture, NL3D::CMaterial::SrcAlpha);
					}
					d = MColor(1,1,1,1);
				}
			}
		}
	} else {
		if(CArgs::instance().verboseMode)
			printf("\tNo shader found\n");
		e = a = d = s = MColor(0,0,0,1);
	}
	mat->setColor(mColor2CRGBA(d));
	mat->setLighting(bLit, mColor2CRGBA(e), mColor2CRGBA(a), mColor2CRGBA(d), mColor2CRGBA(s), shininess); 
	mat->setOpacity(((1-t.r) * 255.0f));

	return MStatus::kSuccess;
}

void getNelShader(NL3D::CMaterial* mat, MFnLambertShader &shader, bool &bLit) {
	int shaderType;
	if(shader.findPlug("ShaderType").getValue(shaderType))
		mat->setShader((NL3D::CMaterial::TShader)shaderType);
	shader.findPlug("Lit").getValue(bLit);

	bool litVertexColor;
	if(shader.findPlug("LitVertexColor").getValue(litVertexColor))
		mat->setLightedVertexColor(litVertexColor);

	bool alphaBlend;
	if(shader.findPlug("AlphaBlend").getValue(alphaBlend))
		mat->setBlend(alphaBlend);
	
	int alphaSource;
	if(shader.findPlug("BlendSource").getValue(alphaSource))
		mat->setSrcBlend((NL3D::CMaterial::TBlend)alphaSource);

	int alphaDestination;
	if(shader.findPlug("BlendDestination").getValue(alphaDestination))
		mat->setDstBlend((NL3D::CMaterial::TBlend)alphaDestination);

	bool alphaTest;
	if(shader.findPlug("AlphaTest").getValue(alphaTest))
		mat->setAlphaTest(alphaTest);

	float alphaTestThreshold;
	if(shader.findPlug("AlphaTestThreshold").getValue(alphaTestThreshold))
		mat->setAlphaTestThreshold(alphaTestThreshold);

	int zFunction;
	if(shader.findPlug("ZFunction").getValue(zFunction))
		mat->setZFunc((NL3D::CMaterial::ZFunc)zFunction);

	bool zWrite;
	if(shader.findPlug("ZWrite").getValue(zWrite))
		mat->setZWrite(zWrite);

	float zBias;
	if(shader.findPlug("ZBias").getValue(zBias))
		mat->setZBias(zBias);

	if((NL3D::CMaterial::TShader)shaderType == NL3D::CMaterial::UserColor) {
		MColor userColor;
		if(getColorAttribute("UserColor", shader.object(), userColor))
			mat->setUserColor(mColor2CRGBA(userColor));
	}

	bool textureAddressing;
	if(shader.findPlug("EnableTextureAddressing").getValue(textureAddressing))
		mat->enableTexAddrMode(textureAddressing);

	NL3D::ITexture *pTex;


	MString lightMap("");
	if(shader.findPlug("LightMap").getValue(lightMap) && (lightMap.length() > 0) && ((NL3D::CMaterial::TShader)shaderType == NL3D::CMaterial::LightMap)) {
		if(CArgs::instance().verboseMode)
			printf("\t\tGot lightmap %s\n", lightMap.asChar());
		pTex = new NL3D::CTextureFile(lightMap.asChar());
		mat->setLightMap(0, pTex);
//		mat->setLMCColors(0, NLMISC::CRGBA(128, 128, 128), NLMISC::CRGBA(255, 255, 255));
//		mat->setLightMapMulx2(true);
	}

	int i = 0;

	for(i=0; i < 2; i++) {
		MString texture("Texture");
		texture += (i+1);
		if(shader.findPlug(texture).getValue(texture) && texture != "") {
			if(CArgs::instance().verboseMode)
				printf("\t\tGot texture %s\n", texture.asChar());
// TODO henri:henri Can't export because don't know the exact location...change this maybe?
//			MGlobal::executeCommand("nelExportTexture \"" + CArgs::instance().filePath
//				+ CArgs::instance().locTextures
//				+ "\" \"" + texture + "\";"); 
			pTex = new NL3D::CTextureFile(texture.asChar());
			mat->setTexture(i, pTex);
			MString t("Tex");
			t += (i+1);
			int value, value2;
			bool bValue;
			if(shader.findPlug(t+"CoordGen").getValue(bValue))
				mat->setTexCoordGen (i, bValue);
			if(shader.findPlug(t+"CoordGenMode").getValue(value))
				mat->setTexCoordGenMode(i, (NL3D::CMaterial::TTexCoordGenMode)value);
			if(shader.findPlug(t+"OperatorRGB").getValue(value))
				mat->texEnvOpRGB(i, (NL3D::CMaterial::TTexOperator)value);
			if(shader.findPlug(t+"Arg0RGBOperand").getValue(value) && shader.findPlug(t+"Arg0RGBSource").getValue(value2))
				mat->texEnvArg0RGB(i, (NL3D::CMaterial::TTexSource)value2, (NL3D::CMaterial::TTexOperand)value);
			if(shader.findPlug(t+"Arg1RGBOperand").getValue(value) && shader.findPlug(t+"Arg1RGBSource").getValue(value2))
				mat->texEnvArg1RGB(i, (NL3D::CMaterial::TTexSource)value2, (NL3D::CMaterial::TTexOperand)value);
			if(shader.findPlug(t+"OperatorAlpha").getValue(value))
				mat->texEnvOpAlpha(i, (NL3D::CMaterial::TTexOperator)value);
			if(shader.findPlug(t+"Arg0AlphaOperand").getValue(value) && shader.findPlug(t+"Arg0AlphaSource").getValue(value2))
				mat->texEnvArg0Alpha(i, (NL3D::CMaterial::TTexSource)value2, (NL3D::CMaterial::TTexOperand)(value+2));
			if(shader.findPlug(t+"Arg1AlphaOperand").getValue(value) && shader.findPlug(t+"Arg1AlphaSource").getValue(value2))
				mat->texEnvArg1Alpha(i, (NL3D::CMaterial::TTexSource)value2, (NL3D::CMaterial::TTexOperand)(value+2));
			MColor constantColor;
			if(getColorAttribute(t+"ConstantColor", shader.object(), constantColor))
				mat->texConstantColor(i, mColor2CRGBA(constantColor));
			if(textureAddressing) {
				if(shader.findPlug(t+"AddressingMode").getValue(value))
					mat->setTexAddressingMode(i, (NL3D::CMaterial::TTexAddressingMode)value);
			}
		}
	}
}

void getLocalIndices(MIntArray &localIndices, const MIntArray &objIndices, const MIntArray &polyIndices) {
	unsigned    v, o;
	bool bMatch;

	for (o=0; o < objIndices.length(); ++o) {
		bMatch = false;
		for (v=0; v < polyIndices.length(); ++v) {
			if (objIndices[o] == polyIndices[v]) {
				localIndices.append(v);
				bMatch = true;
				break;
			}
		}
		if (!bMatch) {
			localIndices.append(-1);
		}
	}
}

int findEdge(const MFnMesh &mesh, const int2 &vertices, MIntArray *edges) {
	bool created = false;
	if(edges == NULL) {
		edges = new MIntArray();
		created = true;
		for(int i = 0; i < mesh.numEdges(); i++)
			edges->append(i);
	}

	for(int i = 0; i < edges->length(); i++) {
		int2 verts;
		mesh.getEdgeVertices(edges->operator [](i), verts);
		if(verts[0] != vertices[0] && verts[0] != vertices[1])
			continue;
		if(verts[1] != vertices[0] && verts[1] != vertices[1])
			continue;
		int retval = edges->operator [](i);
		if(created)
			delete edges;
		return retval;
	}
	if(created)
		delete edges;
	return -1;
}
