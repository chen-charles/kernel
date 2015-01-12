#ifndef __CPP_ENTRY__
#define __CPP_ENTRY__

//IT IS WORTHWILE TO MENTION THAT
//BEFORE MALLOC AND FPU ARE PROPERLY SET UP
//DO NOT ATTEMPT TO CALL ANYTHING IN C++
//VIOLATION'S BEHAVIOUR IS UNDEFINED

#include    "type.h"
#include    "global.h"
#include    "./_object/_object.h"
#include    "./Process/Process.h"


extern "C" uintptr_t cpp_entry();
extern "C" void global_constructor();

#endif
