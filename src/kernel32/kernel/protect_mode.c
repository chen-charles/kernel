#include	"../include/protect_mode.h"


void init_pm_desc(DESCRIPTOR* p_desc, uint32_t base, uint32_t limit, uint16_t attribute)
{
	p_desc->limit_low		= limit & 0x0FFFF;		// 段界限 1		(2 字节)
	p_desc->base_low		= base & 0x0FFFF;		// 段基址 1		(2 字节)
	p_desc->base_mid		= (base >> 16) & 0x0FF;		// 段基址 2		(1 字节)
	p_desc->attr1			= attribute & 0xFF;		// 属性 1
	p_desc->limit_high_attr2	= ((limit >> 16) & 0x0F) |
						(attribute >> 8) & 0xF0;// 段界限 2 + 属性 2
	p_desc->base_high		= (base >> 24) & 0x0FF;		// 段基址 3		(1 字节)
	
}

uint16_t mk_selector(uint16_t index, uint8_t RPL, bool isLDT)
{
	uint16_t selector=0;
	if (isLDT) selector |= 4;
	selector |= RPL;
	return selector + index;
}

void init_idt_desc(void* ptr, void* handler, byte privilege, uint16_t selector)
{
	GATE *p_gate	= ptr;
	uint32_t base	= (uint32_t)handler;
	p_gate->offset_low	= base & 0xFFFF;
	p_gate->selector	= selector;
	p_gate->dcount	= 0;
	p_gate->attr	= DA_386IGate | (privilege << 5);
	p_gate->offset_high	= (base >> 16) & 0xFFFF;
}

