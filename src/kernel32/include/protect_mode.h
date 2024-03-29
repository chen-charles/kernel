#ifndef __PROTECT_MODE__
#define __PROTECT_MODE__

#include "type.h"
#include "global.h"
#include "string.h"

#define DA_32		0x4000	//32 位段
#define DA_LIMIT_4K	0x8000	//段界限粒度为 4K 字节

#define DA_DPL0	0x00
#define DA_DPL1	0x20
#define DA_DPL2	0x40
#define DA_DPL3	0x60

#define DA_DR	0x90	//存在的只读数据段类型值
#define DA_DRW	0x92	//存在的可读写数据段属性值
#define DA_DRWA	0x93	//存在的已访问可读写数据段类型值
#define DA_C		0x98	//存在的只执行代码段属性值
#define DA_CR	0x9A	//存在的可执行可读代码段属性值
#define DA_CCO	0x9C	//存在的只执行一致代码段属性值
#define DA_CCOR	0x9E	//存在的可执行可读一致代码段属性值

#define DA_LDT		0x82	//局部描述符表段类型值
#define DA_TaskGate	0x85	//任务门类型值
#define DA_386TSS	0x89	//可用 386 任务状态段类型值
#define DA_386CGate	0x8C	//386 调用门类型值
#define DA_386IGate	0x8E	//386 中断门类型值
#define DA_386TGate	0x8F	//386 陷阱门类型值

//Paging	DWORD
#define PAGING_DATA_ADDR	0x200000	//2mb
#define PAGING_BASE_ADDR	0x201000	//1024*4 = 1000h
#define PG_RO_S	153
#define PG_RW_S	155
#define PG_RO_U	157
#define PG_RW_U	159


/* 存储段描述符/系统段描述符 */
typedef struct s_descriptor		/* 共 8 个字节 */
{
	uint16_t	limit_low;		/* Limit */
	uint16_t	base_low;		/* Base */
	uint8_t	base_mid;		/* Base */
	uint8_t	attr1;			/* P(1) DPL(2) DT(1) TYPE(4) */
	uint8_t	limit_high_attr2;	/* G(1) D(1) 0(1) AVL(1) LimitHigh(4) */
	uint8_t	base_high;		/* Base */
}DESCRIPTOR;

/* 门描述符 */
typedef struct s_gate
{
	uint16_t	offset_low;	/* Offset Low */
	uint16_t	selector;	/* Selector */
	uint8_t	dcount;		/* 该字段只在调用门描述符中有效。
				如果在利用调用门调用子程序时引起特权级的转换和堆栈的改变，需要将外层堆栈中的参数复制到内层堆栈。
				该双字计数字段就是用于说明这种情况发生时，要复制的双字参数的数量。 */
	uint8_t	attr;		/* P(1) DPL(2) DT(1) TYPE(4) */
	uint16_t	offset_high;	/* Offset High */
}GATE;


typedef struct s_tss {
	uint32_t	backlink;
	uint32_t	esp0;		/* stack pointer to use during interrupt */
	uint32_t	ss0;		/*   "   segment  "  "    "        "     */
	uint32_t	esp1;
	uint32_t	ss1;
	uint32_t	esp2;
	uint32_t	ss2;
	uint32_t	cr3;
	uint32_t	eip;
	uint32_t	flags;
	uint32_t	eax;
	uint32_t	ecx;
	uint32_t	edx;
	uint32_t	ebx;
	uint32_t	esp;
	uint32_t	ebp;
	uint32_t	esi;
	uint32_t	edi;
	uint32_t	es;
	uint32_t	cs;
	uint32_t	ss;
	uint32_t	ds;
	uint32_t	fs;
	uint32_t	gs;
	uint32_t	ldt;
	uint16_t	trap;
	uint16_t	iobase;	/* I/O位图基址大于或等于TSS段界限，就表示没有I/O许可位图 */
	/*uint8_t	iomap[2];*/
}TSS;

void init_pm_desc(DESCRIPTOR* p_desc, uint32_t base, uint32_t limit, uint16_t attribute);
uint16_t mk_selector(uint16_t index, uint8_t RPL, bool isLDT);

void init_idt_desc(void* ptr, void* handler, byte privilege, uint16_t selector);

//these data needs to be accessed in assembly, thus cannot be declared as static
byte gdt_ptr[6];
byte idt_ptr[6];

#endif
