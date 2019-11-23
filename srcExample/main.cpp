/*
 * main.cpp
 *
 *  Created on: 22.06.2019
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

#include "Coroutine68k.h"

extern "C"
{
#include "uart.h"
}

#if !defined(BUILD_FOR_AMIGADOS)
/*
 * FIXME I'm quite confused why this is needed. As it seems this is normally
 * done by libgcc and crtbegin.o inside of that lib. But I have the feeling that
 * libgloss which also provides crt0.o is not compatible with crtbegin.o. While
 * the .ctors segment seems to be correct the .init segment is not and the
 * system will crash. I'm not sure why this happens. So until then global C++
 * constructors must be called by manually processing .ctors after main is
 * called. Usually this would happen before main is called.
 */
void staticConstructors()
{
	extern uint32_t __CTOR_LIST__;

	uint32_t* ptr = &__CTOR_LIST__;
	ptr++; // skip number of pointers

	while (*ptr)
	{
		void (*func)() = (void (*)()) * ptr;
		// printf("Execute static constructor %lx\n", *ptr);
		func();

		ptr++;
	}
}

// FIXME Why is this needed?
void* __dso_handle;

#endif

class TestCoroutine : public Coroutine68k
{
  public:
	void func() override;
	void funcInner();

  private:
};

void TestCoroutine::funcInner()
{
	int iteration = 0;
	printf("    - %d\n", iteration++);
	coYield();
	printf("    - %d\n", iteration++);
	coYield();
	printf("    - %d\n", iteration++);
}

void TestCoroutine::func()
{
	int iterationOuter = 0;
	printf("  - %d\n", iterationOuter++);
	coYield();
	printf("  - %d\n", iterationOuter++);
	funcInner();
	printf("  - %d\n", iterationOuter++);
	funcInner();
	printf("  - %d\n", iterationOuter++);
}

class WaitTest : public Coroutine68k
{
  public:
	volatile bool gotIt = false;

	void func() override
	{
		printf("Not Yet\n");
		CO_WAIT_UNTIL(gotIt);
		printf("Finished\n");
	}
};

int main(int argc, char** argv)
{
	// set_debug_traps();
#if !defined(BUILD_FOR_AMIGADOS)
	staticConstructors();
#endif

	printf("Slamy's Coroutine for 68k\n");

#if 0
	TestCoroutine cotest;
	cotest.init();

	int iteration = 0;

	printf("- %d\n", iteration++);
	while (!cotest.run())
	{
		printf("- %d\n", iteration++);
	}
	printf("- %d\n", iteration++);

	printf("outside finished!\n");
#else

	WaitTest wait;

	wait.init();
	int iteration;

	for (iteration = 0; iteration < 7; iteration++)
	{
		if (iteration == 3)
			wait.gotIt = true;
		bool ret = wait.run();
		if (ret)
		{
			printf("T %d\n", iteration);
		}
		else
		{
			printf("F %d\n", iteration);
		}
	}
#endif

	return 0;
}

// FIXME can this be removed?
// Sorgt dafür, dass exception demangling nicht eingelinkt wird!
// https://developer.mbed.org/forum/platform-32-ST-Nucleo-L152RE-community/topic/4802/?page=2#comment-25593
namespace __gnu_cxx
{
void __verbose_terminate_handler()
{
	for (;;)
		;
}
} // namespace __gnu_cxx
extern "C" void __cxa_pure_virtual(void);
extern "C" void __cxa_pure_virtual(void)
{
	for (;;)
		;
}
