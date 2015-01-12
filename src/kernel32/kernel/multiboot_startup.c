#include    "../include/multiboot.h"
#include    "../include/string.h"
#include    "../include/instruction.h"
#include    "../include/protect_mode.h"
#include    "../include/global.h"
#include    "../include/vesa3.h"

uint32_t multiboot_startup(uint32_t multiboot_info, multiboot_header_t* header);

//int multiboot_startup()
//Returns 0 if the multiboot startup was successful
//
//input:
//32-bit physical address of the Multiboot information structure is needed
//which is provided in EBX
//
//parses multiboot startup information into global constants
//This Function is called right after C-code envir is prepared
uint32_t multiboot_startup(uint32_t multiboot_info, multiboot_header_t* header)
{
    multiboot_info_t* info_p = (multiboot_info_t*)multiboot_info;
    
    //check if any args need to be taken care first
    if (info_p->flags & MULTIBOOT_INFO_CMDLINE)
    {

    }
    
    
    //parse strickly REQUIRED fields
    if (info_p->flags & MULTIBOOT_INFO_MEMORY)
    {
        SystemMemorySize = info_p->mem_lower + info_p->mem_upper;
    }
    else return 1;
    
    if (info_p->flags & MULTIBOOT_INFO_MEM_MAP)
    {
        multiboot_memory_map_t* mmap_p = (multiboot_memory_map_t*)info_p->mmap_addr;
        int j = 0;
        for (int i = 0; i < info_p->mmap_length; i += sizeof(multiboot_memory_map_t))
        {
            if (mmap_p->type != MULTIBOOT_MEMORY_AVAILABLE)
            {
                
                SystemUnUsableMemoryLayout[j].addr = mmap_p->addr;
                SystemUnUsableMemoryLayout[j].len = mmap_p->len;
                if (++j >= nSystemUnUsableMemoryLayoutEntries) return 2;
            }
            mmap_p++;
        }
        SystemUnUsableMemoryLayout[j].addr = -1;
        SystemUnUsableMemoryLayout[j].len = -1;
    }
    else return 2;
    
    //parse OPTIONAL fields
    if (info_p->flags & MULTIBOOT_INFO_MODS)
    {
        multiboot_module_t* mod_p = (multiboot_module_t*)info_p->mods_addr;
        for (int i=0; i<info_p->mods_count; i++)
        {
            //TODO:
            
            mod_p++;
        }
    }
    
    if ((info_p->flags & MULTIBOOT_INFO_VIDEO_INFO) && (header->flags & MULTIBOOT_VIDEO_MODE))
    {
        vesa3_mode_infoblock* modeinfo_p = (vesa3_mode_infoblock*)info_p->vbe_mode_info;
        for (int i = modeinfo_p->phys_base_ptr; i < modeinfo_p->phys_base_ptr+800*600*4; i++)
        {
            *((uint8_t*)i) = 111;
        }
        for (int i = modeinfo_p->phys_base_ptr; i < modeinfo_p->phys_base_ptr+800*600*4; i++)
        {
            *((uint8_t*)i) = 0;
        }
        
        //save vesa info
        memcpy((void*)(&(VesaInfo)), (void*)&(info_p->vbe_control_info), sizeof(vesa_info));
        
    }
    
    return 0;
}
