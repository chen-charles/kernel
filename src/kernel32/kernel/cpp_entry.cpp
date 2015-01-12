#include    "../include/cpp_entry.h"

Process *pr, *kernel;
extern "C" uintptr_t cpp_entry()
{
    page_table* table_p = (page_table*)aligned_malloc(sizeof(page_table), 4*1024);
    memcpy((void*)table_p, (void*)PAGING_DATA_ADDR, sizeof(page_table));
    for (int i=0; i<1024; i++)
    {
        //(bool isPresent, uint32_t virt_loc, uint32_t phy_loc, uint8_t access_r);
        table_p->entry[i] = mm_edit_page(true, i*PG_SIZE, i*PG_SIZE, PG_RW_S);
    }
    
    pr = new Process(table_p, Priviliage::KERNEL, 0);
    
    kernel = new Process(PAGING_DATA_ADDR, Priviliage::KERNEL, 0);
    kernel->mmap_p = &kernel_memmap;
    
    uint8_t tmp;
    kernel->contextSave((void*)&tmp, 1);
    pr->contextSave((void*)&tmp, 1);
    pr->contextRestore((void*)&tmp);
    kernel->contextRestore((void*)&tmp);
    
    return 0;
}


