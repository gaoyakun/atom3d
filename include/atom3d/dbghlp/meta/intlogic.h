#ifndef __ATOM_UTILS_INTLOGIC_H
#define __ATOM_UTILS_INTLOGIC_H

typedef char ATOM_yes_type;
struct ATOM_no_type { char padding[8]; };

// integer and

template <bool b1, bool b2, bool b3 = true, bool b4 = true, bool b5 = true, bool b6 = true, bool b7 = true>
struct ATOM_IntAnd;

template <bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
struct ATOM_IntAnd
{
	static const bool value = false;
};

template <>
struct ATOM_IntAnd<true, true, true, true, true, true, true>
{
	static const bool value = true;
};

// Integer equal

template <int b1, int b2>
struct ATOM_IntEqual
{
	static const bool value = (b1 == b2);
};

template <int b1, int b2>
struct ATOM_IntNotEqual
{
	static const bool value = (b1 != b2);
};

// Integer boolean test

template <bool b>
struct ATOM_IntNot
{
	static const bool value = true;
};

template <>
struct ATOM_IntNot<true>
{
	static const bool value = false;
};

// Integer or

template <bool b1, bool b2, bool b3 = false, bool b4 = false, bool b5 = false, bool b6 = false, bool b7 = false>
struct ATOM_IntOr;

template <bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
struct ATOM_IntOr
{
	static const bool value = true;
};

template <>
struct ATOM_IntOr<false, false, false, false, false, false, false>
{
	static const bool value = false;
};

#endif // __ATOM_UTILS_INTLOGIC_H
