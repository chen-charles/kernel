#include    "../../include/Process/Process.h"

Process::Process(page_table* pgtable_p, Priviliage pp, uint64_t priority)
{
    mmap_p = calloc(1, sizeof(memory_map));
    
    context_p = &context;
    memset((void*)context_p, 0, sizeof(context));
    
    rt_status = RuntimeStatus::PENDING;
    
    this->pgtable_p = pgtable_p;
    
    init_memory_map(mmap_p, pgtable_p);
    
    if (pp == Priviliage::KERNEL) pg_access_r = PG_RW_S;
    else pg_access_r = PG_RW_U;
    
    this->priority = priority;
}

void Process::contextSave(byte* intr_stackptr, greg_t size)
{
    fxsave(&(context_p->fxsave[0]));
    
    memcpy((void*)&(context_p->contextstack[0]), (void*)intr_stackptr, size);
    
    context_p->szStack = size;
    
    rt_status = RuntimeStatus::SLEEPING;
    
}

void Process::contextRestore(byte* intr_stackptr)
{
    fxrstor(&(context_p->fxsave[0]));
    
    memcpy((void*)intr_stackptr, (void*)&(context_p->contextstack[0]), context_p->szStack);
    
    init_mm(mmap_p, pg_access_r, pgtable_p);
    
    rt_status = RuntimeStatus::RUNNING;
    
    set_cr3(pgtable_p);
    flush_cache();
}


Process::~Process()
{
    free(pgtable_p);
    free(mmap_p);
    free(context_p);
}

