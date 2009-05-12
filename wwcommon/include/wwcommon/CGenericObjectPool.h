/**
 * \file CGenericObjectPool.h
 * \date June 2006
 * \author Matt Raykowski
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

#ifndef __CGENERICOBJECTPOOL_H__
#define __CGENERICOBJECTPOOL_H__

//
// Standard Includes
//
#include <deque>

//
// System Includes
//
#include <cmath>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>

//
// Werewolf Includes
//	

//
// Namespaces
//

namespace WWCOMMON {

template<class T>
class CGenericObjectPool;

template<class T>
class CGenericObjectHolder {
public:
	CGenericObjectHolder() : m_Object(NULL), m_ObjIdx(0) { ; }
	~CGenericObjectHolder() { /* object owned by pool - delete in pool dtor */ }
	CGenericObjectHolder(const CGenericObjectHolder &copy) : m_Object(copy.m_Object), m_ObjIdx(copy.m_ObjIdx) { ; }

	operator T*(void) const { return m_Object; }
	T& operator*(void) const { return *m_Object; }
	T* operator->(void) const {return m_Object; }

	T *getObj() { return m_Object; }

private:
	friend class CGenericObjectPool<T>;

	void setObj(T *obj) { m_Object=obj; }
	uint32 getIdx() { return m_ObjIdx; }
	void setIdx(uint32 idx) { m_ObjIdx=idx;	}

	T		*m_Object;
	uint32	m_ObjIdx;

};

template<class T>
class CGenericObjectPool {
public:
	typedef CGenericObjectHolder<T> Holder;
	enum eGrowthType {
		GROWTH_PERCENT = 0,
		GROWTH_FIXED   = 1
	};

	enum eAllocationType {
		ALLOC_EARLY	= 0,
		ALLOC_LAZY  = 1
	};

	CGenericObjectPool() : m_AllObjects(NULL), m_CurrentSize(0), m_GrowthType(GROWTH_PERCENT), m_AllocationType(ALLOC_EARLY) { ; }
	~CGenericObjectPool();

	void initialize(uint32 initialSize, uint32 minSize, eGrowthType growthType, double growthStep, eAllocationType allocType, uint32 maxSize = 0);
	bool isInitialized() { return m_AllObjects != NULL; }

	Holder& checkout();
	inline void checkin(Holder &obj);

	void growObjectQueue();

protected:
	Holder* createObject(uint32 idx);

	Holder *popObject();

	typedef std::deque<uint32> ObjectQueue;
	typedef Holder** ObjectList;
	ObjectQueue m_ObjectPool;
	ObjectList m_AllObjects;

	uint32 m_MaxPoolSize;
	uint32 m_MinPoolSize;
	uint32 m_CurrentSize;
	eGrowthType m_GrowthType;
	eAllocationType m_AllocationType;
	double m_GrowthStep;

	// TODO henri:everyone a hack...ponder this
	Holder m_InvalidHolder;
};

template<class T>
CGenericObjectPool<T>::~CGenericObjectPool() {
	if(m_AllObjects) {
		for(uint32 i = 0; i < m_CurrentSize; ++i) {
			if(m_AllObjects[i]) {
				if(m_AllObjects[i]->getObj())
					delete m_AllObjects[i]->getObj();
				delete m_AllObjects[i];
			}
		}
		delete[] m_AllObjects;
	}
}

template<class T>
void CGenericObjectPool<T>::initialize(uint32 initialSize, uint32 minSize,
									   typename CGenericObjectPool<T>::eGrowthType growthType,
									   double growthStep,
									   typename CGenericObjectPool<T>::eAllocationType allocType,
									   uint32 maxSize) {
	m_MaxPoolSize = maxSize;
	m_MinPoolSize = minSize;
	m_GrowthType=growthType;
	m_GrowthStep=growthStep;
	m_AllocationType=allocType;

	m_AllObjects = new CGenericObjectPool<T>::Holder*[initialSize];
	m_CurrentSize = initialSize;
	if(m_AllocationType == CGenericObjectPool<T>::ALLOC_LAZY) {
		// initialize to NULL
		memset(m_AllObjects, 0, sizeof(typename CGenericObjectPool<T>::Holder*)*m_CurrentSize);
		// TODO henri:everyone is there a better way to add these to the free list?
		for(uint32 i=0; i<initialSize ; i++) {
			m_ObjectPool.push_back(i);
		}
	} else {
		nlinfo("Begin precaching %d objects.", initialSize);
		for(uint32 i=0; i<initialSize ; i++) {
			// Create an object and insert it into the list.
			m_ObjectPool.push_back(i);
			m_AllObjects[i] = createObject(i);
		}
		nlinfo("Done precaching objects.");
	}
}

template<class T>
typename CGenericObjectPool<T>::Holder& CGenericObjectPool<T>::checkout() {
	if(!isInitialized()) {
		nlerror("The pool has not been initialized!");
		return m_InvalidHolder;
	}

	return *popObject();
}

template<class T>
inline void CGenericObjectPool<T>::checkin(typename CGenericObjectPool<T>::Holder &obj) {
	if(!isInitialized()) {
		nlerror("The pool has not been initialized!");
		return;
	}
	m_ObjectPool.push_front(obj.getIdx());
}

template<class T>
void CGenericObjectPool<T>::growObjectQueue() {
	if(!isInitialized()) {
		nlerror("The pool has not been initialized!");
		return;
	}

	// Preset to the GROWTH_FIXED step.
	uint32 growthNum = (uint32)ceil(m_GrowthStep);

	// Change if we're using percentage of growth instead.
	if(m_GrowthType == CGenericObjectPool<T>::GROWTH_PERCENT)
		// set the size to grow to a percentage of the object pool size.
		growthNum = (uint32)ceil((m_CurrentSize * (m_GrowthStep/100)));

	// Check if we have a maximum pool size set.
	if(m_MaxPoolSize != 0) {
		// if the size of the free pool plus growth is more than the maximum free pool size, clamp.
		if( (growthNum + m_CurrentSize) > m_MaxPoolSize)
			growthNum = m_MaxPoolSize - m_CurrentSize;
	}

	if(growthNum == 0)
		return;

	// Create new list.
	uint32 newObjSize = m_CurrentSize + growthNum;
	nlinfo("Growing pool by %d. New size %d.", growthNum, newObjSize);
	ObjectList newObjList;
	newObjList = new CGenericObjectPool<T>::Holder*[newObjSize];

	// Copy over existing pointers. 
	memcpy(newObjList,m_AllObjects,sizeof(CGenericObjectPool<T>::Holder*)*m_CurrentSize);

	delete[] m_AllObjects;
	m_AllObjects = newObjList;
	
	if(m_AllocationType == CGenericObjectPool<T>::ALLOC_LAZY) {
		// Fill new part with NULL
		memset(&m_AllObjects[m_CurrentSize], 0, sizeof(CGenericObjectPool<T>::Holder*)*growthNum);
		// TODO henri:everyone is there a better way to add these to the free list?
		for(uint32 i=m_CurrentSize ; i<newObjSize ; ++i) {
			m_ObjectPool.push_back(i);
		}
	} else {
		// Create the grown number of objects.
		for(uint32 i=m_CurrentSize ; i<newObjSize ; ++i) {
			m_ObjectPool.push_back(i);
			m_AllObjects[i] = createObject(i);
		}
	}
	m_CurrentSize = newObjSize;
}

template<class T>
typename CGenericObjectPool<T>::Holder* CGenericObjectPool<T>::createObject(uint32 idx) {
	CGenericObjectPool<T>::Holder *oh = new CGenericObjectPool<T>::Holder();
	oh->setObj(new T());
	oh->setIdx(idx);
	return oh;
}

template<class T>
typename CGenericObjectPool<T>::Holder *CGenericObjectPool<T>::popObject() {
	if(m_ObjectPool.empty())
		growObjectQueue();

	if(m_ObjectPool.empty()) {
		nlerror("Growing of object pool failed!");
		return NULL;
	}

	int objIdx = *(m_ObjectPool.begin());
	
	// This should only happen when ALLOC_LAZY is used
	if(m_AllObjects[objIdx] == NULL) {
		nlinfo("Lazy alloc of %d", objIdx);
		m_AllObjects[objIdx] = createObject(objIdx);
	}

	typename CGenericObjectPool<T>::Holder *obj = m_AllObjects[objIdx];
	m_ObjectPool.pop_front();
	return obj;
}


}; // END OF NAMESPACE WWCOMMON

#endif // __CGENERICOBJECTPOOL_H__
