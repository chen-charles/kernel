#ifndef __FAST_SYSCALL__
#define __FAST_SYSCALL__

// SysEnter
// Arguments
// ecx: r3 esp
// edx: nxt instruction eip
// eax: fastsyscall_info*
//
// Returns
// eax: fastsyscall_return*
//

#include "type.h"
#include "global.h"
#include "string.h"

typedef struct __fastsyscall_info
{
    uint8_t     Version;
    uint32_t    FunctionID;
    uint32_t    CallerProcessID;
    uint32_t    nArgs;
    //nArgs specifies number of uint32_t arguments
    
}fastsyscall_info;

typedef struct __fastsyscall_return
{
    uint8_t     VersionCheckSum;
    uint32_t    FunctionID;
    uint32_t    CallerProcessID;
    uint32_t    Result; //0 for success
    uint32_t    extraInfo;  //extraInfo
    
}fastsyscall_return;

fastsyscall_return fsc_ret;

//eax-> ptr for info
fastsyscall_return* fastsyscall_handler(fastsyscall_info* info);

int enable_fastsyscall(void* __fastsyscall_entrance, void* __fastsyscall_stacktop);
#endif
