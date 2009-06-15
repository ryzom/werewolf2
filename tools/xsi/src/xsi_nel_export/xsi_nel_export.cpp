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
#include "xsi_nel_export.h"
#include "xsi_nel_mesh_lib.h"
#include "xsi_nel_material.h"

#include "xsi_nel_kd_tree.h"

//
// Namespaces
//

using namespace XSI;
using namespace NLMISC;
using namespace NL3D;

//
// Class Functions
//

CVector CExport::convertVector( MATH::CVector3 xsi_vector )
{
	CVector nel_vector;
	nel_vector.x = float( xsi_vector.GetX() );	
	nel_vector.y = float( xsi_vector.GetY() );	
	nel_vector.z = float( xsi_vector.GetZ() );
	return nel_vector;
}

MATH::CVector3 convertVector( CVector nel_vector )
{
	MATH::CVector3 xsi_vector;
	xsi_vector.PutX( float( nel_vector.x ) );	
	xsi_vector.PutY( float( nel_vector.y ) );
	xsi_vector.PutZ( float( nel_vector.z ) );
	return xsi_vector;
}

CVector CExport::convertVectorAndAxis( MATH::CVector3 xsi_vector )
{
	CVector nel_vector;
	nel_vector.x = float( xsi_vector.GetX() );
	nel_vector.y = float( xsi_vector.GetZ() );
	nel_vector.z = float( xsi_vector.GetY() );
	return nel_vector;
}

CVector CExport::convertVectorAndAxisAndInvertX( MATH::CVector3 xsi_vector )
{
	CVector nel_vector;
	nel_vector.x = float( xsi_vector.GetX() ) * -1.0f;
	nel_vector.y = float( xsi_vector.GetZ() );
	nel_vector.z = float( xsi_vector.GetY() );
	return nel_vector;
}

CQuat CExport::convertQuaternion( MATH::CQuaternion xsi_quaternion )
{
	CQuat nel_quaternion;
	nel_quaternion.x = float( xsi_quaternion.GetX() );
	nel_quaternion.y = float( xsi_quaternion.GetY() );
	nel_quaternion.z = float( xsi_quaternion.GetZ() );
	nel_quaternion.w = float( xsi_quaternion.GetW() );
	return nel_quaternion;
}

MATH::CQuaternion CExport::convertQuaternion( CQuat nel_quaternion )
{
	MATH::CQuaternion xsi_quaternion;
	xsi_quaternion.PutX( float( nel_quaternion.x ) );
	xsi_quaternion.PutY( float( nel_quaternion.y ) );
	xsi_quaternion.PutZ( float( nel_quaternion.z ) );
	xsi_quaternion.PutW( float( nel_quaternion.w ) );
	return xsi_quaternion;
}

CQuat CExport::convertQuaternionAndAxis( MATH::CQuaternion xsi_quaternion )
{
	CQuat nel_quaternion;
	nel_quaternion.x = float( xsi_quaternion.GetX() );
	nel_quaternion.y = float( xsi_quaternion.GetZ() );
	nel_quaternion.z = float( xsi_quaternion.GetY() );
	nel_quaternion.w = float( xsi_quaternion.GetW() );
	return nel_quaternion;
}

_STL::string CExport::convertString( CString xsi_string )
{
	_STL::string nel_string;
	USES_CONVERSION;
	nel_string = W2CA( xsi_string.GetWideString() );
	return nel_string;
}

CString CExport::convertString( _STL::string nel_string )
{
	CString xsi_string;
	USES_CONVERSION;
	xsi_string = A2CW( nel_string.c_str() );
	return xsi_string;
}

CStatus CExport::exportSkeleton()
{
	CNeLMesh* shape = new CNeLMesh();

	shape->createSkeleton();
	shape->writeSkeletonFile();

	return CStatus::OK;
}


CStatus CExport::exportMesh( X3DObject& object )
{

	// Check Object Type
	if ( !object.IsA( siX3DObjectID ) )
	{
		CExport::instance().getApplication().LogMessage( L"Cannot export " + object.GetFullName() + L" is not the required type.", siWarningMsg );
		nlinfo( "Cannot export  %s is not the required type.", CExport::instance().convertString( object.GetFullName() ).c_str() );
		return CStatus::Fail;
	}

	CNeLMesh* shape = new CNeLMesh();

	// Setup X3D Object & Application
	shape->setObject( object );

	// Get Properties
	shape->getXSIProperties();

	if ( shape->isVerbose() )
	{
		// TODO : log to XSI
		nlinfo( "properties : isVegetable '%i', isSkinned '%i', isMRM '%i', isMultiLOD '%i'", shape->isVegetable(), shape->isSkinned(), shape->isMRM(), shape->isMultiLOD() );
	}

	// Get Geometry From XSI
	shape->getXSIPoints();
	shape->getXSIFacets();
	shape->getXSITriangles();
	shape->getXSIMaterials();
	shape->getXSITransforms();

	// Create Shape Core
	shape->createMeshBaseBuild();
	shape->createMeshBuild();

	// Get Skin
	shape->getXSISkin();

	// Build Skeleton
	if ( shape->isExportingSkeleton() )
	{
		shape->createSkeleton();
	}

	// Build Vegetable
	if ( shape->isVegetable() )
	{
		shape->createVegetMesh();
		shape->writeFile();
		return CStatus::OK;
	}

	// Build MRM Mesh With Blend Shapes
	if ( shape->isMRM() )
	{
		shape->getXSIMRMParameters();
		shape->getXSIMRMInterfaces();
		shape->createMRMMesh();
		shape->animateMaterials();
		shape->writeFile();
		return CStatus::OK;
	}

	// Build Multi LOD
	if ( shape->isMultiLOD() )
	{
		shape->createMultiLODMesh();
		shape->animateMaterials();
		shape->writeFile();
		return CStatus::OK;
	}

	// Build Normal Mesh
	if ( !shape->isMRM() )
	{
		shape->createMesh();
		shape->animateMaterials();
		shape->writeFile();
		return CStatus::OK;
	}
	
	return CStatus::Fail;

}

CStatus CExport::buildMRMInterfaces( X3DObject object )
{

	// Get Geometry
	MATH::CVector3Array points ( object.GetActivePrimitive().GetGeometry( 0.0f ).GetPoints().GetPositionArray() );

	// Get Indices
	CLongArray indices ( object.GetActivePrimitive().GetGeometry( 0.0f ).GetPoints().GetIndexArray() );

	// Variables
	CLongArray common_vertices;
	Cluster common_cluster;
	uint32 index;
	uint32 interface_id;
	_STL::vector<CKDTree::SVertex> vertices;

	// Get All Selected Objects
	CRefArray objects = xsi_application.GetSelection().GetArray();

	if ( objects.GetCount() < 1 )
	{
		// Log Error Message
		CExport::instance().getApplication().LogMessage( L"Cannot build interfaces need at least one object.", siWarningMsg );
		nlinfo( "Cannot build interfaces need at least one object." );
		return CStatus::Fail;
	}

	// Check Object Against All Except Itself
	for( long i = 0; i < objects.GetCount(); i++ )
	{
		// Get Object To Test
		X3DObject test_object( objects[ i ] );

		if ( objects[ i ].GetClassID() != siX3DObjectID )
		{
			// Log Error Message
			CExport::instance().getApplication().LogMessage( L"Object '" + objects[ i ].GetAsText() + L"' cannot be compared to an interface." );
			nlinfo("Object '%s' cannot be compared to an interface.", CExport::instance().convertString(  objects[ i ].GetAsText() ).c_str() );
		}
		else if( test_object.GetFullName() != object.GetFullName() )
		{
			// Log Status Message
			CExport::instance().getApplication().LogMessage( L"Checking for interfaces with '" + object.GetFullName() + L"' vs '" + test_object.GetFullName() + L"'." );
			nlinfo("Checking for interfaces with '%s' vs '%s'.", CExport::instance().convertString( object.GetFullName() ).c_str(), CExport::instance().convertString( test_object.GetFullName() ).c_str() );

			// Get Geometry
			MATH::CVector3Array test_points ( test_object.GetActivePrimitive().GetGeometry( 0.0f ).GetPoints().GetPositionArray() );
			CLongArray test_indices ( test_object.GetActivePrimitive().GetGeometry( 0.0f ).GetPoints().GetIndexArray() );

			// Clear Reusable Data
			vertices.clear();
			common_vertices.Clear();

			index = 0;

			// Populate KD Tree Vertice List
			for( long j = 0; j < test_indices.GetCount(); j++ )
			{
				CKDTree::SVertex vertex;

				MATH::CMatrix4 transform_matrix = test_object.GetKinematics().GetGlobal().GetTransform( 0.0f ).GetMatrix4();	

				MATH::CVector3 untransformed_vector = test_points[ test_indices[ j ] ];
				MATH::CVector3 transformed_vector;
				transformed_vector.MulByMatrix4( untransformed_vector, transform_matrix );

				vertex.index = test_indices[ j ];
				vertex.position = CExport::instance().convertVector( transformed_vector );

				vertices.push_back( vertex );
			}

			// Build KD Tree
			if ( CKDTree::instance().build( vertices ) == CStatus::Fail )
			{
				xsi_application.LogMessage( L"Check all the vertices on '" + object.GetFullName() + L"' are welded.");
				nlinfo( "Check all the vertices on '%s' are welded.", CExport::instance().convertString( object.GetFullName() ).c_str() );
				return CStatus::Fail;
			}

			// Compare Vertices
			for( long j = 0; j < indices.GetCount(); j++ )
			{
				MATH::CMatrix4 transform_matrix = object.GetKinematics().GetGlobal().GetTransform( 0.0f ).GetMatrix4();	

				MATH::CVector3 untransformed_vector = points[ indices[ j ] ];
				MATH::CVector3 transformed_vector;
				transformed_vector.MulByMatrix4( untransformed_vector, transform_matrix );

				if( CKDTree::instance().checkIfVertexExists( CExport::instance().convertVector( transformed_vector ) ) )
				{
					common_vertices.Add( j );
				}
			}

			// Build Interface If We Found One
			if ( common_vertices.GetCount() > 0 )
			{
				// Convert CLongArray To Raw Memory Array
				long interface_size = 1 + common_vertices.GetCount();
				long raw_interface_size = interface_size * sizeof ( long );
				long *raw_array = (long *)malloc( raw_interface_size );

				// Store Array Length
				raw_array[ 0 ] = common_vertices.GetCount();

				// Convert Array To Binary
				for ( long k = 0; k  < common_vertices.GetCount(); k++ )
				{
					raw_array[ k + 1 ] = common_vertices[ k ];
				}

				// Build Cluster
				CRefArray clusters ( object.GetActivePrimitive().GetGeometry( 0.0f ).GetClusters() );

				// Clear Interface ID
				interface_id = 0;

				// Get Next Interface
				for( long k = 0; k < clusters.GetCount(); k++ )
				{
					CString mrm_interface_data = L"NeL_MRM_Interface_Data_" + CString( CValue( long( k ) ) );
					UserDataBlob user_data_blob = object.GetProperties().GetItem( mrm_interface_data );
					if ( user_data_blob.IsValid() )
					{
						interface_id++;
					}
				}

				// Generate Name
				CString display_name = L"NeL_MRM_Interface_Display_" + CString( CValue( long( interface_id ) ) );
				CString data_name = L"NeL_MRM_Interface_Data_" + CString( CValue( long( interface_id ) ) );

				// Set Cluster Properties
				object.GetActivePrimitive().GetGeometry( 0.0f ).AddCluster( L"pnt", display_name, common_vertices, common_cluster );
				common_cluster.PutParameterValue( L"red", long(255) );
				common_cluster.PutParameterValue( L"green", long(128) );
				common_cluster.PutParameterValue( L"blue", long(96) );

				// Store User Data
				UserDataBlob common_vertices_blob;
				object.AddProperty( L"UserDataBlob", false, data_name, common_vertices_blob );
				common_vertices_blob.PutValue( (unsigned char *)raw_array, raw_interface_size );
				
				// Release Memory For User Data
				free( raw_array );

				// Log Status Message
				CExport::instance().getApplication().LogMessage( L"An interface found with '" + object.GetFullName() + L"' vs '" + test_object.GetFullName() + L"'." );
				nlinfo("An interface found with '%s' vs '%s'.", CExport::instance().convertString( object.GetFullName() ).c_str(), CExport::instance().convertString( test_object.GetFullName() ).c_str() );

			}
			else
			{
				// Log Error Message
				CExport::instance().getApplication().LogMessage( L"No interfaces found with '" + object.GetFullName() + L"' vs '" + test_object.GetFullName() + L"'." );
				nlinfo("No interfaces found with '%s' vs '%s'.", CExport::instance().convertString( object.GetFullName() ).c_str(), CExport::instance().convertString( test_object.GetFullName() ).c_str() );
			}
		}
		else
		{
			// Log Status Message
			CExport::instance().getApplication().LogMessage( L"Skipping interface check on self '" + object.GetFullName() + L"'." );
			nlinfo("Skipping interface check on self '%s'.", CExport::instance().convertString( object.GetFullName() ).c_str() );
		}
	}

	return CStatus::OK;

}

CStatus CExport::clearMRMInterfaces( X3DObject object )
{

	// Get Clusters From Object
	CRefArray clusters ( object.GetActivePrimitive().GetGeometry( 0.0f ).GetClusters() );

	// Variables
	CValueArray arguments( 1 );
	CValue return_value;

	// Remove Each Interface Cluster And User Data From Object
	for( long i = 0; i < clusters.GetCount(); i++ )
	{
		// Generate Names
		CString mrm_interface_display = L"NeL_MRM_Interface_Display_" + CString( CValue( long( i ) ) );
		CString mrm_interface_data = L"NeL_MRM_Interface_Data_" + CString( CValue( long( i ) ) );
		CString cluster_name = object.GetFullName() + L".polymsh.cls." + mrm_interface_display;
		CString data_name = object.GetFullName() + L"." + mrm_interface_data;

		// Get User Data Blob
		UserDataBlob user_data_blob = object.GetProperties().GetItem( mrm_interface_data );

		// Check For Interface
		if ( !user_data_blob.IsValid() )
		{
			xsi_application.LogMessage( L"Object '" + CString( CValue( long( i ) ) ) + L"' is not an interface." );
		}
		else
		{
			arguments[0] = cluster_name;
			CExport::instance().getApplication().ExecuteCommand( L"DeleteObj", arguments, return_value );

			arguments[0] = data_name;
			CExport::instance().getApplication().ExecuteCommand( L"DeleteObj", arguments, return_value );

			xsi_application.LogMessage( L"Interface '" + CString( CValue( long( i ) ) ) + L"' cleared." );
		}
	}

	return CStatus::OK;

}

CStatus CExport::clearObjectProperties( X3DObject object )
{

	// Variables
	CValueArray arguments( 1 );
	CValue return_value;

	// Get Custom Property
	CustomProperty custom_property = object.GetProperties().GetItem( L"NeLProperties" );

	// Check For Custom Property
	if ( !custom_property.IsValid() )
	{
		xsi_application.LogMessage( L"Object is not NeL properties." );
		// TODO : log to NeL
	}
	else
	{
		arguments[0] = object.GetFullName() + L".NeLProperties";
		CExport::instance().getApplication().ExecuteCommand( L"DeleteObj", arguments, return_value );

		xsi_application.LogMessage( L"NeL properties cleared." );
		nlinfo( "NeL properties cleared." );
	}

	return CStatus::OK;

}

CStatus CExport::configureCameras()
{
	CValueArray arguments( 2 );
	CValue return_value;
	CString view;

	arguments[0] = L"Camera.camvis.compclsselvert";
	arguments[1] = CValue ( true );

	CExport::instance().getApplication().ExecuteCommand( L"SetValue", arguments, return_value );

	for ( uint32 i = 0; i < 4; i++ )
	{
		if ( i == 0 )
		{
			view = L"ViewA";
		}
		else if ( i == 1 )
		{
			view = L"ViewB";
		}
		else if ( i == 2 )
		{
			view = L"ViewC";
		}
		else if ( i == 3 )
		{
			view = L"ViewD";
		}

		arguments[0] = L"Views." + view + L".UserCamera.camvis.compclsselvert";
		arguments[1] = CValue ( true );

		CExport::instance().getApplication().ExecuteCommand( L"SetValue", arguments, return_value );

		arguments[0] = L"Views." + view + L".TopCamera.camvis.compclsselvert";
		arguments[1] = CValue ( true );

		CExport::instance().getApplication().ExecuteCommand( L"SetValue", arguments, return_value );

		arguments[0] = L"Views." + view + L".FrontCamera.camvis.compclsselvert";
		arguments[1] = CValue ( true );

		CExport::instance().getApplication().ExecuteCommand( L"SetValue", arguments, return_value );

		arguments[0] = L"Views." + view + L".RightCamera.camvis.compclsselvert";
		arguments[1] = CValue ( true );

		CExport::instance().getApplication().ExecuteCommand( L"SetValue", arguments, return_value );

		arguments[0] = L"Views." + view + L".SpotCamera.camvis.compclsselvert";
		arguments[1] = CValue ( true );

		CExport::instance().getApplication().ExecuteCommand( L"SetValue", arguments, return_value );
	}

	return CStatus::OK;
	
}


