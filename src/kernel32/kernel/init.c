#include	"../include/multiboot.h"
#include	"../include/string.h"
#include	"../include/protect_mode.h"
#include	"../include/interrupt.h"
#include        "../include/instruction.h"
#include        "../include/mp.h"
#include        "../include/pci.h"
#include        "../include/fastsyscall.h"
#include        "../include/mm.h"

int gdt_init();
int c_idt_init();
int apic_init();
int pci_init();
int fastsyscall_init(void* __fastsyscall_entrance, void* __fastsyscall_stacktop);
int fpu_sse_init();
int mm_init();
void set_tss(uint32_t esp);


int gdt_init()
{
	uint16_t *gdtlen = (uint16_t*)&gdt_ptr[0];
	uint32_t *gdtaddr =  (uint32_t*)&gdt_ptr[2];
	*gdtaddr = PM_GDT_PTR;
	*gdtlen = sizeof(DESCRIPTOR)*16;
	int p=0;

	// Entry 0 -> empty
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0, 0);

	// Entry 1 -> 32-bit Ring 0 Code E/R, Accessed, conforming, shared resources	-> kernel code segment
        // user-mode can access this segment with RPL 3
        // kernel-mode could not access user-mode code/data
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0xffffffff, DA_32 | DA_LIMIT_4K | DA_DPL0 | DA_CCOR);

	// Entry 2 -> 32-bit Ring 0 Code E/R, Accessed, non-conforming, critical session	-> kernel code segment
	// access in the same ring ONLY, restrict ALL access across rings
        init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0xffffffff, DA_32 | DA_LIMIT_4K | DA_DPL0 | DA_CR);

	// Entry 3 -> 32-bit Ring 0 Data R/W, Accessed	-> kernel data segment
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0xffffffff, DA_32 | DA_LIMIT_4K | DA_DPL0 | DA_DRWA);
	
	// Entry 4 -> 32-bit Ring 3 Code E/R			-> user code segment
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0xffffffff, DA_32 | DA_LIMIT_4K | DA_DPL3 | DA_CCOR);

	// Entry 5 -> 32-bit Ring 3 Data R/W, Accessed	-> user data segment
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0xffffffff, DA_32 | DA_LIMIT_4K | DA_DPL3 | DA_DRWA);

	// Entry 6 -> 32-bit Ring 1 Code E/R			-> kernel-service code segment
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0xffffffff, DA_32 | DA_LIMIT_4K | DA_DPL1 | DA_CCOR);

	// Entry 7 -> 32-bit Ring 1 Data R/W, Accessed	-> kernel-service data segment
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0xffffffff, DA_32 | DA_LIMIT_4K | DA_DPL1 | DA_DRWA);

	// Entry 8 -> 32-bit Ring 2 Code E/R			-> user-service code segment
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0xffffffff, DA_32 | DA_LIMIT_4K | DA_DPL2 | DA_CCOR);

	// Entry 9 -> 32-bit Ring 2 Data R/W, Accessed	-> user-service data segment
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0xffffffff, DA_32 | DA_LIMIT_4K | DA_DPL2 | DA_DRWA);

	// Entry 10 -> TSS
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, PM_TSS_PTR, PM_TSS_PTR + sizeof(TSS)-1, DA_386TSS);

	// Entry 11 ~ 15 -> empty
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0, 0);
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0, 0);
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0, 0);
	init_pm_desc((DESCRIPTOR*)*gdtaddr + p++, 0, 0, 0);

	return p;
}

int c_idt_init()
{
    //if an interrupt is launched and no predetermined routines is there to handle,
    //the C-handler will try to index the interrupt redirect table(global.h)
    //if the function pointer is not NULLPTR, the function will be called
    //in __cdecl calling convention with param (void* esp, uint32_t int_id)
    //
    //the redirect table is init-ed here
    //internal / critical interrupt routines are pre-determined
    //they will be redirected from the C-handler before indexing the table
    //just be aware of that
    memset((void*)int_masks[0], 0, 256);
    memset((void*)PM_INT_REDIRECT_TABLE_PTR, 0, OP_SIZE*256);
    idt_init();
    
    return 0;
}

void set_tss(uint32_t esp)
{
        KERNEL_INT_ESP = esp;
	TSS *ptr = (TSS*)PM_TSS_PTR;
	memset(ptr, 0, sizeof(TSS));
        INTR_STACKTOP = esp;
	ptr->ss0 = 0x18;
	ptr->esp0 = esp;
}

int apic_init()
{
//SESSION LAPIC
    *((uint32_t*)SYS_LAPIC_BASE) = get_lapic_base_addr(get_mp_floating_ptr());
    
    //this block is used to clear ALL old PCI interrupts invoked
    //this block ensures the correctness so absolutely NO previous interrupts will get in the way
    //IRQ0~IRQ15
    //MUST AFTER IDT SETUP
    for (int i=0; i<16; i++)
        mask_interrupt(i);
    __sti__();
    for (int i=0; i<100000; i++);
    __cli__();
    for (int i=0; i<16; i++)
        unmask_interrupt(i);
    
    
    enableAPIC();

    //setup APIC, interrupts and timer
    //base_addr, # of ticks per second
    //system timer percision
    mapAPIC(APIC_BASE_ADDR, APIC_TIMER_FREQUENCY);
    
    
//END LAPIC
    
//SESSION IOAPIC
    write_ioapic_register(0xFEC00000, 0x12, INT_VEC_IOAPIC_IRQ1);
    write_ioapic_register(0xFEC00000, 0x13, 0);
    
//END IOAPIC
    return 0;
}

int pci_init()
{
    //PCI Prepare Devices
    int t = (int)pci_scan(PCI_INFO_BUFFER, PCI_INFO_BUFFER_LENGTH);
    PCI_DEVICE_INFO* dev_info_p;
    for (int i=0; i<t; i++)
    {
        dev_info_p = (PCI_DEVICE_INFO*)(PCI_INFO_BUFFER) + i;
        if (PCIHEADERTYPE(dev_info_p->HeaderType) == 0x0)
        {
            if (dev_info_p->ClassCode == 0x1)
            {
                
            }
        }

    }
    return 0;
}

int fastsyscall_init(void* __fastsyscall_entrance, void* __fastsyscall_stacktop)
{
    return enable_fastsyscall(__fastsyscall_entrance, __fastsyscall_stacktop);
}

int fpu_sse_init()
{
    int cr0 = get_cr0();
    int cr4 = get_cr4();
    if (is_fpu_present())
    {
        cr0 = CLEARBIT(cr0, 2);
        cr0 = SETBIT(cr0, 5);
        cr0 = SETBIT(cr0, 1);
        set_cr0(cr0);
        if (is_sse_present())
        {
            cr4 = SETBIT(cr4, 9);
            cr4 = SETBIT(cr4, 10);
            set_cr4(cr4);
        }
    }
    else
    {
        cr0 = SETBIT(cr0, 2);
        cr0 = CLEARBIT(cr0, 5);
        set_cr0(CLEARBIT(cr0, 1));
    }
    
    return 0;
}


int c_test()
{
//    memset((void*)&memmap, MMAP_MAPPED, sizeof(memmap));
//    init_malloc_page(&memmap, (uintptr_t)PG_SIZE*1);
//    
//    
//    free(malloc(50));
//    free(malloc(PG_SIZE - sizeof(malloc_page_h) - sizeof(malloc_entry_h)));
    return 0;
}

int mm_init()
{
    init_paging(0x18, (uintptr_t)PAGING_DATA_ADDR);   //kernel-data
    init_memory_map((memory_map*)PAGING_MMAP_ADDR, (page_table*)PAGING_DATA_ADDR);
    init_memory_map(&kernel_memmap, (page_table*)PAGING_DATA_ADDR);
    
    init_mm(&kernel_memmap, PG_RW_S, PAGING_DATA_ADDR);
    map_page(0, MMAP_S); //this will alloc phy 0 -> 0   //since 1st usable phy is 0 //virt = 0
    
    return 0;
}
