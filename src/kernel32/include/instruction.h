#ifndef __INSTRUCTION__
#define __INSTRUCTION__

#include "global.h"
#include "type.h"

byte in_byte(uint16_t portid);
byte out_byte(uint16_t portid, byte b);

word in_word(uint16_t portid);
word out_word(uint16_t portid, word w);

dword in_dword(uint16_t portid);
dword out_dword(uint16_t portid, dword d);

void readmsr(uint32_t msr, byte* b_array_8);
void writemsr(uint32_t msr, byte* b_array_8);

void ltr(uint16_t tss_selector);

void __sti__();
void __cli__();

uintptr_t retrieve_ptr(const uintptr_t ptr);
bool is_fpu_present();
bool is_sse_present();

//extern memcpy
void elf_load(uintptr_t raw_elf);

uintptr_t get_cr0();
void set_cr0(uintptr_t val);

void set_cr3(uintptr_t val);

uintptr_t get_cr4();
void set_cr4(uintptr_t val);

void fxsave(uintptr_t destination);
void fxrstor(uintptr_t source);

#endif
