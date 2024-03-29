/**
 * @file main.cpp
 * @date 22.06.2019
 * @author andre
 */

#include "config.h"
#include "multiplatform.h"

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
#include "Protothread.h"

#include "sample_cogenerator.h"
#include "sample_coparser.h"
#include "sample_coroutine68.h"
#include "sample_stackless.h"

#ifdef BUILD_FOR_AMIGADOS
volatile struct Custom* mycustom = (volatile struct Custom*)0xDFF000;
volatile struct CIA* myciaa		 = (volatile struct CIA*)0xbfe001;
#endif

extern "C"
{
#include "uart.h"
}

#if !defined(BUILD_FOR_AMIGADOS)

/**
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

/// FIXME Why is this needed?
void* __dso_handle;

#endif

/**
 * Just an empty function for runtime measurement of a function call
 */
void empty()
{
}

/**
 * Just the main function. duh.
 * @param argc	ignored
 * @param argv	ignored
 * @return		always 0
 */
int main(int argc, char** argv)
{
	uint16_t time;

	LambdaWaitTest wait_lambda;
	MacroWaitTest wait_macro;

	// set_debug_traps();
#if !defined(BUILD_FOR_AMIGADOS)
	staticConstructors();
#endif
	printf("Slamy's Coroutine Example for 68k\n");

	time = measureTime(empty);
	printf("ticks inside empty %d\n", time);

	time = measureTime(sample_stackless_protoThread);
	printf("ticks inside test_protoThread %d\n", time);

	time = measureTime(sample_coroutine68k_innerOuter);
	printf("ticks inside test_innerOuter %d\n", time);

	time = measureTime(std::bind(sample_coroutine68k_ioWaiting, wait_lambda));
	printf("ticks inside test_ioWaiting(wait_lambda) %d\n", time);

	time = measureTime(std::bind(sample_coroutine68k_ioWaiting, wait_macro));
	printf("ticks inside test_ioWaiting(wait_macro) %d\n", time);

	time = measureTime(empty);
	printf("ticks inside empty %d\n", time);

	sample_jumpInOut();
	sample_parsers();
	sample_generators();

	printf("finished with everything\n");
	return 0;
}

namespace __gnu_cxx
{
/**
 * FIXME can this be removed?
 * Sorgt dafür, dass exception demangling nicht eingelinkt wird!
 * https://developer.mbed.org/forum/platform-32-ST-Nucleo-L152RE-community/topic/4802/?page=2#comment-25593
 */
void __verbose_terminate_handler()
{
	for (;;)
		;
}
} // namespace __gnu_cxx

/// I forgot what this is
extern "C" void __cxa_pure_virtual(void);
extern "C" void __cxa_pure_virtual(void)
{
	for (;;)
		;
}
