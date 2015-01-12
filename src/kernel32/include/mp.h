#ifndef __MP__
#define __MP__

//Intel MultiProcessor Specification
//http://www.intel.com/design/pentium/datashts/24201606.pdf

#include "global.h"
#include "type.h"

byte checksum(void* start, int length);


typedef struct
{
    char signature[4];  //_MP_
    uint32_t phy_addr_ptr;  //MP configuration table
    uint8_t length; //0x01
    uint8_t spec_rev;
    uint8_t checksum;
    uint8_t mp_feature_1;
    uint8_t mp_feature_2;
    uint8_t reserved[3];
}MP_FLOATING_PTR_T;

bool CHK_MP_FLOATING_PTR(MP_FLOATING_PTR_T* s_ptr);
MP_FLOATING_PTR_T* get_mp_floating_ptr();

typedef struct
{
    char signature[4];  //PCMP
    uint16_t length;
    uint8_t spec_rev;
    uint8_t checksum;
    uint64_t oemid;
    char productid[12];
    uint32_t oemtable_ptr;
    uint16_t oemtable_size;
    uint16_t entry_count;
    uint32_t lapic_base_addr;
    uint16_t ext_table_length;
    uint8_t ext_table_checksum;
}MP_CONFIGURATION_TABLE_HEADER_T;

uint32_t get_lapic_base_addr(MP_FLOATING_PTR_T* mp_floating_ptr);


#endif
