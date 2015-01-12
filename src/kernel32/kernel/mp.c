#include    "../include/mp.h"

byte checksum(void* start, int length)
{
    byte sum = 0;
    for (int i=0; i<length; i++)
    {
        sum += *(((byte*)start)+i);
    }
    return sum;
}

bool CHK_MP_FLOATING_PTR(MP_FLOATING_PTR_T* s_ptr)
{
    return checksum((void*)s_ptr, sizeof(MP_FLOATING_PTR_T)) == 0;
}

MP_FLOATING_PTR_T* get_mp_floating_ptr()
{
    MP_FLOATING_PTR_T* s_ptr;
    char sig[4] = {'_', 'M', 'P', '_'};
    
    //index EBDA
    s_ptr = (MP_FLOATING_PTR_T*)0x040e;
    
    for (int i=0; i<1024/sizeof(MP_FLOATING_PTR_T); i++)
    {
        if (memcmp((s_ptr+i), sig, 4) == 0 && (s_ptr+i)->length == 0x01 && CHK_MP_FLOATING_PTR((s_ptr+i)))
        {
            return (s_ptr+i);
        }
    }
    
    //first 2 bytes
    s_ptr = (MP_FLOATING_PTR_T*)0;
    for (int i=0; i<2048/sizeof(MP_FLOATING_PTR_T); i++)
    {
        if (memcmp((s_ptr+i), sig, 4) == 0 && (s_ptr+i)->length == 0x01 && CHK_MP_FLOATING_PTR((s_ptr+i)))
        {
            return (s_ptr+i);
        }
    }
    
    //BIOS
    s_ptr = (MP_FLOATING_PTR_T*)0x0f0000;
    for (int i=0; i<0x10000/sizeof(MP_FLOATING_PTR_T); i++)
    {
        if (memcmp((s_ptr+i), sig, 4) == 0 && (s_ptr+i)->length == 0x01 && CHK_MP_FLOATING_PTR((s_ptr+i)))
        {
            return (s_ptr+i);
        }
    }
    return 0;
}

uint32_t get_lapic_base_addr(MP_FLOATING_PTR_T* mp_floating_ptr)
{
    MP_CONFIGURATION_TABLE_HEADER_T* p = (MP_CONFIGURATION_TABLE_HEADER_T*)(mp_floating_ptr->phy_addr_ptr);
    return p->lapic_base_addr;   
}
