#include	"../include/pci.h"

uint32_t pci_config_addr(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    return (uint32_t)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));
}

uint32_t pci_config_read(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset)
{
    out_dword(CONFIG_ADDRESS, pci_config_addr(bus, slot, func, offset));
    return (uint32_t)in_dword(0xCFC);
}

PCI_DEVICE_INFO* fetch_pci_dev_common_header(uint8_t bus, uint8_t slot, uint8_t func, PCI_DEVICE_INFO* pci_info_p)
{
    memset((void*)pci_info_p, 0, sizeof(PCI_DEVICE_INFO));
    uint16_t* low_p;
    uint16_t* high_p;
    byte* p;
    uint32_t result;
    int j=0, i=0;
    
    for (i=0; i<4; i++)
    {
        result = pci_config_read(bus, slot, func, j);
        p = (byte*)(&result);
        low_p = (uint16_t*)(p);
        high_p = (uint16_t*)(p+2);
        j += 4;
        switch(i)
        {
        case 0:
            pci_info_p->VendorID = *low_p;
            pci_info_p->DeviceID = *high_p;
            break;
        case 1:
            pci_info_p->Command = *low_p;
            pci_info_p->Status = *high_p;
            break;
        case 2:
            pci_info_p->RevisionID = *((uint8_t*)(p));
            pci_info_p->ClassCode = *((uint8_t*)(p+3));
            break;
        case 3:
            pci_info_p->HeaderType = *((uint8_t*)(p+2));
            break;
        }  
    }
    
    return pci_info_p;
    
}

PCI_DEVICE_INFO buf_pci_scan;    //couldn't be placed inside the function
                        //stack corruption? donno
uint32_t pci_scan(void* pci_info_buffer, uint32_t maxBufferCount)
{
    //memset(&buf, 0, sizeof(PCI_DEVICE_INFO));
    PCI_DEVICE_INFO* pci_info_p = (PCI_DEVICE_INFO*)(pci_info_buffer);
    uint32_t bcount = 0;
    uint32_t i=0, j=0, k=0;
    
    //3 for loops cause unknown corruption
    //pretty sure the compiler generates garbage for 3 for loops
    
    while (true)
    {
        fetch_pci_dev_common_header(i, j, k, &buf_pci_scan);
        if (buf_pci_scan.VendorID != 0xffff)
        {
            bcount += sizeof(PCI_DEVICE_INFO);
            if (bcount > maxBufferCount) return bcount - sizeof(PCI_DEVICE_INFO);
            buf_pci_scan.Bus = (uint8_t)i;
            buf_pci_scan.Slot = (uint8_t)j;
            buf_pci_scan.Func = (uint8_t)k;
            memcpy((void*)(pci_info_p), (void*)(&buf_pci_scan), sizeof(PCI_DEVICE_INFO));
            pci_info_p++;
            k++;
            if (k>=3)
            {
                k=0;
                j++;
            }
        }
        else
        {
            k=0;
            j++;
        }
        
        
        if (j>=32)
        {
            j=0;
            i++;
        }
        if (i>=256)
        {
            break;
        }
    }
    

    return bcount/sizeof(PCI_DEVICE_INFO);
}

uint32_t buf_dev_select[64];    //256 bytes
void* pci_dev_select(PCI_DEVICE_INFO* pci_info_p)   //return pci_dev_header(full))
{
    memset((void*)(&(buf_dev_select[0])), 0, sizeof(PCI_DEVICE_HEADER_00));
    
    //all 256 bytes fields
    for (int i=0; i<64; i++)
    {
        buf_dev_select[i] = pci_config_read(pci_info_p->Bus, pci_info_p->Slot, pci_info_p->Func, i*4);
    }
    
    return (void*)(&buf_dev_select);
}