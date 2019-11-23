/*
 * Coroutine68k.h
 *
 *  Created on: 22.11.2019
 *      Author: andre
 */

#ifndef INCLUDE_COROUTINE68K_H_
#define INCLUDE_COROUTINE68K_H_

#include <assert.h>
#include <stdint.h>
#include <vector>

#define DEBUG_STACK_CHECK

/**
 * Implementation of stackfull coroutines on the 68k architecture.
 * Pausing and resuming can thereforce be done inside sub functions.
 */
class Coroutine68k
{
  private:
	uint32_t* normalStack	= nullptr; // offset 4. Don't move this variable!
	uint32_t* coroutineStack = nullptr; // offset 8. Don't move this variable!

	bool yielded = false;

	uint16_t stackSize;
	std::vector<uint32_t> stack;

	/// Suspends the coroutine context. Restores saved context.
	void saveCoroutineReturnNormal() asm("saveCoroutineReturnNormal");

	/// Saves context. Resumes the coroutine context.
	void saveNormalRestoreCoroutine() asm("saveNormalRestoreCoroutine");

	/// Restores normal saved context without proper suspending the coroutine
	/// context.
	void discardCoroutineReturnNormal() asm("discardCoroutineReturnNormal");

	/// Internal C++ function which is the artifical caller of func() to catch a
	/// return from func()
	void internalStop();

  public:
	/**
	 * Prepares a coroutine with a seperate stack.
	 * @param stackSize		Size of stack in words
	 */
	Coroutine68k(uint16_t stackSize = 200) : stackSize(stackSize), stack(stackSize)
	{
#ifdef DEBUG_STACK_CHECK
		int i;
		for (i = 0; i < 16; i++)
			stack[i] = 0xDEADBEEF;
#endif
	}

	virtual ~Coroutine68k()
	{
	}

	/**
	 * Resume the Coroutine.
	 * @return	True if Coroutine has finished execution. False if yielded.
	 */
	bool run();

	/**
	 * Set the Coroutine to start position and make it resumable.
	 * Doesn't execute yet.
	 */
	void init();

#ifdef DEBUG_STACK_CHECK
	/**
	 * Check stack frame for overflows.
	 * Only for debugging.
	 */
	void checkStack()
	{
		int i;
		for (i = 0; i < 16; i++)
			assert(stack[i] == 0xDEADBEEF);
	}
#endif

  protected:
	/**
	 * Actual worker function.
	 */
	virtual void func() = 0;

	/**
	 * Can be used from func() or a function called by func() to suspend
	 * execution.
	 */
	void coYield()
	{
#ifdef DEBUG_STACK_CHECK
		checkStack();
#endif
		saveCoroutineReturnNormal();
	}

	/**
	 * Suspend execution as long as cond is true
	 */
#define CO_WAIT_WHILE(cond)                                                                        \
	while (cond)                                                                                   \
	{                                                                                              \
		coYield();                                                                                 \
	}

	/**
	 * Suspend execution as long as cond is false
	 */
#define CO_WAIT_UNTIL(cond)                                                                        \
	while (!cond)                                                                                  \
	{                                                                                              \
		coYield();                                                                                 \
	}
};

#endif /* INCLUDE_COROUTINE68K_H_ */