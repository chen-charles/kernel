#ifndef __PROCESS__
#define __PROCESS__

#include    "../type.h"
#include    "../_object/_object.h"
#include    "../mm.h"

extern "C"
{
    void fxsave(uintptr_t destination);
    void fxrstor(uintptr_t source);
    void set_cr3(uintptr_t val);
}

//process running

//interrupt
//save stack
//fxsave
//end of saving context

//fetch next process
//fxrstor
//load stack
//interrupt return
//end of loading context


enum class RuntimeStatus { SLEEPING, RUNNING, PENDING };
enum class Priviliage { KERNEL, DRIVER, USER };
typedef struct
{
    uint16_t szStack;
    byte contextstack[512];
    byte fxsave[512] __attribute__((aligned(16)));
}context;

//ALL METHODS must be exec. in kernel paging && kernel mm !!
//init_mm && set_cr3 should be called to switch into kernel state
class Process: public _object
{
public:
    Process(page_table* pgtable_p, Priviliage pp, uint64_t priority);
    ~Process();
    
    RuntimeStatus rt_status;
    
    uint64_t priority;      //max:0
    
    void contextSave(byte* intr_stackptr, greg_t size);
    void contextRestore(byte* intr_stackptr);       //mm, x87, pg changed   //should call i-ret right after
    
    context* context_p;
    
    
    page_table* pgtable_p;
    memory_map* mmap_p;
private:
    uint8_t pg_access_r;
    context context;

protected:
    
};

//class Thread: public _object
//{
//public:
//    Thread();
//    ~Thread();
//    uint64_t priority;
//    uintptr_t context_p;
//    
//private:
//
//protected:
//    
//};

#endif