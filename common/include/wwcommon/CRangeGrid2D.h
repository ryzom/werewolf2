/**
 * \file CRangeGrid2D.h
 * \date June 2006
 * \author Henri Kuuste
 */

/* Copyright, 2006 Werewolf
 *
 * This file is part of Werewolf.
 * Werewolf is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.

 * Werewolf is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Werewolf; see the file COPYING. If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 */

#ifndef __CRANGEGRID2D_H__
#define __CRANGEGRID2D_H__

//
// Standard Includes
//

//
// System Includes
//
#include <vector>
#include <memory.h>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>

//
// Werewolf Includes
//	
#include "CGenericObjectPool.h"

//
// Namespaces
//

namespace WWCOMMON {

template <class T>
class CRangeGrid2D {
public:
	typedef std::vector<T>				DataList;
	typedef DataList*	 				DataListPtr;
	typedef CGenericObjectPool<DataList> CellPool;
	typedef typename CellPool::Holder	DataListHolder;
	typedef DataListHolder*				DataListHolderPtr;
	typedef std::vector<DataListPtr>	List;
	typedef struct {
		uint x;
		uint y;
	} Position;

	CRangeGrid2D(float sideLength, uint numSideCells);
	~CRangeGrid2D();
	
	void 								insert(T data, Position pos);
	void 								remove(T data, Position pos);
	DataListPtr							getCell(Position pos);
	List								getRange(Position pos, float radius);
	List								getOldRange(Position oldPos, Position newPos, float radius);
	List								getNewRange(Position oldPos, Position newPos, float radius);

	float								getCellWidth();

private:
	typedef DataListHolderPtr**			Grid2D;

	Grid2D								m_Grid;
	CellPool							m_CellPool;

	float 								m_SideLength;
	uint								m_NumSideCells;
	float								m_CellWidth;
};


template <class T>
CRangeGrid2D<T>::CRangeGrid2D(float sideLength, uint numSideCells) 
	: m_SideLength(sideLength), m_NumSideCells(numSideCells), m_CellWidth(sideLength/numSideCells) {
	// TODO henri:evertone calculate the initial size of the pool based on max players and stuff.
	m_CellPool.initialize(50, 0, CRangeGrid2D<T>::CellPool::GROWTH_PERCENT, 25, CRangeGrid2D<T>::CellPool::ALLOC_LAZY);
	m_Grid = new CRangeGrid2D<T>::DataListHolderPtr*[m_NumSideCells];
	for (uint i = 0; i < m_NumSideCells; ++i) {
	        m_Grid[i] = new CRangeGrid2D<T>::DataListHolderPtr[m_NumSideCells];
			// fill with NULL
			memset(&m_Grid[i][0], 0, m_NumSideCells*sizeof(CRangeGrid2D<T>::DataListHolderPtr));
	}
}

template <class T>
CRangeGrid2D<T>::~CRangeGrid2D() {
	for (uint i = 0; i < m_NumSideCells; ++i) {
		for(uint j = 0; j < m_NumSideCells; ++j) {
			if(m_Grid[i][j]) {
				(*m_Grid[i][j])->clear();
//				delete m_Grid[i][j];
			}
		}

		delete[] m_Grid[i];
	}

	delete[] m_Grid;
}

template <class T>
typename CRangeGrid2D<T>::List CRangeGrid2D<T>::getOldRange(Position oldPos, Position newPos, float radius) {
	uint rad = (uint)ceil(radius/m_CellWidth);
//	nldebug("Finding old objects in the move (%d %d) -> (%d %d) with range %d", oldPos.x, oldPos.y, newPos.x, newPos.y, rad);
	uint newLeft	= newPos.x - rad;
	uint newRight	= newPos.x + rad;
	uint newTop		= newPos.y + rad;
	uint newBottom	= newPos.y - rad;

	uint oldLeft	= oldPos.x - rad;
	uint oldRight	= oldPos.x + rad;
	uint oldTop		= oldPos.y + rad;
	uint oldBottom	= oldPos.y - rad;

	// vertical part
	uint vxStart 	= ((oldLeft  > newRight) || (oldLeft  < newLeft)) ? oldLeft      : newRight + 1;
	uint vxEnd		= ((oldRight > newRight) || (oldRight < newLeft)) ? oldRight + 1 : newLeft;

	// horizontal part
	uint hyStart 	= ((oldBottom > newTop) || (oldBottom < newBottom)) ? oldBottom    : newTop + 1;
	uint hyEnd 		= ((oldTop    > newTop) || (oldTop    < newBottom)) ? oldTop + 1   : newBottom;
	// x bounds for the horizontal part (so we do not go over the same cells twice)
	uint hxStart 	= (vxStart == oldLeft  && vxEnd   != (oldRight + 1)) ? newLeft      : oldLeft;
	uint hxEnd 		= (vxEnd   == (oldRight + 1) && vxStart != oldLeft)  ? newRight + 1 : oldRight + 1;
	// if the vertical part already covers the whole range, make horizontal part 0 size
	if(vxStart == oldLeft && vxEnd == (oldRight + 1)) {
		hxEnd = hxStart;
	}

	
	typename CRangeGrid2D<T>::List oldRange;

	uint x = 0;
	uint y = 0;
	for(x = vxStart; x < vxEnd; ++x) {
		for(y = oldBottom; y <= oldTop; ++y) {
			if(m_Grid[x][y])
				oldRange.push_back((*m_Grid[x][y]));
		}
	}

	for(x = hxStart; x < hxEnd; ++x) {
		for(y = hyStart; y < hyEnd; ++y) {
			if(m_Grid[x][y])
				oldRange.push_back((*m_Grid[x][y]));
		}
	}

	return oldRange;
}

template <class T>
typename CRangeGrid2D<T>::List CRangeGrid2D<T>::getNewRange(Position oldPos, Position newPos, float radius) {
	uint rad = (uint)ceil(radius/m_CellWidth);
//	nldebug("Finding new objects in the move (%d %d) -> (%d %d) with range %d", oldPos.x, oldPos.y, newPos.x, newPos.y, rad);
	uint newLeft	= newPos.x - rad;
	uint newRight	= newPos.x + rad;
	uint newTop		= newPos.y + rad;
	uint newBottom	= newPos.y - rad;

	uint oldLeft	= oldPos.x - rad;
	uint oldRight	= oldPos.x + rad;
	uint oldTop		= oldPos.y + rad;
	uint oldBottom	= oldPos.y - rad;

	// vertical part
	uint vxStart 	= ((newLeft  > oldRight) || (newLeft  < oldLeft)) ? newLeft      : oldRight + 1;
	uint vxEnd 		= ((newRight > oldRight) || (newRight < oldLeft)) ? newRight + 1 : oldLeft;
	// horizontal part
	uint hyStart	= ((newBottom > oldTop) || (newBottom < oldBottom)) ? newBottom    : oldTop + 1;
	uint hyEnd		= ((newTop    > oldTop) || (newTop    < oldBottom)) ? newTop + 1   : oldBottom;
	// x bounds for the horizontal part (so we do not go over the same cells twice)
	uint hxStart	= (vxStart == newLeft  && vxEnd   != (newRight + 1)) ? oldLeft      : newLeft;
	uint hxEnd		= (vxEnd   == (newRight + 1) && vxStart != newLeft)  ? oldRight + 1 : newRight + 1;
	// if the vertical part already covers the whole range, make horizontal part 0 size
	if(vxStart == newLeft && vxEnd == (newRight + 1)) {
		hxEnd = hxStart;
	}

	typename CRangeGrid2D<T>::List newRange;

	uint x = 0;
	uint y = 0;
	for(x = vxStart; x < vxEnd; ++x) {
		for(y = newBottom; y <= newTop; ++y) {
			if(m_Grid[x][y])
				newRange.push_back((*m_Grid[x][y]));
		}
	}

	for(x = hxStart; x < hxEnd; ++x) {
		for(y = hyStart; y < hyEnd; ++y) {
			if(m_Grid[x][y])
				newRange.push_back((*m_Grid[x][y]));
		}
	}

	return newRange;
}

template <class T>
typename CRangeGrid2D<T>::List CRangeGrid2D<T>::getRange(Position pos, float radius) {
	uint rad = (uint)ceil(radius/m_CellWidth);
	uint left	= pos.x - rad;
	uint right	= pos.x + rad;
	uint top	= pos.y + rad;
	uint bottom	= pos.y - rad;

	typename CRangeGrid2D<T>::List range;

	for(uint x = left; x <= right; ++x) {
		for(uint y = bottom; y <= top; ++y) {
			if(m_Grid[x][y])
				range.push_back((*m_Grid[x][y]));
		}
	}

	return range;
}

template <class T>
void CRangeGrid2D<T>::insert(T data, Position pos) {
	CRangeGrid2D<T>::DataListHolderPtr cell = m_Grid[pos.x][pos.y];
	if(cell == NULL) {
		cell = &m_CellPool.checkout();
		m_Grid[pos.x][pos.y] = cell;
	}

	(*cell)->push_back(data);
}

template <class T>
void CRangeGrid2D<T>::remove(T data, Position pos) {
	CRangeGrid2D<T>::DataListHolderPtr cell = m_Grid[pos.x][pos.y];
	if(cell == NULL)
		return;
	CRangeGrid2D<T>::DataList::iterator iter = (*cell)->begin();
	for( ; iter != (*cell)->end(); ++iter) {
		if((*iter) == data) {
			(*cell)->erase(iter);
			break;
		}
	}

	if((*cell)->size() == 0) {
		m_CellPool.checkin(*cell);
		m_Grid[pos.x][pos.y] = NULL;
	}
}

template <class T>
typename CRangeGrid2D<T>::DataListPtr CRangeGrid2D<T>::getCell(Position pos) {
	return (*m_Grid[pos.x][pos.y]);
}

template <class T>
float CRangeGrid2D<T>::getCellWidth() {
	return m_CellWidth;
}

}; // END OF NAMESPACE WWCOMMON

#endif // __CRANGEGRID2D_H__
