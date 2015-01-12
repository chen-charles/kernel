global  __fastsyscall_entrance
extern  fastsyscall_handler

__fastsyscall_entrance:
        nop
        push    edi

        pushad

        push    eax
        mov     ax, ss
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
        pop     eax

        push    eax
        call    fastsyscall_handler
        add     esp, 4
        
        pop     edi
        mov     edi, eax
        push    edi
        popad
        
        mov     eax, edi    ;return ptr

        pop     edi
        
;SYSEXIT
;The "problem" is that, while SYSENTER disables interrupts, SYSEXIT
;doesn't touch IF.  We have to enable interrupts before going back into
;userspace, so the SYSEXIT path must STI.  If not done carefully, this
;might allow a timer interrupt to preempt the kernel.  However, STI
;waits one instruction before enabling interrupts, so if the exit
;sequence is exactly STI; SYSEXIT we will enable interrupts, but only
;after we've returned to userspace.

        sti
        sysexit
        nop
        nop