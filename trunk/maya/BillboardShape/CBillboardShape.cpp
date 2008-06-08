// Author: Henri Kuuste

// Maya includes
#include <maya/MFnPlugin.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya/MPoint.h>

// Local includes
#include "CBillboardShape.h"
#include "CBillboardShapeUI.h"

// Global includes
#include <iostream>

// Globals
CGcontext shaderContext;

/////////////////////////////////////////////////////////////////////

#define MCHECKERROR(STAT,MSG)       \
    if ( MS::kSuccess != STAT ) {   \
		std::cerr << MSG << ": " << STAT.errorString().asChar() << std::endl;   \
        return MS::kFailure;        \
    }

#define MAKE_NUMERIC_ATTR( NAME, SHORTNAME, TYPE, DEFAULT, KEYABLE )	\
	MStatus NAME##_stat;                                            \
	MFnNumericAttribute NAME##_fn;                                  \
	NAME = NAME##_fn.create( #NAME, SHORTNAME, TYPE, DEFAULT, &NAME##_stat );     \
	MCHECKERROR(NAME##_stat, "numeric attr create error");		\
	NAME##_fn.setHidden( !KEYABLE );				\
	NAME##_fn.setKeyable( KEYABLE );				\
	NAME##_stat = addAttribute( NAME );                             \
	MCHECKERROR(NAME##_stat, "numeric addAttribute error");

#define MAKE_NUMERIC_ATTR_ARRAY( NAME, SHORTNAME, TYPE, DEFAULT, KEYABLE )	\
	MStatus NAME##_stat;                                            \
	MFnNumericAttribute NAME##_fn;                                  \
	NAME = NAME##_fn.create( #NAME, SHORTNAME, TYPE, DEFAULT, &NAME##_stat );     \
	MCHECKERROR(NAME##_stat, "numeric array attr create error");		\
	NAME##_fn.setHidden( !KEYABLE );				\
	NAME##_fn.setKeyable( KEYABLE );				\
	NAME##_fn.setArray( true );						\
	NAME##_stat = addAttribute( NAME );                             \
	MCHECKERROR(NAME##_stat, "numeric array addAttribute error");

#define MAKE_COLOR( NAME, SHORTNAME, KEYABLE )	\
	MStatus NAME##_stat;                                            \
	MFnNumericAttribute NAME##_fn;                                  \
	NAME = NAME##_fn.createColor( #NAME, SHORTNAME, &NAME##_stat );     \
	MCHECKERROR(NAME##_stat, "color attr create error");		\
	NAME##_fn.setHidden( !KEYABLE );				\
	NAME##_fn.setKeyable( KEYABLE );				\
	NAME##_stat = addAttribute( NAME );                             \
	MCHECKERROR(NAME##_stat, "color addAttribute error");

#define MAKE_POINT( NAME, SHORTNAME, KEYABLE )	\
	MStatus NAME##_stat;                                            \
	MFnNumericAttribute NAME##_fn;                                  \
	NAME = NAME##_fn.createPoint( #NAME, SHORTNAME, &NAME##_stat );     \
	MCHECKERROR(NAME##_stat, "point attr create error");		\
	NAME##_fn.setHidden( !KEYABLE );				\
	NAME##_fn.setKeyable( KEYABLE );				\
	NAME##_stat = addAttribute( NAME );                             \
	MCHECKERROR(NAME##_stat, "point addAttribute error");

#define MAKE_POINT_ARRAY( NAME, SHORTNAME, KEYABLE )	\
	MStatus NAME##_stat;                                            \
	MFnNumericAttribute NAME##_fn;                                  \
	NAME = NAME##_fn.createPoint( #NAME, SHORTNAME, &NAME##_stat );     \
	MCHECKERROR(NAME##_stat, "point array attr create error");		\
	NAME##_fn.setHidden( !KEYABLE );				\
	NAME##_fn.setKeyable( KEYABLE );				\
	NAME##_fn.setArray( true );						\
	NAME##_stat = addAttribute( NAME );                             \
	MCHECKERROR(NAME##_stat, "point array addAttribute error");

#define MAKE_NUMERIC_ATTR_MINMAX( NAME, SHORTNAME, TYPE, DEFAULT, KEYABLE, MIN, MAX )	\
	MStatus NAME##_stat;                                             		\
	MFnNumericAttribute NAME##_fn;                                   		\
	NAME = NAME##_fn.create( #NAME, SHORTNAME, TYPE, DEFAULT, &NAME##_stat );      		\
	MCHECKERROR(NAME##_stat, "numeric min/max attr create error");		     		\
	NAME##_fn.setHidden( !KEYABLE );						\
	NAME##_fn.setKeyable( KEYABLE );						\
	NAME##_fn.setMin(MIN);								\
	NAME##_fn.setMax(MAX);								\
	NAME##_stat = addAttribute( NAME );                              		\
	MCHECKERROR(NAME##_stat, "numeric min/max addAttribute error");

/////////////////////////////////////////////////////////////////////
// SHAPE NODE IMPLEMENTATION
/////////////////////////////////////////////////////////////////////

MObject	CBillboardShape::useBBSize;
MObject CBillboardShape::radiusX;
MObject CBillboardShape::radiusY;
MObject CBillboardShape::globalRadiusX;
MObject CBillboardShape::globalRadiusY;

MObject CBillboardShape::useRadialNormals;
MObject CBillboardShape::faceNormal;
MObject CBillboardShape::translucency;
MObject CBillboardShape::clusterCenter;
MObject CBillboardShape::clusterRadius;
MObject CBillboardShape::isLeaf;
MObject	CBillboardShape::useCastShadows;
MObject CBillboardShape::casterPositions;
MObject CBillboardShape::casterSizes;
MObject CBillboardShape::penumbra;

MObject CBillboardShape::useOrientation;
MObject CBillboardShape::orientation;

MObject CBillboardShape::alignType;
MObject CBillboardShape::constrainAxis;

MObject CBillboardShape::useBBColor;
MObject CBillboardShape::color;
MObject CBillboardShape::globalBBColor;

MTypeId CBillboardShape::id( 0x80111 ); // TODO: change this

CBillboardShape::CBillboardShape() {
	fGeometry = new CBillboardGeom();
}

CBillboardShape::~CBillboardShape() {
	delete fGeometry;
}

/* override */
void CBillboardShape::postConstructor() { 
	// This call allows the shape to have shading groups assigned
	setRenderable( true );
}

/* override */
MStatus CBillboardShape::compute( const MPlug& /*plug*/, MDataBlock& /*datablock*/ ) { 
	return MS::kUnknownParameter;
}



/* override */
bool CBillboardShape::isBounded() const { return true; }

/* override */
MBoundingBox CBillboardShape::boundingBox() const {
	MBoundingBox result;	
	CBillboardShape* nonConstThis = const_cast <CBillboardShape*> (this);
	

	double r;
	MPlug rxPlug(nonConstThis->thisMObject(), radiusX);
	rxPlug.getValue(r);
	result.expand( MPoint(r,r,r) );	result.expand( MPoint(-r,-r,-r) );
	MPlug ryPlug(nonConstThis->thisMObject(), radiusY);
	ryPlug.getValue(r);
	result.expand( MPoint(r,r,r) );	result.expand( MPoint(-r,-r,-r) );

    return result;
}

void* CBillboardShape::creator()
{
	return new CBillboardShape();
}

MStatus CBillboardShape::initialize()
{ 
	MStatus				stat;
	MFnNumericAttribute		numericAttr;

	
	MAKE_NUMERIC_ATTR( useBBSize, "ubs", MFnNumericData::kBoolean, 0, true );
	MAKE_NUMERIC_ATTR( radiusX, "rX", MFnNumericData::kDouble, 0.5, true );
	MAKE_NUMERIC_ATTR( radiusY, "rY", MFnNumericData::kDouble, 0.5, true );
	MAKE_NUMERIC_ATTR( globalRadiusX, "grX", MFnNumericData::kDouble, 0.5, true );
	MAKE_NUMERIC_ATTR( globalRadiusY, "grY", MFnNumericData::kDouble, 0.5, true );

	MAKE_NUMERIC_ATTR( useRadialNormals, "urn", MFnNumericData::kBoolean, 0, true );
	MAKE_NUMERIC_ATTR( isLeaf, "ilf", MFnNumericData::kBoolean, 0, true );
	MAKE_NUMERIC_ATTR( useCastShadows, "ucs", MFnNumericData::kBoolean, 0, true );
	MAKE_NUMERIC_ATTR_MINMAX( translucency, "tlc", MFnNumericData::kDouble, 0.0, true, 0.0, 1.0 );
	MAKE_NUMERIC_ATTR_MINMAX( faceNormal, "fn", MFnNumericData::kDouble, 0.0, true, 0.0, 1.0 );
	MAKE_POINT( clusterCenter, "cc", true );
	MAKE_NUMERIC_ATTR( clusterRadius, "cr", MFnNumericData::kDouble, 1.0, true );
	MAKE_NUMERIC_ATTR_ARRAY( casterSizes, "scs", MFnNumericData::kDouble, 0.0, true );
	MAKE_POINT_ARRAY( casterPositions, "scp", true);
	MAKE_NUMERIC_ATTR_MINMAX( penumbra, "pnm", MFnNumericData::kDouble, 0.5, true, 0.0, 1.0 ) ;

	MAKE_NUMERIC_ATTR( useOrientation, "uor", MFnNumericData::kBoolean, 0, true );
	MAKE_NUMERIC_ATTR( orientation, "or", MFnNumericData::kDouble, 0.0, true );

	MAKE_NUMERIC_ATTR( useBBColor, "ubc", MFnNumericData::kBoolean, 0, true );
	MAKE_COLOR( color, "col", true );
	MAKE_COLOR( globalBBColor, "bgc", true );

	MAKE_POINT( constrainAxis, "cax", true);
	MFnEnumAttribute	enumAttr;
	alignType = enumAttr.create( "alignType", "at", 1, &stat );
	MCHECKERROR( stat, "create alignType attribute" );
		enumAttr.addField( "autoAlign", 0 );
		enumAttr.addField( "screenAlign", 1 );
		enumAttr.addField( "viewAlign", 2 );
		enumAttr.addField( "axisConstrain", 3 );
	enumAttr.setHidden( false );
	enumAttr.setKeyable( true );
	stat = addAttribute( alignType );
	MCHECKERROR( stat, "Error adding alignType attribute." );

	return stat;
}

CBillboardGeom* CBillboardShape::geometry() {
	MObject this_object = thisMObject();
	MPlug plug( this_object, radiusX );	plug.getValue( fGeometry->radiusX );
	plug.setAttribute( radiusY );		plug.getValue( fGeometry->radiusY );
	plug.setAttribute( globalRadiusX );	plug.getValue( fGeometry->globalRadiusX );
	plug.setAttribute( globalRadiusY );	plug.getValue( fGeometry->globalRadiusY );
	plug.setAttribute( useBBSize );		plug.getValue( fGeometry->useBBSize );

	plug.setAttribute( translucency );	plug.getValue( fGeometry->translucency );
	plug.setAttribute( faceNormal );	plug.getValue( fGeometry->faceNormal );
	plug.setAttribute( useRadialNormals );	plug.getValue( fGeometry->useRadialNormals );
	plug.setAttribute( clusterCenter ); plug.child(0).getValue( fGeometry->clusterCenter.x );
										plug.child(1).getValue( fGeometry->clusterCenter.y );
										plug.child(2).getValue( fGeometry->clusterCenter.z );
	plug.setAttribute( clusterRadius );	plug.getValue( fGeometry->clusterRadius );
	plug.setAttribute( isLeaf );		plug.getValue( fGeometry->isLeaf );
	plug.setAttribute( useCastShadows );plug.getValue( fGeometry->useCastShadows );
	MPlug casterPosPlug( this_object, casterPositions );
	fGeometry->casterPositions.clear();
	for(unsigned int i = 0; i < casterPosPlug.numElements(); ++i) {
		MPlug elem = casterPosPlug.elementByPhysicalIndex(i);
		MPoint point;
		elem.child(0).getValue( point.x );
		elem.child(1).getValue( point.y );
		elem.child(2).getValue( point.z );
		fGeometry->casterPositions.append(point);
	}
	MPlug casterSizePlug( this_object, casterSizes );
	fGeometry->casterSizes.clear();
	for(unsigned int i = 0; i < casterSizePlug.numElements(); ++i) {
		MPlug elem = casterSizePlug.elementByPhysicalIndex(i);
		double size;
		elem.getValue( size );
		fGeometry->casterSizes.append(size);
	}
	plug.setAttribute( penumbra );		plug.getValue( fGeometry->penumbra );

	plug.setAttribute( alignType );		plug.getValue( fGeometry->alignType );
	plug.setAttribute( constrainAxis ); plug.child(0).getValue( fGeometry->constrainAxis.x );
										plug.child(1).getValue( fGeometry->constrainAxis.y );
										plug.child(2).getValue( fGeometry->constrainAxis.z );

	plug.setAttribute( useOrientation );plug.getValue( fGeometry->useOrientation );
	plug.setAttribute( orientation );	plug.getValue( fGeometry->orientation );

	plug.setAttribute( useBBColor );	plug.getValue( fGeometry->useBBColor );
	plug.setAttribute( color );			plug.child(0).getValue( fGeometry->color.r );
										plug.child(1).getValue( fGeometry->color.g );
										plug.child(2).getValue( fGeometry->color.b );
	plug.setAttribute( globalBBColor );	plug.child(0).getValue( fGeometry->globalColor.r );
										plug.child(1).getValue( fGeometry->globalColor.g );
										plug.child(2).getValue( fGeometry->globalColor.b );

	return fGeometry;	
}

void cgErrorCallback(void) {
    CGerror LastError = cgGetError();
	
    if(LastError)
    {
        const char *Listing = cgGetLastListing(shaderContext);
        printf("\n---------------------------------------------------\n");
        printf("%s\n\n", cgGetErrorString(LastError));
        printf("%s\n", Listing);
        printf("---------------------------------------------------\n");
        printf("Cg error, exiting...\n");
    }
}

MStatus initializePlugin( MObject obj ) { 
	MFnPlugin plugin( obj, "Ironic Entertainment", "1.0", "Any");
	MStatus stat = plugin.registerShape( "billboardShape", CBillboardShape::id, &CBillboardShape::creator, &CBillboardShape::initialize, &CBillboardShapeUI::creator  );
	if(stat == MStatus::kSuccess) {
		cgSetErrorCallback(cgErrorCallback);
		shaderContext = cgCreateContext();
	}
	return stat;
}

MStatus uninitializePlugin( MObject obj) {
	MFnPlugin plugin( obj );
	MStatus stat = plugin.deregisterNode( CBillboardShape::id );
	if(stat == MStatus::kSuccess) {
		if(shaderContext) cgDestroyContext(shaderContext);
	}
	return stat;
}
