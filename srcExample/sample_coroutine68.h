/**
 * @file sample_coroutine68.h
 * @date 24.11.2019
 * @author andre
 */

#ifndef SRCEXAMPLE_SAMPLE_COROUTINE68_H_
#define SRCEXAMPLE_SAMPLE_COROUTINE68_H_

#include "Coroutine68k.h"

/**
 * Very simple coroutine which just does some yielding in top level function and one call deeper.
 */
class TestCoroutine : public CoTask
{
  public:
	void func() override;
	void funcInner();

  private:
};

/**
 * abstract class which contains a boolean signal
 */
class WaitTest : public CoTask
{
  public:
	/// Set to true from outside to give a signal to the coroutine.
	volatile bool gotIt = false;
};

/**
 * Coroutine which waits with macros until it got an external signal
 */
class MacroWaitTest : public WaitTest
{
  public:
	void func() override
	{
		debugLogf("Not Yet\n");
		CO_WAIT_UNTIL(gotIt);
		debugLogf("Finished\n");
	}
};

/**
 * Coroutine which waits with lambdas until it got an external signal
 */
class LambdaWaitTest : public WaitTest
{
  public:
	void func() override
	{
		debugLogf("Not Yet\n");
		coWaitUntil([&] { return gotIt; });
		debugLogf("Finished\n");
	}
};

/**
 * A very primitive \ref CoTask which just yields() to measure context switching overhead.
 */
class JumpInOutCoroutine : public CoTask
{
  public:
	/**
	 * just does some nop instructions as these are good to find in cpu trace logs.
	 */
	inline void breakpoint()
	{
		asm volatile("nop");
		asm volatile("nop");
		asm volatile("nop");
	}

	void func() override;

	/// an empty function to measure std::bind call time
	void empty()
	{
	}

  private:
};

void sample_coroutine68k_innerOuter();
void sample_coroutine68k_ioWaiting(WaitTest& wait);
void sample_jumpInOut();

#endif /* SRCEXAMPLE_SAMPLE_COROUTINE68_H_ */
