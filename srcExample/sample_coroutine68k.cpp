/*
 * sample_coroutine68k.cpp
 *
 *  Created on: 24.11.2019
 *      Author: andre
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

TestCoroutine cotest;

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
