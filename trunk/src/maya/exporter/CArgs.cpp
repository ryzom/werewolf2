// Local includes
#include "CArgs.h"
#include <cstdio>

// Maya includes
#include <maya/MStringArray.h>
#include <maya/MGlobal.h>

CArgs::CArgs() {
	reset();
	    
	mParserMap["verbose"		] = &CArgs::parseVerbose;
	mParserMap["dump"			] = &CArgs::parseDump;

	mParserMap["exportMesh"		] = &CArgs::parseMesh;
	mParserMap["exportSkeleton"	] = &CArgs::parseSkeleton;
	mParserMap["exportVBA"		] = &CArgs::parseVBA;
	mParserMap["exportNormals"	] = &CArgs::parseNormals;
	mParserMap["exportColours"	] = &CArgs::parseColours;
	mParserMap["exportColors"	] = &CArgs::parseColours;
	mParserMap["exportUVs"		] = &CArgs::parseUVs;
	mParserMap["exportAnimation"] = &CArgs::parseAnimation;
	mParserMap["exportWater"	] = &CArgs::parseWater;
	mParserMap["exportLOD"		] = &CArgs::parseLOD;
	mParserMap["exportZone"		] = &CArgs::parseZone;
	mParserMap["exportIG"		] = &CArgs::parseIG;
	mParserMap["exportCollision"] = &CArgs::parseCollision;
	mParserMap["exportBillboard"] = &CArgs::parseBillboard;
	mParserMap["exportLeaf"		] = &CArgs::parseLeaf;
	mParserMap["exportVeget"	] = &CArgs::parseVeget;

	mParserMap["locMeshes"		] = &CArgs::parseLocMeshes;
	mParserMap["locSkeletons"	] = &CArgs::parseLocSkeletons;
	mParserMap["locAnimations"	] = &CArgs::parseLocAnimations;
	mParserMap["locWater"		] = &CArgs::parseLocWater;
	mParserMap["locZones"		] = &CArgs::parseLocZones;
	mParserMap["locIG"			] = &CArgs::parseLocIG;
	mParserMap["locCollisions"	] = &CArgs::parseLocCollisions;
	mParserMap["locBillboards"	] = &CArgs::parseLocBillboards;
	mParserMap["locLeaves"		] = &CArgs::parseLocLeaves;
	mParserMap["locVegets"		] = &CArgs::parseLocVegets;
	mParserMap["locTextures"	] = &CArgs::parseLocTextures;
}

void CArgs::reset() {
	mValid			= false;
	verboseMode		= false;
	dumpMode		= false;
    
	exportMesh		= false;
	exportSkeleton	= false;
	exportVBA		= false;
	exportNormals	= false;
	exportColours	= false;
	exportUVs		= false;
	exportAnimation = false;
	exportWater		= false;
	exportLOD		= false;
	exportZone		= false;
	exportIG		= false;
	exportCollision	= false;
	exportBillboard	= false;
	exportLeaf		= false;
	exportVeget		= false;
	conversionMatrix.setToIdentity();

	mArgs.clear();
	fileName.clear();
	filePath.clear();
	locMeshes.clear();
	locZones.clear();
	locIG.clear();
	locCollisions.clear();
	locBillboards.clear();
	locLeaves.clear();
	locSkeletons.clear();
	locAnimations.clear();
	locWater.clear();
	locVegets.clear();
	locTextures.clear();
}

void CArgs::init(MString args) {
	conversionMatrix = MMatrix::identity;
	scaleConversionMatrix = MMatrix::identity;
	if(MGlobal::isYAxisUp()) {
		MMatrix zupMatrix;
		zupMatrix.matrix[0][0] = -1;
		zupMatrix.matrix[1][1] =  0;
		zupMatrix.matrix[1][2] =  1;
		zupMatrix.matrix[2][1] =  1;
		zupMatrix.matrix[2][2] =  0;
		conversionMatrix = conversionMatrix*zupMatrix;
		scaleConversionMatrix = conversionMatrix;
		scaleConversionMatrix.matrix[0][0] = -conversionMatrix.matrix[0][0];
	}
	mArgs = args;
	if(mArgs.length() > 0) {
		MStringArray argList;
		MStringArray arg;
		mArgs.split(';', argList);

		unsigned nArgs = argList.length();
		for (unsigned i = 0; i < nArgs; i++) {
			arg.clear();
			argList[i].split('=', arg);
			if (arg.length() < 2) {
				continue;
			}
			void (CArgs::*p)(MString) = mParserMap[arg[0].asChar()];
			if(p) {
				(this->*p)(arg[1]);
			}

		}
	}
}

void CArgs::debugOutput() {
	printf("=== Arguments ===============================\n");
	printf("file name       : %s\n", fileName.asChar());
	printf("file path       : %s\n", filePath.asChar());
	printf("\n****Flags****\n");
	printf("dump            : %s\n", (dumpMode ? "true" : "false"));
	printf("export mesh     : %s\n", (exportMesh ? "true" : "false"));
	printf("export skeleton : %s\n", (exportSkeleton ? "true" : "false"));
	printf("export normals  : %s\n", (exportNormals ? "true" : "false"));
	printf("export colours  : %s\n", (exportColours ? "true" : "false"));
	printf("export UVs      : %s\n", (exportUVs ? "true" : "false"));
	printf("export VBA      : %s\n", (exportVBA ? "true" : "false"));
	printf("export Animation: %s\n", (exportAnimation ? "true" : "false"));
	printf("export Water    : %s\n", (exportWater ? "true" : "false"));
	printf("export LOD      : %s\n", (exportLOD ? "true" : "false"));
	printf("export Zone     : %s\n", (exportZone ? "true" : "false"));
	printf("export IG       : %s\n", (exportIG ? "true" : "false"));
	printf("export Collision: %s\n", (exportCollision ? "true" : "false"));
	printf("export Billboard: %s\n", (exportBillboard ? "true" : "false"));
	printf("export Leaf     : %s\n", (exportLeaf ? "true" : "false"));
	printf("export Veget    : %s\n", (exportVeget ? "true" : "false"));
	printf("\n****Locations****\n");
	printf("Meshes          : %s%s\n", filePath.asChar(), locMeshes.asChar());
	printf("Skeletons       : %s%s\n", filePath.asChar(), locSkeletons.asChar());
	printf("Animations      : %s%s\n", filePath.asChar(), locAnimations.asChar());
	printf("Water           : %s%s\n", filePath.asChar(), locWater.asChar());
	printf("Zones           : %s%s\n", filePath.asChar(), locZones.asChar());
	printf("IG              : %s%s\n", filePath.asChar(), locIG.asChar());
	printf("Collisions      : %s%s\n", filePath.asChar(), locCollisions.asChar());
	printf("Billboards      : %s%s\n", filePath.asChar(), locBillboards.asChar());
	printf("Leaves          : %s%s\n", filePath.asChar(), locLeaves.asChar());
	printf("Vegets          : %s%s\n", filePath.asChar(), locVegets.asChar());
	printf("Textures		: %s%s\n", filePath.asChar(), locTextures.asChar());
	printf("=============================================\n\n");
}

void CArgs::setFile(MFileObject file) {
	mFile = file;
	#if defined (OSMac_)
		char nameBuffer[MAXPATHLEN];
		strcpy (nameBuffer, file.name().asChar());
		convertFileRepresentation (nameBuffer, kCFURLPOSIXPathStyle, kCFURLHFSPathStyle);
		fileName(nameBuffer);
		strcpy (nameBuffer, file.path().asChar());
		convertFileRepresentation (nameBuffer, kCFURLPOSIXPathStyle, kCFURLHFSPathStyle);
		filePath(nameBuffer);
	#else
		fileName = file.name();
		filePath = file.path();
	#endif
}

void CArgs::parseVerbose(MString value) {
	verboseMode = (value.asInt()) ? true : false;
}

void CArgs::parseDump(MString value) {
	dumpMode = (value.asInt()) ? true : false;
}

void CArgs::parseMesh(MString value) {
	exportMesh = (value.asInt()) ? true : false;
}

void CArgs::parseSkeleton(MString value) {
	exportSkeleton = (value.asInt()) ? true : false;
}

void CArgs::parseVBA(MString value) {
	exportVBA = (value.asInt()) ? true : false;
}

void CArgs::parseNormals(MString value) {
	exportNormals = (value.asInt()) ? true : false;
}

void CArgs::parseColours(MString value) {
	exportColours = (value.asInt()) ? true : false;
}

void CArgs::parseUVs(MString value) {
	exportUVs = (value.asInt()) ? true : false;
}

void CArgs::parseAnimation(MString value) {
	exportAnimation = (value.asInt()) ? true : false;
}

void CArgs::parseWater(MString value) {
	exportWater = (value.asInt()) ? true : false;
}

void CArgs::parseLOD(MString value) {
	exportLOD = (value.asInt()) ? true : false;
}

void CArgs::parseZone(MString value) {
	exportZone = (value.asInt()) ? true : false;
}

void CArgs::parseIG(MString value) {
	exportIG = (value.asInt()) ? true : false;
}

void CArgs::parseCollision(MString value) {
	exportCollision = (value.asInt()) ? true : false;
}

void CArgs::parseBillboard(MString value) {
	exportBillboard = (value.asInt()) ? true : false;
}

void CArgs::parseLeaf(MString value) {
	exportLeaf = (value.asInt()) ? true : false;
}

void CArgs::parseVeget(MString value) {
	exportVeget = (value.asInt()) ? true : false;
}

void CArgs::parseLocMeshes(MString value) {
	locMeshes = correctPath(value);
	createFolder(locMeshes);
}

void CArgs::parseLocSkeletons(MString value) {
	locSkeletons = correctPath(value);
	createFolder(locSkeletons);
}

void CArgs::parseLocAnimations(MString value) {
	locAnimations = correctPath(value);
	createFolder(locAnimations);
}

void CArgs::parseLocWater(MString value) {
	locWater = correctPath(value);
	createFolder(locWater);
}

void CArgs::parseLocZones(MString value) {
	locZones = correctPath(value);
	createFolder(locZones);
}

void CArgs::parseLocIG(MString value) {
	locIG = correctPath(value);
	createFolder(locIG);
}

void CArgs::parseLocCollisions(MString value) {
	locCollisions = correctPath(value);
	createFolder(locCollisions);
}

void CArgs::parseLocBillboards(MString value) {
	locBillboards = correctPath(value);
	createFolder(locBillboards);
}

void CArgs::parseLocLeaves(MString value) {
	locLeaves = correctPath(value);
	createFolder(locLeaves);
}

void CArgs::parseLocVegets(MString value) {
	locVegets = correctPath(value);
	createFolder(locVegets);
}

void CArgs::parseLocTextures(MString value) {
	locTextures = correctPath(value);
	createFolder(locTextures);
}

MString CArgs::correctPath(MString value) {
	if(value.length() > 0) {
		char end = value.asChar()[value.length()-1];
		if(end != '/' && end != '\\') {
			return value + "/";
		}
		return value;
	}
	return value;
}

void CArgs::createFolder(MString value) {
	if(value.length() > 0) {
		MGlobal::executeCommand(MString("sysFile -makeDir \"" + filePath + value + "\";"));
	}
}