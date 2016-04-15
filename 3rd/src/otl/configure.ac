#----------------------------------------------------------------------------
# Autoconf input script. Invoke the ./autogen.sh script to generate a
# configure script from this file.
#----------------------------------------------------------------------------
AC_PREREQ([2.54])

#----------------------------------------------------------------------------
# Initialize Autoconf.
#----------------------------------------------------------------------------
AC_INIT([opentree], [0.1.0], [syastrov@users.sourceforge.net])
CS_PACKAGEINFO(
    [OpenTree 3D Tree Generation Library],
    [Copyright (c) 2004 Pascal Kirchdorfer, Seth Yastrov],
    [http://opentreelib.sourceforge.net/])
AC_CONFIG_SRCDIR([mk/jam/build.jam])
AC_CONFIG_AUX_DIR([mk/autoconf])
AC_CANONICAL_HOST

#----------------------------------------------------------------------------
# Setup for the configuration header.
#----------------------------------------------------------------------------
AC_CONFIG_HEADERS([config.h])

#----------------------------------------------------------------------------
# Check for tools.
#----------------------------------------------------------------------------
CS_CHECK_MKDIR
CS_EMIT_BUILD_PROPERTY([CMD.MKDIR], [$MKDIR])
CS_EMIT_BUILD_PROPERTY([CMD.MKDIRS], [$MKDIRS])

CS_PROG_CC
AS_IF([test -z "$CC"],
    [AC_MSG_ERROR([Could not find a usable C compiler.])])
CS_PROG_CXX
AS_IF([test -z "$CXX"],
    [AC_MSG_ERROR([Could not find a usable C++ compiler.])])
CS_PROG_LINK

CS_CHECK_PROGS([INSTALL], [install])
CS_EMIT_BUILD_PROPERTY([INSTALL], [$INSTALL])

# The default RANLIB in Jambase is wrong on some platforms, and is also
# unsuitable during cross-compilation, so we set the value unconditionally
# (sixth argument of CS_EMIT_BUILD_PROPERTY).
AC_PROG_RANLIB
CS_EMIT_BUILD_PROPERTY([RANLIB], [$RANLIB], [], [], [], [Y])

CS_CHECK_PROGS([PERL], [perl5 perl])
CS_EMIT_BUILD_PROPERTY([PERL], [$PERL])

CS_CHECK_TEMPLATE_TOOLKIT2([emit])

CS_CHECK_PROGS([DOXYGEN], [doxygen])
CS_EMIT_BUILD_PROPERTY([CMD.DOXYGEN], [$DOXYGEN])

#----------------------------------------------------------------------------
# Determine system type
#----------------------------------------------------------------------------
CS_CHECK_HOST

#----------------------------------------------------------------------------
# Check for syntax problems / header files
#----------------------------------------------------------------------------
# Nothing yet.

#----------------------------------------------------------------------------
# Check for external libraries
#----------------------------------------------------------------------------
CS_CHECK_CPPUNIT([emit])

#----------------------------------------------------------------------------
# Determine build mode; either "optimize", "debug", or "profile".
#----------------------------------------------------------------------------
AC_ARG_ENABLE([optimize], [AC_HELP_STRING([--enable-optimize],
    [build with optimizations enabled (default YES);
	same as --enable-mode=optimize])],
    [test "$enableval" = "yes" && build_mode=optimize])

AC_ARG_ENABLE([debug], [AC_HELP_STRING([--enable-debug],
    [build with debugging information (default NO);
	same as --enable-mode=debug])],
    [test "$enableval" = "yes" && build_mode=debug])

AC_ARG_ENABLE([profile], [AC_HELP_STRING([--enable-profile],
    [build with profiling information (default NO);
	same as --enable-mode=profile])],
    [test "$enableval" = "yes" && build_mode=profile])

AC_ARG_ENABLE([mode], [AC_HELP_STRING([--enable-mode=mode],
    [set build mode; recognized modes are `optimize', `debug', `profile'
	(default OPTIMIZE)])],
    [case $enableval in
	optimize|debug|profile) build_mode=$enableval ;;
	*) CS_MSG_ERROR([m4_text_wrap(
	    [unrecognized mode --enable-mode=$enableval;
	    use `optimize', `debug', or `profile'], [    ], [[]], [60])]) ;;
    esac])

AC_MSG_CHECKING([build mode])
AS_VAR_SET_IF([build_mode], [], [build_mode=optimize])
AC_MSG_RESULT([$build_mode])
CS_EMIT_BUILD_PROPERTY([MODE], [$build_mode])

#------------------------------------------------------------------------------
# Check for typical required libraries (libm, libmx, libdl, libnsl).
#------------------------------------------------------------------------------

AC_LANG_PUSH([C++])
AC_CHECK_HEADERS([cmath], [CS_HEADER_PROPERTY([CS_HAVE_CMATH_H])])
AC_LANG_POP([C++])

AC_DEFUN([CS_CHECK_COMMON_LIBS],
    [AC_LANG_PUSH([C])
    AC_CHECK_LIB([m], [pow], [cs_cv_libm_libs=-lm], [cs_cv_libm_libs=])
    AC_CHECK_LIB([m], [cosf], [cs_cv_libm_libs=-lm])
    AC_CHECK_LIB([mx], [cosf])
    AC_CHECK_LIB([dl], [dlopen], [cs_cv_libdl_libs=-ldl], [cs_cv_libdl_libs=])
    AC_CHECK_LIB([nsl], [gethostbyname])
    AC_LANG_POP([C])])

CS_CHECK_COMMON_LIBS

AS_IF([test $ac_cv_lib_m_pow = yes || test $ac_cv_lib_m_cosf = yes],
    [CS_EMIT_BUILD_PROPERTY([COMPILER.LFLAGS], [-lm], [+])])
AS_IF([test $ac_cv_lib_mx_cosf = yes],
    [CS_EMIT_BUILD_PROPERTY([COMPILER.LFLAGS], [-lmx], [+])])
AS_IF([test $ac_cv_lib_dl_dlopen = yes],
    [CS_EMIT_BUILD_PROPERTY([COMPILER.LFLAGS], [-ldl], [+])])
AS_IF([test $ac_cv_lib_nsl_gethostbyname = yes],
    [CS_EMIT_BUILD_PROPERTY([COMPILER.LFLAGS], [-lnsl], [+])])

AS_IF([test $ac_cv_lib_m_cosf != no || test $ac_cv_lib_mx_cosf != no],
    [CS_HEADER_PROPERTY([CS_HAVE_MATH_H_FLOAT_FUNCS])])



#------------------------------------------------------------------------------
# Check for pthread.  Also check if pthread implementation supports the
# recursive mutex extension.
#------------------------------------------------------------------------------
CS_CHECK_PTHREAD([cygwin*])
CS_EMIT_BUILD_RESULT([cs_cv_sys_pthread], [PTHREAD])
AS_IF([test $cs_cv_sys_pthread_mutex_recursive != no],
    [CS_HEADER_PROPERTY([CS_PTHREAD_MUTEX_RECURSIVE],
	[$cs_cv_sys_pthread_mutex_recursive])])

#----------------------------------------------------------------------------
# Find libraries
#---------------------------------------------------------------------------

# CS_CHECK_LIB_WITH(LIBRARY, PROGRAM, [SEARCH-LIST], [LANGUAGE],
#                   [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND], [OTHER-CFLAGS],
#                   [OTHER-LFLAGS], [OTHER-LIBS], [ALIASES])

# SDL
CS_CHECK_LIB_WITH([SDL],
    [AC_LANG_PROGRAM(
    [[#include <SDL.h>
    #undef main]],
    [SDL_Init(0); SDL_Quit();])], [/usr/include/SDL /usr/local/include/SDL|], [], [], [], [], [], [], [sdl])

AS_IF([test $cs_cv_libSDL = yes],
    [CS_CHECK_BUILD([if SDL is sufficiently recent], [cs_cv_libSDL_recent],
    [AC_LANG_PROGRAM(
        [[#include <SDL.h>
        #undef main]],
        [SDL_EnableKeyRepeat(0,0)])],
    [], [], [CS_EMIT_BUILD_RESULT([cs_cv_libSDL], [SDL])], [], [],
    [$cs_cv_libSDL_cflags], [$cs_cv_libSDL_lflags], [$cs_cv_libSDL_libs])

    # SDL_image

    CS_CHECK_LIB_WITH([SDL_image],
        [AC_LANG_PROGRAM(
        [[#include <SDL_image.h>
        #undef main]],
        [IMG_Load("");])], [], [], [], [], [$cs_cv_libSDL_cflags])
    
    AS_IF([test $cs_cv_libSDL_image = yes],
        [CS_CHECK_BUILD([if SDL_image is sufficiently recent], [cs_cv_libSDL_image_recent],
        [AC_LANG_PROGRAM(
            [[#include <SDL_image.h>
            #undef main]],
            [IMG_Load("");])],
        [], [], [CS_EMIT_BUILD_RESULT([cs_cv_libSDL_image], [SDL_image])], [], [],
        [$cs_cv_libSDL_image_cflags], [$cs_cv_libSDL_image_lflags], [$cs_cv_libSDL_image_libs])])])

#------------------------------------------------------------------------------
# Check for OpenGL and companions.  Also check if GLX extensions are supported
# and define the (backward) header property CSGL_EXT_STATIC_ASSERTION if they
# are _not_ supported. Finally, check if the type GLhandleARB is exported by
# the GL headers. It is starting to show up in very modern headers, but is
# absent from most.
#
# IMPLEMENTATION NOTES
#
# Some Mesa installations require pthread, so pthread flags are employed if
# available.
#
# The check for opengl32 needs to precede other checks because Cygwin users
# often have Mesa installed, and Mesa's OpenGL library is compiled without the
# __stdcall flags which results in link errors, whereas Microsoft's native
# opengl32 works fine.  Conversely, some Unix implementations have Wine
# installed (Windows emulation layer) which includes an opengl32.so library.
# We need to avoid detection of this library on Unix since it would cause an
# undesirable dependence upon Wine.
#
# Many OpenGL libraries on Unix already contain GLX, so there is no separate
# GLX library, thus we first check for GLX using the discovered OpenGL library
# before attempting to locate a separate GLX-specific library.  The same logic
# is also applied to GLU and GLUT checks.
#
# On MacOS/X, some users have XFree86 installed which creates a link from
# /usr/include/GL to /usr/X11R6/include/GL.  We want to ignore this directory
# and instead check for Apple's OpenGL.framework, if we are not cross-building
# for Darwin.  We accomplish this by placing the OpenGL.framework test ahead of
# the other tests.
#
# At least one user (Jorrit) has a strange installation in which inclusion of
# <windows.h> fails if an int32 is not present, thus we must take this into
# account.
#------------------------------------------------------------------------------
AC_CHECK_TYPE([int32], [AC_DEFINE([HAVE_TYPE_INT32], [], [int32 is present])], [])
m4_define([cs_define_int32],
    [[#if !HAVE_TYPE_INT32
    typedef long int32;
    #endif
    ]])

AC_DEFUN([CS_CHECK_GL_PREPARE],
    [AC_REQUIRE([CS_CHECK_HOST])
    AC_REQUIRE([CS_CHECK_COMMON_LIBS])
    AC_REQUIRE([CS_CHECK_PTHREAD])
    AC_REQUIRE([AC_PATH_X])
    AC_REQUIRE([AC_PATH_XTRA])
    AC_CHECK_TYPE([int32], [AC_DEFINE([HAVE_TYPE_INT32])], [])
    AC_CHECK_HEADERS([windows.h], [], [], [cs_define_int32])])

# CS_GL_INCLUDE(CPP-MACRO,FALLBACK,HEADER)
AC_DEFUN([CS_GL_INCLUDE],
    [AC_REQUIRE([CS_CHECK_GL_PREPARE])
    [#if HAVE_WINDOWS_H
    #if !HAVE_TYPE_INT32
    typedef long int32;
    #endif
    #include <windows.h>
    #endif
    #ifndef CS_HEADER_GLOBAL
    #define CS_HEADER_GLOBAL(X,Y) CS_HEADER_GLOBAL_COMPOSE(X,Y)
    #define CS_HEADER_GLOBAL_COMPOSE(X,Y) <X/Y>
    #endif
    #ifdef $1
    #include CS_HEADER_GLOBAL($1,$3)
    #else
    #include <$2/$3>
    #endif]])

CS_CHECK_GL_PREPARE

# Apply plaform-specific flags if necessary.
cs_gl_plat_cflags=''
cs_gl_plat_lflags=''
cs_gl_plat_libs=''
AS_IF([test -n "$cs_cv_libm_cflags$cs_cv_libm_lflags$cs_cv_libm_libs"],
    [cs_gl_plat_cflags="$cs_cv_libm_cflags $cs_gl_plat_cflags"
    cs_gl_plat_lflags="$cs_cv_libm_lflags $cs_gl_plat_lflags"
    cs_gl_plat_libs="$cs_cv_libm_libs $cs_gl_plat_libs"])
AS_IF([test $cs_cv_sys_pthread = yes],
    [cs_gl_plat_cflags="$cs_cv_sys_pthread_cflags $cs_gl_plat_cflags"
    cs_gl_plat_lflags="$cs_cv_sys_pthread_lflags $cs_gl_plat_lflags"
    cs_gl_plat_libs="$cs_cv_sys_pthread_libs $cs_gl_plat_libs"])
AS_IF([test "$no_x" != yes],
    [cs_gl_plat_cflags="$X_CFLAGS $cs_gl_plat_cflags"
    cs_gl_plat_lflags="$cs_gl_plat_lflags"
    cs_gl_plat_libs="
	$X_PRE_LIBS $X_LIBS -lX11 -lXext $X_EXTRA_LIBS $cs_gl_plat_libs"])

# Mesa requested?
AC_ARG_WITH([mesa], [AC_HELP_STRING([--with-mesa],
	[use Mesa OpenGL library if available (default YES)])],
	[], [with_mesa=yes])

AS_IF([test $with_mesa != no],
    [cs_mesa_gl=CS_CREATE_TUPLE([],[],[-lMesaGL])
    cs_mesa_glu=CS_CREATE_TUPLE([],[],[-lMesaGLU])
    cs_mesa_glx=CS_CREATE_TUPLE([],[],[-lMesaGLX])])

# MacOS/X or Darwin?
AS_IF([test "x$cs_host_macosx" = "xyes"],
    [cs_osx_gl=CS_CREATE_TUPLE([-DCS_OPENGL_PATH=OpenGL],[],
	[-framework OpenGL])
    cs_osx_glu=CS_CREATE_TUPLE([-DCS_GLU_PATH=OpenGL],[],[-framework OpenGL])
    cs_osx_glut=CS_CREATE_TUPLE([-DCS_GLUT_PATH=GLUT],[],[-framework GLUT])])

# Windows?
case $host_os in
    mingw*|cygwin*)
	cs_win32_gl=CS_CREATE_TUPLE([],[],[-lopengl32])
	cs_win32_glu=CS_CREATE_TUPLE([],[],[-lglu32])
	cs_win32_glut=CS_CREATE_TUPLE([],[],[-lglut32])
    ;;
esac

# Check for OpenGL.
CS_CHECK_BUILD([for OpenGL], [cs_cv_libgl],
    [AC_LANG_PROGRAM([CS_GL_INCLUDE([CS_OPENGL_PATH],[GL],[gl.h])],[glEnd()])],
    [$cs_win32_gl \
    $cs_osx_gl \
    CS_CREATE_TUPLE([],[],[-lGL]) \
    CS_CREATE_TUPLE([],[],[-lgl]) \
    $cs_mesa_gl], [],
    [CS_EMIT_BUILD_RESULT([cs_cv_libgl], [GL])], [], [],
    [$cs_gl_plat_cflags], [$cs_gl_plat_lflags], [$cs_gl_plat_libs])

AS_IF([test $cs_cv_libgl = yes],
    [# Check for GLhandleARB.
    CS_CHECK_BUILD([for GLhandleARB], [cs_cv_libgl_glhandlearb],
	[AC_LANG_PROGRAM(
	    [CS_GL_INCLUDE([CS_OPENGL_PATH],[GL],[gl.h])],
	    [GLhandleARB x; (void)x;])],
	[], [],	[CS_HEADER_PROPERTY([CS_HAVE_GLHANDLEARB_T])], [], [],
	[$cs_cv_libgl_cflags], [$cs_cv_libgl_lflags], [$cs_cv_libgl_libs])

    # Check for GLU.
    CS_CHECK_BUILD([for GLU], [cs_cv_libglu],
	[AC_LANG_PROGRAM(
	    [CS_GL_INCLUDE([CS_GLU_PATH],[GL],[glu.h])], [gluNewQuadric()])],
	[$cs_osx_glu \
	CS_CREATE_TUPLE() \
	$cs_win32_glu \
	CS_CREATE_TUPLE([],[],[-lGLU]) \
	CS_CREATE_TUPLE([],[],[-lglu]) \
	$cs_mesa_glu], [],
	[CS_EMIT_BUILD_RESULT([cs_cv_libglu], [GLU])], [], [],
	[$cs_cv_libgl_cflags], [$cs_cv_libgl_lflags], [$cs_cv_libgl_libs])

    # Check for GLUT.
    CS_CHECK_BUILD([for GLUT], [cs_cv_libglut],
	[AC_LANG_PROGRAM(
	    [CS_GL_INCLUDE([CS_GLUT_PATH],[GL],[glut.h])], [glutSwapBuffers])],
	[$cs_osx_glut \
	CS_CREATE_TUPLE() \
	$cs_win32_glut \
	CS_CREATE_TUPLE([],[],[-lGLUT]) \
	CS_CREATE_TUPLE([],[],[-lglut])], [],
	[CS_EMIT_BUILD_RESULT([cs_cv_libglut], [GLUT])], [], [],
	[$cs_cv_libgl_cflags], [$cs_cv_libgl_lflags], [$cs_cv_libgl_libs])

    # Check for GLX.
    CS_CHECK_BUILD([for GLX], [cs_cv_libglx],
	[AC_LANG_PROGRAM([[#include <GL/glx.h>]], [glXWaitGL()])],
	[CS_CREATE_TUPLE() \
	CS_CREATE_TUPLE([],[],[-lGLX]) \
	CS_CREATE_TUPLE([],[],[-lglx]) \
	$cs_mesa_glx], [],
	[CS_EMIT_BUILD_RESULT([cs_cv_libglx], [GLX])], [], [],
	[$cs_cv_libgl_cflags], [$cs_cv_libgl_lflags], [$cs_cv_libgl_libs])

    # Check for GLX extensions.  Define CSGL_EXT_STATIC_ASSERTION if _not_
    # present.
    AS_IF([test $cs_cv_libglx = yes],
	[CS_CHECK_BUILD([for GLX extensions], [cs_cv_libglx_extensions],
	    [AC_LANG_PROGRAM(
		[[#define GLX_GLXEXT_PROTOTYPES
		#include <GL/glx.h>]],
		[glXGetProcAddressARB(0)])],
	    [CS_CREATE_TUPLE(
		[$cs_cv_libglx_cflags],
		[$cs_cv_libglx_lflags],
		[$cs_cv_libglx_libs])],
	    [], [], [CS_HEADER_PROPERTY([CSGL_EXT_STATIC_ASSERTION])])])])

#------------------------------------------------------------------------------
# Check for Cg shader package.
#------------------------------------------------------------------------------
CS_CHECK_PROGS([CGC], [cgc])
CS_EMIT_BUILD_PROPERTY([CGC], [$CGC])

AC_DEFUN([CS_CHECK_LIBCG],
    [AC_REQUIRE([CS_CHECK_COMMON_LIBS])
    AC_REQUIRE([CS_CHECK_PTHREAD])
    CS_CHECK_LIB_WITH([Cg],
	[AC_LANG_PROGRAM(
	    [[#include <Cg/cg.h>]],
	    [CGcontext c = cgCreateContext()])],
	[], [], [], [],
	[$cs_cv_libm_cflags $cs_cv_sys_pthread_cflags], 
	[$cs_cv_libm_lflags $cs_cv_sys_pthread_lflags], 
	[$cs_cv_libm_libs   $cs_cv_sys_pthread_libs],
	[cg, CG])

    AS_IF([test $cs_cv_libCg = yes],
	[CS_CHECK_BUILD([if Cg is sufficiently recent], [cs_cv_libCg_recent],
	    [AC_LANG_PROGRAM(
	    	[[#include <Cg/cg.h>]], [cgGetEnumString(CG_PROFILE_VP40);])],
	    [], [], [CS_DEFINE_EMITTER([HAS_CG])], [], [],
	    [$cs_cv_libCg_cflags],
	    [$cs_cv_libCg_lflags],
	    [$cs_cv_libCg_libs])],
	[cs_cv_libCg_recent=no])

    AS_IF([test $cs_cv_libCg_recent = yes],
	[CS_CHECK_LIB_WITH([CgGL],
	    [AC_LANG_PROGRAM(
		[[#include <Cg/cg.h>
		#include <Cg/cgGL.h>]],
		[CGprogram p; cgGLBindProgram(p)])],
	    [], [],
	    [CS_EMIT_BUILD_RESULT([cs_cv_libCgGL], [CG])], [],
	    [$cs_cv_libCg_cflags $cs_cv_libgl_cflags $cs_cv_libglu_cflags],
	    [$cs_cv_libCg_lflags $cs_cv_libgl_lflags $cs_cv_libglu_lflags],
	    [$cs_cv_libCg_libs   $cs_cv_libgl_libs   $cs_cv_libglu_libs],
	    [cggl, cgGL, CGGL, Cg, cg, CG])])])


CS_CHECK_LIBCG


#-----------------------------------------------------------------------------
# Emit install paths and package information.
#-----------------------------------------------------------------------------
CS_OUTPUT_INSTALLDIRS
CS_EMIT_PACKAGEINFO

#----------------------------------------------------------------------------
# Emit generated files.
#----------------------------------------------------------------------------
CS_JAMCONFIG_OUTPUT([Jamconfig])
AC_CONFIG_FILES([Jamfile])
AC_OUTPUT

AC_MSG_NOTICE([

Please note that this project uses Jam as its build tool,
therefore you must type 'jam' to build the project.

Jam home: http://www.perforce.com/jam/jam.html
Jam source: ftp://ftp.perforce.com/jam/
])
