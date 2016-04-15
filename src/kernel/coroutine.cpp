#include "StdAfx.h"
#include "coroutine.h"

static unsigned stackSize = 0;
static void *threadFiber = 0;
static unsigned numCoroutines = 0;
static ATOM_Coroutine::Coroutine *coroutineHead = 0;
static ATOM_Coroutine::Coroutine *currentCoroutine = 0;

struct ATOM_Coroutine::Coroutine
{
	ATOM_Coroutine::CoroutineFunc func;
	void *userdata;
	unsigned sleepTime;
	unsigned lastTime;
	bool finished;
	void *WIN32Fiber;
	Coroutine *next;
};

static void CALLBACK FiberFunc (void *p)
{
	ATOM_Coroutine::Coroutine *data = (ATOM_Coroutine::Coroutine*)p;
	data->finished = false;
	data->func (data->userdata);
	data->finished = true;
	::SwitchToFiber (threadFiber);
}

void ATOM_Coroutine::init (unsigned coroutineStackSize)
{
	if (!threadFiber)
	{
		stackSize = coroutineStackSize ? coroutineStackSize : 4 * 1024;
		threadFiber = ::ConvertThreadToFiber (NULL);
		coroutineHead = 0;
	}
}

void ATOM_Coroutine::done (void)
{
	if (threadFiber)
	{
		ATOM_Coroutine::Coroutine *c = coroutineHead;
		while (c)
		{
			ATOM_Coroutine::Coroutine *next = c->next;
			::DeleteFiber (c->WIN32Fiber);
			ATOM_FREE(c);
			c = next;
		}
		coroutineHead = 0;
		currentCoroutine = 0;
		::ConvertFiberToThread ();
		threadFiber = 0;
		numCoroutines = 0;
	}
}

ATOM_Coroutine::Coroutine *ATOM_Coroutine::createCoroutine (CoroutineFunc func, void *param)
{
	if (!threadFiber)
	{
		init (0);
	}

	if (threadFiber && func)
	{
		Coroutine *c = (Coroutine*)ATOM_MALLOC(sizeof(Coroutine));
		c->func = func;
		c->userdata = param;
		c->finished = false;
		c->WIN32Fiber = ::CreateFiber (stackSize, &FiberFunc, c);
		if (!c->WIN32Fiber)
		{
			ATOM_FREE(c);
			return nullptr;
		}
		c->sleepTime = 0;
		c->lastTime = 0;
		numCoroutines++;

#if 1
		c ->next = 0;
		if (coroutineHead)
		{
			Coroutine *p = coroutineHead;
			for (; p->next; p = p->next)
				;
			p->next = c;
		}
		else
		{
			coroutineHead = c;
		}
#else
		c->next = coroutineHead;
		coroutineHead = c;
#endif

		return c;
	}
	return 0;
}

ATOM_Coroutine::Coroutine *ATOM_Coroutine::getCurrentCoroutine (void)
{
	return currentCoroutine;
}

bool ATOM_Coroutine::isCoroutine (Coroutine *coroutine)
{
	if (coroutine)
	{
		for (Coroutine *c = coroutineHead; c; c = c->next)
		{
			if (c == coroutine)
			{
				return true;
			}
		}
	}
	return false;
}

void ATOM_Coroutine::deleteCoroutine (Coroutine *coroutine)
{
	if (!threadFiber)
	{
		init (0);
	}

	if (threadFiber)
	{
		ATOM_Coroutine::Coroutine *c = coroutineHead;
		ATOM_Coroutine::Coroutine *prev = NULL;
		while (c)
		{
			if (c == coroutine)
			{
				::DeleteFiber (c->WIN32Fiber);
				numCoroutines--;

				if (prev)
				{
					prev->next = c->next;
				}
				else
				{
					coroutineHead = c->next;
				}
				ATOM_FREE(c);
				break;
			}
			else
			{
				prev = c;
				c = c->next;
			}
		}
	}
}

unsigned ATOM_Coroutine::getNumCoroutines (void)
{
	return numCoroutines;
}

void ATOM_Coroutine::yieldTo (Coroutine *target, unsigned sleepTimeInMs)
{
	if (threadFiber)
	{
		if (currentCoroutine)
		{
			currentCoroutine->sleepTime = sleepTimeInMs;
			currentCoroutine->lastTime = sleepTimeInMs ? ATOM_APP->getFrameStamp().currentTick : 0;
		}

		if (target && !target->finished)
		{
			currentCoroutine = target;
			currentCoroutine->sleepTime = 0;
			currentCoroutine->lastTime = 0;

			::SwitchToFiber (target->WIN32Fiber);
		}
		else
		{
			currentCoroutine = 0;
			::SwitchToFiber (threadFiber);
		}
	}
}

#if 1
void ATOM_Coroutine::dispatch (void)
{
	static ATOM_Coroutine::Coroutine *start = 0;
	static ATOM_Coroutine::Coroutine *prev = NULL;

	if (threadFiber)
	{
		unsigned t = ATOM_APP->getFrameStamp().currentTick;

		if (!start)
		{
			start = coroutineHead;
			prev = NULL;
		}
		else
		{
			for (ATOM_Coroutine::Coroutine *c = coroutineHead; c; c = c->next)
			{
				if (c->next == start)
				{
					prev = c;
					break;
				}
			}
		}

		while (start)
		{
			if (start->finished)
			{
				::DeleteFiber (start->WIN32Fiber);
				numCoroutines--;

				if (prev)
				{
					prev->next = start->next;
				}
				else
				{
					coroutineHead = start->next;
				}
				ATOM_FREE(start);
				start = prev ? prev->next : coroutineHead;
			}
			else
			{
				if (start != currentCoroutine)
				{
					ATOM_Coroutine::Coroutine *c = 0;
					if (start->sleepTime == 0 || t - start->lastTime >= start->sleepTime)
					{
						c = start;
					}
					prev = start;
					start = start->next;

					if (c)
					{
						currentCoroutine = c;
						yieldTo (c, 0);
					}
				}
				else
				{
					prev = start;
					start = start->next;
				}
			}
		}

		start = 0;
		prev = 0;
		currentCoroutine = 0;
	}
}

#else
void ATOM_Coroutine::dispatch (void)
{
	if (threadFiber)
	{
		unsigned t = ATOM_APP->getFrameStamp().currentTick;

		ATOM_Coroutine::Coroutine *c = coroutineHead;
		ATOM_Coroutine::Coroutine *prev = NULL;
		while (c)
		{
			if (c->finished)
			{
				::DeleteFiber (c->WIN32Fiber);
				numCoroutines--;

				if (prev)
				{
					prev->next = c->next;
				}
				else
				{
					coroutineHead = c->next;
				}
				ATOM_FREE(c);
				c = prev ? prev->next : coroutineHead;
			}
			else
			{
				if (c->sleepTime == 0 || t - c->lastTime >= c->sleepTime)
				{
					currentCoroutine = c;
					yieldTo (c, 0);
				}
				prev = c;
				c = c->next;
			}
		}
		currentCoroutine = 0;
	}
}

#endif

bool ATOM_Coroutine::isEmpty (void)
{
	return coroutineHead == NULL;
}

