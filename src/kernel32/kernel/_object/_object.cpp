#include    "../../include/_object/_object.h"


//extern "C" void __cxa_pure_virtual()
//{
//    // Do nothing or print an error message.
//}

void *operator new(size_t size)
{
    return calloc(1, size);
}
 
void *operator new[](size_t size)
{
    return calloc(1, size);
}
 
void operator delete(void *p)
{
    free(p);
}
 
void operator delete[](void *p)
{
    free(p);
}
