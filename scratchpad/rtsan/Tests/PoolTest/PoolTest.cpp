// PoolTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <wwcommon/CGenericObjectPool.h>
#include <vector>
#include <conio.h>

typedef WWCOMMON::CGenericObjectPool<int> IntPool;
typedef WWCOMMON::CGenericObjectPool<std::vector<void*> > PointerVectorPool;
int _tmain(int argc, _TCHAR* argv[])
{
	IntPool pool;
	pool.initialize(2, 10, IntPool::GROWTH_PERCENT, 50, IntPool::ALLOC_LAZY);

	IntPool::Holder myInt = pool.checkout();
	(*myInt) = 1;
	printf("1. Got int %p %d\n", myInt.getObj(), (*myInt));
	IntPool::Holder myInt2 = pool.checkout();
	printf("2. Got int %p %d\n", myInt2.getObj(), (*myInt2));
	pool.checkin(myInt);
	IntPool::Holder myInt3 = pool.checkout();
	printf("3. Got int %p %d (should be the same as 1)\n", myInt3.getObj(), (*myInt3));
	IntPool::Holder myInt4 = pool.checkout();
	printf("4. Got int %p %d\n", myInt4.getObj(), (*myInt4));
	pool.checkin(myInt2);
	IntPool::Holder myInt5 = pool.checkout();
	printf("5. Got int %p %d (should be the same as 2)\n", myInt5.getObj(), (*myInt5));

	printf("std::vector<ptr> size:           %d\n", sizeof(std::vector<void*>));
	printf("Holder of std::vector<ptr> size: %d\n", sizeof(PointerVectorPool::Holder));

	getch();

	return 0;
}

