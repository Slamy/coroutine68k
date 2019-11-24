/*
 * sample_coroutine68.h
 *
 *  Created on: 24.11.2019
 *      Author: andre
 */

#ifndef SRCEXAMPLE_SAMPLE_COROUTINE68_H_
#define SRCEXAMPLE_SAMPLE_COROUTINE68_H_

#include "Coroutine68k.h"

class TestCoroutine : public Coroutine68k
{
  public:
	void func() override;
	void funcInner();

  private:
};

class WaitTest : public Coroutine68k
{
  public:
	volatile bool gotIt = false;
};

class MacroWaitTest : public WaitTest
{
  public:
	void func() override
	{
#ifndef MEASURE_TIME
		printf("Not Yet\n");
#endif
		CO_WAIT_UNTIL(gotIt);
#ifndef MEASURE_TIME
		printf("Finished\n");
#endif
	}
};

class LambdaWaitTest : public WaitTest
{
  public:
	void func() override
	{
#ifndef MEASURE_TIME
		printf("Not Yet\n");
#endif
		coWaitUntil([&] { return gotIt; });
#ifndef MEASURE_TIME
		printf("Finished\n");
#endif
	}
};

void sample_coroutine68k_innerOuter();
void sample_coroutine68k_ioWaiting(WaitTest& wait);

#endif /* SRCEXAMPLE_SAMPLE_COROUTINE68_H_ */
