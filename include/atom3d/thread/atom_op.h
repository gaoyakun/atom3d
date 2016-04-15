#ifndef __ATOM3D_THREAD_ATOM_OP_H
#define __ATOM3D_THREAD_ATOM_OP_H

#if _MSC_VER > 1000
#pragma once
#endif

static inline long ATOM_AtomExchange (long volatile *p, long val)
{
#if defined(_M_IA64)||defined(_M_AMD64)
	return ::InterlockedExchange (p, val);
#else
	register long result;

#	if defined(__GNUC__)
	__asm__ __volatile__ (
		"lock; xchgl %0, (%%ecx)\n"
		:"=a"(result)
		:"c"(p),"0"(val));
#	elif defined(_MSC_VER)
	__asm 
	{
		mov ecx, p
			mov eax, val
			mov edx, p
			lock xchg [ecx], eax
			mov result, eax
	}
#	else
#		error "Not supported"
#	endif
	return result;
#endif
}

static inline long ATOM_AtomExchangeAdd (long volatile *p, long val)
{
	register long result;

#if defined(_M_IA64)||defined(_M_AMD64)
	::InterlockedExchangeAdd(p, val);
#else
#	if defined(__GNUC__)
	__asm__ __volatile__ (
		"lock; xaddl %%eax, (%%ecx)\n"
		:"=a"(result)
		:"c"(p),"a"(val));
#	elif defined(_MSC_VER)
	__asm
	{
		mov ecx, p
			mov eax, val
			lock xadd [ecx], eax
			mov result, eax
	}
#	else
#			error "Not supported"
#	endif
#endif

	return result;
}

static inline long ATOM_AtomIncrement (long volatile *p)
{
	register long result;

#if defined(_M_IA64)||defined(_M_AMD64)
	::InterlockedIncrement(p);
#else
#if defined(__GNUC__)
	__asm__ __volatile__(
		"lock; xaddl %0, (%%ecx)\n"
		: "=a"(result)
		:"c"(p),"0"(1));
#elif defined(_MSC_VER)
	__asm
	{
		mov ecx, p
			mov eax, 1
			lock xadd [ecx], eax
			mov result, eax
	}
#else
#error "Not supported"
#endif
#endif

	return result+1;
}

static inline long ATOM_AtomDecrement (long volatile *p)
{
	register long result;

#if defined(_M_IA64)||defined(_M_AMD64)
	::InterlockedDecrement (p);
#else
#if defined(__GNUC__)
	__asm__ __volatile__(
		"lock; xaddl %0, (%%ecx)\n"
		: "=a"(result)
		: "c"(p), "0"(-1));
#elif defined(_MSC_VER)
	__asm
	{
		mov ecx, p
			mov eax, -1
			lock xadd [ecx], eax
			mov result, eax
	}
#else
#error "Not supported"
#endif
#endif

	return result-1;
}

static inline bool ATOM_AtomCompareExchange (long volatile *p, long oldval, long newval)
{
#if defined(_M_IA64)||defined(_M_AMD64)
	return ::InterlockedCompareExchange (p, newval, oldval) == oldval;
#else
	char result;

#if defined(__GNUC__)
	__asm__ __volatile__(
		"lock; cmpxchgl %2, %1; setz %0;\n"
		:"=a"(result), "=m"(*p)
		:"r"(newval),"a"(oldval)
		:"cc");
#elif defined(_MSC_VER)
	__asm
	{
		mov ecx,p
			mov eax,oldval
			mov ebx,newval
			lock cmpxchg [ecx],ebx
			setz result
	}
#else
#error "Not supported"
#endif
	return result != 0;
#endif
}

#endif // __ATOM3D_THREAD_ATOM_OP_H
