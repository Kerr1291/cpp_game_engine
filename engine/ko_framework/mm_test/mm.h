#ifndef MM_H
#define MM_H
#include <new>
namespace mm
{

struct mem_pool{
    mem_pool(unsigned size)
    :search_cache(0)
    ,last_free_location(0)
    ,last_free_size(0)
    ,_size(size)
    ,used(0)
    {
        _mem = (unsigned char*)malloc( _size );
        memset( _mem, 0, _size );
        _bitfield = (unsigned char*)malloc( _size );
        memset( _bitfield, 1, _size );
    }

    ~mem_pool(){
        free(_mem);
        free(_bitfield);
    }

    template <typename T>
    T* alloc()
    {
        const unsigned alloc_size = sizeof( T );
        if( alloc_size > (_size - used) ) {
            //std::cout<<"error: cannot allocate object of size "<<alloc_size <<" pool out of memory.\n";
            return 0;
        }

        if( last_free_size > 0 && alloc_size <= last_free_size ){
            search_cache = last_free_location;
            last_free_location = 0;
            last_free_size = 0;
        }

        int block = find_block( alloc_size );

        if( block < 0 ) {
            //std::cout<<"error: cannot allocate object of size "<<alloc_size <<". memory too fragmented\n";
            return 0;
        }

        T* block_ptr = (T*)(&_mem[block]);
        memset( block_ptr, 0, alloc_size );
        memset( &_bitfield[block], 0, alloc_size );
        new (block_ptr) T();
        used += alloc_size;
        return block_ptr;
    }

    template <typename T>
    void free(T* ptr)
    {
        const unsigned free_size = sizeof( T );
        ptr->~T();
        const unsigned location = ((unsigned char*)ptr)-_mem;
        memset( ptr, 0, free_size );
        memset( _bitfield+location, 1, free_size );
        used -= free_size;

        if( last_free_size == 0 || location < last_free_location )
        {
            last_free_location=location;
            last_free_size=free_size;
        }
    }

    void print(){
        for(unsigned i = 0; i < _size;++i)
            std::cout<<"["<<i<<"]"<< (int)(_mem[i])<<" \n";
        std::cout<<"\n";
    }

private:
    unsigned search_cache;
    unsigned last_free_location;
    unsigned last_free_size;
    int find_block(unsigned size){
        const unsigned start = search_cache;
        bool looped = false;
        for(unsigned i = search_cache; i < _size;){
                if( _bitfield[i] == 0 )
                    ++i;
                else
                {
                    if( check_block(i, size) )
                    {
                        search_cache = i+size-1;
                        if( search_cache >= _size )
                            search_cache = 0;
                        return i;
                    }
                    i+=size;
                }

                if( looped )
                {
                    if( i >= start )
                        break;
                }

                if( i >= _size && start != 0 )
                {
                    std::cout<<"looped!\n";
                    looped = true;
                    i = 0;
                }
        }
        return -1;
    }

    unsigned char check_block(unsigned start, unsigned size){
        const unsigned block_end = start+size;
        while( start < _size && start < block_end ){
            if( _bitfield[start++] == 0 ){
                return 0;
            }
        }
        if( start >= _size && start < block_end )
            return 0;
        return 1;
    }

    const unsigned _size;
    unsigned used;
    unsigned char* _mem;
    unsigned char* _bitfield;
};


}//namespace mm
#endif MM_H
