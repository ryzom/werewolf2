// Copyright (C) 
// File: billboardLighterCmd.cpp
// MEL Command: billboardLighter
// Author: Henri Kuuste

#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MFnMesh.h>
#include <maya/MFnDagNode.h>
#include <maya/MFn.h>
#include <maya/MDagPath.h>
#include <maya/MSyntax.h>
#include <maya/MArgList.h>
#include <maya/MArgDatabase.h>
#include <maya/MSelectionList.h>
#include <maya/MFnPlugin.h>
#include <maya/MObject.h>
#include <maya/MPxCommand.h>
#include <maya/MVector.h>
#include <maya/MFnTransform.h>
#include <maya/MPoint.h>
#include <maya/M3dView.h>
#include <maya/MPlug.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MDGModifier.h>
#include <maya/MStatus.h>

#define kCenterFlag		"-c"
#define kCenterFlagLong	"-center"

///////////////////////////////////////////////////
//
// Command class declaration
//
///////////////////////////////////////////////////
class billboardLighter : public MPxCommand
{
public:
	billboardLighter();
	virtual			~billboardLighter(); 

	MStatus			doIt( const MArgList& args );

	static MSyntax	newSyntax();
	static void*	creator();

private:
	MStatus			parseArgs( const MArgList& args );
	bool			isBBCluster( MDagPath path );
	bool			isLeafCluster( MDagPath path );
	bool			isBB( MDagPath path );
	bool			isBBClusterSlot( MDagPath path );
	bool			isLeafClusterSlot( MDagPath path );

	MDagPath		m_CenterPath;
	bool			m_bCanDoIt;
};



///////////////////////////////////////////////////
//
// Command class implementation
//
///////////////////////////////////////////////////

billboardLighter::billboardLighter()
: m_bCanDoIt (false)
{}

billboardLighter::~billboardLighter() {}

void* billboardLighter::creator()
{
	return new billboardLighter;
}

bool billboardLighter::isBBCluster(MDagPath path) {
	MFnDagNode node(path);
	MStatus stat;
	node.findPlug("BBCluster", &stat);
	if(stat == MStatus::kSuccess) {
		return true;
	}
	return false;
}

bool billboardLighter::isLeafCluster(MDagPath path) {
	MFnDagNode node(path);
	MStatus stat;
	node.findPlug("LeafCluster", &stat);
	if(stat == MStatus::kSuccess) {
		return true;
	}
	return false;
}

bool billboardLighter::isBB(MDagPath path) {
	MFnDagNode node(path.transform());
	MStatus stat;
	node.findPlug("Billboard", &stat);
	if(stat == MStatus::kSuccess) {
		return true;
	}
	return false;
}

bool billboardLighter::isBBClusterSlot(MDagPath path) {
	MFnDagNode node(path);
	MStatus stat;
	node.findPlug("BBSlot", &stat);
	if(stat == MStatus::kSuccess) {
		return true;
	}
	return false;
}

bool billboardLighter::isLeafClusterSlot(MDagPath path) {
	MFnDagNode node(path);
	MStatus stat;
	node.findPlug("LeafSlot", &stat);
	if(stat == MStatus::kSuccess) {
		return true;
	}
	return false;
}


MSyntax billboardLighter::newSyntax()
{
	MSyntax syntax;
	//	syntax.addFlag(kCenterFlag, kCenterFlagLong, MSyntax::kSelectionItem);
	return syntax;
}

MStatus billboardLighter::parseArgs( const MArgList& args )
{
	MStatus			stat;
	MArgDatabase	argData(syntax(), args);

	/*	if (argData.isFlagSet(kCenterFlag)) {
	MSelectionList center;
	argData.getFlagArgument(kCenterFlag, 0, center);
	if(!center.isEmpty()) {
	MDagPath path;
	center.getDagPath(0, path);
	if(path.node().hasFn(MFn::kTransform)) {
	m_bCanDoIt = true;
	m_CenterPath = path;
	}
	}
	}
	*/

	m_bCanDoIt = true;
	return stat;
}

MStatus billboardLighter::doIt( const MArgList& args ) {
	MStatus stat = MS::kSuccess;
	parseArgs ( args );
	if(m_bCanDoIt) {
		/*		MFnTransform centerNode(m_CenterPath);
		MPoint centerPos = centerNode.translation(MSpace::kWorld);
		*/		MStatus stat;

		MDagPath camera;
		if((stat = M3dView::active3dView().getCamera(camera)) != MStatus::kSuccess) {
			stat.perror("active3dView");
		}

		MObject camXform = camera.transform();
		while(camera.pop() == MStatus::kSuccess) {
			if(camera.node() == camXform)
				break;
		}

/*		MFnTransform cam(camera);
		MVector viewVector = cam.translation(MSpace::kWorld);
		viewVector.normalize();
		MPlug angleChange = cam.findPlug("angleChange", &stat);
		if(stat != MStatus::kSuccess) {
			stat.perror("plug not found");
			MString command = MString("addAttr -ln \"angleChange\" -sn \"acg\" -at \"double\" \"") + cam.partialPathName() + "\";";
			printf("%s\n", command.asChar());
			MGlobal::executeCommand(command);
			angleChange = cam.findPlug("angleChange", &stat);
			if(stat != MStatus::kSuccess) {
				stat.perror("angleChange plug");
			}
		}
		bool bAngleChanged = true;
		double oldAngleChange = 0;
		angleChange.getValue(oldAngleChange);
		double newAngleChange = viewVector * MVector::xAxis;
		double error = 0.06;
		if(oldAngleChange < (newAngleChange+error) && oldAngleChange > (newAngleChange-error)) {
			bAngleChanged = false;
		} else {
			angleChange.setValue(newAngleChange);
		}
*/
//		if(bAngleChanged) {
			MItDag it(MItDag::kDepthFirst, MFn::kLodGroup);
			for(;!it.isDone();it.next()) {
				MDagPath clusterPath;
				if (MStatus::kFailure == it.getPath(clusterPath)) {
					continue;
				}
				if(!isBBCluster(clusterPath) && !isLeafCluster(clusterPath))
					continue;

				MFnTransform cluster(clusterPath);
				MVector centerPos = cluster.translation(MSpace::kWorld);
				int numSlots = clusterPath.childCount();
				for(int slot = 0; slot < numSlots; slot++) {
					MDagPath slotPath = clusterPath;
					slotPath.push(slotPath.child(slot));
					if(!isBBClusterSlot(slotPath) && !isLeafClusterSlot(slotPath))
						continue;

					bool lodVisibility = false;
					MFnDagNode slotNode(slotPath);
					slotNode.findPlug("lodVisibility").getValue(lodVisibility);
					if(lodVisibility) {
						float useFaceNormal = 0;
						slotNode.findPlug("useFaceNormal").getValue(useFaceNormal);
						int numBillboards = slotPath.childCount();
						for(int bb = 0; bb < numBillboards; bb++) {
							MDagPath bbPath = slotPath;
							bbPath.push(bbPath.child(bb));

							MFnMesh mesh(bbPath);
							MFnTransform bbTrans(bbPath);
							int vertices = mesh.numVertices();
							MVector faceNormal = bbTrans.translation(MSpace::kWorld) - centerPos;
							faceNormal.normalize();
							for(int i = 0; i < vertices; i++) {
								MPoint vtxPos;
								mesh.getPoint(i, vtxPos, MSpace::kWorld);
								MVector normal = vtxPos - centerPos;
								normal.normalize();
								normal = ((1-useFaceNormal)*normal) + (useFaceNormal*faceNormal);
								mesh.setVertexNormal(normal, i, MSpace::kWorld);
							}
						}
					}
				}
			}
//		}
		//		setResult( "billboardLighter command executed!\n" );
	} else {
		//		setResult( "Center not valid!\n" );
	}
	return stat;
}

/////////////////////////////////////////////////////////////////
//
// The following routines are used to register/unregister
// the command we are creating within Maya
//
/////////////////////////////////////////////////////////////////
MStatus initializePlugin( MObject obj )
{
	MStatus   status;
	MFnPlugin plugin( obj, "Alias", "3.0", "Any");

	status = plugin.registerCommand( "billboardLighter",
		billboardLighter::creator,
		billboardLighter::newSyntax);
	if (!status) {
		status.perror("registerCommand");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)
{
	MStatus	  status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterCommand( "billboardLighter" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}

	return status;
}
