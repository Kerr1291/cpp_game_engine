//#include <ComponentPool.h>

#include <GameObject.h>

template <typename T>
ComponentPool<T>::ComponentPool()
{
    _used = 0;
    _num_dead = 0;
    _pool.reserve(100);
}

template <typename T>
ComponentPool<T>::ComponentPool(unsigned size)
{
    _used = 0;
    _num_dead = 0;
    _pool.reserve(size);
}

template <typename T>
unsigned ComponentPool<T>::MaxSize()
{
	return _pool.capacity();
}

template <typename T>
unsigned ComponentPool<T>::UsedCount()
{
	return _used;
}

template <typename T>
unsigned ComponentPool<T>::DeadCount()
{
	return _num_dead;
}

template <typename T>
int ComponentPool<T>::AliveCount()
{
	return _used - _num_dead;
}

template <typename T>
bool ComponentPool<T>::IsFull()
{
    if( UsedCount() < MaxSize() )
      return false;
    return true;
}

template <typename T>
void ComponentPool<T>::MergeOtherPool(ComponentPool& other, bool additive)
{
  std::vector<T> merged_pool;
  unsigned merged_size = _pool.capacity();
  if( additive )
  {
    merged_size += other.MaxSize();
  }
  else
  {
    if( (AliveCount() + other.AliveCount()) >= _pool.capacity() )
      return;
  }
  
  //copy everything into another array so we can rebuild our array here
  std::vector<T> pool_copy = _pool;
  _pool.clear();
  
  //necessary so our push_backs don't invalidate the pointers we will assign
  _pool.reserve(merged_size);
  
  
  for(unsigned i = 0; i < Size(); ++i)
  {
    if( !pool_copy[i]._parent )
      continue;
    
    _pool.push_back( pool_copy[i] );
    (_pool.back())._parent = pool_copy[i]._parent;
	(_pool.back())._owner = this;
    (_pool.back())._parent->SetEngineComponent( &(_pool.back()) );
  }

  for(unsigned i = 0; i < other.Size(); ++i)
  {
    if( !other._pool[i]._parent )
      continue;
    
    _pool.push_back( other._pool[i] );
    (_pool.back())._parent = other._pool[i]._parent;
	(_pool.back())._owner = this;
    (_pool.back())._parent->SetEngineComponent( &(_pool.back()) );
  }
  other._pool.clear();  
  other._used = 0;
  other._num_dead = 0;
  
  _used = _pool.size();
  _num_dead = 0;
}

template <typename T>
T* ComponentPool<T>::MakeComponent(GameObject* obj)
{
  if( UsedCount() >= MaxSize() )
    return 0;
  
  if(!obj)
    return 0;
  
  _used++;
  _pool.push_back( T( obj->_transform ) );
  (_pool.back())._parent = obj;
  (_pool.back())._owner = this;
  obj->SetEngineComponent( &(_pool.back()) );
  return &(_pool.back());
}

template <typename T>
void ComponentPool<T>::KillComponent(T* component)
{
  if( !component )
    return;
  
  _num_dead++;

  component->KillThisComponent();
}

template <typename T>
void ComponentPool<T>::MoveComponentToOtherPool(unsigned i, ComponentPool& other)
{
  if( i >= _used )
    return;
  
  if( !_pool[i]._parent )
    return;
  
  _num_dead++;
  
  other.MakeComponent( _pool[i]._parent );
  _pool[i]._parent = 0;
}