/**
 * @file sample_stackless.cpp
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
#include "sample_stackless.h"

/// global construction to avoid construction having an influence on runtime.
Inner in;

/// global construction to avoid construction having an influence on runtime.
Outer co(in);

/**
 * Yields quite some time and does only some test printing
 * @return True as long as as coroutine hasn't ended
 */
bool Inner::Run()
{
	PT_BEGIN();

	iteration = 0;
	debugLogf("    - %d\n", iteration++);
	PT_YIELD();
	debugLogf("    - %d\n", iteration++);
	PT_YIELD();
	debugLogf("    - %d\n", iteration++);

	PT_END();
}

/**
 * Yields quite some time and spawns and executes an inner coroutine for tests.
 * @return True as long as as coroutine hasn't ended
 */
bool Outer::Run()
{
	PT_BEGIN();

	iterationOuter = 0;
	debugLogf("  - %d\n", iterationOuter++);
	PT_YIELD();
	debugLogf("  - %d\n", iterationOuter++);
	PT_SPAWN(in);
	debugLogf("  - %d\n", iterationOuter++);
	PT_SPAWN(in);
	debugLogf("  - %d\n", iterationOuter++);

	PT_END();
}

/**
 * Executes an \ref Outer coroutine until finished
 */
void sample_stackless_protoThread()
{
	debugLogf("%s\n", __func__);
	int iteration = 0;
	debugLogf("- %d\n", iteration++);
	while (co.Run())
	{
		debugLogf("- %d\n", iteration++);
	}
	debugLogf("- %d\n", iteration++);
}
