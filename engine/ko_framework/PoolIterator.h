#ifndef __POOLITERATOR_H
#define __POOLITERATOR_H

#include <ComponentPool.h>

template <typename T>
class PoolIterator
{
	PoolIterator(const ComponentPool& source);
	~PoolIterator();

	const ComponentPool& _source;
};


#endif