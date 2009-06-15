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

#include "xsi_nel_kd_tree.h"

//
// Namespaces
//

using namespace XSI;
using namespace NLMISC;
using namespace NL3D;

//
// Functions
//

CStatus CKDTree::subdivide( uint32 node_index, const _STL::vector<SVertex> &vertices )
{
	uint32 node_depth;

	CVector center_point = CVector( 0.0f, 0.0f, 0.0f );
	CVector distance_from_center = CVector( 0.0f, 0.0f, 0.0f );

	static CVector planes_normal[ 3 ] = { CVector( 1.0f, 0.0f, 0.0f ), CVector( 0.0f, 1.0f, 0.0f ), CVector( 0.0f, 0.0f, 1.0f ) };

	_STL::vector< SVertex > front_vertices, back_vertices;

	// Sub Division
	if( vertices.size() == 1 )
	{
		nodes[ node_index ].is_leaf = true;
		nodes[ node_index ].vertex = vertices[ 0 ];
	}
	else
	{
		nodes[ node_index ].is_leaf	= false;

		// Calculate Center Point
		for( uint32 i = 0; i < vertices.size(); i++ )
		{
			center_point += vertices[ i ].position;
		}

		center_point /= float( vertices.size() );

		// Compute Distance From Center For Each Axis
		for( uint32 i = 0; i < vertices.size(); i++ )
		{
			distance_from_center.x += fabsf( vertices[ i ].position.x - center_point.x );
			distance_from_center.y += fabsf( vertices[ i ].position.y - center_point.y );
			distance_from_center.z += fabsf( vertices[ i ].position.z - center_point.z );
		}

		node_depth = 0;

		// We Choose Node Depth That Has Largest Distance
		if( distance_from_center.x > distance_from_center.y )
		{
			if( distance_from_center.x > distance_from_center.z )
			{
				node_depth = 0;
			}
			else
			{
				node_depth = 2;
			}
		}
		else if( distance_from_center.y > distance_from_center.x )
		{
			if( distance_from_center.y > distance_from_center.z )
			{
				node_depth = 1;
			}
			else 
			{
				node_depth = 2;
			}
		}

		if( distance_from_center.x == 0.0f && node_depth == 0 )
		{
			if( distance_from_center.y == 0.0f )
			{
				node_depth = 2;
			}
			else
			{
				node_depth = 1;
			}
		} 
		if( distance_from_center.y == 0.0f && node_depth == 1 )
		{
			if( distance_from_center.x == 0.0f )
			{
				node_depth = 2;
			}
			else
			{
				node_depth = 0;
			}
		} 
		if( distance_from_center.z == 0.0f && node_depth == 2 )
		{
			if( distance_from_center.y == 0.0f )
			{
				node_depth = 0;
			}
			else
			{
				node_depth = 1;
			}
		}
	
		// Duplicate Vertexes
		if( distance_from_center.x == 0.0f && distance_from_center.y == 0.0f && distance_from_center.z == 0.0f )
		{
			return CStatus::Fail;
		}

		// Setup Splitter Plane
		nodes[ node_index ].splitter_plane.make( planes_normal[ node_depth % 3 ], center_point );

		// Setup Front & Back Vertices
		for( uint32 i = 0; i < vertices.size(); i++ )
		{
			if( nodes[ node_index ].splitter_plane * vertices[ i ].position > 0.0f )
			{
				front_vertices.push_back( vertices[ i ] );
			}
			else
			{
				back_vertices.push_back( vertices[ i ] );
			}
		}

		// Create Front & Back Node Next Sub Division Populates
		uint32 current_node_index = nodes.size();
		SKDTreeNode node;

		nodes.push_back(node);
		nodes.push_back(node);

		// Setup Child Nodes
		nodes[ node_index ].front_node = current_node_index;
		nodes[ node_index ].back_node = current_node_index + 1;

		if ( subdivide( current_node_index, front_vertices ) != CStatus::OK )
		{
			return CStatus::Fail;
		};
		if ( subdivide( current_node_index + 1, back_vertices ) != CStatus::OK )
		{
			return CStatus::Fail;
		};
	}

	return CStatus::OK;
}

CStatus CKDTree::build( const _STL::vector<SVertex> &vertices )
{

	// Remove Any Previous Tree
	nodes.clear();

	// Create A Root
	SKDTreeNode root_node;
	nodes.push_back( root_node );

	// Create Branches And Leaves
	return subdivide( 0, vertices );

}

bool CKDTree::checkIfVertexExists( const CVector &vertice )
{

	// Scan Tree
	return checkIfVertexExists( 0, vertice );

}

bool CKDTree::checkIfVertexExists( uint32 node_index, const CVector &vertice )
{

	if( nodes[ node_index ].is_leaf )
	{
		if( nodes[ node_index ].vertex.position == vertice )
		{
			return true;
		}
	}
	else
	{
		if( nodes[ node_index ].splitter_plane * vertice > 0.0f )
		{
			// Check Front Nodes
			return checkIfVertexExists( nodes[ node_index ].front_node, vertice );
		}
		else
		{
			// Check Back Nodes
			return checkIfVertexExists( nodes[ node_index ].back_node, vertice );
		}
	} 

	return false;

}
