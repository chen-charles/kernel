%include    "multiboot.inc"
%include    "asm.inc"

;multiboot required fields
;FLAGS       equ  MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE
FLAGS       equ  MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MAGIC       equ  MULTIBOOT_HEADER_MAGIC
CHECKSUM    equ -(MAGIC + FLAGS)

bits 32
align 32

extern  multiboot_startup

extern	gdt_ptr
extern	idt_ptr
extern  gdt_init
extern  c_idt_init
extern  fpu_sse_init
extern  mm_init
extern  c_test

extern  apic_init
extern  pci_init

extern  fastsyscall_init
global  __fastsyscall_stacktop
extern  __fastsyscall_entrance

extern  cpp_entry

[SECTION .data]
clock_int_msg		db	"^", 0
pci_device_info         dd      0, 0, 0, 0  ;16 bytes reserved
multiboot_info_p        dq      0
fpu_test_val: dq 123.45  ;declare quad word (double precision)


[SECTION .bss]
StackSpace		resb	10 * 1024
StackTop:
__fastsyscall_stacktop:		


[section .text]
global _start


_start:
	jmp .realstart


;ensures the multiboot header is in the very front of the program
align 32
align 4
._multiboot:
    ;multiboot required header
	dd MAGIC
	dd FLAGS
	dd CHECKSUM
        dd 0
        dd 0
        dd 0
        dd 0
        dd 0
        ;graphics info
        ;linear(0); width(pixels); height(pixels); depth(bits)
        dd 0
        dd 800
        dd 600
        dd 32
    ;end of multiboot required header
align 32


.raw_boot:
        cli
        hlt

.hang:
        cli
        hlt
        

.realstart:
        cli
        mov esp, StackTop   ;setup stack pointer
        cmp eax, MULTIBOOT_BOOTLOADER_MAGIC     ;is multiboot failed
        jne .raw_boot
        
        push    0
        popf

        ;parse multiboot here
        ;machine-state is ready to be in C mode now
        push    ._multiboot
        push    ebx
        call    multiboot_startup
        add esp, 8

        test    eax, eax
        jnz .raw_boot

;SESSION INIT

	;setup new gdt
	sgdt    [gdt_ptr]
	call	gdt_init
	lgdt	[gdt_ptr]

	;disable 8259A
	mov al, 0xff
	out 0xa1, al
	out 0x21, al
        call    c_idt_init
        lidt	[idt_ptr]


        mov eax, 0x10   ;kernel-code

	push eax
	push .1
	retf	
.1:
        mov ax, 0x18    ;kernel-data
        
        mov ss, ax
        mov es, ax
        mov ds, ax
        mov fs, ax
        mov gs, ax

        call    apic_init

        call    pci_init

        push    __fastsyscall_stacktop
        push    __fastsyscall_entrance
        call    fastsyscall_init
        add esp, 8


        call    fpu_sse_init


;setup mm(malloc) even earlier
;finished
        call    mm_init


;setup cpp
;finished
        ;malloc and fpu are ready
        ;enter cpp
        call    cpp_entry

;cpp_process_interface


;process-interface is needed
;synchr. is needed
;setup acpica

;setup mindrvr

;setup elf dynamic linking

;END INIT

        ;load kernel-plugins

        ;exit kernel-init; jmp to kernel-daemon process;


        
        sti
        jmp $
        jmp $

