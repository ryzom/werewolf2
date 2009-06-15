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

#ifndef NEL_TOOLS_KD_TREE_H
#define NEL_TOOLS_KD_TREE_H

//
// Includes
//



//
// class CKDTree in xsi_nel_kd_tree.h
//

class CKDTree : public CSingleton<CKDTree>
{
public:

	struct SVertex
	{
		uint32				index;
		NLMISC::CVector		position;
	};

	XSI::CStatus build( const _STL::vector< SVertex >& vertices );
	bool checkIfVertexExists( const NLMISC::CVector& vert);

protected:

	struct SKDTreeNode
	{
		bool					is_leaf;
		
		NLMISC::CPlane			splitter_plane;
		SVertex					vertex;

		uint32					front_node;
		uint32					back_node;
	};

	_STL::vector< SKDTreeNode >	nodes;
	
	XSI::CStatus subdivide( uint32 node_index, const _STL::vector< SVertex >& vertices );
	bool checkIfVertexExists( uint32 node_index, const NLMISC::CVector& vert );

};


#endif