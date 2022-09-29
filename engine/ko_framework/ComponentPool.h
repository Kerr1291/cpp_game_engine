#ifndef __COMPONENT_POOL_H
#define __COMPONENT_POOL_H

#include <vector>

class GameObject;

template <typename T>
class ComponentPool
{
	friend class RenderManager;
	public:
		ComponentPool();
		ComponentPool(unsigned size);
		unsigned MaxSize();
		unsigned UsedCount();
		unsigned DeadCount();
		int AliveCount();
		bool IsFull();
		void MergeOtherPool(ComponentPool& other, bool additive);
		T* MakeComponent(GameObject* obj);
		void KillComponent(unsigned i);
		void KillComponent(T* component);
		void MoveComponentToOtherPool(unsigned i, ComponentPool& other);

	private:
		unsigned _used;
		unsigned _num_dead;
		std::vector<T> _pool;
};


#include <ComponentPool.cpp>
#endif