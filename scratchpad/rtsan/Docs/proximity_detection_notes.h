///////////////////////////
// Pooling
///////////////////////////
/*
Requirements:
	* Generic - template based.
	* Predefined "sensible" size
	* Ability to increase size as more objects are requested.
		* Ability to specify increment. Default 1. (maybe as a fraction - difficult to specify 1 tho)
	* Thread safe (locking).
Possible addons:
	* Object expiration (not returned to pool - timer and reference count?)
	* Specify type - fixed size, temp data, dynamic size (required).
	* Lock timeout (how long do people wait when fixed size pool and all objects are in use).
*/

template <class T>
class CPool {
public:
	T* checkout();
	void checkin(T *pObj);

	...
};


///////////////////////////
// Grid
///////////////////////////

template <class T>
class CRangeGrid2D {
private:
	typedef std::vector<T>			DataList;

public:
	typedef DataList*	 		DataListPtr;
	typedef std::vector<DataListPtr>	List;
	typedef uint[2]				Position;

	CRangeGrid2D(float sideLength, uint numSideCells);
	~CRangeGrid2D();
	
	void 					insert(T data, Position pos);
	void 					remove(T data, Position pos);
	DataList				getCell(Position pos);
	List					getRange(Position pos, float radius);
	List					getOldRange(Position oldPos, Position newPos, float radius);
	List					getNewRange(Position oldPos, Position newPos, float radius);

private:
	typedef DataListPtr**			Grid2D;

	Grid2D					m_Grid;
	CPool<DataList>				m_DataPool;

	float 					m_SideLength;
	uint					m_NumSideCells;
	float					m_CellWidth;
};


template <class T>
CRangeGrid2D<T>::CRangeGrid2D(float sideLength, uint numSideCells) 
	: m_SideLength(sideLength), m_NumSideCells(numSideCells), m_CellWidth(sideLength/numSideCells) {
	m_Grid = new DataListPtr*[m_NumSideCells];
	for (uint i = 0; i < m_NumSideCells; ++i) {
	        m_Grid[i] = new DataListPtr[m_NumSideCells];
	}

	// TODO henri:henri fill with NULL
}

template <class T>
CRangeGrid2D<T>::~CRangeGrid2D() {
	// TODO henri:henri if pool does not delete data, then delete here.
	// IMO when pool is destroyed it should delete all of the data, in use or otherwise.
	
	for (uint i = 0; i < m_NumSideCells; ++i) {
	        delete[] m_Grid[i];
	}

	delete[] m_Grid;
}

template <class T>
CRangeGrid2D<T>::List CRangeGrid2D<T>::getOldRange(Position oldPos, Position newPos, float radius) {
	uint rad = ceil(radius/m_CellWidth);
	uint newLeft	= newPos[0] - rad;
	uint newRight	= newPos[0] + rad;
	uint newTop	= newPos[1] + rad;
	uint newBottom	= newPos[1] - rad;

	uint oldLeft	= oldPos[0] - rad;
	uint oldRight	= oldPos[0] + rad;
	uint oldTop	= oldPos[1] + rad;
	uint oldBottom	= oldPos[1] - rad;

	// vertical part
	uint vxStart 	= ((oldLeft  > newRight) || (oldLeft  < newLeft)) ? oldLeft      : newRight + 1;
	uint vxEnd 	= ((oldRight > newRight) || (oldRight < newLeft)) ? oldRight + 1 : newLeft;

	// horizontal part
	uint hyStart 	= ((oldBottom > newTop) || (oldBottom < newBottom)) ? oldBottom    : newTop + 1;
	uint hyEnd 	= ((oldTop    > newTop) || (oldTop    < newBottom)) ? oldTop + 1   : newBottom;
	// x bounds for the horizontal part (so we do not go over the same cells twice)
	uint hxStart 	= (vxStart == oldLeft  && vxEnd   != (oldRight + 1)) ? newLeft      : oldLeft;
	uint hxEnd 	= (vxEnd   == (oldRight + 1) && vxStart != oldLeft)  ? newRight + 1 : oldRight + 1;
	// if the vertical part already covers the whole range, make horizontal part 0 size
	if(vxStart == oldLeft && vxEnd == (oldRight + 1)) {
		hxEnd = hxStart;
	}

	
	CRangeGrid2D<T>::List oldRange;

	uint x = 0;
	uint y = 0;
	for(x = vxStart; x < vxEnd; ++x) {
		for(y = oldBottom; y <= oldTop; ++y) {
			oldRange.push_back(m_Grid[x][y]);
		}
	}

	for(x = hxStart; x < hxEnd; ++x) {
		for(y = hyStart; y < hyEnd; ++y) {
			oldRange.push_back(m_Grid[x][y]);
		}
	}

	return oldRange;
}

template <class T>
CRangeGrid2D<T>::List CRangeGrid2D<T>::getNewRange(Position oldPos, Position newPos, float radius) {
	uint rad = ceil(radius/m_CellWidth);
	uint newLeft	= newPos[0] - rad;
	uint newRight	= newPos[0] + rad;
	uint newTop	= newPos[1] + rad;
	uint newBottom	= newPos[1] - rad;

	uint oldLeft	= oldPos[0] - rad;
	uint oldRight	= oldPos[0] + rad;
	uint oldTop	= oldPos[1] + rad;
	uint oldBottom	= oldPos[1] - rad;

	// vertical part
	uint vxStart 	= ((newLeft  > oldRight) || (newLeft  < oldLeft)) ? newLeft      : oldRight + 1;
	uint vxEnd 	= ((newRight > oldRight) || (newRight < oldLeft)) ? newRight + 1 : oldLeft;
	// horizontal part
	uint hyStart	= ((newBottom > oldTop) || (newBottom < oldBottom)) ? newBottom    : oldTop + 1;
	uint hyEnd	= ((newTop    > oldTop) || (newTop    < oldBottom)) ? newTop + 1   : oldBottom;
	// x bounds for the horizontal part (so we do not go over the same cells twice)
	uint hxStart	= (vxStart == newLeft  && vxEnd   != (newRight + 1)) ? oldLeft      : newLeft;
	uint hxEnd	= (vxEnd   == (newRight + 1) && vxStart != newLeft)  ? oldRight + 1 : newRight + 1;
	// if the vertical part already covers the whole range, make horizontal part 0 size
	if(vxStart == newLeft && vxEnd = (newRight + 1)) {
		hxEnd = hxStart;
	}

	CRangeGrid2D<T>::List newRange;

	uint x = 0;
	uint y = 0;
	for(x = vxStart; x < vxEnd; ++x) {
		for(y = newBottom; y <= newTop; ++y) {
			newRange.push_back(m_Grid[x][y]);
		}
	}

	for(x = hxStart; x < hxEnd; ++x) {
		for(y = hyStart; y < hyEnd; ++y) {
			newRange.push_back(m_Grid[x][y]);
		}
	}

	return newRange;
}

template <class T>
CRangeGrid2D<T>::List CRangeGrid2D<T>::getRange(Position pos, float radius) {
	uint rad = ceil(radius/m_CellWidth);
	uint left	= pos[0] - rad;
	uint right	= pos[0] + rad;
	uint top	= pos[1] + rad;
	uint bottom	= pos[1] - rad;

	CRangeGrid2D<T>::List range;

	for(uint x = left; x <= right; ++x) {
		for(uint y = bottom; y <= top; ++y) {
			range.push_back(m_Grid[x][y]);
		}
	}

	return range;
}

template <class T>
void CRangeGrid2D<T>::insert(T data, Position pos) {
	CRangeGrid2D<T>::DataListPtr cell = m_Grid[pos[0]][pos[1]];
	if(cell == NULL) {
		cell = m_DataPool.checkout();
		m_Grid[pos[0]][pos[1]] = cell;
	}

	cell->push_back(data);
}

template <class T>
void CRangeGrid2D<T>::remove(T data, Position pos) {
	CRangeGrid2D<T>::DataListPtr cell = m_Grid[pos[0]][pos[1]];
	if(cell == NULL)
		return;
	CRangeGrid2D<T>::DataList::iterator iter = cell->begin();
	for( ; iter != cell->end(); ++iter) {
		if((*iter) == data) {
			cell->erase(iter);
			break;
		}
	}

	if(cell->size() == 0) {
		m_DataPool.checkin(cell);
		m_Grid[pos[0]][pos[1]] = NULL;
	}
}

template <class T>
CRangeGrid2D<T>::DataList CRangeGrid2D<T>::getCell(Position pos) {
	return m_Grid[pos[0]][pos[1]];
}

