/**
 * @file Coroutine68k.h
 * @date 22.11.2019
 * @author andre
 */

#ifndef INCLUDE_COROUTINE68K_H_
#define INCLUDE_COROUTINE68K_H_

#include <assert.h>
#include <functional>
#include <stdint.h>
#include <vector>

//#define DEBUG_RUNTIME_CHECK

/**
 * Implementation of stackful coroutines on the 68k architecture.
 * Pausing and resuming can therefore be done inside sub functions.
 */
class Coroutine68k
{
  protected:
	/// saved stack pointer outside of coroutine
	uint32_t* normalStack = nullptr; // offset 4. Don't move this variable!

	/// saved stack pointer inside of coroutine
	uint32_t* coroutineStack = nullptr; // offset 8. Don't move this variable!

	/// True if execution is possible. false if either not initialized or ended.
	bool contextValid = false;

	/// size of stack in 32 bit words
	uint16_t stackSize;

	/// the stack used by the coroutine. stack pointer is placed aligned at the top
	std::vector<uint32_t> stack;

	/// Suspends the coroutine context. Restores saved context.
	void saveCoroutineReturnNormal() asm("saveCoroutineReturnNormal");

	/// Saves context. Resumes the coroutine context.
	void saveNormalRestoreCoroutine() asm("saveNormalRestoreCoroutine");

	/// Restores normal saved context without proper suspending the coroutine
	/// context.
	void discardCoroutineReturnNormal() asm("discardCoroutineReturnNormal");

	/**
	 * Internal C++ function which is the start of execution of the coroutine and the artificial
	 * caller of func() to catch a return from func() as leaving the coroutine context is not
	 * possible through return.
	 */
	void internalStop()
	{
		// start the coroutine
		func();

		/*
		 * If we have reached this point this means that the coroutine has stopped execution by
		 * returning without yielding. But there is nothing to return to. So at this point we define
		 * that the context is now invalid and continue execution on the normal context.
		 */

		contextValid = false;
		discardCoroutineReturnNormal();

#ifdef DEBUG_RUNTIME_CHECK
		// this line must never be reached.
		assert(false);
		for (;;)
			;
#endif
	}

  public:
	/**
	 * Prepares a coroutine with a seperate stack.
	 * @param stackSize		Size of stack in words
	 */
	Coroutine68k(uint16_t stackSize = 200) : stackSize(stackSize), stack(stackSize)
	{
#ifdef DEBUG_RUNTIME_CHECK
		int i;
		for (i = 0; i < 16; i++)
			stack[i] = 0xDEADBEEF;
#endif
	}

	virtual ~Coroutine68k()
	{
	}

	// clang-format off
	/**
	 * Initializes separate stack for Coroutine but doesn't execute it yet.
	 * The stack is constructed like this:
	 * ---------------- Top End of Stack
	 * [     this     ] The first parameter of internalStop which is the this pointer of this class.
	 * [      0       ] Usually this would be the return address of internalStop. But it doesn't exist.
	 * [ internalStop ] Pointer to internalStop() as the return address of saveNormalRestoreCoroutine()
	 * [  d2-d7/a2-a6 ] 11 Words. Suspended registers. Used by functions in context.asm
	 *      V V V V     Growing direction of stack.
	 */
	void init();
	// clang-format on

#ifdef DEBUG_RUNTIME_CHECK
	/**
	 * Check stack frame for overflows.
	 * Only for debugging.
	 */
	void checkStack()
	{
		int i;
		for (i = 0; i < 16; i++)
			assert(stack[i] == 0xDEADBEEF);
	}
#endif

  public:
	/**
	 * Actual worker function.
	 */
	virtual void func() = 0;
};

/**
 * Implementation of a coroutine which can just yield without any promises or expectations.
 * It's more like a cooperative task.
 */
class CoTask : public Coroutine68k
{
  public:
	/**
	 * Resume the Coroutine.
	 * @return	True if Coroutine has finished execution. False if yielded.
	 */
	bool operator()()
	{
		if (contextValid == true)
		{
			saveNormalRestoreCoroutine();
		}

		return !contextValid;
	}

  protected:
	/**
	 * Can be used from func() or a function called by func() to suspend
	 * execution.
	 */
	void coYield()
	{
#ifdef DEBUG_RUNTIME_CHECK
		checkStack();
		assert(contextValid);
#endif
		saveCoroutineReturnNormal();
	}

	/**
	 * Suspend execution as long as cond is true
	 * FIXME By C++ standard this is considered bad as it is a macro. But maybe it's faster...
	 */
#define CO_WAIT_WHILE(cond)                                                                        \
	while (cond)                                                                                   \
	{                                                                                              \
		coYield();                                                                                 \
	}

	/**
	 * Suspend execution as long as cond is false.
	 * FIXME By C++ standard this is considered bad as it is a macro. But maybe it's faster...
	 */
#define CO_WAIT_UNTIL(cond)                                                                        \
	while (!cond)                                                                                  \
	{                                                                                              \
		coYield();                                                                                 \
	}

	/**
	 * Suspend execution as long as cond is false
	 * FIXME This is a more modern C++ solution than the macro CO_WAIT_UNTIL.
	 * But I need to analyze performance.
	 */
	void coWaitUntil(std::function<bool()> cond)
	{
		while (!cond())
		{
			coYield();
		}
	}

	/**
	 * Suspend execution as long as cond is true
	 * FIXME This is a more modern C++ solution than the macro CO_WAIT_WHILE.
	 * But I need to analyze performance.
	 */
	void coWaitWhile(std::function<bool()> cond)
	{
		while (cond())
		{
			coYield();
		}
	}
};

/**
 * Implementation of a coroutine which expects an external token to continue it's work.
 * Inspired by push_type from:
 * https://www.boost.org/doc/libs/1_59_0/libs/coroutine2/doc/html/coroutine2/coroutine/asymmetric.html
 */
template <class T> class CoParser : public Coroutine68k
{
  public:
	/**
	 * creates coroutine context and starts the execution to yield at the next source() call.
	 */
	void init()
	{
		// I would really want to call init() in the constructor here.
		// But it's forbidden by C++ standard. So this has to do.

		Coroutine68k::init();

		/*
		 * resume coroutine until first source() to give the outer world the chance to put something
		 * into the token before the coroutine expects it
		 */
		saveNormalRestoreCoroutine();
	}

	/**
	 * Continues execution of the parser coroutine with given data.
	 * @param token		Value to returned by the currently yielded source() call
	 * @return			True if parser can still take tokens after the call. False if parser has
	 * ended execution.
	 */
	bool operator()(T token)
	{
		if (contextValid == true)
		{
			this->token = token;
			saveNormalRestoreCoroutine();
		}
		else
		{
			printf("ignored token\n");
		}

		return !contextValid;
	}

  private:
	T token;

  protected:
	/**
	 * Called by the coroutine to yield and expect some data from the outer world
	 * @return	Data provided by the callee.
	 */
	inline T source()
	{
#ifdef DEBUG_RUNTIME_CHECK
		assert(contextValid);
#endif
		saveCoroutineReturnNormal();
		return token;
	}
};

/**
 * Implementation of a coroutine which pushes out tokens and pauses until taken from outside.
 * Inspired by pull_type from:
 * https://www.boost.org/doc/libs/1_59_0/libs/coroutine2/doc/html/coroutine2/coroutine/asymmetric.html
 *
 * Iterators for range loops:
 * https://www.cs.helsinki.fi/u/tpkarkka/alglib/k06/lectures/iterators.html
 */
template <class T> class CoGenerator : public Coroutine68k
{
  public:
	/**
	 * creates coroutine context and starts the execution as the outer world must be able to detect
	 * whether this generator holds on sink() or has ended.
	 */
	void init()
	{
		// I would really want to call init() in the constructor here.
		// But it's forbidden by C++ standard. So this has to do.
		Coroutine68k::init();

		/*
		 * resume coroutine until first sink() to give the outer world the chance to detect whether
		 * a token is available
		 */
		saveNormalRestoreCoroutine();
	}

	/**
	 * A generator only yields to give it's output to the outer world. The context is only valid
	 * when yielded().
	 * @return	true when output is available. false if generator has finished its work.
	 */
	inline bool hasNext()
	{
		return contextValid;
	}

	/**
	 * Executes the coroutine until it calls sink() to yield.
	 * @return	The generated output
	 */
	inline T operator()()
	{
		T ret = token;
		run();
		return ret;
	}

	/**
	 * iterator class which provides access to the \ref CoGenerator using STL iterator type access.
	 * makes range loops possible.
	 */
	class iterator
	{
	  private:
		CoGenerator<T>& ptr;

	  public:
		/// creates an iterator using the given \ref CoGenerator
		iterator(CoGenerator<T>& p) : ptr(p)
		{
		}

		/// returns the last produced result by the \ref CoGenerator
		T& operator*()
		{
			return ptr.token;
		}

		/// continues execution
		iterator& operator++()
		{
			ptr.run();
			return *this;
		}

		/// currently unused and untested
		iterator operator++(int)
		{
			iterator tmp = *this;
			++*this;
			return tmp;
		}

		/// currently unused and untested
		bool operator==(const iterator& other) const
		{
			return !ptr.contextValid;
		}

		/// overloaded operator!= used for range loops and iterator loops
		bool operator!=(const iterator& other) const
		{
			return ptr.contextValid;
		}
	};

	/**
	 * Initializes context and therefore resets the coroutine.
	 * @return	An iterator to access the coroutines results.
	 */
	iterator begin()
	{
		init();
		return iterator(*this);
	}

	/**
	 * Returns an iterator which is kinda not functional and only exists for artifical comparsion.
	 * @return	An iterator to access the coroutines results.
	 */
	iterator end()
	{
		return iterator(*this);
	}

  private:
	T token;

	inline void run()
	{
		if (contextValid == true)
		{
			saveNormalRestoreCoroutine();
		}
		else
		{
			printf("ignored token\n");
		}
	}

  protected:
	/**
	 * Called by the coroutine to yield and give a result to the caller.
	 * @param t	Token to return to the outer world.
	 */
	inline void sink(T t)
	{
#ifdef DEBUG_RUNTIME_CHECK
		assert(contextValid);
#endif
		token = t;
		saveCoroutineReturnNormal();
	}
};

#endif /* INCLUDE_COROUTINE68K_H_ */
