
#ifndef __APIC__
#define __APIC__

#include	"global.h"
#include	"type.h"
#include	"instruction.h"
#include	"string.h"

#define APIC_APICID 0x20
#define APIC_APICVER    0x30
#define APIC_TASKPRIOR  0x80
#define APIC_EOI    0xB0
#define APIC_LDR    0xD0
#define APIC_DFR    0xE0
#define APIC_SPURIOUS   0xF0
#define APIC_ESR    0x280
#define APIC_ICRL   0x300
#define APIC_ICRH   0x310
#define APIC_LVT_TMR    0x320
#define APIC_LVT_PERF   0x340
#define APIC_LVT_LINT0  0x350
#define APIC_LVT_LINT1  0x360
#define APIC_LVT_ERR    0x370
#define APIC_TMRINITCNT 0x380
#define APIC_TMRCURRCNT 0x390
#define APIC_TMRDIV 0x3E0
#define APIC_LAST   0x38F
#define APIC_DISABLE    0x10000
#define APIC_SW_ENABLE  0x100
#define APIC_CPUFOCUS   0x200
#define APIC_NMI    (4<<8)
#define TMR_PERIODIC    0x20000
#define TMR_BASEDIV (1<<20)

//FETCH MP Configuration Information
//Intel Multiprocessor Specification Chap. 4 (MP Configuration Table)




void cpuSetAPICBase(uintptr_t apic);
uintptr_t cpuGetAPICBase();

void setbit(uint32_t* uint_p, uint32_t bit);
void clearbit(uint32_t* uint_p, uint32_t bit);
void togglebit(uint32_t* uint_p, uint32_t bit);
bool checkbit(uint32_t* uint_p, uint32_t bit);

void enableAPIC();
void mapAPIC();


//PARSE MADT
typedef struct
{
	char Signature[4];
	uint32_t Length;
	uint8_t Revision;
	uint8_t Checksum;
	char OEMID[6];
	char OEMTableID[8];
	uint32_t OEMRevision;
	uint32_t CreatorID;
	uint32_t CreatorRevision;
	
	uint32_t LC_addr;
	uint32_t flags;

}MADT;

typedef struct
{
	byte PID;
	byte APICID;
	uint32_t Flags;
}ProcessorLAPIC;

typedef struct
{
	byte IOAPICID;
	byte Reserved;
	uint32_t IOAPIC_addr;
	uint32_t GSIB;	//Global System Interrupt Base

}IOAPIC;

typedef struct
{
	byte type;	//const 1
	byte length;	//const 12
	byte id;
	byte reserved;
	uint32_t addr;
	uint32_t gsib;	//Global System Interrupt Base
}ioapic_info;

uint32_t get_ioapics(void* madt_ptr, void* buffer_addr);	//returns counts in num of entries written
void write_ioapic_register(const uintptr_t apic_base, const uint8_t offset, const uint32_t val);
uint32_t read_ioapic_register(const uintptr_t apic_base, const uint8_t offset);


#endif
