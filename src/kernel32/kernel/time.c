#include	"../include/time.h"

void delay(int milliseconds)
{
    uint64_t start = SYS_INTERNAL_TIME;
    while (SYS_INTERNAL_TIME >= start + milliseconds);
    return;
}

bool wait_for_interrupt(uint32_t int_id, uint64_t timeout)
{
    if (!VALID_INTID(int_id)) return false;
    const uint64_t start = GET_INTERRUPT_COUNT(int_id);
    int count = 0;
    while (count < timeout)
    {
        if (start != GET_INTERRUPT_COUNT(int_id)) return true;
        delay(1);
        count ++;
    }
    return false;
}

