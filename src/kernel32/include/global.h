#ifndef __GLOBAL__
#define __GLOBAL__

#include "type.h"

//used for cpp object ID
static uint64_t __object_count = 0;

//Session SystemInformation
/*
 * system information provided by multiboot
 * usable after multiboot_startup is called
 * (which is called right after the C-envir setup)
 * 
 */
static uint32_t SystemMemorySize;       //KiloByte(s)

typedef struct _memory
{
    uint64_t    addr;
    uint64_t    len;
}memory;
#define nSystemUnUsableMemoryLayoutEntries 32
static memory SystemUnUsableMemoryLayout[nSystemUnUsableMemoryLayoutEntries];
static int isMemUsable(uint64_t addr, uint64_t len)
{
    int i = 0;
    while (true)
    {
        if (SystemUnUsableMemoryLayout[i].addr == -1 && SystemUnUsableMemoryLayout[i].len == -1) return 1;
        else if (SystemUnUsableMemoryLayout[i].addr < addr+len && 
                SystemUnUsableMemoryLayout[i].addr+SystemUnUsableMemoryLayout[i].len > addr) return 0;
        
        if (++i >= nSystemUnUsableMemoryLayoutEntries) return -1;
    }
}

//vesa info IF SPECIFIED!
//check if ZEROS FIRST!

typedef struct _vesa_info
{
  uint32_t vbe_control_info;
  uint32_t vbe_mode_info;
  uint16_t vbe_mode;
  uint16_t vbe_interface_seg;
  uint16_t vbe_interface_off;
  uint16_t vbe_interface_len;
}vesa_info;
static vesa_info VesaInfo;

//Terminated By 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF
//Spaces Left is Reserved

//End of SystemInformation


#define VALID_INTID(x) (x>0 & x<256) ? 1:0
#define OP_SIZE 4

#define SETBIT(var,pos) ((var) |= (1<<(pos)))
#define CLEARBIT(var,pos) ((var) &= ~(1<<(pos)))
#define FLIPBIT(var,pos) ((var) ^= (1<<(pos)))
#define TESTBIT(var,pos) ((var) & (1<<(pos)))

#define PM_GDT_PTR	0x00000500	//160 entries max
#define PM_IDT_PTR	0x00001000	//256 entries max	//ends at 0x0000c100
#define PM_TSS_PTR	0x0000c100	//104 bytes	//ends at 0x0000c168
										//align to 0x0000c200
static uint32_t    INTR_STACKTOP = 0;

#define PM_IOAPIC_COUNT_PTR 0x0000c1fc	//a uint32_t describes the number of entries in the INFO_PTR
#define PM_IOAPIC_INFO_PTR 0x0000c200	//256 entries(uint32_t) max (256 APICs.......)	//ends at 0x0000c600

#define PM_RING0_STACKBASE  0x0000c600
#define PM_RING0_STACKTOP   0x0000e000

#define PM_INT_REDIRECT_TABLE_PTR 0xe000    //256 entries(uint32_t) ends at 0x0000e400

#define SYS_INTERNAL_TIME_ADDR   0xe400  //uint64_t  (max for 5 million centries if 10 ticks per second) //ends at 0xe408
#define SYS_FSC_ENABLED_ADDR  0xe408    //ends at 0xe409
#define SYS_MEMMAP_SIZE     0xe40c  //a dword for length
#define SYS_MEMMAP_ADDR     0xe410

#define SYS_MALLOC_BUF      0xe418  //ends at 0xe44c
static uint64_t* USR_MALLOC_BUF = (uint64_t*)0xffffffff;

#define SYS_INTERRUPT_SIGNALS_ADDR   0xe500  //uint8_t 256 interrupts //ends at 0xed00
#define GET_INTERRUPT_COUNT(x) (const uint64_t)(*(SYS_INTERRUPT_SIGNALS+x))
#define SYS_INTERNAL_TIME *SYS_INTERNAL_TIME_PTR

#define SYS_LAPIC_ADDR 0xed00  //ends at 0xed08
static uint64_t* SYS_LAPIC_BASE = (uint64_t*)SYS_LAPIC_ADDR;
#define LAPIC_BASE (uint64_t)(*(SYS_LAPIC_BASE))
#define LAPIC_EOI  0xb0


//Processes & Threads
//
#define SYS_PROCESS_COUNT_ADDR 0xed08   //ends at 0xed0c
#define SYS_PROCESS_COUNT *((uint32_t*)SYS_PROCESS_COUNT_ADDR)

//approx.(or less than) 3 (4MB) pages
#define MAX_PROCESSES 236
#define MAX_THREADS 1024
//def calc(nThreads, nProcesses): return (0x1020+0x30*nThreads)*nProcesses/(0x00400000)
#define IS_KERNEL_PROCESS(PID)  PID<10 ? true:false


//physical memory usage
//index for phy page
//22:31 for virt page
//0: if in use
#define SYS_MEM_USAGE_TABLE_ADDR    0xf000  //1024*4 0x1000 ends at 0x10000


#define PCI_INFO_BUFFER  0x00080000     //0x48000
#define PCI_INFO_BUFFER_LENGTH  0x00010000

//PGing
#define PAGING_DATA_ADDR 0x200000   //PHY = VIRT KERNEL-MAPPING
#define PAGING_MMAP_ADDR 0x201000   //MEMORY_MAP GLOBAL PHY
#define PG_SIZE 0x400000

//uint8_t access_r
#define PG_RO_S 153
#define PG_RW_S 155
#define PG_RO_U 157
#define PG_RW_U 159


//MSR
#define SYSENTER_CS_MSR     0x00000174
#define SYSENTER_ESP_MSR    0x00000175
#define SYSENTER_EIP_MSR    0x00000176



//Processor built-in Interrupts Intel 3A 6-20 ~ 6-50
#define INT_VEC_DE	0
#define INT_VEC_DB	1
#define INT_VEC_NMI	2
#define INT_VEC_BP	3
#define INT_VEC_OF	4
#define INT_VEC_BR	5
#define INT_VEC_UD	6
#define INT_VEC_NM	7
#define INT_VEC_DF	8
#define INT_VEC_CSO	9
#define INT_VEC_TS	10
#define INT_VEC_NP	11
#define INT_VEC_SS	12
#define INT_VEC_GP	13
#define INT_VEC_PF	14
#define INT_VEC_15	15
#define INT_VEC_MF	16
#define INT_VEC_AC	17
#define INT_VEC_MC	18
#define INT_VEC_XM	19
#define INT_VEC_VE	20

//User-Defined Interrupts Intel 3A 6-51
#define INTERNEL_INTERRUPT_BASE	128
//APIC Interrupts
//#define INT_VEC_APIC_CMCI	INTERNEL_INTERRUPT_BASE + 32	

//system timer
static uintptr_t APIC_BASE_ADDR = (uintptr_t)0xFEE00000;
#define APIC_TIMER_FREQUENCY    1000    //number of ticks per second

#define INT_VEC_APIC_TIMER	32	//apci-timer tick	IRQ0
#define INT_VEC_APIC_SPUR	39	//spurious	IRQ7
#define INT_VEC_APIC_TM	INTERNEL_INTERRUPT_BASE + 34
#define INT_VEC_APIC_PC	INTERNEL_INTERRUPT_BASE + 35
#define INT_VEC_APIC_LINT0	INTERNEL_INTERRUPT_BASE + 36
#define INT_VEC_APIC_LINT1	INTERNEL_INTERRUPT_BASE + 37
#define INT_VEC_APIC_ERR	INTERNEL_INTERRUPT_BASE + 38

#define INT_VEC_IOAPIC_IRQ0 INTERNEL_INTERRUPT_BASE + 39
#define INT_VEC_IOAPIC_IRQ1 INTERNEL_INTERRUPT_BASE + 40


//global var

// this is declared as static const because the system time should be read-only to ALL procedures except the interrupt handler
// editing the time could cause system failure
// the only way to change this value is access through its memory address
// (*(uint64_t*)SYS_INTERNAL_TIME_ADDR) ++; //increment by one
static const uint64_t* SYS_INTERNAL_TIME_PTR = (uint64_t*)0xe400;
static uint64_t* SYS_INTERRUPT_SIGNALS = (uint64_t*)SYS_INTERRUPT_SIGNALS_ADDR;
static uint64_t PHY_MEM_SIZE;

//Fast System Call
#define FSC_ENABLED (*(uint8_t*)(SYS_FSC_ENABLED_ADDR))
#define FSC_SEND_EOI    0




/*
 * Memory Layout
 * kernel-mode processes runs under PHY memory(phy_mem = virt_mem)
 * other processes runs under VIRT memory(mapped)
 * Min. Sys. Required Memory 512 MB
 * 
 * phy memory
 * 0x00000000~0x00400000    phy_mem = virt_mem  PG_S
 * 0x00400000~0x28400000    400MB System Usage(MUST PRESENT)    //100 pages
 * 0x28400000~MEM_LIMIT     usable pages
 * 0xFB000000~0xFFFFFFFF    USED WITH CAUTION(hardware mapping addr) phy_mem = virt_mem //20 pages
 * 
 * virt memory
 * 0x00000000~0x00400000    phy_mem = virt_mem  PG_S
 * 0x00800000~0xC0000000    PG_U
 * 0xC0000000~0xE8000000    400MB System Usage Mapping Addr
 * 0xE8000000~0xFB000000    248MB PG_U
 * 0xFB000000~0xFFFFFFFF    phy_mem = virt_mem  PG_S
 * 
 */

#define PHY_USABLE_MEMORY_PTR   0x28400000

#define PHY_PROCESS_INFO_TABLE  0x00400000      //ends at 0x01000000
#define VIRT_PROCESS_INFO_TABLE 0xC0000000      //3 (4MB) pages, ends at 0xC0C00000

#define PHY_KERNEL_MALLOC_BUF   0x01000000      //ends at 0x01100000
#define VIRT_KERNEL_MALLOC_BUF  0xC0C00000      //1MB, ends at 0xC0D00000


#endif
