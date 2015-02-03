#ifndef __PROCESS__
#define __PROCESS__

#include    "../type.h"
#include    "../_object/_object.h"
#include    "../mm.h"
#include    "../queue.h"
#include    "../Vector.h"
#include    "../sync.h"

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


//idling: waiting for a system event to activate the process (IO idling ... )
//note: when blocked system calls (such as WaitForSingleObject) are called, process will enter idling state
//running: currently using the time slice
//pending: waiting to use the time slice
enum class RuntimeStatus { IDLING, RUNNING, PENDING };
enum class Privilege { KERNEL, DRIVER, SERVICE, USER };


typedef struct
{
    uint16_t szStack;
    byte contextstack[510];
    byte fxsave[512];
}context;

//	push	dword [c_param(2)]	;ss
//	push	dword [c_param(4)]	;esp
//
//	pushf	;eflags
//	pop	eax
//	or	eax, 0b00000000000000000000001000000000	;enable interrupts after iretd
//	or	eax, 0b0011000000000000	;IOPL = 3
//	push	eax
//	
//	
//	push	dword [c_param(0)]	;cs
//	push	dword [c_param(3)]	;eip

typedef struct
{
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
}ProcessStartupContext;

//ALL METHODS must be exec. in kernel paging && kernel mm !!
//init_mm && set_cr3 should be called to switch into kernel state
class Process: public _object
{
public:
    Process(){};
    Process(page_table* pgtable_p, Privilege pp, uint64_t priority);
    ~Process();
    
    void setStartupContext(ProcessStartupContext* psc)
    {
        memcpy((void*)((&((this->context_p->contextstack[0])))), (void*)psc, sizeof(ProcessStartupContext));
        this->context_p->szStack = sizeof(ProcessStartupContext);
    }
    
    RuntimeStatus rt_status;
    
    uint64_t priority;      //max:0
    uint64_t max_priority;
    
    void contextSave(byte* iret_ptr, greg_t size);
    void contextRestore(byte* iret_ptr);       //mm, x87, pg changed   //should call i-ret right after
    
    context* context_p;
    
    
    page_table* pgtable_p;
    memory_map* mmap_p;
    
    uint64_t tick = 0;
    
    Waitable *waitable_p = 0;
    
    
private:
    uint8_t pg_access_r;
    

protected:
    
};

class Scheduler: public _object
{
public:
    Scheduler()
    {
        vec_p = new Vector<Process>();
    }
    
    Process& CreateProcess(page_table* pgtable_p, Privilege pp, uint64_t priority)
    {
        vec_p->push_back(*(new Process(pgtable_p, pp, priority)));
        return vec_p->back();
    }
    
    void d(int i)
    {
            *((byte* volatile)(0x0b8000+100)) = 0x30+i;
            *((byte* volatile)(0x0b8000+101)) = 0x07;
    }
    bool tick(uintptr_t iret_ptr)
    {
        if (vec_p->size() > 0)
        {
            if (this->running != 0) 
            {
                this->running->contextSave(iret_ptr, sizeof(ProcessStartupContext));
                if (this->running->rt_status == RuntimeStatus::RUNNING)
                    this->running->rt_status = RuntimeStatus::PENDING;
            }
            
            this->running = &(vec_p->at(0));
            for (int i=0; i<vec_p->size(); i++)
            {
                if (vec_p->at(i).rt_status == RuntimeStatus::IDLING)
                {
                    if (vec_p->at(i).waitable_p != 0 && vec_p->at(i).waitable_p->wait())
                    {
                        vec_p->at(i).rt_status = RuntimeStatus::PENDING;
                    }
                }
                
                if (vec_p->at(i).priority < this->running->priority && vec_p->at(i).rt_status == RuntimeStatus::PENDING)
                {
                    this->running = &(vec_p->at(i));
                }
            }
            
            if (this->running->rt_status != RuntimeStatus::PENDING)
            {
                this->running = 0;
                return false;
            }
            else
            {
                this->running->rt_status = RuntimeStatus::RUNNING;
            }
            
            int past_pri = this->running->priority;
            
            for (int i=0; i<vec_p->size(); i++)
            {
                if (past_pri > (vec_p->at(i)).priority)
                {
                    (vec_p->at(i)).priority = 0;
                }
                else
                {
                    (vec_p->at(i)).priority -= past_pri;
                }
            }
            
            this->running->priority = this->running->max_priority;
            this->running->tick ++;
            this->running->contextRestore(iret_ptr);
            return true;
        }
        else
        {
            return false;
        }
    }
    
    ~Scheduler();
    
    Process* running = 0;
private:
    Vector<Process>* vec_p;
    
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
