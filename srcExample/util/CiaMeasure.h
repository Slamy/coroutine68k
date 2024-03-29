/**
 * @file CiaMeasure.h
 * @date 24.11.2019
 * @author andre
 */

#ifndef SRCEXAMPLE_CIAMEASURE_H_
#define SRCEXAMPLE_CIAMEASURE_H_

#include "config.h"
#include "multiplatform.h"

#include <functional>
#include <stdarg.h>

extern "C"
{
#include <hardware/cia.h>
#include <hardware/custom.h>
#ifdef BUILD_FOR_AMIGADOS
#include <dos/dos.h>
#include <exec/exec.h>
#include <proto/cia.h>
#include <proto/dos.h>
#include <proto/exec.h>
#endif
}

/**
 * Will call printf if MEASURE_TIME is not defined.
 * Otherwise it does some volatile stuff to avoid being optimized out.
 */
static inline void debugLogf(const char* format, ...)
{
#ifdef MEASURE_TIME
	// some calculation so this is not optimized out.
	static volatile int16_t a = 0;
	a++;
#else
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
#endif
}

/**
 * Executes a function and returns the time passed by in form of CIAA ticks.
 * @param funcToMeasure		Function to execute
 * @return					Number of CIAA ticks it took to execute the given function
 */
static inline uint16_t measureTime(std::function<void()> funcToMeasure)
{
#ifdef MEASURE_TIME
	// disable interrupts
#ifdef BUILD_FOR_AMIGADOS
	Disable();
#else
	// disable all interrupts
	custom->intena = 0x7FFF;
#endif

	// disable all DMA to avoid influence
	UWORD old_dmacon = custom->dmaconr | 0x8000;
	custom->dmacon   = 0x7fff;

	// reset and start timer
	ciaa->ciatahi = 0xff;
	ciaa->ciatalo = 0xff;
	ciaa->ciacra  = CIACRAF_START | CIACRAF_LOAD | CIACRAF_RUNMODE;

	funcToMeasure();

	uint8_t high = ciaa->ciatahi;
	uint8_t low  = ciaa->ciatalo;

	// CIAA is clocked with 0.715909 MHz
	// This means one tick is 1.39682557420007 µs

	custom->dmacon = old_dmacon;
#ifdef BUILD_FOR_AMIGADOS
	Enable();
#else

#endif

	return 0xffff - (((uint16_t)high << 8) | ((uint16_t)low));
#else
	funcToMeasure();
	return 0;
#endif
}

#endif /* SRCEXAMPLE_CIAMEASURE_H_ */
