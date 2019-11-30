# coroutine68k
An implementation of stackful coroutines for the 68k architecture as a C++ class.

# Why does this exist?
Well this is a long story.

Recently I've read through the enhancements the C++ fans get with C++20.

I was especially interested in the concept of [coroutines](https://en.cppreference.com/w/cpp/language/coroutines) as it is an approach for asynchronous programming and an alternative to state machines.
The latter always have the problem that they are unstructured and it sometimes is difficult to "see" the flow of the states.
On the other hand a synchronous algorithm as a structured program might be easier to understand.

It turns out that the idea of a resumable function is not new and multiple variants of them exist:

* Stackless
    * Suspend and Resume of a function is not possible in a normal function called by a resumable one.
    * The reason for this is that there is no seperate stack to save that call.
    * No local variables on stack are allowed.
* Stackful
    * Suspend and Resume can happen even inside a called function as the coroutine manages it's own stack.

Both concepts advantages and disadvantages and both were implemented:

* Stackless
    * [Protothreads from Adum Dunkels](http://dunkels.com/adam/pt/) for C
    * [Protothreads from benhoyt](https://github.com/benhoyt/protothreads-cpp) Adaption of Dunkels idea for C++
    * C++20 (but not yet available on GCC)
* Stackful
    * [Boost Coroutine2](https://www.boost.org/doc/libs/1_71_0/libs/coroutine2/doc/html/index.html). But this needs the boost library and is not header only!

I've tried quite some of them as I wanted to evalute their usefulnes.

## Stackless

Protothreads is quite cute and works even on the smallest machines. It is even standard C and uses a quirk of the switch statement to perform its task. It does have a problem with local variables though as these must be defined as static or coming from an external struct.

The Protothreads from benhoyt use advantages from C++. The fact that no local variables can be declared is hidden by declaring them as members of your own coroutine class.

Both of these implementation make heavy use of macros and are unable to suspend during a switch statement.
The latter can be fixed by using a Protothreads variant with computed gotos. But that's a different story.
This "heavily obfuscated code" as one might say might scare someone off and only C++20 might finally bring stackless coroutines in a clean way to the language.
I was unable to find a better solution for stackless at the moment. :-(


## Stackful

Stackful coroutines don't need any enhancements of the C++ compiler. It's just a library that does context switches.
Boost Coroutine2 manages a custom context with it's own stack and just switches out the CPU registers and the stack pointer in case a yield must be performed.
This approach can be time consuming at best. But the worst part is that it is architecture dependend. Every type of CPU needs it's own context switching function.

... context switching ... ah yes!

At this point you as the reader must have figured it out as well. This is what every operating system does all the time.
Preemptive multitasking is switches between threads. Every thread has it's own stack.
So a stackful coroutine is nothing else than a cooperative context switching thread. And also there lies the power. You don't need mutexes to synchronize them because of that as race conditions don't exist between cooperative threads.
Also the suspending of the stackful coroutine can happen anytime.

# Purpose of this project.

So where does this project fit?
Boost Coroutine2 heavily relies on system libraries and therefore can't be used on a bare metal environment.
As I'm currently on a 68k trip I wondered how easy it would be to create a similar thing for that processor.
My motivation was also boosted (ha, get it?) as FreeRTOS is also available for it and so I can inspire the context switching by reading the source code of that.

Like my other 68k GCC projects this again can be build for AmigaDOS and also as NDOS image for the Amiga to boot directly into.

# Implementation of the context switch

The context switch itself cannot be written in C and is thereforce implemented inside `context.asm`. The registers `d2-d7/a2-a6` are pushed onto the stack using `movem.l` and the stack pointer is replaced by the one of another context to pull the same registers again to continue.
This of course means that the current implementation might not be working with an FPU as only the registers are saved which are needed to be saved according to the 68k GCC ABI. D0, D1, A0 and A1 are only temporary registers and don't need any saving. A7 doesn't need any saving too, because .... it's the stack pointer.

The rest is some C++ magic inside the Coroutine68k class.

# Results

The project does use the CIAA timer to perform some measurements.
When the project is compiled with MEASURE_TIME enabled, debug logging is disabled and the values can be used for profiling.
This is the output executed on an emulated A1200 without further modifications. The numbers represents CIAA ticks.
Execute `scripts/serialout.sh` to open a virtual serial port.
Execute `fs-uae a1200.fs-uae` and you should get similar results on the serial port.

	Slamy's Coroutine Example for 68k
	ticks inside empty 11
	ticks inside test_protoThread 136
	ticks inside test_innerOuter 241
	ticks inside test_ioWaiting(wait_lambda) 215
	ticks inside test_ioWaiting(wait_macro) 189
	ticks inside empty 11
	ticks inside jumpInOut 51
	ticks inside jumpInOut 50
	ticks inside jumpInOut std::bind empty 14
	ticks inside parserTest(B) 61
	ticks inside parserTest(C) 62
	ticks using while loop + gen.hasNext() + gen() 231
	ticks using iterator loop 215
	ticks using range loop 217



These results might explain why C++20 is planned to use stackless coroutines.
These small examples might not be that representative but the context switching seems to take more time.
Also I've compared macros against lambdas for checking waiting conditions and it again seems that the C approach is a little bit more performant.
Further investigation might be needed at this point.

JumpInOutCoroutine was added to allow instruction tracing of a switch from a coroutine out and directly back in.

	Inside <JumpInOutCoroutine::func()>:
	 0 00005234 4e71                     NOP 
	 0 00005236 2f02                     MOVE.L D2,-(A7)
	 0 00005238 4e92                     JSR (A2)
	<saveCoroutineReturnNormal>:
	 0 00006914 206f 0004                MOVEA.L (A7, $0004) == $0000ca94,A0
	 0 00006918 48e7 3f3e                MOVEM.L D2-D7/A2-A6,-(A7)
	 0 0000691C 214f 0008                MOVE.L A7,(A0, $0008) == $0000c6f8
	 0 00006920 2e68 0004                MOVEA.L (A0, $0004) == $0000c6f4,A7
	 0 00006924 4cdf 7cfc                MOVEM.L (A7)+,D2-D7/A2-A6
	 0 00006928 4e75                     RTS
	Continue in <main>:
	 0 00005D30 588f                     ADDA.L #$00000004,A7
	 0 00005D32 4a39 0000 c6fc           TST.B $0000c6fc
	 0 00005D38 6700 fede                BEQ.W #$fede == $00005c18 (F)
	 0 00005D3C 2f02                     MOVE.L D2,-(A7)
	 0 00005D3E 4e93                     JSR (A3)
	<saveNormalRestoreCoroutine>:
	 0 0000692A 206f 0004                MOVEA.L (A7, $0004) == $0007fe88,A0
	 0 0000692E 48e7 3f3e                MOVEM.L D2-D7/A2-A6,-(A7)
	 0 00006932 214f 0004                MOVE.L A7,(A0, $0004) == $0000c6f4
	 0 00006936 2e68 0008                MOVEA.L (A0, $0008) == $0000c6f8,A7
	 0 0000693A 4cdf 7cfc                MOVEM.L (A7)+,D2-D7/A2-A6
	 0 0000693E 4e75                     RTS
	Continue inside <JumpInOutCoroutine::func()>:
	 0 0000523A 4e71                     NOP 
 