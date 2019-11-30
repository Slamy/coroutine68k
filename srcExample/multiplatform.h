/**
 * @file multiplatform.h
 * @date 02.12.2018
 * @author andre
 */

#ifndef INCLUDE_MULTIPLATFORM_H_
#define INCLUDE_MULTIPLATFORM_H_

#include "config.h"

#ifdef BUILD_FOR_AMIGADOS
#define PRINTF uart_printf
#define ASSERT uart_assert
void waitForVBlank();

/// Amiga custom chip registers
extern volatile struct Custom* mycustom;

/// Registers of the CIA-A
extern volatile struct CIA* myciaa;

#define custom mycustom
#define ciaa myciaa

#else

#include "uart.h"

/// Amiga custom chip registers
extern volatile struct Custom* const custom;

/// Registers of the CIA-A
extern volatile struct CIA* const ciaa;

#endif

#endif /* INCLUDE_MULTIPLATFORM_H_ */
