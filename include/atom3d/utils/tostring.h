#ifndef __ATOM_UTILS_TOSTRING_H
#define __ATOM_UTILS_TOSTRING_H

#if _MSC_VER > 1000
# pragma once
#endif

#define ATOM_TOSTRING2(x) #x
#define ATOM_TOSTRING(x) N3_TOSTRING2(x)

#endif // __ATOM_UTILS_TOSTRING_H
