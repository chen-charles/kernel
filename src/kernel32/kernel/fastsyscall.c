#include	"../include/fastsyscall.h"

int enable_fastsyscall(void* __fastsyscall_entrance, void* __fastsyscall_stacktop)
{
    byte arr[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint16_t selector = mk_selector(0x10, 0, false);
    
    memcpy((void*)(&arr[0]), (void*)(&selector), sizeof(selector));
    writemsr((uint32_t)SYSENTER_CS_MSR, &arr[0]);
    
    memcpy((void*)(&arr[0]), &__fastsyscall_entrance, sizeof(__fastsyscall_entrance));
    writemsr((uint32_t)SYSENTER_EIP_MSR, &arr[0]);
    
    memcpy((void*)(&arr[0]), &__fastsyscall_stacktop, sizeof(__fastsyscall_stacktop));
    writemsr((uint32_t)SYSENTER_ESP_MSR, &arr[0]);
    
    FSC_ENABLED = 1;
    return 0;
}

fastsyscall_return* fastsyscall_handler(fastsyscall_info* info)
{
    memset((void*)&fsc_ret, 0, sizeof(fsc_ret));
    fsc_ret.VersionCheckSum = 0xff-info->Version;
    fsc_ret.FunctionID = info->FunctionID;
    fsc_ret.CallerProcessID = info->CallerProcessID;
    if (info->FunctionID == FSC_SEND_EOI)
    {
        *(uint32_t*)(LAPIC_BASE+LAPIC_EOI) = 0;
        //*(uint32_t*)(0xfee000b0) = 0;
    }
    else if (info->FunctionID == 1) //page fault launched
    {
        //mm_edit_page(0, 0, PG_RW_S);
    }
    
    return &fsc_ret;
}
