#ifndef __ATOM_UTILS_FALSETRUE_H
#define __ATOM_UTILS_FALSETRUE_H

struct ATOM_FalseType { static const bool value = false; };
struct ATOM_TrueType { static const bool value = true; };

template <bool b> struct ATOM_FalseTrue { typedef ATOM_FalseType type; };
template<> struct ATOM_FalseTrue<true> { typedef ATOM_TrueType type; };

#endif // __ATOM_UTILS_FALSETRUE_H

