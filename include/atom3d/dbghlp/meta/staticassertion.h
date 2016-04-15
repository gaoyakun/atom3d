#ifndef __ATOM_UTILS_STATICASSERTION_H
#define __ATOM_UTILS_STATICASSERTION_H

template <bool b>
struct ATOM_checker_;

template <>
struct ATOM_checker_<true>
{
};

template <unsigned sz>
struct ATOM_checker2_
{
};

#ifndef ATOM_STATIC_ASSERT
#define ATOM_STATIC_ASSERT(b)  \
  typedef ATOM_checker2_<sizeof(ATOM_checker_<(bool)(b)>)> ATOM_check_result_##__LINE__;
#endif	// ATOM_STATIC_ASSERT

#endif // __ATOM_UTILS_STATICASSERTION_H
