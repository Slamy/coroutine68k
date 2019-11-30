/**
 * @file sample_coroutine68k.cpp
 * @date 24.11.2019
 * @author andre
 */

#include "config.h"

#include <iomanip>
#include <list>
#include <malloc.h>
#include <numeric>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include <algorithm> //algorithm must be included latest for bebbo toolchain

#include "CiaMeasure.h"
#include "Coroutine68k.h"
#include "config.h"
#include "multiplatform.h"
#include "sample_coroutine68.h"

/// global construction of TestCoroutine to avoid construction having an influence on runtime.
TestCoroutine cotest;

/**
 * A function which is not the top-level of the coroutine but is still able do coYield()
 * because this is a stackful coroutine.
 */
void TestCoroutine::funcInner()
{
	int iteration = 0;
	debugLogf("    - %d\n", iteration++);
	coYield();
	debugLogf("    - %d\n", iteration++);
	coYield();
	debugLogf("    - %d\n", iteration++);
}

void TestCoroutine::func()
{
	int iterationOuter = 0;
	debugLogf("  - %d\n", iterationOuter++);
	coYield();
	debugLogf("  - %d\n", iterationOuter++);
	funcInner();
	debugLogf("  - %d\n", iterationOuter++);
	funcInner();
	debugLogf("  - %d\n", iterationOuter++);
}

void JumpInOutCoroutine::func()
{
	for (;;)
	{
		coYield();
		breakpoint();
		coYield();
		breakpoint();
		coYield();
		breakpoint();
		coYield();
		breakpoint();
		coYield();
		breakpoint();
	}
	coYield();
}

/**
 * executes \ref cotest until finished. used as way to measure performance against
 * \ref sample_stackless_protoThread
 */
void sample_coroutine68k_innerOuter()
{
	cotest.init();

	debugLogf("%s\n", __func__);
	int iteration = 0;
	debugLogf("- %d\n", iteration++);
	while (!cotest())
	{
		debugLogf("- %d\n", iteration++);
	}
	debugLogf("- %d\n", iteration++);
	debugLogf("outside finished!\n");
}

/**
 * Executes given WaitTest coroutine for 3 times and the gives it a signal to react to.
 * @param wait	Coroutine which implements WaitTest
 */
void sample_coroutine68k_ioWaiting(WaitTest& wait)
{
	debugLogf("%s\n", __func__);

	wait.init();
	int iteration;

	for (iteration = 0; iteration < 7; iteration++)
	{
		if (iteration == 3)
			wait.gotIt = true;
		bool ret = wait();
		if (ret)
		{
			debugLogf("T %d\n", iteration);
		}
		else
		{
			debugLogf("F %d\n", iteration);
		}
	}
}

/**
 * executes \ref JumpInOutCoroutine and measures round trip time and therefore context switching
 * overhead.
 */
void sample_jumpInOut()
{
	uint16_t time;

	JumpInOutCoroutine jumpInOut;
	jumpInOut.init();

	jumpInOut();
	jumpInOut();
	jumpInOut();

	time = measureTime(std::bind(&JumpInOutCoroutine::operator(), &jumpInOut));
	printf("ticks inside jumpInOut %d\n", time);
	time = measureTime(std::bind(&JumpInOutCoroutine::operator(), &jumpInOut));
	printf("ticks inside jumpInOut %d\n", time);
	time = measureTime(std::bind(&JumpInOutCoroutine::empty, &jumpInOut));
	printf("ticks inside jumpInOut std::bind empty %d\n", time);
}
