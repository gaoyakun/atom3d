/****************cvs log:********************
$Log: config.h,v $
Revision 1.1  2006/05/16 10:35:31  zhz
*** empty log message ***

Revision 1.3  2006/04/10 00:58:08  ds
*** empty log message ***

Revision 1.1  2006/01/08 09:28:21  yangkun
*** empty log message ***

Revision 1.1  2005/06/14 01:28:52  houstond
*** empty log message ***

Revision 1.2  2005/04/03 13:45:39  houstond
*** empty log message ***

Revision 1.1  2005/04/03 10:12:15  houstond
*** empty log message ***

Revision 1.1  2005/04/02 14:01:12  houstond
*** empty log message ***

Revision 1.1  2004/08/12 06:21:32  gaoyakun
Initial check in.

*********************************************/

#ifdef SAVE_RCSID
static char rcsid = "@(#) $Id: config.h 1736 2008-12-15 07:59:49Z gyk $";
#endif

#ifndef __ATOM_UTILS_CONFIG_H
#define __ATOM_UTILS_CONFIG_H

#if (_MSC_VER <= 1200)
# define NS_STD
# define TYPENAME
#endif

#ifndef NS_STD
# define NS_STD std::
#endif

#ifndef TYPENAME
# define TYPENAME typename
#endif

#undef ATOM_COMPILER_NO_ARRAYTYPE_SPEC
#define ATOM_COMPILER_HAS_LONG_LONG
#define ATOM_COMPILER_HAS_MS_INT64
#undef ATOM_COMPILER_HAS_QUALIFIERS_ON_REFERENCE

#endif // __ATOM_UTILS_CONFIG_H
