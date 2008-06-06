// Local includes
#include "CNeLZoneWriter.h"
#include "CArgs.h"
#include "general.h"

// NeL includes
#include <nel/misc/file.h>

// Maya includes
#include <maya/MPoint.h>
#include <maya/MPlugArray.h>
#include <maya/MFnDagNode.h>
#include <maya/MColor.h>

// CVs in Maya
// (3,3) (3,2) (3,1) (3,0)
// (2,3) (2,2) (2,1) (2,0)
// (1,3) (1,2) (1,1) (1,0)
// (0,3) (0,2) (0,1) (0,0)

// Names in NeL docs
// C  CB BC B
// CD IC IB BA
// DC ID IA AB
// D  DA AD A

// Indices in NeL
// Vertices  Tangents  Interiors  Result
// 2 - - 1   - 3 2 -   - - - -    2 3 2 1
// - - - -   4 - - 1   - 2 1 -    4 2 1 1
// - - - -   5 - - 0   - 3 0 -    5 3 0 0
// 3 - - 0   - 6 7 -   - - - -    3 6 7 0

// The Resulting map :)
unsigned int patchCVMap[4][4] = {{0, 7, 6, 3},
								 {0, 0, 3, 5},
								 {1, 1, 2, 4},
								 {1, 2, 3, 2}};

// Old map - wrong way around (clockwise)
//unsigned int patchCVMap[4][4] = {{0, 0, 1, 1},
//								 {7, 0, 1, 2},
//								 {6, 3, 2, 3},
//								 {3, 5, 4, 2}};

unsigned int neighbourEdgeMap[4] = {2, 3, 0, 1};
unsigned int neighbourPatchMap[4][2] = {{0, -1}, {1, 0}, {0, 1}, {-1, 0}};
// this was wrong as well (clockwise)
//unsigned int neighbourPatchMap[4][2] = {{1, 0}, {0, 1}, {1, 0}, {0, -1}};

CNeLZoneWriter::CNeLZoneWriter(const MDagPath path) {
	mPath = path;
	mbHasPatchPainter = false;
	mSurface.setObject(path);
	mbHasPatchPainter = getPatchPainter();
	extractPatches();
	makeZone();
}

CNeLZoneWriter::~CNeLZoneWriter() {
	// Does nothing
}

MStatus CNeLZoneWriter::writeToFile(MString f) {
	NLMISC::COFile file;
	f += ".zone";
	if(file.open(f.asChar())) {
		try {
			mZone.serial (file);
		}
		catch(...) {
			return MStatus::kFailure;
		}
		file.close ();
	}
	return MStatus::kSuccess;
}

void CNeLZoneWriter::displayCV(unsigned int uPatch, unsigned int vPatch, unsigned int u, unsigned int v) {
	MPoint pt;
	mSurface.getCV((uPatch*3)+u, (vPatch*3)+v, pt, MSpace::kWorld);
	printf("\t\t\t%i : [%i][%i] : [%i][%i] : %f %f %f\n", patchCVMap[u][v], u, v, (uPatch*3)+u, (vPatch*3)+v, pt);
}

void CNeLZoneWriter::insertCV(unsigned int uPatch, unsigned int vPatch, unsigned int u, unsigned int v, NLMISC::CVector *vectorArray) {
	MPoint pt;
	mSurface.getCV((uPatch*3)+u, (vPatch*3)+v, pt, MSpace::kWorld);
	vectorArray[patchCVMap[u][v]] = NLMISC::CVector(pt.x, pt.y, pt.z);
}

void CNeLZoneWriter::extractPatches() {
	if(CArgs::instance().verboseMode)
		printf("\tThe zone has %i patches\n", mSurface.numPatches());
	for(unsigned int uPatch = 0; uPatch < mSurface.numPatchesInU(); uPatch++) {
		for(unsigned int vPatch = 0; vPatch < mSurface.numPatchesInV(); vPatch++) {
			extractPatch(uPatch, vPatch);
		}
	}
}

void CNeLZoneWriter::extractPatch(unsigned int uPatch, unsigned int vPatch) {
	if(CArgs::instance().dumpMode)
		printf("\tPatch %i %i\n", uPatch, vPatch);

	NL3D::CPatchInfo patchInfo;

	unsigned int u;
	unsigned int v;
	unsigned int count = 0;
	if(CArgs::instance().dumpMode)
		printf("\t\tVertices:\n");
	for(u = 0; u < 4; u++) {
		for(v = 0; v < 4; v++) {
			if((u%3 == 0) && (v%3 == 0)) {
				if(CArgs::instance().dumpMode)
					displayCV(uPatch, vPatch, u, v);
				insertCV(uPatch, vPatch, u, v, patchInfo.Patch.Vertices);
				unsigned int uCV, vCV;
				uCV = (uPatch*3)+u;
				vCV = (vPatch*3)+v;
				patchInfo.BaseVertices[patchCVMap[u][v]] = ((uCV/3)*(mSurface.numPatchesInV()+1)) + (vCV/3);
				if(CArgs::instance().dumpMode)
					printf("\t\t\tBaseVertex[%i] = %i : %i %i\n", patchCVMap[u][v], patchInfo.BaseVertices[patchCVMap[u][v]], uCV, vCV);
				count++;
			}
		}
	}
	if(CArgs::instance().dumpMode)
		printf("\t\tTangents:\n");
	for(u = 0; u < 4; u++) {
		for(v = 0; v < 4; v++) {
			if(((u%3 != 0) && (v%3 == 0)) || ((u%3 == 0) && (v%3 != 0))) {
				if(CArgs::instance().dumpMode)
					displayCV(uPatch, vPatch, u, v);
				insertCV(uPatch, vPatch, u, v, patchInfo.Patch.Tangents);
			}
		}
	}
	if(CArgs::instance().dumpMode)
		printf("\t\tInternals:\n");
	for(u = 0; u < 4; u++) {
		for(v = 0; v < 4; v++) {
			if((u%3 != 0) && (v%3 != 0)) {
				if(CArgs::instance().dumpMode)
					displayCV(uPatch, vPatch, u, v);
				insertCV(uPatch, vPatch, u, v, patchInfo.Patch.Interiors);
			}
		}
	}

	// Find patch connectivity info
	for(unsigned int e = 0; e < 4; e++) {
		int neighbourUPatch = uPatch + neighbourPatchMap[e][0];
		int neighbourVPatch = vPatch + neighbourPatchMap[e][1];
		// check if there is a neighbour
		if((neighbourUPatch < 0) || !(neighbourUPatch < mSurface.numPatchesInU()) || (neighbourVPatch < 0) || !(neighbourVPatch < mSurface.numPatchesInV()))
			continue;

		patchInfo.BindEdges[e].NPatchs = 1;
		patchInfo.BindEdges[e].Edge[0] = neighbourEdgeMap[e];
		patchInfo.BindEdges[e].Next[0] = (neighbourUPatch * mSurface.numPatchesInV()) + neighbourVPatch;

		if(CArgs::instance().dumpMode)
			printf("Edge: %i, NPatchs: %i, Edge: %i, Next: %i\n", e, 1, patchInfo.BindEdges[e].Edge[0], patchInfo.BindEdges[e].Next[0]);
	}
	int tilesU = 8;
	int tilesV = 8;
	if(mbHasPatchPainter) {
		mPatchPainter.findPlug("NumTilesU").getValue(tilesU);
		mPatchPainter.findPlug("NumTilesV").getValue(tilesV);
	}
	patchInfo.OrderS = tilesU;
	patchInfo.OrderT = tilesV;
	patchInfo.Tiles.resize(patchInfo.OrderS * patchInfo.OrderT);
	patchInfo.TileColors.resize((patchInfo.OrderS+1) * (patchInfo.OrderT+1));

	MPlug tiles, tileColors;
	bool bGotTiles = false;
	if(mbHasPatchPainter) {
        bGotTiles = getTilesFromPatch(uPatch, vPatch, tiles, tileColors);
	}
	for (v=0; v<patchInfo.OrderT+1; v++)
	for (u=0; u<patchInfo.OrderS+1; u++) {
		int index = u+v*(patchInfo.OrderS+1);
		// Create a rgba value
		NLMISC::CRGBA rgba ( 255, 255, 255);
		if(bGotTiles) {
			rgba = getTileColorFromPlug(u, v, patchInfo.OrderS, tileColors);
		}
		// Store it in the tile info
		patchInfo.TileColors[index].Color565=rgba.get565();
	}
	for (v=0; v<patchInfo.OrderT; v++)
	for (u=0; u<patchInfo.OrderS; u++) {
		int index = u+v*patchInfo.OrderS;
		int layer[3] = {0xFF, 0xFF, 0xFF};
		if(bGotTiles) {
			getTileFromPlug(index, tiles, layer[0], layer[1], layer[2]);
//			printf("Got tiles %i %i %i\n", layer[0], layer[1], layer[2]);
		}
		for (int l=0; l<3; l++)	{
			patchInfo.Tiles[index].Tile[l]=layer[l];
			patchInfo.Tiles[index].setTile256Info (false, 0);
			patchInfo.Tiles[index].setVegetableState (NL3D::CTileElement::AboveWater);
		}
	}
	patchInfo.Lumels.resize ((patchInfo.OrderS*4)*(patchInfo.OrderT*4), 255);

	mPatchInfos.push_back(patchInfo);
}

void CNeLZoneWriter::makeZone() {
	mZone.build (0, mPatchInfos, std::vector<NL3D::CBorderVertex>());
}

bool CNeLZoneWriter::getPatchPainter() {
	MStatus stat;
//	MFnDagNode surface(mPath.transform());
//	MPlug connection = surface.findPlug("ConnectionToNelPatchPainter", stat);
	MPlug connection = mSurface.findPlug("worldSpace", &stat);
	if(stat == MStatus::kFailure)
		return false; 
	for(unsigned int i = 0; i < connection.numElements(); i++) {
		MPlugArray connections;
		connection.elementByPhysicalIndex(i).connectedTo(connections, false, true);
		for(int i = 0; i < connections.length(); i++) {
			MPlug painterPlug = connections[i];
			mPatchPainter.setObject(painterPlug.node());
			mPatchesPlug = mPatchPainter.findPlug("Patches", &stat);
			if(stat == MStatus::kSuccess) {
				printf("\tFound patch painter: %s\n", mPatchPainter.name().asChar());
				return true;
			}
		}
	}
	return false;
}

bool CNeLZoneWriter::getTilesFromPatch(unsigned int uPatch, unsigned int vPatch, MPlug &tiles, MPlug &tileColors) {
	MStatus status;
	// some weird stuff going on here...the patch painter stores values with the following index
	// vPatch*numPatchesInU + uPatch
	// however due to some weirdness getting the values in the same doesn't work
	// should investigate later where this problem is introduced
	// but the current index works just as well
	unsigned int index = uPatch*mSurface.numPatchesInV() + vPatch;
	MPlug compound = mPatchesPlug.elementByLogicalIndex(index, &status);
	if(status == MStatus::kSuccess) {
		tiles = compound.child(0);
		tileColors = compound.child(1);
//		printf("\tGot tiles %s and tile colors %s\n", tiles.partialName(true).asChar(), tileColors.partialName(true).asChar());
		return true;
	}
	return false;
}

void CNeLZoneWriter::getTileFromPlug(unsigned int index, MPlug &tiles, int &layer0, int &layer1, int &layer2) {
	MStatus status;
	MPlug tilePlug = tiles.elementByLogicalIndex(index, &status);
	layer0 = layer1 = layer2 = 0xFF;
	if(status == MStatus::kSuccess) {
		MPlug layerPlug = tilePlug.child(0);
		layerPlug.getValue(layer0);
		layerPlug = tilePlug.child(1);
		layerPlug.getValue(layer1);
		layerPlug = tilePlug.child(2);
		layerPlug.getValue(layer2);
//		printf("\tGot tile value %i at %i\n", tile, index);
	}
}

NLMISC::CRGBA CNeLZoneWriter::getTileColorFromPlug(unsigned int u, unsigned int v, unsigned int numUTiles, MPlug &tileColors) {
	MColor color;
	MStatus status;
	if(u > 0) u--;
	if(v > 0) v--;
	unsigned int index = v*numUTiles + u;
	MPlug colorPlug = tileColors.elementByLogicalIndex(index, &status);
	if(status == MStatus::kSuccess) {
		colorPlug.child(0).getValue(color.r);
		colorPlug.child(1).getValue(color.g);
		colorPlug.child(2).getValue(color.b);
//		printf("\tGot color value %f %f %f at %i\n", color.r, color.g, color.b, index);
		return mColor2CRGBA(color);
	}
	return NLMISC::CRGBA(255, 255, 255);
}
