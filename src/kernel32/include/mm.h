#ifndef	__MEMORY_MANAGEMENT__
#define	__MEMORY_MANAGEMENT__

#include	"type.h"
#include	"global.h"
#include	"string.h"

EXTERN_C

//Structures

typedef struct
{
	uint8_t map[1024];
	
	//bit 7: bool isMapped
	#define	MMAP_UNMAPPED	0b00000000
	#define	MMAP_MAPPED	0b10000000
	#define	isMemoryMapped(val) TESTBIT(val, 7)
	
	//bit 0~6: memory page type val: 0~127
        #define MMAP_FREE   0
        #define MMAP_S  1
        #define MMAP_DATA   2
	#define	MMAP_BUFFER	127

}
memory_map;	//1024 pages -> 4096 MBs -> 4MB page

typedef struct
{
    uint32_t    entry[1024];
}
page_table;	//PG

typedef struct
{
	char    signature[8];	//_BuffeR_
	uint64_t    nEntries;
	//malloc entries
}
malloc_page_h;

//at least ONE entry MUST present
typedef struct
{
	size_t  size;	//including entry size
	
	uint32_t    properties;	//bit 0: isInUse?
	#define	MALLOC_ENTRY_FREE	0b00000000
	#define	MALLOC_ENTRY_INUSE	0b00000001
	
	//data followed by this
}
malloc_entry_h;




//Process Independent Functions
int nextFreePhyPG();
int nextFreeVirtPG();
bool isFreePhyPG(int page_num);
uintptr_t allocPhyPG(uint8_t usage); //phy_loc return
void freePhyPG(uintptr_t phy_loc);
uintptr_t allocPhyLoc(uintptr_t ptr, uint8_t usage);
void freePhyLoc(uintptr_t phy_loc);
uint16_t nextBufferPG(memory_map* mmap_p, uintptr_t ptr);	//since first page will NEVER be bufferPG, thus, a value of ZERO indicates an ERROR
void init_malloc_page(memory_map* mmap_p, uintptr_t ptr);
void init_memory_map(memory_map* mmap_p, page_table* pgtable_p);
void flush_cache();
void init_paging(uint32_t s_flatdata32, uintptr_t paging_data_addr);    //4MB page	//ALL PG_S	//global::PAGING_DATA_ADDR



//Process Dependent Functions

//init_mm should be called to setup process-dep data
//the behaviour of all process-dep functions is dep on these data
void init_mm(memory_map* mmap_p, uint8_t access_r, uintptr_t paging_data_addr);


//SECTION MM PROCESS DEPENDENT DATA
static memory_map kernel_memmap;

//VIRTUAL!!!
static memory_map* mm_cur_mmap_p; //VIRTUAL PAGING MMAP
static uint8_t mm_cur_access_r;
static uintptr_t mm_paging_data_addr;

//END SECTION

//this function maps phy_ptr to virt_ptr
//it checks if virt_ptr is inuse, it does NOT check the phy_ptr
//it edits the page table and marks the memory page to its usage
//if the virtual page is not available, the function fails with a false
//if usage is MMAP_FREE, the function fails with a false
//otherwise, it returns true
bool map_page(uintptr_t virt_loc, uint8_t usage);
void free_page(uintptr_t virt_loc);

#ifndef	PG_SIZE
#define	PG_SIZE 0x400000
#endif

void* malloc(size_t size);
void* aligned_malloc(size_t size, size_t alignment);

void* calloc (size_t num, size_t size);
void* realloc (void* ptr, size_t size); //not tested yet    //even expands, the data is unchanged, but the ptr might do
void free(void* ptr);	//this ptr can be any byte of the ptr allocated

uint32_t mm_edit_page(bool isPresent, uint32_t virt_loc, uint32_t phy_loc, uint8_t access_r);
void change_page_priviliage(uint32_t page_num, uint8_t access_r);

EXTERN_C_END

#endif
