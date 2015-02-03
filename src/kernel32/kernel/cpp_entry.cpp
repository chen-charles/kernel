#include    "../include/cpp_entry.h"
#include    "../include/Vector.h"

Process *pr, *kernel;


extern "C" bool tick(Scheduler* scheduler, uintptr_t iret_ptr)
{
    return scheduler->tick(iret_ptr);
}

extern "C" Process* CreateProcess(Scheduler* scheduler, page_table* pgtable_p, uint8_t priv, uint64_t priority)
{
    //Process& CreateProcess(page_table* pgtable_p, Privilege pp, uint64_t priority)
    Privilege p;
    
    if (priv == 0) p = Privilege::KERNEL;
    else if (priv == 1) p = Privilege::DRIVER;
    else if (priv == 2) p = Privilege::SERVICE;
    else p = Privilege::USER;
    
    
    return &scheduler->CreateProcess(pgtable_p, p, priority);
}


extern "C" uintptr_t cpp_entry()
{
    init_sync();
    scheduler = new Scheduler();
    
    
    page_table* table_p = (page_table*)aligned_malloc(sizeof(page_table), 4*1024);
    memcpy((void*)table_p, (void*)PAGING_DATA_ADDR, sizeof(page_table));
    for (int i=0; i<1024; i++)
    {
        //(bool isPresent, uint32_t virt_loc, uint32_t phy_loc, uint8_t access_r);
        table_p->entry[i] = mm_edit_page(true, i*PG_SIZE, i*PG_SIZE, PG_RW_S);
    }
    
    ProcessStartupContext psc;
    
    
    pr = &scheduler->CreateProcess(table_p, Privilege::KERNEL, 5);
    psc.ss = 0x18;
    psc.esp= 0x800000;
    psc.eflags = 0x212;
    psc.eip= 0x304f3;
    psc.cs = 0x10;
    pr->setStartupContext(&psc);
    
    kernel = &scheduler->CreateProcess(PAGING_DATA_ADDR, Privilege::KERNEL, 10);
    kernel->mmap_p = &kernel_memmap;
    psc.ss = 0x18;
    psc.esp= 0x800000;
    psc.eflags = 0x212;
    psc.eip= 0x304f1;
    psc.cs = 0x10;
    kernel->setStartupContext(&psc);
    
    Timer* timer = new Timer();
    timer->setTimer(Timer::TimerMode::MOD_ONESHOT, 1000);
    timer->start();
    
    while (!timer->wait()) SYS_INTERNAL_TIME++;
    timer->end();
    
    return 0;
}


extern "C" void __stack_chk_fail(){}
extern "C" int snprintf ( char * s, size_t n, const char * format, ...)
{
    return 0;
}

extern "C" long int strtol (const char* str, char** endptr, int base)
{
    return 0;
}

extern "C" const unsigned short * * __ctype_b_loc (void)
{
    return 0;
}
extern "C" int * __errno_location(void)
{
    return 0;
}
extern "C" char *strdup(const char *s1)
{
    return 0;
}