/* Copyright, 2003 Neverborn Entertainment.

 * This file is part of our XSI Plugins.

 * The XSI Plugins are free software, you can redistribute them and/or modify
 * them under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2, or (at your option)
 * any later version.

 * XSI Plugins are distributed WITHOUT ANY WARRANTY or implied warranty of
 * MERCHANTABILITY. See the GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with the XSI Plugins; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.

 */

//
// Includes
//

#include "std_pch.h"
#include "xsi_nel_mesh_lib.h"
#include "xsi_nel_export.h"
#include "xsi_nel_material.h"

//
// Namespaces
//

using namespace XSI;
using namespace NLMISC;
using namespace NL3D;

//
// Global Variables
//



//
// Class Constructors & Deconstructors
//

CNeLMesh::CNeLMesh()
{

	// Set Verbosity Default Options
	is_verbose = false;
	is_very_verbose = false;
	is_insane_verbosity = false;

	// Set MRM Default Option
	is_mrm = false;

	// Set Skinning Default Option
	is_skinned = false;

	// Set LOD Default Option
	is_multi_lod = false;

	// Set Light Attenuation Default Option
	is_local_light_attenuated = false;

	// Set Shadow Caster Default Option
	is_shadow_caster = false;

	// Set Shadow Reciever Default Option
	is_shadow_reciever = false;

	// Set Shadow Reciever Default Option
	is_blend_shaped = false;

	// Set Vegetable Export Default Option
	is_vegetable = false;

	// Set File Path  And Name Default Options
	file_path = L"";
	file_name = L"";

	// Set Time ( Animation Frame )
	time = 0.0f;

	// Bone LOD Distance
	bone_distance_max = 0.0f;

	// Current Bone
	current_bone = -1;

}

CNeLMesh::~CNeLMesh()
{
}

sint32 CompareProc( const void *arg1, const void *arg2 )
{
	single_weight* pItem1 = &((single_weight*)arg1)[0];
	single_weight* pItem2 = &((single_weight*)arg2)[0];

	if ( pItem1->index > pItem2->index )
	{	
		return 1;
	}

	if ( pItem1->index < pItem2->index )
	{
		return -1;
	}

	return NULL;
}

bool CNeLMesh::isDeformer( X3DObject potential_bone )
{

	for( int i = 0; i < object.GetApplication().GetActiveSceneRoot().FindChildren( L"", L"", CStringArray() ).GetCount(); i++)
	{
		X3DObject node = object.GetApplication().GetActiveSceneRoot().FindChildren( L"", L"", CStringArray() )[ i ];
  		if ( node.IsValid() )
		{
			CRefArray envelopes = node.GetEnvelopes();
   			for ( long j = 0; j < envelopes.GetCount(); j++ )
			{
				Envelope envelope( envelopes[ j ] );
				CRefArray deformers = envelope.GetDeformers();
 				for ( long k = 0; k < deformers.GetCount(); k++ )
				{
    				if ( X3DObject( deformers[ k ] ) == potential_bone )
					{
						return true;
					}
				}	
			}
		}
 	}
 
	return false;
 
}

//
// Class Functions
//

CVector CNeLMesh::getLocalNormal( uint32 index )
{

	MATH::CVector3 point = triangle_geometry_normals[index];
	CVector normal = CExport::instance().convertVectorAndAxis( point );

	return normal;

}

void CNeLMesh::createMeshBaseBuild()
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Creating mesh base build structure." );
		nlinfo( "Creating mesh base build structure." );
	}
	
	// Set Shadow Flags
	mesh_base_build.bCastShadows = is_shadow_caster;
	mesh_base_build.bRcvShadows = is_shadow_reciever;

	// Set Lighting
	mesh_base_build.UseLightingLocalAttenuation = is_local_light_attenuated;

	// Set Skinning
	Envelope envelope = object.GetEnvelopes()[ 0 ];
	CRefArray deformers = envelope.GetDeformers();

	if ( deformers.GetCount() > 0 )
	{
		is_skinned = true;
		mesh_build.VertexFlags |= CVertexBuffer::PaletteSkinFlag;
	}

	// Set Transform
	if ( !is_skinned )
	{
		mesh_base_build.DefaultScale = CExport::instance().convertVectorAndAxis( local_scale );
		mesh_base_build.DefaultRotQuat = CExport::instance().convertQuaternionAndAxis( local_rotation_quaternion );
		mesh_base_build.DefaultRotEuler = CExport::instance().convertVectorAndAxis( local_rotation );
		mesh_base_build.DefaultPivot = CExport::instance().convertVectorAndAxis( local_pivot );
		mesh_base_build.DefaultPos = CExport::instance().convertVectorAndAxis( local_translation );
	}
	else
	{
		mesh_base_build.DefaultScale = CExport::instance().convertVectorAndAxis( global_scale );
		mesh_base_build.DefaultRotQuat = CExport::instance().convertQuaternionAndAxis( global_rotation_quaternion );
		mesh_base_build.DefaultRotEuler = CExport::instance().convertVectorAndAxis( global_rotation );
		mesh_base_build.DefaultPivot = CExport::instance().convertVectorAndAxis( global_pivot );
		mesh_base_build.DefaultPos = CExport::instance().convertVectorAndAxis( global_translation );
	}

	// Camera Collision
	if ( camera_collision == eAuto )
	{
		mesh_base_build.CollisionMeshGeneration = NL3D::CMeshBase::AutoCameraCol;
	}
	else if ( camera_collision == eDisabled )
	{
		mesh_base_build.CollisionMeshGeneration = NL3D::CMeshBase::NoCameraCol;
	}
	else if ( camera_collision == eForced )
	{
		mesh_base_build.CollisionMeshGeneration = NL3D::CMeshBase::ForceCameraCol;
	}

	// Blend Shapes

	// TODO : mesh deform / shape deform / morph ( it is not essential )

	mesh_base_build.DefaultBSFactors.clear();
	mesh_base_build.BSNames.clear();

	//MFnBlendShapeDeformer fn(findBlendShape(mDagPath->node()), &status);
	//if(status) {
	//	if(CArgs::instance().verboseMode)
	//		printf("\tChecking %s\n", fn.name().asChar());
	//	for(unsigned int i = 0; i < fn.numWeights(); i++) {
	//		MObjectArray targets;
	//		fn.getTargets(mDagPath->node(), i, targets);
	//		MString name = getMorphName(targets[0]);
	//		mBuildBaseMesh.DefaultBSFactors.push_back(0.f);
	//		mBuildBaseMesh.BSNames.push_back(name.asChar());
	//		if(CArgs::instance().verboseMode)
	//			printf("\t\tAdded blendShape target %s\n", name.asChar());
	//	}
	//}

}

void CNeLMesh::createMeshBuild()
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Creating mesh build structure." );
		nlinfo( "Creating mesh build structure." );
	}

	// Variables
	uint32 faces = triangle_geometry_index.GetCount() / 3;
	uint32 vertices = point_geometry_index.GetCount();
	uint32 j = 0;

	if ( isInsaneVerbosity() )
	{
		CExport::instance().getApplication().LogMessage( L"Unique Vertices '" + CString( CValue( long( vertices ) ) ) + L"', Faces '" + CString( CValue( long( faces ) ) ) + L"'" );
		nlinfo( "Unique Vertices '%u', Faces '%u'", vertices, faces );
	}

	// Clear Existing
	mesh_build.Vertices.clear();
	mesh_build.Faces.clear();

	// Set Faces And Vertices
	mesh_build.Vertices.resize( vertices );
	mesh_build.Faces.resize( faces );

	// Set Vertex Flags
	mesh_build.VertexFlags |= CVertexBuffer::PositionFlag;
	mesh_build.VertexFlags |= CVertexBuffer::NormalFlag;

	// Export Vertices
	for( uint32 i = 0; i < vertices; i++ )
	{
		mesh_build.Vertices[i] = CExport::instance().convertVectorAndAxis( point_geometry_vertices[ i ] );

		if( isVeryVerbose() )
		{
			CExport::instance().getApplication().LogMessage( L"Processing vertice '" + CString( CValue( long( i ) ) ) + L"'" );
			nlinfo( "Processing vertice '%u'", i );
		}
		if ( isInsaneVerbosity() )
		{
			CExport::instance().getApplication().LogMessage( L"Processing vertice '" + CExport::instance().convertString( CExport::instance().convertVectorAndAxis( point_geometry_vertices[ i ] ).asString() ) + L"'" );
			nlinfo( "Stored Vertex '%s'", CExport::instance().convertVectorAndAxis( point_geometry_vertices[ i ] ).asString().c_str() );
		}
	}

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Vertices embedded." );
		nlinfo( "Vertices embedded." );
	}

	// Export Materials

	for( uint32 i = 0; i < faces; i++ )
	{
		mesh_build.Faces[ i ].MaterialId = NULL;
		mesh_build.Faces[ i ].SmoothGroup = NULL;
		if( isVeryVerbose() )
		{
			CExport::instance().getApplication().LogMessage( L"Processing face '" + CString( CValue( long( i ) ) ) + L"' material and smoothing group pass." );
			nlinfo( "Processing face '%u' material and smoothing group pass.", i );
		}
	}

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Material id & smoothing group embedded." );
		nlinfo( "Material id & smoothing group embedded." );
	}

	// Export Faces

	for( uint32 i = 0; i < faces; i++ )
	{
		mesh_build.Faces[ i ].Corner[ 2 ].Vertex = triangle_geometry_index[ j ];
		mesh_build.Faces[ i ].Corner[ 2 ].Normal = getLocalNormal( j++ );
		mesh_build.Faces[ i ].Corner[ 1 ].Vertex = triangle_geometry_index[ j ];
		mesh_build.Faces[ i ].Corner[ 1 ].Normal = getLocalNormal( j++ );
		mesh_build.Faces[ i ].Corner[ 0 ].Vertex = triangle_geometry_index[ j ];
		mesh_build.Faces[ i ].Corner[ 0 ].Normal = getLocalNormal( j++ );
		if( isVeryVerbose() )
		{
			CExport::instance().getApplication().LogMessage( L"Processing face '" + CString( CValue( long( i ) ) ) + L"' vertices and normals pass." );
			nlinfo( "Processing face '%u' vertices and normals pass.", i );
		}
		if( isInsaneVerbosity() )
		{
			// TODO : log to XSI
			nlinfo( "corner '0' vertice id '%u'", (uint32)getTriangleIndices()[j - 3] );
			nlinfo( "corner '0' normal '%s' from index '%u'", getLocalNormal( j - 3 ).asString().c_str(), j - 3 );
			nlinfo( "corner '1' vertice id '%u'", (uint32)getTriangleIndices()[j - 2] );
			nlinfo( "corner '1' normal '%s' from index '%u'", getLocalNormal( j - 2 ).asString().c_str(), j - 2 );
			nlinfo( "corner '2' vertice id '%u'", (uint32)getTriangleIndices()[ j - 1 ] );
			nlinfo( "corner '2' normal '%s' from index '%u'", getLocalNormal( j - 1 ).asString().c_str(), j - 1 );
		}
	}

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Face vertice indexes and normals embedded." );
		nlinfo( "Face vertice indexes and normals embedded." );
	}

	// Export UVW And Mapping Channels

	mesh_build.VertexFlags |= 1 << ( CVertexBuffer::TexCoord0 );

	j = 0;
	for( uint32 i = 0; i < faces; i++ )
	{
		mesh_build.Faces[ i ].Corner[ 2 ].Uvws[ 0 ] = CUVW( 1.0f - float( triangle_geometry_uvws[ j ].u ), 1.0f - float( triangle_geometry_uvws[ j++ ].v ), 0.0f );
		mesh_build.Faces[ i ].Corner[ 1 ].Uvws[ 0 ] = CUVW( 1.0f - float( triangle_geometry_uvws[ j ].u ), 1.0f - float( triangle_geometry_uvws[ j++ ].v ), 0.0f );
		mesh_build.Faces[ i ].Corner[ 0 ].Uvws[ 0 ] = CUVW( 1.0f - float( triangle_geometry_uvws[ j ].u ), 1.0f - float( triangle_geometry_uvws[ j++ ].v ), 0.0f );
		if( isVeryVerbose() )
		{
			CExport::instance().getApplication().LogMessage( L"Processing face '" + CString( CValue( long( i ) ) ) + L"' uvw pass." );
			nlinfo( "Processing face '%u' uvw pass.", i );
		}
		if( isInsaneVerbosity() )
		{
			// TODO : log to XSI
			nlinfo( "corner '0' uvw '%f, %f, %f'", triangle_geometry_uvws[ j - 3 ].u, triangle_geometry_uvws[ j - 3 ].v, 0 );
			nlinfo( "corner '1' uvw '%f, %f, %f'", triangle_geometry_uvws[ j - 2 ].u, triangle_geometry_uvws[ j - 2 ].v, 0 );
			nlinfo( "corner '2' uvw '%f, %f, %f'", triangle_geometry_uvws[ j - 1 ].u, triangle_geometry_uvws[ j - 1 ].v, 0 );
		}
	}
	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"UVW And Mapping Channels embedded." );
		nlinfo( "UVW And Mapping Channels embedded." );
	}

	// Export Vertex Program

	// TODO : Export Vertex Program
	
	// IMeshVertexProgram
	// CMeshVPWindTree
	// NeedVertexColor = true

	mesh_build.MeshVertexProgram = NULL;

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Vertex programs embedded." );
		nlinfo( "Vertex programs embedded." );
	}

	// Export Colors

	mesh_build.Faces.resize( faces );

	// TODO : decide if vertexcolors are to be exported

	// if NeedVertexColor
	// mesh_build.VertexFlags |= CVertexBuffer::PrimaryColorFlag;

	for( uint32 i = 0; i < faces; i++ )
	{
		//mesh_build.Faces[ i ].Corner[ 0 ].Color = CRGBA( 0, 0, 0, 255 );
		//mesh_build.Faces[ i ].Corner[ 1 ].Color = CRGBA( 0, 0, 0, 255 );
		//mesh_build.Faces[ i ].Corner[ 2 ].Color = CRGBA( 0, 0, 0, 255 );
		if( isVeryVerbose() )
		{
			//CExport::instance().getApplication().LogMessage( L"Processing face '" + CString( CValue( long( i ) ) ) + L"' vertex color pass." );
			//nlinfo( "Processing face '%u' vertex color pass.", i );
		}
		if( isInsaneVerbosity() )
		{
			// TODO : log to XSI
			//nlinfo( "corner '0' color '%s' ", " 0, 0, 0, 255 " );
			//nlinfo( "corner '1' color '%s' ", " 0, 0, 0, 255 " );
			//nlinfo( "corner '2' color '%s' ", " 0, 0, 0, 255 " );
		}
	}

	if( is_verbose )
	{
		//CExport::instance().getApplication().LogMessage( L"Face vertice colors embedded." );
		//nlinfo( "Face vertice colors embedded." );
	}

}

void CNeLMesh::getXSIProperties()
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Aquiring NeL property page data from XSI." );
		nlinfo( "Aquiring NeL property page data from XSI." );
	}

	// Get NeL Property Page From X3D Object
	Property custom_property = object.GetProperties().GetItem( L"NeLProperties" );
	Parameter parameter;

	// Get Verbose ( IO Page )
	parameter = custom_property.GetParameters().GetItem(L"IO_Never_Export_CheckBox" );
	//is_exported = bool( parameter.GetValue() );

	// Get Verbose ( IO Page )
	parameter = custom_property.GetParameters().GetItem(L"IO_Verbose_CheckBox" );
	is_verbose = bool( parameter.GetValue() );

	// Get Very Verbose ( IO Page )
	parameter = custom_property.GetParameters().GetItem(L"IO_Very_Verbose_CheckBox" );
	is_very_verbose = bool( parameter.GetValue() );

	// Get Insane Verbosity ( IO Page )
	parameter = custom_property.GetParameters().GetItem(L"IO_Insane_Verbosity_CheckBox" );
	is_insane_verbosity = bool( parameter.GetValue() );

	// Get MRM Option ( MRM Page )
	parameter = custom_property.GetParameters().GetItem(L"MRM_Active_CheckBox" );
	is_mrm = bool( parameter.GetValue() );

	// Get Max Distance Value ( MRM & LOD Pages ) 
	parameter = custom_property.GetParameters().GetItem(L"MRM_Distance_Max_Slider" );
	max_distance = float( parameter.GetValue() );

	// Get Shadow Caster Option ( Lighting Page )
	parameter = custom_property.GetParameters().GetItem(L"LIGHT_Shadow_Caster_CheckBox" );
	is_shadow_caster = bool( parameter.GetValue() );

	// Get Shadow Reciever Option ( Lighting Page )
	parameter = custom_property.GetParameters().GetItem(L"LIGHT_Shadow_Reciever_CheckBox" );
	is_shadow_reciever = bool( parameter.GetValue() );

	// Get Shadow Reciever Option ( Collision Page )
	parameter = custom_property.GetParameters().GetItem(L"COLLISION_Camera_RadioGroup" );
	camera_collision = TCameraCollision( long( parameter.GetValue() ) );

	// Get Max Percent Shadow Faces Value ( Lighting Page ) 
	parameter = custom_property.GetParameters().GetItem(L"LIGHTING_Shadow_Mesh_Percent_Slider" );
	shadow_percent_faces = float( parameter.GetValue() );

	// Get Max Number Shadow Faces ( Lighting Page ) 
	parameter = custom_property.GetParameters().GetItem(L"LIGHTING_Shadow_Mesh_Faces_Slider" );
	shadow_number_faces = float( parameter.GetValue() );

	// Get Local Light Attenuation Option ( Lighting Page ) 
	parameter = custom_property.GetParameters().GetItem(L"LIGHTING_Local_Lighting_CheckBox" );
	is_local_light_attenuated = bool( parameter.GetValue() );

	// Get Export Animated Materials Option ( Animation Page ) 
	parameter = custom_property.GetParameters().GetItem(L"ANIMATION_Export_Animated_Materials_CheckBox" );
	is_exporting_animated_materials = bool( parameter.GetValue() );

	// Get Export Shape Option ( IO Page ) 
	parameter = custom_property.GetParameters().GetItem(L"IO_Export_As_Shape_CheckBox" );
	is_shape = bool( parameter.GetValue() );

	// Get Export Vegetable Option ( IO Page ) 
	parameter = custom_property.GetParameters().GetItem(L"IO_Export_As_Vegetable_CheckBox" );
	is_vegetable = bool( parameter.GetValue() );

	// Get Export Character LOD Option ( IO Page ) 
	parameter = custom_property.GetParameters().GetItem(L"IO_Export_As_Character_Lod_CheckBox" );
	is_character_lod = bool( parameter.GetValue() );

	// Get Export Object Name As File Name Option ( IO Page ) 
	parameter = custom_property.GetParameters().GetItem(L"IO_Use_Object_Name_For_Export_CheckBox" );
	is_object_name_file_name = bool( parameter.GetValue() );

	// Get Alpha Option ( Vegetable Page ) 
	parameter = custom_property.GetParameters().GetItem(L"VEGETATION_Alpha_CheckBox" );
	vegetable_alpha_blend = bool( parameter.GetValue() );

	// Get Alpha Sub Option ( Vegetable Page ) 
	parameter = custom_property.GetParameters().GetItem(L"VEGETATION_Alpha_Sub_Radio_Group" );
	if( long( parameter.GetValue() ) == 0l )
	{
		vegetable_pre_compute_lighting = true;
		vegetable_lighted = true;
	}
	else if( long( parameter.GetValue() ) == 1l )
	{
		vegetable_pre_compute_lighting = false;
		vegetable_lighted = true;
	}
	else if( long( parameter.GetValue() ) == 2l )
	{
		vegetable_pre_compute_lighting = false;
		vegetable_lighted = false;
	}

	// Get Bend Center Mode Option ( Vegetable Page ) 
	parameter = custom_property.GetParameters().GetItem(L"VEGETATION_Bend_Radio_Group" );
	vegetable_bend_center_mode = long( parameter.GetValue() );

	// Get Bend Factor Option ( Vegetable Page ) 
	parameter = custom_property.GetParameters().GetItem(L"VEGETATION_Bend_Factor_Slider" );
	vegetable_bend_factor = float( parameter.GetValue() );

	// Get Best Sided Lighting Option ( Vegetable Page ) 
	parameter = custom_property.GetParameters().GetItem(L"VEGETATION_Best_Sided_Lighting_CheckBox" );
	vegetable_best_sided_pre_computed_lighting = bool( parameter.GetValue() );

	// Get Double Sided Option ( Vegetable Page ) 
	parameter = custom_property.GetParameters().GetItem(L"VEGETATION_Double_Sided_CheckBox" );
	vegetable_double_sided = bool( parameter.GetValue() );

	// Get Double Sided Option ( Vegetable Page ) 
	parameter = custom_property.GetParameters().GetItem(L"LOD_Bone_Distance_Max_Slider" );
	bone_distance_max = float( parameter.GetValue() );

	// Get Double Sided Option ( Vegetable Page ) 
	parameter = custom_property.GetParameters().GetItem(L"IO_Export_Associated_Skeleton_CheckBox" );
	is_exporting_skeleton = bool( parameter.GetValue() );

}

void CNeLMesh::getXSIPoints()
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Aquiring point vertex data from XSI." );
		nlinfo( "Aquiring point vertex data from XSI." );
		CExport::instance().getApplication().LogMessage( L"Aquiring point index data from XSI." );
		nlinfo( "Aquiring point index data from XSI." );
		CExport::instance().getApplication().LogMessage( L"Aquiring point normal data from XSI." );
		nlinfo( "Aquiring point normal data from XSI." );
	}

	// Get Primitive Data From X3D Object
	Primitive primitive( object.GetActivePrimitive() );

	// Get Geometry Data From Primitve Data
	Geometry geometry( primitive.GetGeometry( getTime() ) );

	// Get Point Data From Geometry Data
	CPointRefArray points( geometry.GetPoints() );

	// Get & Store Indice Data From Point Data
	point_geometry_index = points.GetIndexArray();

	// Get & Store Vertice Data From Point Data
	point_geometry_vertices = points.GetPositionArray();

	// Get & Store Normal Data From Point Data
	point_geometry_normals = points.GetNormalArray();

}

void CNeLMesh::getXSIFacets()
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Aquiring facet index data from XSI." );
		nlinfo( "Aquiring facet index data from XSI." );
		CExport::instance().getApplication().LogMessage( L"Aquiring facet normal data from XSI." );
		nlinfo( "Aquiring facet normal data from XSI." );
	}

	// Get Primitive Data From X3D Object
	Primitive primitive( object.GetActivePrimitive() );

	// Get Geometry Data From Primitve Data
	Geometry geometry( primitive.GetGeometry( getTime() ) );

	// Get Facet Data From Geometry Data
	CFacetRefArray facets( geometry.GetFacets() );

	// Get & Store Indice Data From Facet Data
	facet_geometry_index = facets.GetIndexArray();

	// Get & Store Normal Data From Facet Data
	facet_geometry_normals = facets.GetNormalArray();

}

void CNeLMesh::getXSITriangles()
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Aquiring triangle vertex data from XSI." );
		nlinfo( "Aquiring triangle vertex data from XSI." );
		CExport::instance().getApplication().LogMessage( L"Aquiring triangle index data from XSI." );
		nlinfo( "Aquiring triangle index data from XSI." );
		CExport::instance().getApplication().LogMessage( L"Aquiring triangle normal data from XSI." );
		nlinfo( "Aquiring triangle normal data from XSI." );
		CExport::instance().getApplication().LogMessage( L"Aquiring triangle vertex color data from XSI." );
		nlinfo( "Aquiring triangle vertex color data from XSI." );
		CExport::instance().getApplication().LogMessage( L"Aquiring triangle uvw data from XSI." );
		nlinfo( "Aquiring triangle uvw data from XSI." );
	}

	// Get Primitive Data From X3D Object
	Primitive primitive( object.GetActivePrimitive() );

	// Get Geometry Data From Primitve Data
	Geometry geometry( primitive.GetGeometry( getTime() ) );

	// Get Triangle Data From Geometry Data
	CTriangleRefArray triangles( geometry.GetTriangles() );

	// Get & Store Indice Data From Triangle Data
	triangle_geometry_index = triangles.GetIndexArray();

	// Get & Store Vertice Data From Triangle Data
	triangle_geometry_vertices = triangles.GetPositionArray();

	// Get & Store Normal Data From Triangle Data
	triangle_geometry_normals = triangles.GetPolygonNodeNormalArray();

	// Get & Store Color Data From Triangle Data
	triangle_geometry_uvws = triangles.GetUVArray();

	// Get & Store UVW Data From Triangle Data
	triangle_geometry_vertice_colors = triangles.GetColorArray();

}

void CNeLMesh::getXSIMaterials()
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Aquiring material data from XSI." );
		nlinfo( "Aquiring material data from XSI." );
	}

	CRefArray materials( object.GetMaterials() );

	mesh_base_build.Materials.clear();
	uint32 material_count = NULL;

	// Iterate Materials
	for ( long i = 0; i < materials.GetCount(); i++ )
	{
		// Get Shader
		Material xsi_material( materials[ i ] );
		CRefArray shaders( xsi_material.GetShaders() );
		Shader xsi_shader = shaders.GetItem( L"NeL_Material" );
		if ( xsi_shader.IsValid() )
		{
			// Export NeL Material From Shader
			CMaterial nel_material;
			CMaterials::instance().convertMaterial( xsi_shader, nel_material );
			mesh_base_build.Materials.push_back( nel_material );
			material_count++;
		}
	}

	if ( material_count == 0 )
	{
		CMaterial nel_material;
		nel_material.initLighted();
		nel_material.setLighting( true, CRGBA( 150, 150, 150, 255 ), CRGBA( 150, 150, 150, 255 ), CRGBA( 150, 150, 150, 255 ), CRGBA( 229, 229, 229, 255 ), 50 );
		mesh_base_build.Materials.push_back( nel_material );
	}

}

void CNeLMesh::getXSITransforms()
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Aquiring local transform data from XSI." );
		nlinfo( "Aquiring local transform data from XSI." );
		CExport::instance().getApplication().LogMessage( L"Aquiring global transform data from XSI." );
		nlinfo( "Aquiring global transform data from XSI." );
	}

	// Get Kinematics From X3D Object
	XSI::Kinematics kinematics( object.GetKinematics() );

	// Get Local Kinematics State From Kinematics
	XSI::KinematicState kinematics_local_state( kinematics.GetLocal() );

	// Get Global Kinematics State From Kinematics
	XSI::KinematicState kinematics_global_state( kinematics.GetGlobal() );

	// Get Local Transformation From Kinematics State
	XSI::MATH::CTransformation local_transform( kinematics_local_state.GetTransform( getTime() ) );

	// Get Global Transformation From Kinematics State
	XSI::MATH::CTransformation global_transform( kinematics_local_state.GetTransform( getTime() ) );

	// Store Local Space Transform Data For Reuse
	local_scale = local_transform.GetScaling();
	local_rotation = local_transform.GetRotationXYZAngles();
	local_translation = local_transform.GetTranslation();
	local_rotation_quaternion = local_transform.GetRotationQuaternion();
	local_pivot = MATH::CVector3( 0.0f, 0.0f, 0.0f );

	// Store Global Space Transform Data For Reuse
	global_scale = global_transform.GetScaling();
	global_rotation = global_transform.GetRotationXYZAngles();
	global_translation = global_transform.GetTranslation();
	global_rotation_quaternion = global_transform.GetRotationQuaternion();
	global_pivot = MATH::CVector3( 0.0f, 0.0f, 0.0f );

	// Variables
	//MATH::CVector3 convert_vector;
	//MATH::CQuaternion convert_quaternion;

	// Rotate Axis To NeL Format ( Zup )
	//convert_vector.Set( 1.5707963267948966192313216916398, 0.0, 0.0 );
	//convert_quaternion.SetFromXYZAnglesValues( 1.5707963267948966192313216916398, 0.0, 0.0 );

	// Store New Vector And Quaternion
	//local_rotation_quaternion *= convert_quaternion;
	//local_rotation += convert_vector;
	//global_rotation_quaternion *= convert_quaternion;
	//global_rotation += convert_vector;

}

void CNeLMesh::getXSISkin()
{

	Envelope envelope = object.GetEnvelopes()[ 0 ];
	CRefArray deformers = envelope.GetDeformers();

	// Check For Skinning
	if ( !is_skinned )
	{
		return;
	}

	// Clear Existing
	mesh_build.SkinWeights.clear();
	mesh_build.BonesNames.clear();

	// Resize Skin Weights To Vertices
	mesh_build.SkinWeights.resize( mesh_build.Vertices.size() );

	// Clear Existing
	skinning_weights.clear();
	skinning_vertices.clear();
	skinning_bones.clear();

	// Pre Aquire Weights
	for ( long i = 0; i < deformers.GetCount(); i++ )
	{

		skinning_vertices.clear();

		// Get List Of Deformers
		CSIBCArray< single_weight > array;
		X3DObject current_deformer = deformers[ i ];
		CDoubleArray weight_array = envelope.GetDeformerWeights( current_deformer, 0.0 );
		CClusterElementArray cluster_elements = envelope.GetElements( 0 );
		CLongArray index_array = cluster_elements.GetArray();
		CString deformer_name = current_deformer.GetFullName();

		for ( long j = 0; j < weight_array.GetCount(); j++ )
		{
			array.Extend(1);
			array[ array.GetUsed() - 1 ].index = index_array[ j ];
			array[ array.GetUsed() - 1 ].weight = weight_array[ j ];
		}
		
		qsort( array.ArrayPtr(), array.GetUsed(), sizeof( single_weight ), CompareProc );
		
		for ( long j = 0; j < weight_array.GetCount(); j++ )
		{	
			if ( is_verbose )
			{
				nlinfo( "Aquired weight '%f' for vertex '%u' on bone '%s'.", float( array[ j ].weight ), array[ j ].index , CExport::instance().convertString( deformer_name ).c_str() );
			}
			skinning_vertices.insert( _STL::make_pair( array[ j ].index, float( array[ j ].weight ) ) );
		}

		// Store Deformers Weights & Name
		skinning_bones.push_back( skinning_vertices );
		mesh_build.BonesNames.push_back( CExport::instance().convertString( deformer_name ) );
	}

	// Setup Vertex Weights
	for ( uint32 i = 0; i < mesh_build.SkinWeights.size(); i++ )
	{

		// Clear Existing
		skinning_weights.clear();

		// Obtain Weights For This Vertex
		long bone = skinning_bones.size() - 1;

		for ( _STL::vector< _STL::multimap< long, float > >::reverse_iterator it = skinning_bones.rbegin(); it != skinning_bones.rend(); it++ )
		{
			for ( _STL::multimap< long, float >::reverse_iterator it2 = it->rbegin(); it2 != it->rend(); it2++ )
			{
				if ( it2->first == i && it2->second > 0.0f )
				{
					if ( is_verbose )
					{
						nlinfo( "Stored weight for vertex '%u' has weight '%f' for bone '%u'.", it2->first, it2->second, bone );
					}
					skinning_weights.insert( _STL::make_pair( it2->second, bone ) );
				}
			}
			bone--;
		}
		
		// Clear Existing
		for ( long j = 0; j < NL3D_MESH_SKINNING_MAX_MATRIX; j++ )
		{
			mesh_build.SkinWeights[ i ].MatrixId[ j ] = NULL;
			mesh_build.SkinWeights[ i ].Weights[ j ] = NULL;
		}

		// Remove Smaller Weights Until We Are Under Max
		while ( skinning_weights.size() > NL3D_MESH_SKINNING_MAX_MATRIX )
		{
			if ( is_verbose )
			{
				nlinfo( "WARNING : erased a low effector weight on vertex '%u' to optomise skinning.", i );
			}
			skinning_weights.erase( skinning_weights.begin() );
		}

		float total_weight = NULL;

		// Calculate Total Weight
		for ( _STL::multimap< float, long >::iterator it = skinning_weights.begin(); it != skinning_weights.end(); it++ )
		{
			total_weight += it->first;
		}

		// Log Status Message
		if ( is_verbose )
		{
			nlinfo( "Total weight of vertex '%u' is '%f'", i, total_weight );
		}

		long id = 0;

		// Store Weights
		for ( _STL::multimap< float, long >::iterator it = skinning_weights.begin(); it != skinning_weights.end(); it++ )
		{
			mesh_build.SkinWeights[ i ].MatrixId[ id ]= it->second;
			mesh_build.SkinWeights[ i ].Weights[ id ]= it->first / total_weight;

			if ( is_verbose )
			{
				nlinfo( "%u, %u, %f, %u", i, id, it->first, it->second );
			}		

			id++;
		}
	}

}

void CNeLMesh::getXSIBone( long parent, X3DObject xsi_bone )
{
	
	CBoneBase nel_bone;

	current_bone++;

	//nldebug( "Parsing bone '%i : %s' parent was '%i'.", current_bone, CExport::instance().convertString( xsi_bone.GetFullName() ).c_str(), parent );

	nel_bone.Name = CExport::instance().convertString( xsi_bone.GetFullName() );
	nel_bone.FatherId = parent;
	nel_bone.UnheritScale = false;
	nel_bone.LodDisableDistance = bone_distance_max;

	// Get Transforms From Kinematics
	Kinematics kinematics( xsi_bone.GetKinematics() );

	KinematicState kinematics_state( kinematics.GetLocal() );
	StaticKinematicState kinematics_static_state( xsi_bone.GetStaticKinematicState() );

	MATH::CTransformation bone_transform( kinematics_state.GetTransform( getTime() ) );
	MATH::CTransformation bone_static_transform( kinematics_static_state.GetTransform( getTime() ) );

	// Parent's Parent Transform Compensation
	if ( SIObject( SIObject( xsi_bone.GetParent() ).GetParent() ).IsA( siChainEffectorID ) )
	{
		CExport::instance().getApplication().LogMessage( L"" + xsi_bone.GetFullName() + L"'s father's father was an effector." );
		bone_transform.MulInPlace( X3DObject( X3DObject( xsi_bone.GetParent() ).GetParent() ).GetKinematics().GetLocal().GetTransform( getTime() ) );
	}

	// Parent Transform Compensation
	if ( SIObject( xsi_bone.GetParent() ).IsA( siChainRootID ) )
	{
		CExport::instance().getApplication().LogMessage( L"" + xsi_bone.GetFullName() + L"'s father was a root." );
		bone_transform.MulInPlace( X3DObject( xsi_bone.GetParent() ).GetKinematics().GetLocal().GetTransform( getTime() ) );
	}

	// Create Static Matrix
	CMatrix	bone_static_transform_matrix;
	bone_static_transform_matrix.identity();
	bone_static_transform_matrix.setRot( CExport::instance().convertQuaternionAndAxis( bone_static_transform.GetRotationQuaternion() ) );
	bone_static_transform_matrix.setPos( CExport::instance().convertVectorAndAxisAndInvertX( bone_static_transform.GetTranslation() ) );

	bone_static_transform_matrix.invert();

	// Store Space Transform
	nel_bone.DefaultRotEuler = CVector( 0.0f, 0.0f, 0.0f );
	nel_bone.DefaultPivot = CVector( 0.0f, 0.0f, 0.0f );
	nel_bone.DefaultScale = CExport::instance().convertVectorAndAxis( bone_transform.GetScaling() );

	// Move Root Bone To 0, 0, 0
	if ( parent == -1 )
	{
		nel_bone.DefaultRotQuat = CQuat::Identity;
		nel_bone.DefaultPos = CVector( 0.0f, 0.0f, 0.0f );
	}
	else
	{
		nel_bone.DefaultRotQuat = CExport::instance().convertQuaternionAndAxis( bone_transform.GetRotationQuaternion() );
		nel_bone.DefaultPos = CExport::instance().convertVectorAndAxisAndInvertX( bone_transform.GetTranslation() );
	}

	// Bind Location
	nel_bone.InvBindPos = bone_static_transform_matrix;

	// Skin Scale
	nel_bone.SkinScale = CVector( 1.0f, 1.0f, 1.0f );

	// Store Bone
	skeleton_bones.push_back( nel_bone );

	// Call Recursivly For All Children
	getXSIBoneChildren( current_bone, xsi_bone );

}

void CNeLMesh::getXSIBoneChildren( long parent, X3DObject xsi_bone )
{

	// Get Children
	CRefArray children = xsi_bone.GetChildren();

	// Sort Through Children For Bones, Roots And Effectors
	for ( long i = 0; i < children.GetCount(); i++ )
	{
		if ( SIObject( children[ i ] ).IsA( siChainRootID ) )
		{
			CExport::instance().getApplication().LogMessage( X3DObject( children[ i ] ).GetFullName() + L", is a root, and will NOT be exported." );
			getXSIBoneChildren( parent, X3DObject( children[ i ] ) );
		}
		else if ( SIObject( children[ i ] ).IsA( siChainEffectorID ) )
		{
			CExport::instance().getApplication().LogMessage( X3DObject( children[ i ] ).GetFullName() + L", is an effector, and will NOT be exported." );
			getXSIBoneChildren( parent, X3DObject( children[ i ] ) );
		}
		else if ( SIObject( children[ i ] ).IsA( siChainBoneID ) )
		{
			CExport::instance().getApplication().LogMessage( X3DObject( children[ i ] ).GetFullName() + L", is a bone, and will be exported." );
			getXSIBone( parent, X3DObject( children[ i ] ) );
		}
		else
		{
			CExport::instance().getApplication().LogMessage( X3DObject( children[ i ] ).GetFullName() + L", is an unknown, and will NOT be exported." );
		}
	}

}

void CNeLMesh::createSkeleton()
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Aquiring Bones from XSI." );
		nlinfo( "Aquiring Bones from XSI." );
	}

	// TODO : handle multiple skeletons in same scene

	// Create A New Skeleton Object
	skeleton = new CSkeletonShape();

	// Variables
	X3DObject root_bone;

	// Find The First Occurance Of A Root Bone
	for( int i = 0; i < CExport::instance().getApplication().GetActiveSceneRoot().FindChildren( L"", L"", CStringArray() ).GetCount(); i++)
	{
		X3DObject node = CExport::instance().getApplication().GetActiveSceneRoot().FindChildren( L"", L"", CStringArray() )[ i ];
		if ( isDeformer( node ) )
		{
			if ( SIObject( node.GetParent() ).IsA( siChainRootID ) )
			{
				if ( !SIObject( SIObject( node.GetParent() ).GetParent() ).IsA( siChainRootID ) || !SIObject( SIObject( node.GetParent() ).GetParent() ).IsA( siChainEffectorID ) || !SIObject( SIObject( node.GetParent() ).GetParent() ).IsA( siChainBoneID ) || !SIObject( SIObject( node.GetParent() ).GetParent() ).IsA( siChainElementID ) )
				{
					CExport::instance().getApplication().LogMessage( L"" + node.GetFullName() + L" is the root bone, and will be exported." );
					root_bone = node;
					break;
				}
			}
		}
	}

	// Parse Bones
	getXSIBone( -1, root_bone );

	// Build Skeleton From Bones
	skeleton->build( skeleton_bones );

}

void CNeLMesh::getXSIMRMParameters()
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Aquiring MRM Paramaters from XSI." );
		nlinfo( "Aquiring MRM Paramaters from XSI." );
	}

	// Get NeL Property Page From X3D Object
	Property custom_property = object.GetProperties().GetItem( L"NeLProperties" );
	Parameter parameter;

	// Get Skin Reduction
	parameter = custom_property.GetParameters().GetItem(L"MRM_Skinning_RadioGroup" );
	mrm_parameters.SkinReduction = CMRMParameters::TSkinReduction( long( parameter.GetValue() ) );

	// Get Steps
	parameter = custom_property.GetParameters().GetItem(L"MRM_Steps_Slider" );
	mrm_parameters.NLods = long( parameter.GetValue() );

	// Get Poly Divisor
	parameter = custom_property.GetParameters().GetItem(L"MRM_Poly_Slider" );
	mrm_parameters.Divisor = long( parameter.GetValue() );

	// Get Distance Finest
	parameter = custom_property.GetParameters().GetItem(L"MRM_Distance_Finest_Slider" );
	mrm_parameters.DistanceFinest = float( parameter.GetValue() );

	// Get Distance Middle
	parameter = custom_property.GetParameters().GetItem(L"MRM_Distance_Middle_Slider" );
	mrm_parameters.DistanceMiddle = float( parameter.GetValue() );

	// Get Distance Course
	parameter = custom_property.GetParameters().GetItem(L"MRM_Distance_Coarse_Slider" );
	mrm_parameters.DistanceCoarsest = float( parameter.GetValue() );

}

void CNeLMesh::getXSIMRMInterfaces()
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Aquiring MRM Interfaces from XSI." );
		nlinfo( "Aquiring MRM Interfaces from XSI." );
	}

	mesh_build.Interfaces.clear();
	mesh_build.InterfaceLinks.clear();
	mesh_build.InterfaceVertexFlag.clear();

	mesh_build.InterfaceLinks.resize( mesh_build.Vertices.size() );
	mesh_build.InterfaceVertexFlag.resize( mesh_build.Vertices.size() );

	// Set Link To None
	for ( uint32 i = 0; i < mesh_build.InterfaceLinks.size(); i++ )
	{
		mesh_build.InterfaceLinks[ i ].InterfaceId = -1;
	}

	// Get Clusters From Object
	CRefArray clusters ( object.GetActivePrimitive().GetGeometry( 0.0f ).GetClusters() );

	// Variables
	CValueArray arguments( 1 );
	CValue return_value;
	
	uint32 id = 0;

	for( long i = 0; i < clusters.GetCount(); i++ )
	{

		// Generate Names
		CString mrm_interface_data = L"NeL_MRM_Interface_Data_" + CString( CValue( long( i ) ) );
		CString data_name = object.GetFullName() + L"." + mrm_interface_data;

		// Get User Data Blob
		UserDataBlob user_data_blob = object.GetProperties().GetItem( mrm_interface_data );

		// Check For Interface
		if ( !user_data_blob.IsValid() )
		{
			if( is_verbose )
			{
				// Log Error Message
				CExport::instance().getApplication().LogMessage( L"Object '" + CString( CValue( long( i ) ) ) + L"' is not an interface." );
				nlinfo( "Object '%u' is not an interface.", i );
			}
		}
		else
		{
			// Create Binary Buffer
			unsigned char * blob_buffer = NULL;
			unsigned int blob_size = NULL;

			// Fill Buffer
			user_data_blob.GetValue( blob_buffer, blob_size );
	
			// Trap Errors
			if ( blob_buffer == NULL )
			{
				CExport::instance().getApplication().LogMessage( L"Critical error an interface data block is empty." );
				nlinfo( "Critical error an interface data block is empty." );
				break;
			}

			// Convert Back To CLongArray
			long *raw_array = (long *)blob_buffer;

			CLongArray common_vertices;

			common_vertices.Resize( raw_array[ 0 ] );

			for ( long j = 0; j  < raw_array[ 0 ]; j++ )
			{
				common_vertices[ j ] = uint32( raw_array[ j + 1 ] );
			}

			// Configure Interface
			mesh_build.Interfaces.resize( id + 1 );
			mesh_build.Interfaces[ id ].Vertices.resize( common_vertices.GetCount() );

			for ( long j = 0; j < common_vertices.GetCount(); j++ )
			{
				// Configure Interface Vertex
				mesh_build.Interfaces[ id ].Vertices[ j ].Pos = CExport::instance().convertVectorAndAxis( point_geometry_vertices[ common_vertices[ j ] ] );
				mesh_build.Interfaces[ id ].Vertices[ j ].Normal = CExport::instance().convertVectorAndAxis( point_geometry_normals[ common_vertices[ j ] ] );

				// Configure Link Between Mesh Vertex And Interface Vertex
				mesh_build.InterfaceLinks[ common_vertices[ j ] ].InterfaceId = id;
				mesh_build.InterfaceLinks[ common_vertices[ j ] ].InterfaceVertexId = j;

				// Flag Vertex
				mesh_build.InterfaceVertexFlag.set( common_vertices[ j ] );
			}
			id++;
		}
	}

}

void CNeLMesh::writeFile() {

	if( is_verbose )
	{
		if ( is_shape )
		{
		// Log Status Message
			CExport::instance().getApplication().LogMessage( L"Writing shape file." );
			nlinfo( "Writing shape file." );
		}
		else if ( is_vegetable )
		{
			// Log Status Message
			CExport::instance().getApplication().LogMessage( L"Writing vegetable file." );
			nlinfo( "Writing vegetable file." );
		}
		else if ( is_character_lod )
		{
			// Log Status Message
			CExport::instance().getApplication().LogMessage( L"Writing character lod file." );
			nlinfo( "Writing character lod file." );
		}
	}

	if ( !is_object_name_file_name )
	{
		CComAPIHandler xsi_ui_tool_kit;
		xsi_ui_tool_kit.CreateInstance( L"XSI.UIToolkit");

		CValue file_browser_pointer = xsi_ui_tool_kit.GetProperty( L"FileBrowser" );
		CComAPIHandler file_browser( file_browser_pointer );

		if ( is_shape )
		{
			file_browser.PutProperty( L"DialogTitle", L"Export Shape As" ); 
			file_browser.PutProperty( L"Filter", L"Shape (*.shape)|*.shape||" ); 
		}
		else if ( is_vegetable )
		{
			file_browser.PutProperty( L"DialogTitle", L"Export Vegetable As" ); 
			file_browser.PutProperty( L"Filter", L"Vegetable (*.veget)|*.veget||" ); 
		}
		else if ( is_character_lod )
		{
			file_browser.PutProperty( L"DialogTitle", L"Export Character LOD As" ); 
			file_browser.PutProperty( L"Filter", L"Character LOD (*.clod)|*.clod||" ); 
		}

		file_browser.PutProperty( L"InitialDirectory", L"C:/" ); 
		file_browser.PutProperty( L"FileBaseName", object.GetFullName() ); 

		CValue result;

		file_browser.Call( L"ShowSave", result );

		file_name = CExport::instance().convertString( CFile::getFilenameWithoutExtension( CExport::instance().convertString( CString( file_browser.GetProperty( L"FilePathName" ) ) ) ) );
		file_path = CExport::instance().convertString( CFile::getPath( CExport::instance().convertString( CString( file_browser.GetProperty( L"FilePathName" ) ) ) ) );

		if ( is_shape )
		{
			file_name = file_path + file_name + L".shape";
		}
		else if ( is_vegetable )
		{
			file_name = file_path + file_name + L".veget";
		}
		else if ( is_character_lod )
		{
			file_name = file_path + file_name + L".clod";
		}
	}
	else
	{
		if ( is_shape )
		{
			file_name = CExport::instance().file_path + object.GetFullName() + L".shape";
		}
		else if ( is_vegetable )
		{
			file_name = CExport::instance().file_path + object.GetFullName() + L".veget";
		}
		else if ( is_character_lod )
		{
			file_name = CExport::instance().file_path + object.GetFullName() + L".clod";
		}
	}

	// New File Stream
	COFile file;

	// Write File
	if( file.open( CExport::instance().convertString( file_name ), false, false, false ))
	{
		try
		{
			if ( is_shape )
			{
				CShapeStream streamable_mesh( i_shape );
				streamable_mesh.serial( file );
			}
			else if ( is_vegetable )
			{
				vegetable.serial( file );
			}
			else if ( is_character_lod )
			{
				//lod.serial( file );
			}
		}
		catch(...)
		{
			CExport::instance().getApplication().LogMessage( L"Failed to stream object to file." );
			nlinfo( "Failed to stream object to file." );
		}
		file.close();
	}

	if ( is_exporting_skeleton )
	{
		writeSkeletonFile();
	}

}

void CNeLMesh::writeSkeletonFile()
{

	if( is_verbose )
	{
		// Log Status Message
		CExport::instance().getApplication().LogMessage( L"Writing skeleton file." );
		nlinfo( "Writing skeleton file." );
	}

	if ( is_exporting_skeleton )
	{
			file_name = CExport::instance().file_path + object.GetFullName() + L".skel";
	}
	else
	{
		CComAPIHandler xsi_ui_tool_kit;
		xsi_ui_tool_kit.CreateInstance( L"XSI.UIToolkit");

		CValue file_browser_pointer = xsi_ui_tool_kit.GetProperty( L"FileBrowser" );
		CComAPIHandler file_browser( file_browser_pointer );

		file_browser.PutProperty( L"DialogTitle", L"Export Skeleton As" ); 
		file_browser.PutProperty( L"Filter", L"Skeleton (*.skel)|*.skel||" ); 

		file_browser.PutProperty( L"InitialDirectory", L"C:/" ); 
		file_browser.PutProperty( L"FileBaseName", L"" ); 

		CValue result;

		file_browser.Call( L"ShowSave", result );

		file_name = CExport::instance().convertString( CFile::getFilenameWithoutExtension( CExport::instance().convertString( CString( file_browser.GetProperty( L"FilePathName" ) ) ) ) );
		file_path = CExport::instance().convertString( CFile::getPath( CExport::instance().convertString( CString( file_browser.GetProperty( L"FilePathName" ) ) ) ) );

		file_name = file_path + file_name + L".skel";
	}

	COFile skeleton_file;

	if( skeleton_file.open( CExport::instance().convertString( file_name ), false, false, false ))
	{
		try
		{
			CShapeStream streamable_skeleton( skeleton );
			streamable_skeleton.serial( skeleton_file );
		}
		catch(...)
		{
			CExport::instance().getApplication().LogMessage( L"Failed to stream skeleton to file." );
			nlinfo( "Failed to stream skeleton to file." );
		}
		skeleton_file.close();
	}

}

void CNeLMesh::createMesh()
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Creating a basic CMesh." );
		nlinfo( "Creating a basic CMesh." );
	}

	// TODO : getBlendShapes();

	CMesh* mesh = new NL3D::CMesh();

	mesh->build( mesh_base_build, mesh_build );
    mesh->setBlendShapes( getMeshBuildObject().BlendShapes );

	i_shape = mesh;
	i_shape->setDistMax( max_distance );

}

void CNeLMesh::createMRMMesh()
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Creating a CMeshMRM." );
		nlinfo( "Creating a CMeshMRM." );
	}

	// TODO : getMRMBlendShapes();

	if ( CMeshMRMSkinned::isCompatible( mesh_build ) && mesh_build_list.empty() )
	{
		CMeshMRMSkinned* mesh_mrm = new CMeshMRMSkinned;
		mesh_mrm->build( mesh_base_build, mesh_build, mrm_parameters );
		if( is_shadow_caster )
		{
			CMeshMRMSkinnedGeom* mrm_mesh_geometry = (CMeshMRMSkinnedGeom*)&mesh_mrm->getMeshGeom();
			createMRMShadows( mrm_mesh_geometry );
		}
		i_shape = mesh_mrm;
		i_shape->setDistMax( max_distance );
	}
	else
	{
		CMeshMRM* mesh_mrm = new CMeshMRM;
		mesh_mrm->build( mesh_base_build, mesh_build, mesh_build_list, mrm_parameters );
		if( is_shadow_caster && is_skinned )
		{
			CMeshMRMGeom* mrm_mesh_geometry = (CMeshMRMGeom*)&mesh_mrm->getMeshGeom();
			createMRMShadows( mrm_mesh_geometry );
		}
		i_shape = mesh_mrm;
		i_shape->setDistMax( max_distance );
	}

}

void CNeLMesh::createMultiLODMesh()
{
}

void CNeLMesh::createMRMShadows( CMeshMRMGeom* mrm_mesh_geometry )
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Creating MRM shadows." );
		nlinfo( "Creating MRM shadows." );
	}

	const _STL::vector< CMesh::CSkinWeight > &skin_weights = mrm_mesh_geometry->getSkinWeights();
	const CVertexBuffer &vertex_buffer = mrm_mesh_geometry->getVertexBuffer();

	_STL::vector< sint > used_vertices;
	_STL::vector< CMeshMRMGeom::CShadowVertex > shadow_vertices;
	_STL::vector< sint > vertex_to_v_skin;
	_STL::vector< uint32 > shadow_triangles;

	_STL::map< CMeshMRMGeom::CShadowVertex, uint32 > shadow_vertex_map;

	uint32 number_merged = NULL;
	uint32 lod_count = mrm_mesh_geometry->getNbLod();

	CVertexBufferRead vba;
	vertex_buffer.lock( vba );

	float number_minimum_mesh_faces = float( mrm_mesh_geometry->getLevelDetail().MinFaceUsed );
	float number_maximum_mesh_faces = float( mrm_mesh_geometry->getLevelDetail().MaxFaceUsed );
	float maximum_wanted_faces = number_maximum_mesh_faces * shadow_percent_faces;

	maximum_wanted_faces = min( maximum_wanted_faces, float( shadow_number_faces ) );

	float find_lod = ( ( maximum_wanted_faces - number_minimum_mesh_faces ) / ( number_maximum_mesh_faces - number_minimum_mesh_faces ) ) * ( lod_count - 1 );

	sint lod_id = sint( floor( 0.5f + find_lod ) );
	clamp( lod_id, 0, sint( lod_count - 1 ) );

	used_vertices.resize( skin_weights.size(), -1 );

	for( uint32 i = 0; i < mrm_mesh_geometry->getNbRdrPass( lod_id ); i++ )
	{
		const CIndexBuffer *pb = &mrm_mesh_geometry->getRdrPassPrimitiveBlock( lod_id, i );

		CIndexBufferRead iba;
		pb->lock( iba );
		if ( iba.getFormat() == CIndexBuffer::Indices32 )
		{
			const uint32 *triangle_pointer = (const uint32 *) iba.getPtr();
			for( uint32 j = 0; j < pb->getNumIndexes(); j++ )
			{
				uint index = *triangle_pointer;
				used_vertices[ index ] = index;
				triangle_pointer++;
			}
		}
		else
		{
			const uint16 *triangle_pointer = (const uint16 *) iba.getPtr();
			for( uint32 j = 0; j < pb->getNumIndexes(); j++ )
			{
				uint index = *triangle_pointer;
				used_vertices[ index ] = index;
				triangle_pointer++;
			}
		}
	}

	const _STL::vector< CMRMWedgeGeom > &geomorphs = mrm_mesh_geometry->getGeomorphs( lod_id );

	for( uint32 i = 0; i < geomorphs.size(); i++ )
	{
		uint true_index = geomorphs[ i ].End;
		used_vertices[ i ]= true_index;
		used_vertices[ true_index ]= true_index;
	}

	vertex_to_v_skin.resize( used_vertices.size() );
	shadow_vertices.reserve( used_vertices.size() );

	for( uint32 i = geomorphs.size(); i < used_vertices.size(); i++)
	{
		if( used_vertices[ i ] != -1 )
		{
			// Build Vertex
			CMeshMRMGeom::CShadowVertex shadow_vertice;
			shadow_vertice.Vertex = *(CVector*)vba.getVertexCoordPointer( i );

			// Select Best Matrix
			CMesh::CSkinWeight sw = skin_weights[ i ];
			float max_w = 0.0f;
			uint mat_id = NULL;

			for( uint32 j = 0; j < NL3D_MESH_SKINNING_MAX_MATRIX; j++)
			{
				if( sw.Weights[ j ] == 0 )
				{
					break;
				}
				if( sw.Weights[ j ] > max_w )
				{
					mat_id = sw.MatrixId[ j ];
					max_w = sw.Weights[ j ];
				}
			}

			shadow_vertice.MatrixId = mat_id;

			_STL::map< CMeshMRMGeom::CShadowVertex, uint >::iterator it = shadow_vertex_map.find( shadow_vertice );

			if( it == shadow_vertex_map.end() )
			{
				// Append
				uint32 index = shadow_vertices.size();
				vertex_to_v_skin[ i ] = index;
				shadow_vertices.push_back( shadow_vertice );
				shadow_vertex_map.insert( make_pair( shadow_vertice, index ) );
			}
			else
			{
				// Ok, map.
				vertex_to_v_skin[ i ] = it->second;
				number_merged++;
			}
		}
	}

	shadow_triangles.reserve( 1024 );

	for( uint32 i = 0; i < mrm_mesh_geometry->getNbRdrPass( lod_id ); i++)
	{
		const CIndexBuffer *pb = &mrm_mesh_geometry->getRdrPassPrimitiveBlock( lod_id, i );

		CIndexBufferRead iba;
		pb->lock( iba );

		if ( iba.getFormat() == CIndexBuffer::Indices32 )
		{
			const uint32 *triangle_pointer = (const uint32 *) iba.getPtr();
			for( uint32 j = 0; j < pb->getNumIndexes(); j++ )
			{
				uint32 index = *triangle_pointer;
				index = used_vertices[ index ];
				index = vertex_to_v_skin[ index ];
				shadow_triangles.push_back( index );
				triangle_pointer++;
			}
		}
		else
		{
			const uint16 *triangle_pointer = (const uint16 *) iba.getPtr();
			for( uint32 j = 0; j < pb->getNumIndexes(); j++ )
			{
				uint32 index = *triangle_pointer;
				index = used_vertices[ index ];
				index = vertex_to_v_skin[ index ];
				shadow_triangles.push_back( index );
				triangle_pointer++;
			}
		}
	}

	if( shadow_triangles.size() )
	{
		CIndexBuffer pb;
		pb.setNumIndexes( shadow_triangles.size() );
		{
			CIndexBufferReadWrite iba;
			pb.lock( iba );
			for( uint32 i = 0; i < shadow_triangles.size() / 3; i++ )
			{
				iba.setTri( i* 3, shadow_triangles[ i * 3 + 0 ], shadow_triangles[ i * 3 + 1 ], shadow_triangles[ i * 3 + 2 ] );
			}
		}

		// Optomise
		CStripifier stripifier;
		stripifier.optimizeTriangles( pb, pb );

		{
			CIndexBufferReadWrite iba;
			pb.lock( iba );

			if ( iba.getFormat() == CIndexBuffer::Indices32 )
			{
				const uint32 *triangle_pointer = (const uint32 *) iba.getPtr();
				for( uint32 j = 0; j < shadow_triangles.size(); j++ )
				{
					shadow_triangles[ j ] = *triangle_pointer;
					triangle_pointer++;
				}
			}
			else
			{
				const uint16 *triangle_pointer = (const uint16 *) iba.getPtr();
				for( uint32 j = 0; j < shadow_triangles.size(); j++ )
				{
					shadow_triangles[ j ] = *triangle_pointer;
					triangle_pointer++;
				}
			}
		}
	}

	// Set Shadow
	mrm_mesh_geometry->setShadowMesh( shadow_vertices, shadow_triangles );

}

void CNeLMesh::createMRMShadows( CMeshMRMSkinnedGeom* mrm_mesh_geometry )
{

	if( is_verbose )
	{
		CExport::instance().getApplication().LogMessage( L"Creating MRM shadows." );
		nlinfo( "Creating MRM shadows." );
	}

	_STL::vector< CMesh::CSkinWeight > skin_weights;
	mrm_mesh_geometry->getSkinWeights( skin_weights );

	CVertexBuffer vertex_buffer;
	mrm_mesh_geometry->getVertexBuffer( vertex_buffer );

	uint32 lod_count = mrm_mesh_geometry->getNbLod();

	CVertexBufferRead vba;
	vertex_buffer.lock( vba );

	float number_minimum_mesh_faces = float( mrm_mesh_geometry->getLevelDetail().MinFaceUsed );
	float number_maximum_mesh_faces = float( mrm_mesh_geometry->getLevelDetail().MaxFaceUsed );
	float maximum_wanted_faces = number_maximum_mesh_faces * shadow_percent_faces;

	maximum_wanted_faces = min( maximum_wanted_faces, float( shadow_number_faces ) );

	float find_lod = ( ( maximum_wanted_faces - number_minimum_mesh_faces ) / ( number_maximum_mesh_faces - number_minimum_mesh_faces ) ) * ( lod_count - 1 );

	sint lod_id = sint( floor( 0.5f + find_lod ) );
	clamp( lod_id, 0, sint( lod_count - 1 ) );

	_STL::vector< sint > used_vertices;

	used_vertices.resize( skin_weights.size(), -1 );

	for( uint32 i = 0; i < mrm_mesh_geometry->getNbRdrPass( lod_id ); i++ )
	{
		static CIndexBuffer block;
		mrm_mesh_geometry->getRdrPassPrimitiveBlock( lod_id, i, block );

		const CIndexBuffer *pb = &block;

		CIndexBufferRead iba;
		pb->lock( iba );

		if ( iba.getFormat() == CIndexBuffer::Indices32 )
		{
			const uint32 *triangle_pointer = (const uint32 *) iba.getPtr();
			for( uint32 j = 0; j < pb->getNumIndexes(); j++ )
			{
				uint32 index = *triangle_pointer;
				used_vertices[ index ] = index;
				triangle_pointer++;
			}
		}
		else
		{
			const uint16 *triangle_pointer = (const uint16 *) iba.getPtr();
			for( uint32 j = 0; j < pb->getNumIndexes(); j++ )
			{
				uint32 index = *triangle_pointer;
				used_vertices[ index ] = index;
				triangle_pointer++;
			}
		}
	}

	const _STL::vector< CMRMWedgeGeom > &geomorphs = mrm_mesh_geometry->getGeomorphs( lod_id );

	for( uint32 i = 0; i < geomorphs.size(); i++ )
	{
		uint32 true_index = geomorphs[ i ].End;
		used_vertices[ i ] = true_index;
		used_vertices[ true_index ] = true_index;
	}

	_STL::vector< CMeshMRMSkinnedGeom::CShadowVertex > shadow_vertices;
	_STL::vector< sint > vertex_to_v_skin;

	vertex_to_v_skin.resize( used_vertices.size() );
	shadow_vertices.reserve( used_vertices.size() );

	_STL::map< CMeshMRMSkinnedGeom::CShadowVertex, uint32 > shadow_vertex_map;

	uint32 number_merged = NULL;

	for( uint32 i = geomorphs.size(); i < used_vertices.size(); i++)
	{
		if( used_vertices[ i ] != -1 )
		{
			// Build Vertex
			CMeshMRMSkinnedGeom::CShadowVertex shadow_vertice;
			shadow_vertice.Vertex = *(CVector*)vba.getVertexCoordPointer( i );

			// Select Best Matrix
			CMesh::CSkinWeight sw = skin_weights[ i ];
			float max_w = NULL;
			uint mat_id = NULL;

			for( uint32 j = 0; j < NL3D_MESH_SKINNING_MAX_MATRIX; j++)
			{
				if( sw.Weights[ j ] == 0 )
				{
					break;
				}
				if( sw.Weights[ j ] > max_w )
				{
					mat_id = sw.MatrixId[ j ];
					max_w = sw.Weights[ j ];
				}
			}

			shadow_vertice.MatrixId = mat_id;

			_STL::map< CMeshMRMSkinnedGeom::CShadowVertex, uint >::iterator it = shadow_vertex_map.find( shadow_vertice );

			if( it == shadow_vertex_map.end() )
			{
				// Append
				uint32 index = shadow_vertices.size();
				vertex_to_v_skin[ i ] = index;
				shadow_vertices.push_back( shadow_vertice );
				shadow_vertex_map.insert( make_pair( shadow_vertice, index ) );
			}
			else
			{
				// Ok, map.
				vertex_to_v_skin[ i ] = it->second;
				number_merged++;
			}
		}
	}

	_STL::vector< uint32 > shadow_triangles;

	shadow_triangles.reserve( used_vertices.size() );

	for( uint32 i = 0; i < mrm_mesh_geometry->getNbRdrPass( lod_id ); i++)
	{
		static CIndexBuffer block;
		mrm_mesh_geometry->getRdrPassPrimitiveBlock( lod_id, i, block );

		const CIndexBuffer *pb = &block;

		CIndexBufferRead iba;
		pb->lock( iba );

		if ( iba.getFormat() == CIndexBuffer::Indices32 )
		{
			const uint32 *triangle_pointer = (const uint32 *) iba.getPtr();
			for( uint32 j = 0; j < pb->getNumIndexes(); j++ )
			{
				uint32 index = *triangle_pointer;
				index = used_vertices[ index ];
				index = vertex_to_v_skin[ index ];
				shadow_triangles.push_back( index );
				triangle_pointer++;
			}
		}
		else
		{
			const uint16 *triangle_pointer = (const uint16 *) iba.getPtr();
			for( uint32 j = 0; j < pb->getNumIndexes(); j++ )
			{
				uint32 index = *triangle_pointer;
				index = used_vertices[ index ];
				index = vertex_to_v_skin[ index ];
				shadow_triangles.push_back( index );
				triangle_pointer++;
			}
		}
	}

	if( shadow_triangles.size() )
	{
		CIndexBuffer pb;
		pb.setNumIndexes( shadow_triangles.size() );
		{
			CIndexBufferReadWrite iba;
			pb.lock( iba );
			for( uint32 i = 0; i < shadow_triangles.size() / 3; i++ )
			{
				iba.setTri( i* 3, shadow_triangles[ i * 3 + 0 ], shadow_triangles[ i * 3 + 1 ], shadow_triangles[ i * 3 + 2 ] );
			}
		}

		// Optomise
		CStripifier stripifier;
		stripifier.optimizeTriangles( pb, pb );

		{
			CIndexBufferReadWrite iba;
			pb.lock( iba );

			if ( iba.getFormat() == CIndexBuffer::Indices32 )
			{
				const uint32 *triangle_pointer = (const uint32 *) iba.getPtr();
				for( uint32 j = 0; j < shadow_triangles.size(); j++ )
				{
					shadow_triangles[ j ] = *triangle_pointer;
					triangle_pointer++;
				}
			}
			else
			{
				const uint16 *triangle_pointer = (const uint16 *) iba.getPtr();
				for( uint32 j = 0; j < shadow_triangles.size(); j++ )
				{
					shadow_triangles[ j ] = *triangle_pointer;
					triangle_pointer++;
				}
			}
		}
	}

	// Set Shadow
	mrm_mesh_geometry->setShadowMesh( shadow_vertices, shadow_triangles );

}

void CNeLMesh::animateMaterials()
{

	if ( is_exporting_animated_materials )
	{

		//meshBase->setAnimatedMaterial( material_id, material_name );

	}

}

void CNeLMesh::createVegetMesh()
{

	// Create Mesh
	CMesh mesh;
	mesh.build( mesh_base_build, mesh_build );

	// Copy The Built Vertex Buffer
	vegetable_build.VB = mesh.getVertexBuffer();

	// Copy The Built Primiitive Block
	vegetable_build.PB = mesh.getRdrPassPrimitiveBlock( 0, 0 );

	// Set Alpha Properties
	vegetable_build.AlphaBlend = vegetable_alpha_blend;
	vegetable_build.PreComputeLighting = vegetable_pre_compute_lighting;
	vegetable_build.DoubleSided = vegetable_double_sided;
	vegetable_build.Lighted = vegetable_lighted;
	vegetable_build.BestSidedPreComputeLighting = vegetable_best_sided_pre_computed_lighting;

	// Set Bend Properties
	vegetable_build.MaxBendWeight = vegetable_bend_factor;
	vegetable_build.BendCenterMode = ( CVegetableShapeBuild::TBendCenterMode )vegetable_bend_center_mode;

	// Build
	vegetable.build( vegetable_build );

}
