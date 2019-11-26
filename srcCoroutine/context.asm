;
; context.asm
;
;  Created on: 22.11.2019
;      Author: andre
;

	section .text

	xdef saveCoroutineReturnNormal
	xdef saveNormalRestoreCoroutine
	xdef discardCoroutineReturnNormal


; Structure of an instance of the class Coroutine68k
	rsreset
Co68k_vtable			rs.l	1	; 0: pointer to vtable. internal c++ stuff
Co68k_normalStack		rs.l	1	; 4: uint32_t* Coroutine68k::normalStack
Co68k_coroutineStack	rs.l	1	; 8: uint32_t* Coroutine68k::coroutineStack

; GCC stack frame for 68k
	rsreset
Stack_cpp_retAdr		rs.l	1
Stack_cpp_this			rs.l	1


; void Coroutine68k::saveCoroutineReturnNormal()
; void saveCoroutineReturnNormal(Coroutine68k* this)
saveCoroutineReturnNormal:
	move.l		Stack_cpp_this(sp),a0	; Get "this" pointer
	movem.l 	d2-d7/a2-a6,-(sp)		; Save Coroutine context

	; The stack at this point:
	; Return Address to coroutine
	; Registers of coroutine

	move.l		sp,Co68k_coroutineStack(a0)				; Save to coroutine stack pointer to this->coroutineStack
	move.l		Co68k_normalStack(a0),sp				; Restore normal Stack from this->normalStack

	; The stack at this point:
	; Return Address to normal world
	; Registers of normal world

	movem.l		(sp)+,d2-d7/a2-a6		; Restore normal context

	rts									; Return to normal function

; void Coroutine68k::saveNormalRestoreCoroutine()
; void saveNormalRestoreCoroutine(Coroutine68k* this)
saveNormalRestoreCoroutine:
	move.l		Stack_cpp_this(sp),a0			; Get "this" pointer
	movem.l 	d2-d7/a2-a6,-(sp)				; Save normal context

	move.l		sp,Co68k_normalStack(a0)		; Save normal stack pointer to this->normalStack
	move.l		Co68k_coroutineStack(a0),sp		; Restore Coroutine Stack from this->coroutineStack

	movem.l		(sp)+,d2-d7/a2-a6				; Restore Coroutine Context

	rts											; Return to Coroutine

; void Coroutine68k::discardCoroutineReturnNormal()
; void discardCoroutineReturnNormal(Coroutine68k* this)
discardCoroutineReturnNormal:
	move.l		Stack_cpp_this(sp),a0		; Get "this" pointer
	move.l		Co68k_normalStack(a0),sp	; Restore normal Stack from this->normalStack
	movem.l		(sp)+,d2-d7/a2-a6			; Restore normal context

	rts										; Return to normal function


