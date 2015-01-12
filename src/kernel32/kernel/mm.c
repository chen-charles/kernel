 #include	"../include/mm.h"
 

uintptr_t allocPhyPG(uint8_t usage)
{
    return allocPhyLoc(nextFreePhyPG()*PG_SIZE, usage);
}

uintptr_t allocPhyLoc(uintptr_t ptr, uint8_t usage)
{
    int n = ptr/PG_SIZE;
    memory_map* mmap_p = (memory_map*)PAGING_MMAP_ADDR;
    if (isFreePhyPG(n))
    {
        mmap_p->map[n] = MMAP_MAPPED | usage;
        return n*PG_SIZE;
    }
    else
    {
        return (uintptr_t)0;
    }
}

void freePhyLoc(uintptr_t phy_loc)
{
    return freePhyPG(phy_loc);
}

void freePhyPG(uintptr_t phy_loc)
{
    int n = phy_loc/PG_SIZE;
    memory_map* mmap_p = (memory_map*)PAGING_MMAP_ADDR;
    mmap_p->map[n] = MMAP_MAPPED | MMAP_FREE;
}

bool isFreePhyPG(int page_num)
{
    if (page_num >= 1024 || page_num < 0) return false;
    memory_map* mmap_p = (memory_map*)PAGING_MMAP_ADDR;
    int memstat = mmap_p->map[page_num];
    CLEARBIT(memstat, 7);
    if (memstat == MMAP_FREE)
    {
        return true;
    }
    else return false;
}

int nextFreePhyPG()
{
    for (int i=0; i<1024; i++)
    {
        if (isFreePhyPG(i))
        {
            return i;
        }
    }
    
    return 0;
}

uint16_t nextBufferPG(memory_map* mmap_p, uintptr_t ptr)	//since first page will NEVER be bufferPG, thus, a value of ZERO indicates an ERROR
{
	int cur = ptr/PG_SIZE;
	if (cur >= 1024) return 0;
	char sig[8] = {'_', 'B', 'u', 'f', 'f', 'e', 'R', '_'};
	for (; cur < 1024; cur ++)
	{
                int memtype = mmap_p->map[cur];
		if (isMemoryMapped(mmap_p->map[cur]) && CLEARBIT(memtype, 7) == MMAP_BUFFER)
		{
			malloc_page_h* h = (malloc_page_h*)(cur*PG_SIZE);
			if (memcmp((void*)sig, (void*)h, 8) == 0)
			{
				return cur;
			}
		}
	}
	return 0;
}

void init_malloc_page(memory_map* mmap_p, uintptr_t ptr)
{
    int page_num = ptr/PG_SIZE;
    mmap_p->map[page_num] = MMAP_BUFFER | MMAP_MAPPED;
    
    uintptr_t p = (uintptr_t)(PG_SIZE*page_num);
    memset((void*)p, 0, PG_SIZE);
    malloc_page_h* h = (malloc_page_h*)p;
    char sig[8] = {'_', 'B', 'u', 'f', 'f', 'e', 'R', '_'};
    memcpy((void*)p, sig, 8);
    h->nEntries = 1;
    p += sizeof(malloc_page_h);
    
    malloc_entry_h* entry_h = (malloc_entry_h*)p;
    entry_h->size = PG_SIZE-sizeof(malloc_page_h);
}

void init_memory_map(memory_map* mmap_p, page_table* pgtable_p)
{
    memset((void*)mmap_p, MMAP_UNMAPPED, sizeof(memory_map));
    for (int i=0; i<1024; i++)
    {
        if (pgtable_p->entry[i] & 0x1)  //is page present
        {
            mmap_p->map[i] = MMAP_MAPPED;
        }
    }
}

//end of mm process independent functions

//SECTION MM PROCESS DEPENDENT

void* malloc(size_t size)
{
        if (size == 0) return (void*)0;
        int cur = nextBufferPG(mm_cur_mmap_p, 0);
        
        
	//is size too large?
	//be care about the headers
	//maxium data in a page is PG_SIZE - sizeof(malloc_page_h) - sizeof(malloc_entry_h)
	if (size > PG_SIZE - sizeof(malloc_page_h) - sizeof(malloc_entry_h))
	{
		//syscall
		//alloc multiple usable pages(continuous)
		//note that the other pages should not be noted as BUFFER(since the header check)	//this could be a bug for the others to fake a buffer section
		return 0;
	}
	
        
	while (cur != 0)
	{
		int baseptr = cur*PG_SIZE;
		//check page availablity
		uint64_t* nEntries_p = &(((malloc_page_h*)(baseptr))->nEntries);
		uintptr_t p = (uintptr_t)(baseptr + sizeof(malloc_page_h));
		for (int i = 0; i < (*nEntries_p); i++)
		{
			
			malloc_entry_h* entry_p = (malloc_entry_h*)p;
			//try to merge with the next one
			while (i != (*(nEntries_p))-1 && !(entry_p->properties & MALLOC_ENTRY_INUSE) && !(((malloc_entry_h*)(p+(entry_p->size)))->properties & MALLOC_ENTRY_INUSE)) //if i is not the last element and both are free
			{
				//merge
				entry_p->size += ((malloc_entry_h*)(p+(entry_p->size)))->size;
				(*(nEntries_p)) --;
			}
			
			
			
			//pretend the malloc header is 5 bytes long
			//a request of 6 bytes would actually need 11 bytes in memory
			//thus, a spare memory of 10 bytes(+5 header) will not fit in a 6 bytes request
			
			//h h h h h x x x x x x x x x x
			//ONEONEONE DATAONEDATAONEDATAONE
			
			//h h h h h h h h h h x x x x x
			//ONEONEONE TWOTWOTWO DATATWODATA
			
			//in the real case, a 5 bytes request will return a 10(+5) bytes spare that is present
			//since the additional 5 bytes of a spare header would be wasted anyways
			
			//also, a 6 bytes request would return the 10(+5) bytes spare as well
			//otherwise, the memory table structure would be broken
                        
			if (!(entry_p->properties & MALLOC_ENTRY_INUSE) && (entry_p->size-sizeof(malloc_entry_h) >= size))
			{
				//setup the new header
				//check if the amount left over is bigger than the size of the header(even one byte is good in practice)
				if (entry_p->size-sizeof(malloc_entry_h) - size > sizeof(malloc_entry_h))	//where need a new header
				{
					int orisize = entry_p->size;
					entry_p->size = sizeof(malloc_entry_h) + size;
					SETBIT((entry_p->properties), 0);	//in use now
					
					p += entry_p->size;
					((malloc_entry_h*)p)->size = orisize - entry_p->size;	//left over size
					CLEARBIT((((malloc_entry_h*)p)->properties), 0);	//new section not in use
					(*(nEntries_p))++;
                                        
                                        return (void*)(((uintptr_t)entry_p) + sizeof(malloc_entry_h));
				}
				else
				{
					SETBIT((entry_p->properties), 0);	//in use now
					return (void*)(p + sizeof(malloc_entry_h));
				}
			}
			
			p += entry_p->size;
			//assuming p is not exceeding limit
			//if it is, the behavior is undefined.  
                        
		}
		
		
		//failed to find a suitable place
		cur = nextBufferPG(mm_cur_mmap_p, PG_SIZE*(cur+1));
	}
	
	
	//failed to find any suitable page
	//then alloc a new page
	//requested size is checked already(could be suited in one page)
	
	//syscall
	//alloc one usable page

        uintptr_t n_ptr = nextFreeVirtPG()*PG_SIZE;
        if (map_page(n_ptr, MMAP_BUFFER))
        {
            init_malloc_page(mm_cur_mmap_p, n_ptr);
            return malloc(size);
        }
        else
        {
            return -1;
        }
	
}

//the cheapest way ......
//PLZ FIX ME ......
void* aligned_malloc(size_t size, size_t alignment)
{
    
    int sz;
    if (size > alignment)
        sz = size*2;
    
    else
        sz = alignment*2;
    uintptr_t ptr = (uintptr_t)malloc(sz);
    
    if (!ptr) return (void*)0;
    for (uintptr_t i=ptr; i<ptr+size*2; i++)
    {
        if (i%alignment == 0)
        {
            return (void*)i;
        }
    }
    return (void*)0;
}


void free(void* ptr)	//this ptr MUST be the first byte of the data space allocated
{
	uintptr_t baseptr = ((uintptr_t)ptr)/PG_SIZE*PG_SIZE;
	uintptr_t p = (uintptr_t)(baseptr + sizeof(malloc_page_h));
	for (int i = 0; i < ((malloc_page_h*)(baseptr))->nEntries; i++)
	{
		malloc_entry_h* entry_p = (malloc_entry_h*)p;

		if (p+sizeof(malloc_entry_h) == (uintptr_t)ptr)
		{
			CLEARBIT(entry_p->properties, 0);
			return;
		}
		
		p += entry_p->size;
	}
}

void* calloc (size_t num, size_t size)
{
    void* ptr = malloc(num*size);
    if ((uintptr_t)ptr != 0) memset(ptr, 0, num*size);
    return ptr;
}

void* realloc (void* ptr, size_t size)
{
    if ((uintptr_t)ptr == 0) return malloc(size);
    
    
    uintptr_t baseptr = ((uintptr_t)ptr)/PG_SIZE*PG_SIZE;
    uintptr_t p = (uintptr_t)(baseptr + sizeof(malloc_page_h));
    uint64_t* nEntries_p = &(((malloc_page_h*)(baseptr))->nEntries);
    for (int i = 0; i < (*nEntries_p); i++)
    {
            malloc_entry_h* entry_p = (malloc_entry_h*)p;

            if (p+sizeof(malloc_entry_h) == (uintptr_t)ptr)
            {
                
                if (size < ((malloc_entry_h*)p)->size)  //shrink
                {
                    //check if it is possible to shrink
                    if (entry_p->size-sizeof(malloc_entry_h) - size > sizeof(malloc_entry_h))
                    {
                        //yes
                        int orisize = entry_p->size;
                        entry_p->size = sizeof(malloc_entry_h) + size;
                        SETBIT((entry_p->properties), 0);	//in use now

                        ((malloc_entry_h*)(p+entry_p->size))->size = orisize - entry_p->size;	//left over size
                        CLEARBIT((((malloc_entry_h*)(p+entry_p->size))->properties), 0);	//new section not in use
                        (*(nEntries_p))++;
                        return ptr;
                    }
                    else
                    {
                        //no
                        return ptr;
                    }
                }
                else    //expand
                {
                    
                    
                    //try to merge with the next one
                    while (i != (*(nEntries_p))-1 && !(entry_p->properties & MALLOC_ENTRY_INUSE) && !(((malloc_entry_h*)(p+(entry_p->size)))->properties & MALLOC_ENTRY_INUSE)) //if i is not the last element and both are free
                    {
                            //merge
                            entry_p->size += ((malloc_entry_h*)(p+entry_p->size))->size;
                            (*(nEntries_p)) --;
                    }
                    
                    //check if it is possible to expand right there
                    if (entry_p->size > size)  //yes
                        return ptr;   
                    else    //no
                    {
                        void* newp = malloc(size);
                        if ((uintptr_t)newp == 0) return newp;
                        memcpy(newp, ptr, entry_p->size-sizeof(malloc_entry_h));
                        free(ptr);
                        return newp;
                    }
                }
            }

            p += entry_p->size;
    }
    
    return (void*)0;
}

uint32_t mm_edit_page(bool isPresent, uint32_t virt_loc, uint32_t phy_loc, uint8_t access_r)
{
    uint32_t* p = (uint32_t*)mm_paging_data_addr;
    p += virt_loc/PG_SIZE;
    *p = (phy_loc-phy_loc%PG_SIZE)|access_r;
    if (isPresent) SETBIT(*p, 0);
    else CLEARBIT(*p, 0);
    flush_cache();
    return *p;
}

bool map_page(uintptr_t virt_loc, uint8_t usage)
{
    if (usage == MMAP_FREE) return false;
    
    //is in use?
    int page_num = virt_loc/PG_SIZE;
    int memstat = mm_cur_mmap_p->map[page_num];
    CLEARBIT(memstat, 7);
    if (memstat == MMAP_FREE)
    {
        mm_edit_page(true, virt_loc, allocPhyPG(usage), mm_cur_access_r);
        mm_cur_mmap_p->map[page_num] = MMAP_MAPPED | usage;
        return true;
    }
    else
    {
        return false;
    }
}

void free_page(uintptr_t virt_loc)//get phy ptr from virt ptr
{
    int page_num = virt_loc/PG_SIZE;
    uintptr_t phy_loc = ((uintptr_t)(((byte*)mm_paging_data_addr) + page_num*4)) & 0b11111111110000000000000000000000;
    freePhyPG(phy_loc);
    mm_edit_page(false, virt_loc, phy_loc, PG_RW_S); //wait for garbage collection
    mm_cur_mmap_p->map[page_num] = MMAP_UNMAPPED | MMAP_FREE;
}

void change_page_priviliage(uint32_t page_num, uint8_t access_r)
{
	byte *ptr = (byte*)mm_paging_data_addr;
	ptr += page_num*4;
	*ptr = (byte)access_r;
}

int nextFreeVirtPG()
{
    memory_map* mmap_p = mm_cur_mmap_p;
    int memstat;
    for (int i=0; i<1024; i++)
    {
        memstat = mmap_p->map[i];
        CLEARBIT(memstat, 7);
        if (memstat == MMAP_FREE)
        {
            return i;
        }
    }
    
    return 0;
}

//END MM PROCESS DEPENDENT SECTION

void init_mm(memory_map* mmap_p, uint8_t access_r, uintptr_t paging_data_addr)
{
    mm_cur_mmap_p = mmap_p;
    mm_cur_access_r = access_r;
    mm_paging_data_addr = paging_data_addr;
}

