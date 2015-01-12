%include	"asm.inc"

global memcpy
global memmove
global memset
global memchr
global memcmp

;void * memcpy ( void * destination, const void * source, size_t num );
memcpy:
	push	ebp
	mov	ebp, esp
	
	push	ecx
	push	esi
	push	edi
	
		mov	edi, [c_param(0)]
		mov	esi, [c_param(1)]
		mov	ecx, [c_param(2)]
		rep	movsb
	
	pop	edi
	pop	esi
	pop	ecx
	
	mov	eax, [c_param(0)]
	pop	ebp
	ret
	
;void * memmove ( void * destination, const void * source, size_t num );
memmove:
	push	ebp
	mov	ebp, esp
	
	push	ecx
	push	esi
	push	edi
	
		mov	edi, [c_param(0)]
		mov	esi, [c_param(1)]
		mov	ecx, [c_param(2)]
		
		sub	edi, esi	;dest - sour
		test	edi, edi
		mov	edi, [c_param(0)]
		js	.1
		
		;source is moving forward
		.2:
			mov	al, [esi+ecx-1]
			mov	[edi+ecx-1], al
			loop	.2
		jmp	.end
		
		;source is moving backward
		.1:
		rep	movsb
	
	.end:
	pop	edi
	pop	esi
	pop	ecx
	
	mov	eax, [c_param(0)]
	pop	ebp
	ret
	
;void * memset ( void * ptr, int value, size_t num );
memset:
	push	ebp
	mov	ebp, esp
	
	push	edi
	push	ecx
	
		mov	edi, [c_param(0)]
		mov	eax, [c_param(1)]
		mov	ecx, [c_param(2)]
		
		.1:
			mov	[edi], al
			inc     edi
			loop	.1
		
	pop	ecx
	pop	edi
	
	mov	eax, [c_param(0)]
	pop	ebp
	ret
	
;void * memchr ( void * ptr, int value, size_t num );
memchr:
	push	ebp
	mov	ebp, esp

	push	ebx
	push	ecx
	
		mov	ebx, [c_param(0)]
		mov	eax, [c_param(1)]
		mov	ecx, [c_param(2)]
		
		.1:
			cmp	[ebx], al
			je	.2
			inc	ebx
			loop	.1
		
		mov	eax, NULLPTR
		jmp	.end
		
		.2:
			mov	eax, ebx
		
	.end:
	pop	ecx
	pop	ebx
	
	pop	ebp
	ret
	
;int memcmp ( const void * ptr1, const void * ptr2, size_t num );
memcmp:
	push	ebp
	mov	ebp, esp

	push	ecx
	push	esi
	push	edi
	
	xor	eax, eax
	mov	ecx, [c_param(2)]
	mov	esi, [c_param(0)]
	mov	edi, [c_param(1)]
	
	.1:
		mov	al, [esi]	;ptr1
		mov	ah, [edi]	;ptr2
		sub	al, ah
		test	al, al
		jnz	.end
		inc	esi
		inc	edi
		loop	.1
		
	.end:
	pop	edi
	pop	esi
	pop	ecx
	
	xor	ah, ah
	pop	ebp
	ret
	
	
