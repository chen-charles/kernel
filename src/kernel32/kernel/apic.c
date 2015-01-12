#include	"../include/apic.h"

#define IA32_APIC_BASE_MSR 0x1B
#define IA32_APIC_BASE_MSR_BSP 0x100 // Processor is a BSP
#define IA32_APIC_BASE_MSR_ENABLE 0x800
#define IA32_APIC_BASE_ADDR 0xfee00000

void enableAPIC()
{
/* Hardware enable the Local APIC if it wasn't enabled */
	uintptr_t p = cpuGetAPICBase();
	cpuSetAPICBase(p);
	
/* Set the Spourious Interrupt Vector Register bit 8 to start receiving interrupts */
	byte* ptr = (byte*)(IA32_APIC_BASE_ADDR + 0xf0);
	(*((uint32_t*)ptr)) = 39 | 0x100;
}

/* Set the physical address for local APIC registers */
void cpuSetAPICBase(uintptr_t apic)
{
	uint32_t edx = 0;
	uint32_t eax = (apic & 0xfffff100) | IA32_APIC_BASE_MSR_ENABLE;
 
#ifdef __PHYSICAL_MEMORY_EXTENSION__
	edx = (apic >> 32) & 0x0f;
#endif

	byte arr[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	memcpy(&arr[0], &eax, sizeof(eax));
	memcpy(&arr[4], &edx, sizeof(edx));
	writemsr(IA32_APIC_BASE_MSR, (byte*)&arr);
}
 
/**
 * Get the physical address of the APIC registers page
 * make sure you map it to virtual memory ;)
 */
uintptr_t cpuGetAPICBase()
{
	uint32_t eax, edx;
	byte arr[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	readmsr(IA32_APIC_BASE_MSR, (byte*)&arr);
	memcpy(&eax, &arr[0], sizeof(eax));
	memcpy(&edx, &arr[4], sizeof(edx));
 
#ifdef __PHYSICAL_MEMORY_EXTENSION__
	return (eax & 0xfffff100) | ((edx & 0x0f) << 32);
#else
	return (eax & 0xfffff100);
#endif
}
 

void mapAPIC(uintptr_t apic_base, uint32_t ticks_per_second)
{
/*
	//CMCI
	uint32_t CMCI = 0;
	setbit(&CMCI, 16);	//not masked
	CMCI += INT_VEC_APIC_CMCI;
	memcpy(0xFEE002F0, &CMCI, sizeof(CMCI));
*/
	//Timer
        //osdev code
    
        //initialize PIT Ch 2 in one-shot mode
	//wait for 1/100 second
        uint32_t tmp, cpubusfreq;
	out_byte(0x61, (in_byte(0x61)&0xFD)|1);
	out_byte(0x43,0xB2);
	//1193180/100 Hz = 11931 = 2e9bh
	out_byte(0x42,0x9B);	//LSB
	in_byte(0x60);	//short delay
	out_byte(0x42,0x2E);	//MSB
 
	//reset PIT one-shot counter (start counting)
	tmp=(uint32_t)(in_byte(0x61)&0xFE);
	out_byte(0x61,(uint8_t)tmp);		//gate low
	out_byte(0x61,(uint8_t)tmp|1);		//gate high
	//reset APIC timer (set counter to -1)
	*(uint32_t*)(apic_base+APIC_TMRINITCNT)=0xFFFFFFFF;
 
	//now wait until PIT counter reaches zero
	while(!(in_byte(0x61)&0x20));
 
	//stop APIC timer
	*(uint32_t*)(apic_base+APIC_LVT_TMR)=APIC_DISABLE;
 
	//now do the math...
	cpubusfreq=((0xFFFFFFFF-*(uint32_t*)(apic_base+APIC_TMRCURRCNT))+1)*16*100;
	tmp=cpubusfreq/ticks_per_second/16;
 
	//sanity check, now tmp holds appropriate number of ticks, use it as APIC timer counter initializer
	*(uint32_t*)(apic_base+APIC_TMRINITCNT)=(tmp<16?16:tmp);
	//finally re-enable timer in periodic mode
	*(uint32_t*)(apic_base+APIC_LVT_TMR)=32|TMR_PERIODIC;
	//setting divide value register again not needed by the manuals
	//although I have found buggy hardware that required it
	*(uint32_t*)(apic_base+APIC_TMRDIV)=0x03;
        
//	uint32_t TIMER = 0;
////	setbit(&TIMER, 16);
//	setbit(&TIMER, 17);	//periodic
//	TIMER |= INT_VEC_APIC_TIMER;
//	memcpy((void*)0xFEE00320, &TIMER, sizeof(TIMER));
//	uint32_t TIMER_DCR = 0;	//Divide Configuration Reg.	//Intel 3A 10-16
//	clearbit(&TIMER_DCR, 0);
//	setbit(&TIMER_DCR, 1);
//	setbit(&TIMER_DCR, 3);
//	memcpy((void*)0xFEE003E0, &TIMER_DCR, sizeof(TIMER_DCR));
//	uint32_t TIMER_IC = 0x0000ffff;	//Initial Count	//Intel 3A 10-17
//	memcpy((void*)0xFEE00380, &TIMER_IC, sizeof(TIMER_IC));

	//Thermal Monitor
	uint32_t TM = 0;
//	setbit(&TM, 16);
	TM += INT_VEC_APIC_TM;
	memcpy((void*)0xFEE00330, &TM, sizeof(TM));

	//Performance Counter
	uint32_t PC = 0;
//	setbit(&PC, 16);
	PC += INT_VEC_APIC_PC;
	memcpy((void*)0xFEE00340, &PC, sizeof(PC));

	//LINT0
	uint32_t LINT0 = 0;
//	setbit(&LINT0, 16);
	setbit(&LINT0, 15);
	LINT0 += INT_VEC_APIC_LINT0;
	memcpy((void*)0xFEE00350, &LINT0, sizeof(LINT0));

	//LINT1
	uint32_t LINT1 = 0;
//	setbit(&LINT1, 16);
	LINT1 += INT_VEC_APIC_LINT1;
	memcpy((void*)0xFEE00360, &LINT1, sizeof(LINT1));

	//Error
	uint32_t ERR = 0;
//	setbit(&ERR, 16);
	ERR += INT_VEC_APIC_ERR;
	memcpy((void*)0xFEE00370, &ERR, sizeof(ERR));

}

void setbit(uint32_t* uint_p, uint32_t bit)
{
	(*uint_p) |= 1 << bit;
}
void clearbit(uint32_t* uint_p, uint32_t bit)
{
	(*uint_p) &= ~(1 << bit);
}
void togglebit(uint32_t* uint_p, uint32_t bit)
{
	(*uint_p) ^= 1 << bit;
}
bool checkbit(uint32_t* uint_p, uint32_t bit)
{
	return (bool)((*uint_p) & (1 << bit));
}


uint32_t get_ioapics(void* madt_ptr, void* buffer_addr)	//returns counts in num of entries written
{
	byte* p = (byte*)(madt_ptr+sizeof(MADT));
	MADT* madt_p = (MADT*) madt_ptr;
	
	int i=0;
	byte* dev_t;
	byte* recordlen;
	ProcessorLAPIC* plapic_p;
	IOAPIC* ioapic_p;
	int b_indx = 0;
	for (i=0; i<madt_p->Length-sizeof(MADT);)
	{
		dev_t = p + i;
		recordlen = dev_t + 1;
		
		if ((int)(*dev_t) == 0)	//	PL APIC
		{
			plapic_p = (ProcessorLAPIC*)(dev_t+2);
			//*((uint32_t*)buffer_addr + b_indx) = plapic_p;
		}
		else
		if ((int)(*dev_t) == 1)	//	IO APIC
		{
			ioapic_p = (IOAPIC*)(dev_t+2);
			*((uint32_t*)buffer_addr + b_indx) = *((uint32_t*)dev_t);
			b_indx ++;
			*((uint32_t*)buffer_addr + b_indx) = ioapic_p->IOAPIC_addr;
			b_indx ++;
			*((uint32_t*)buffer_addr + b_indx) = ioapic_p->GSIB;
			b_indx ++;
		}
		else	//not looking at the others
		{
			;
		}
		i += (int)(*recordlen);
	}
	
	return b_indx/3;
}

void write_ioapic_register(const uintptr_t apic_base, const uint8_t offset, const uint32_t val)
{
	/* tell IOREGSEL where we want to write to */
	*(uint32_t*)(apic_base) = offset;
	/* write the value to IOWIN */
	*(uint32_t*)(apic_base + 0x10) = val;
}

uint32_t read_ioapic_register(const uintptr_t apic_base, const uint8_t offset)
{
	/* tell IOREGSEL where we want to read from */
	*(uint32_t*)(apic_base) = offset;
	/* return the data from IOWIN */
	return *(uint32_t*)(apic_base + 0x10);
}

