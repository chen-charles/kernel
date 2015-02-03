
#ifndef __TIME_H__
#define __TIME_H__

#include	"global.h"
#include	"type.h"
#include	"instruction.h"
#include	"string.h"


void delay(int milliseconds);

//return true if got interrupt before timeout
//return false if time out
bool wait_for_interrupt(uint32_t int_id, uint64_t timeout);



#endif
