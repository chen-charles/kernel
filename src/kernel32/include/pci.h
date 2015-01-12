
#ifndef __PCI__
#define __PCI__

#include	"global.h"
#include	"type.h"
#include	"instruction.h"
#include	"string.h"

//All PCI devices, except host bus bridges
//are required to provide 256 bytes of configuration registers for this purpose. 
#define     CONFIG_ADDRESS  0xCF8
#define     CONFIG_DATA     0xCFC
#define     CONEJOWIF       0935
#define     PCIHEADERTYPE(x)   (x<<1)>>1

//All PCI compliant devices must support:
//the Vendor ID, Device ID, Command and Status, Revision ID, Class Code and Header Type fields. 
typedef struct _PCI_DEVICE_INFO
{
    uint16_t    VendorID;
    uint16_t    DeviceID;
    uint16_t    Command;
    uint16_t    Status;
    uint8_t     RevisionID;
    uint8_t     ClassCode;
    uint8_t     HeaderType;
    uint8_t     Bus;
    uint8_t     Slot;
    uint8_t     Func;
    uint16_t    Reserved;
}PCI_DEVICE_INFO;

//16 bytes
typedef struct _PCI_DEVICE_COMMON_HEADER
{
    uint16_t    VendorID;
    uint16_t    DeviceID;
    uint16_t    Command;
    uint16_t    Status;
    uint8_t     RevisionID;
    uint8_t     ProgIF;
    uint8_t     SubClass;
    uint8_t     ClassCode;
    uint8_t     CacheLineSize;
    uint8_t     LatencyTimer;
    uint8_t     HeaderType;
    uint8_t     BIST;
}PCI_DEVICE_COMMON_HEADER;


//256 bytes
typedef struct _PCI_DEVICE_HEADER_00 
{
    PCI_DEVICE_COMMON_HEADER common;    //16 bytes  0x10
    uint32_t    BAR0;
    uint32_t    BAR1;
    uint32_t    BAR2;
    uint32_t    BAR3;
    uint32_t    BAR4;
    uint32_t    BAR5;
    uint32_t    CCISPTR;   
    uint16_t    SubSysVendorID;
    uint16_t    SubSysID;
    uint32_t    EROMBASEPTR;
    uint8_t     CAPPTR;
    uint8_t     Reserved_0;
    uint16_t    Reserved_1;
    uint32_t    Reserved_2;
    uint8_t     IntLN;
    uint8_t     IntPIN;
    uint8_t     MinGrant;
    uint8_t     MaxLatncy;
}PCI_DEVICE_HEADER_00;



typedef struct _PCI_DEVICE_HEADER_01
{
    PCI_DEVICE_COMMON_HEADER common;
    uint32_t    BAR0;
    uint32_t    BAR1;
    uint8_t     PrimaryBusNum;
    uint8_t     SecondaryBusNum;
    uint8_t     SubOrdinateBusNum;
    uint8_t     SecondaryLatencyTimer;
    uint8_t     IOBaseL;
    uint8_t     IOLimitL;
    uint16_t    SecondaryStatus;
    uint16_t    MemoryBase;
    uint16_t    MemoryLimit;
    uint16_t    PreMemoryBaseL;
    uint16_t    PreMemoryLimitL;
    uint32_t    PreMemoryBaseH;
    uint32_t    PreMemoryLimitH;
    uint16_t    IOBaseH;
    uint16_t    IOLimitH;
    uint8_t     CAPPTR;
    uint8_t     Reserved_0;
    uint16_t    Reserved_1;
    uint32_t    EROMBASEPTR;
    uint8_t     IntLN;
    uint8_t     IntPIN;
    uint16_t    BridgeControl;
}PCI_DEVICE_HEADER_01;



typedef struct _PCI_DEVICE_HEADER_02 
{
    PCI_DEVICE_COMMON_HEADER common;
    
    uint32_t    CBSECBASEADDR;
    
    uint8_t     CAPoffset;
    uint8_t     Reserved_0;
    uint16_t    SecondaryStatus;
    
    uint8_t     PCIBusNum;
    uint8_t     CBBusNum;
    uint8_t     SubrdinateBusNum;
    uint8_t     CBLatencyTimer;
    
    uint32_t    MemoryBaseAddr0;
    uint32_t    MemoryLimit0;
    uint32_t    MemoryBaseAddr1;
    uint32_t    MemoryLimit1;
    uint32_t    IOBaseAddr0;
    uint32_t    IOLimit0;
    uint32_t    IOBaseAddr1;
    uint32_t    IOLimit1;
    
    uint8_t     IntLN;
    uint8_t     IntPIN;
    uint16_t    BridgeControl;
    
    uint16_t    SubSysDeviceID;
    uint16_t    SubSysVendorID;
    
    uint32_t    PCCBaseAddr;
}PCI_DEVICE_HEADER_02;

uint32_t pci_config_addr(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);
uint32_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset);

//256 buses, 32 devices(slots)/bus
PCI_DEVICE_INFO* fetch_pci_dev_common_header(uint8_t bus, uint8_t slot, uint8_t func, PCI_DEVICE_INFO* pci_info_p);
uint32_t pci_scan(void* pci_info_buffer, uint32_t maxBufferCount);  //return entries
void* pci_dev_select(PCI_DEVICE_INFO* pci_info_p);


/*
 * Here is the layout of the Header Type register:
Bit 7 	Bits 6 to 0
MF 	Header Type

MF - If MF = 1 Then this device has multiple functions.

Header Type - 00h Standard Header - 01h PCI-to-PCI Bridge - 02h CardBus Bridge 
 * 
 * 
 * 
 * Here is the layout of the BIST register:
Bit 7 	Bit 6 	Bits 4 and 5 	Bits 0 to 3
BIST Capable 	Start BIST 	Reserved 	Completion Code

BIST Capable - Will return 1 the device supports BIST.

Start BIST - When set to 1 the BIST is invoked. This bit is reset when BIST completes. If BIST does not complete after 2 seconds the device should be failed by system software.

Completion Code - Will return 0, after BIST execution, if the test completed successfully. 
 */


#endif
