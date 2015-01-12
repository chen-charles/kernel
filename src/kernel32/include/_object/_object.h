#ifndef ___OBJECT__
#define ___OBJECT__

#define __cplusplus 201103L
#include    "../type.h"
#include    "../global.h"
#include    "../string.h"
#include    <typeinfo>
#include    "../libcxxrt/cxxabi.h"

//must be redeclared as extern "C", since cpp makes diff symname
extern "C" void* malloc(size_t size);
extern "C" void* calloc (size_t num, size_t size);
extern "C" void* realloc (void* ptr, size_t size); //not tested yet    //even expands, the data is unchanged, but the ptr might do
extern "C" void free(void* ptr);	//this ptr MUST be the first byte of the data space allocated


void *operator new(size_t size);
void *operator new[](size_t size);

void operator delete(void *p);
void operator delete[](void *p);

class _object
{
public:
    uint64_t getID(){ return this->ID; }
    _object()
    {
        ID = __object_count++;
    }
    
private:
    uint64_t ID;
    
protected:
    
};

#endif
