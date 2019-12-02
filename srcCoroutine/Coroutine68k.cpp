/**
 * @file Coroutine68k.cpp
 * @date 22.11.2019
 * @author andre
 */

#include "Coroutine68k.h"

#include <assert.h>
#include <stdio.h>

void Coroutine68k::init()
{
	/*
	 * The coroutine has no context yet. Let's make one that looks exactly
	 * like we are inside at the start of func() which was called by
	 * Coroutine68k::internalStop.
	 */

	/*
	 * This is a very dirty way to extract the internal representation of C++ function
	 * pointers.
	 * FIXME Is there a better way?
	 * This is not possible with GCC:
	 * *coroutineStack = reinterpret_cast<uint32_t>(&Coroutine68k::internalStop);
	 */
	union {
		void (Coroutine68k::*fn)(void);

		// Lowest bit of val[0] marks if this is a virtual call?
		// If bit 0 is set it's an offset on the vtable -1 in bytes
		// If bit 0 is not set it's an absolute pointer and no vtable call
		uint32_t val[2];
	} temp;

	/*
	 * set stack to top and align it for 32 bit words.
	 * it's ok that the pointer is overflowing the buffer as
	 * the 68k first decrements and then writes
	 */
	coroutineStack = (uint32_t*)((uint32_t)&stack[stackSize] & ~3);

	// add "this" pointer as parameter for Coroutine68k::internalStop()
	coroutineStack--;
	*coroutineStack = reinterpret_cast<uint32_t>(this);

	// add fake return address for Coroutine68k::internalStop() which doesn't really have a caller.
	// that's the reason why internalStop() is not allowed to return
	coroutineStack--;
	*coroutineStack = 0;

	/*
	 * add function pointer to Coroutine68k::internalStop() as return address on stack for
	 * saveNormalRestoreCoroutine() to use to call internalStop() by returning.
	 */
	coroutineStack--;
	temp.fn			= &Coroutine68k::internalStop;
	*coroutineStack = temp.val[0];

	// d2-d7/a2-a6 are saved as context. d0,d1,a0,a1,a7 are missing as they are
	// temporary for GCC So 16 - 5 = 11 registers need space.
	coroutineStack -= 11;

	// mark the context as established
	contextValid = true;
}
