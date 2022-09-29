#ifndef __POOL_MANAGER_H
#define __POOL_MANAGER_H

#include <list>
#include <ComponentPool.h>

template <typename T>
class PoolManager
{
	friend class RenderManager;

	static const unsigned k_initial_pool_size = 100;

	public:

		PoolManager()
			:_default_pool_size(k_initial_pool_size)
		{}

		void ClearPools()
		{
			_default_pool_size = k_initial_pool_size;
			_pools.clear();
		}

		//call this periodically to see if we should clean our memory pools
		void CheckMemoryLayout()
		{
			//rules that define a pool that should be cleaned
			const float k_min_used_percent_for_cleanup = 0.5f;
			const float k_min_garbage_percent_for_cleanup = 0.3f;

			std::list< ComponentPool<T>* > clean_pools;

			//check the pools to see if they have too much garbage
			std::list< ComponentPool<T> >::iterator iter = _pools.begin();
			for( ; iter != _pools.end(); ++iter )
			{
				float max_size = iter->MaxSize();
				float used = iter->UsedCount();
				float used_percent = used / max_size;
				if( used_percent > k_min_used_percent_for_cleanup )
				{
					float dead = iter->DeadCount();
					float garbage_percent = dead / used;
					if( garbage_percent > k_min_garbage_percent_for_cleanup )
					{
						clean_pools.push_back( &(*iter) );
					}
				}
			}

			if( clean_pools.size() > 0 )
			{
				CleanMemoryPools( clean_pools );
			}
		}

		void CleanAllPools()
		{
			std::list< ComponentPool<T>* > clean_pools;
			std::list< ComponentPool<T> >::iterator iter = _pools.begin();
			for( ; iter != _pools.end(); ++iter )
			{
				unsigned dead = iter->DeadCount();
				if( dead > 0 )
					clean_pools.push_back( &(*iter) );
			}
			if( clean_pools.size() > 0 )
			{
				CleanMemoryPools( clean_pools );
			}
		}

		//remove all the garbage from the pools in clean_pools
		void CleanMemoryPools(std::list< ComponentPool<T>* >& clean_pools)
		{
			ComponentPool<T>* previous_pool = 0;

			//remove all the garbage from every pool we passed in
			while( !clean_pools.empty() )
			{
				ComponentPool<T>* current_pool = clean_pools.back();

				//as a small optimization, see if the next pool will fit in our
				//previous pool and just use that if possible
				if( previous_pool )
				{
					int free_space = previous_pool->MaxSize() - previous_pool->UsedCount();
					if( current_pool->AliveCount() <= free_space )
					{
						previous_pool->MergeOtherPool( *current_pool, false );
						clean_pools.pop_back();
						continue;
					}
				}
				
				//pick the bigger of the pool sizes to use for the new pool
				const unsigned k_new_pool_size = 
					( _default_pool_size > current_pool->MaxSize() ? 
					  _default_pool_size : current_pool->MaxSize() );

				_pools.push_back( ComponentPool<T>(k_new_pool_size) );

				ComponentPool<T>* new_pool = _pools.back();
				new_pool->MergeOtherPool( *current_pool, false );
				previous_pool = new_pool;
				clean_pools.pop_back();
			}

			//remove all empty pools from our manager
			std::list< ComponentPool<T> >::iterator iter = _pools.begin();
			for( ; iter != _pools.end(); ++iter )
			{
				if( iter->UsedCount() <= 0 )
				{
					iter = _pools.erase( iter );
					if( iter == _pools.end() )
						break; // need to do this in case we just erased the last item
				}
			}
		}

		//note that if this is too small MakeComponent will automatically start
		//making it bigger
		void SetNewDefaultPoolSize(unsigned size)
		{
			_default_pool_size = size;
		}

		T* MakeComponent(GameObject* owner)
		{
			//search the pools for space to make a component
			std::list< ComponentPool<T> >::iterator iter = _pools.begin();
			for( ; iter != _pools.end(); ++iter )
			{
				if( !iter->IsFull() )
				{
					T* component = iter->MakeComponent( owner );
					if( component )
						return component;
				}
			}

			//if we get to here, we're out of space, need to make a new pool
			if( _pools.size() > 2 )
			{
				//...but first, check to see if we have been making "too many" pools
				const float k_bad_pool_count_to_size_ratio = 0.2f;
				const float k_current_default_pool_size = static_cast<float>(_default_pool_size);
				const float k_current_pool_count = static_cast<float>(_pools.size());
				const float k_ratio = k_current_pool_count / k_current_default_pool_size;

				//if the ratio is bad, double our default pool size for new pools
				//old pools will be updated when they fill with garbage
				//and are cleaned
				if( k_ratio > k_bad_pool_count_to_size_ratio )
				{
					_default_pool_size *= 2;
				}
			}

			//so now, we're going to make a new pool...
			//and put the new component in that
			_pools.push_back( ComponentPool<T>(_default_pool_size) );
			return _pools.back().MakeComponent( owner );
		}

	private:

		unsigned _default_pool_size;
		std::list< ComponentPool<T> > _pools;

};



#endif