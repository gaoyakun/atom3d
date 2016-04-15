#ifndef __ATOM3D_FONT_TTF_IMPL_H
#define __ATOM3D_FONT_TTF_IMPL_H

extern "C" {
#include <ft2build.h>
} // extern "C"

#include <ATOM_kernel.h>
#include "fontapi.h"
#include "ttf_handle.h"

struct ATOM_StringClipInfo;

bool init_font_system ();
bool font_system_initialized ();
void shutdown_font_system ();
bool install_font (const char *name, ATOM_File *fontfile);
bool install_font (const char *name, const char *filename);
int get_num_fonts ();
bool create_font (ATOM_FontInfo *info);
int find_face_index (const char *name);
int get_face_height (ATOM_FontInfo *info);
int get_face_width (ATOM_FontInfo *info);
int get_face_ascender (ATOM_FontInfo *info);
const char *get_name (int font);
void render_string (ATOM_FontInfo *info, const char *str, int len, int x, int y, ATOM_FontRenderCallback *callback);
bool get_string_bounds (ATOM_FontInfo *info, const char *str, int len, int *l, int *t, int *w, int *h);
void set_char_margin (int margin);
int get_char_margin (void);
void get_underline (ATOM_FontInfo *info, int *pos, int *thickness);
unsigned clip_string (ATOM_FontInfo *info, unsigned clip_size, const char *str, int *l, int *t, int *w, int *h);
bool clip_string2 (ATOM_FontInfo *info, unsigned clip_size, const char *str, ATOM_StringClipInfo *clipinfo);

void *begin_query_face_info (const char *fileName, ATOM_FaceInfo *info);
bool query_next_face_info (void *handle, ATOM_FaceInfo *info);
void end_query_face_info (void *handle);

#endif // __ATOM3D_FONT_TTF_IMPL_H
