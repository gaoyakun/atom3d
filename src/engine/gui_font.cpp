#include "stdafx.h"
#include <shlobj.h>

#include "gui_font.h"

const ATOM_GUIFont::handle ATOM_GUIFont::invalid_handle = 0;
const char *ATOM_GUIFont::default_font_name = "_GUIDefault";

struct DefaultFont
{
	ATOM_FontHandle hFont;
	ATOM_HASHMAP<int, ATOM_GUIFont::handle> handles;
};

struct DefaultFontWrapper
{
	ATOM_HASHMAP<int, DefaultFont> defaultFonts;

	~DefaultFontWrapper (void)
	{
		ATOM_HASHMAP<int, DefaultFont>::iterator it;
		for (it = defaultFonts.begin(); it != defaultFonts.end(); ++it)
		{
			ATOM_ReleaseFont (it->second.hFont);
			for (ATOM_HASHMAP<int, ATOM_GUIFont::handle>::iterator itHandle = it->second.handles.begin(); itHandle != it->second.handles.end(); ++itHandle)
			{
				ATOM_GUIFont::releaseFont (itHandle->second);
			}
		}
	}
};

ATOM_GUIFont::handle ATOM_GUIFont::getDefaultFont (int size, int charmargin)
{
	ATOM_STACK_TRACE(ATOM_GUIFont::getDefaultFont);

	static DefaultFontWrapper defaultFonts;
	static bool defaultFontLoaded = false;
	static bool defaultFontLoadSucceeded = false;

	ATOM_HASHMAP<int, DefaultFont>::iterator it = defaultFonts.defaultFonts.find (size);
	if (it != defaultFonts.defaultFonts.end ())
	{
		ATOM_HASHMAP<int, ATOM_GUIFont::handle>::iterator itHandle = it->second.handles.find (charmargin);
		if (itHandle == it->second.handles.end ())
		{
			_handle *font = ATOM_NEW(ATOM_GUIFont::_handle);
			font->handle = it->second.hFont;
			font->charset = ATOM_CC_CP936;
			font->charmargin = charmargin;
			it->second.handles[charmargin] = font;
			return font;
		}
		return itHandle->second;
	}

	if (!defaultFontLoaded)
	{
		char sysFontPath[MAX_PATH];
		if (SUCCEEDED(::SHGetFolderPathA (NULL, CSIDL_FONTS, NULL, 0, sysFontPath)))
		{
			if (sysFontPath[strlen(sysFontPath)-1] != '\\')
			{
				strcat (sysFontPath, "\\");
			}
			strcat (sysFontPath, "simsun.ttc");
			ATOM_LOGGER::information ("Loading font <%s>..\n", sysFontPath);
			defaultFontLoadSucceeded = ATOM_LoadFont (ATOM_GUIFont::default_font_name, sysFontPath);
		}
		else
		{
			ATOM_LOGGER::error ("Get system font directory failed!\n");
			return ATOM_GUIFont::invalid_handle;
		}
		defaultFontLoaded = true;
	}

	if (!defaultFontLoadSucceeded)
	{
		ATOM_LOGGER::error ("Load system font <simsun.ttc> failed!\n");
		return ATOM_GUIFont::invalid_handle;
	}

	defaultFonts.defaultFonts[size].hFont = ATOM_CreateFont (ATOM_GUIFont::default_font_name, size, ATOM_CC_CP936);

	return getDefaultFont (size, charmargin);
}

ATOM_GUIFont::handle ATOM_GUIFont::createFont (const char *name, int size, int charset, int charmargin)
{
  ATOM_STACK_TRACE(ATOM_GUIFont::createFont);

  ATOM_FontHandle hFont = ATOM_CreateFont (name, size, charset);
  
  if (hFont)
  {
    _handle *font = ATOM_NEW(ATOM_GUIFont::_handle);
    font->handle = hFont;
    font->charset = charset;
    font->charmargin = charmargin;
    return font;
  }

  return ATOM_GUIFont::getDefaultFont (size, charmargin);
}

void ATOM_GUIFont::releaseFont (ATOM_GUIFont::handle font)
{
  ATOM_STACK_TRACE(ATOM_GUIFont::releaseFont);

  if (font != ATOM_GUIFont::invalid_handle)
  {
    _handle *p = (_handle*)font;
    ATOM_ReleaseFont (p->handle);
    ATOM_DELETE(font);
  }
}

ATOM_FontHandle ATOM_GUIFont::getFontHandle (ATOM_GUIFont::handle font)
{
  ATOM_STACK_TRACE(ATOM_GUIFont::getFontHandle);

  if (font != ATOM_GUIFont::invalid_handle)
  {
    _handle *p = (_handle*)font;
    return p->handle;
  }

  return 0;
}

int ATOM_GUIFont::getFontCharSet (ATOM_GUIFont::handle font)
{
  ATOM_STACK_TRACE(ATOM_GUIFont::getFontCharSet);

  if (font != ATOM_GUIFont::invalid_handle)
  {
    _handle *p = (_handle*)font;
    return p->charset;
  }

  return -1;
}

int ATOM_GUIFont::getFontCharMargin (ATOM_GUIFont::handle font)
{
  ATOM_STACK_TRACE(ATOM_GUIFont::getFontCharMargin);

  if (font != ATOM_GUIFont::invalid_handle)
  {
    _handle *p = (_handle*)font;
    return p->charmargin;
  }
  return 0;
}

ATOM_GUIFont::handle ATOM_GUIFont::ensureValidFont (ATOM_GUIFont::handle font, int size, int charmargin)
{
	return (font == ATOM_GUIFont::invalid_handle) ? getDefaultFont(size, charmargin) : font;

}

typedef ATOM_HASHMAP<ATOM_STRING, ATOM_GUIFont::GUIFontInfo> FontInfoMap;
static ATOM_SET<ATOM_STRING> _installedFonts;
static FontInfoMap _fontInfoMap;
static ATOM_SET<ATOM_STRING> _loadedConfigurations;

ATOM_GUIFont::handle createFont (const char *fontFilename, int size, int charset, int margin)
{
	char completedPath[ATOM_VFS::max_filename_length];
	ATOM_CompletePath (fontFilename, completedPath);
	strlwr (completedPath);
	bool fontValid = true;

	if (_installedFonts.find (completedPath) == _installedFonts.end ())
	{
		if (ATOM_LoadFont (completedPath, completedPath))
		{
			_installedFonts.insert (completedPath);
		}
		else
		{
			fontValid = false;
		}
	}
	ATOM_GUIFont::handle guiFont = fontValid ? ATOM_GUIFont::createFont (completedPath, size,  charset, margin) : ATOM_GUIFont::getDefaultFont(size, margin);
	return guiFont;
}

static bool loadFontSet (const char *filename)
{
	ATOM_AutoFile f(filename, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		return false;
	}
	unsigned size = f->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = f->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument doc;
	doc.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (doc.Error())
	{
		return false;
	}

	ATOM_TiXmlElement *root = doc.RootElement ();
	if (!root)
	{
		return false;
	}

	for (ATOM_TiXmlElement *font = root->FirstChildElement ("font"); font; font = font->NextSiblingElement("font"))
	{
		const char *name = font->Attribute ("name");
		if (!name)
		{
			continue;
		}

		const char *file = font->Attribute ("file");
		if (!file)
		{
			continue;
		}

		int size = 0;
		font->QueryIntAttribute ("size", &size);

		int charset = ATOM_CC_CP936;
		font->QueryIntAttribute ("set", &charset);

		int margin = 0;
		font->QueryIntAttribute ("margin", &margin);

		ATOM_GUIFont::GUIFontInfo &fi = _fontInfoMap[name];
		fi.handle = createFont (file, size, charset, margin);
		fi.name = name;
		fi.filename = file;
		fi.size = size;
		fi.charset = charset;
		fi.margin = margin;
	}

	return true;
}

static bool saveFontSet (const char *filename)
{
	char nativeFileName[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (filename, nativeFileName);
	ATOM_TiXmlDocument doc (nativeFileName);

	ATOM_TiXmlDeclaration eDecl("1.0", "gb2312", "");
	doc.InsertEndChild (eDecl);

	ATOM_TiXmlElement root("root");

	for (FontInfoMap::const_iterator it = _fontInfoMap.begin(); it != _fontInfoMap.end(); ++it)
	{
		ATOM_TiXmlElement font("font");
		font.SetAttribute ("name", it->second.name.c_str());
		font.SetAttribute ("file", it->second.filename.c_str());
		font.SetAttribute ("size", it->second.size);
		font.SetAttribute ("set", it->second.charset);
		font.SetAttribute ("margin", it->second.margin);
		root.InsertEndChild (font);
	}

	doc.InsertEndChild (root);
	return doc.SaveFile ();
}

void ATOM_GUIFont::loadFontConfig (const char *configFileName)
{
	char completedPath[ATOM_VFS::max_filename_length];
	ATOM_CompletePath (configFileName, completedPath);
	strlwr (completedPath);

	if (_loadedConfigurations.find(completedPath) == _loadedConfigurations.end ())
	{
		loadFontSet (completedPath);
		_loadedConfigurations.insert (completedPath);
	}
}

ATOM_GUIFont::handle ATOM_GUIFont::lookUpFont (const char *fontName)
{
	if (fontName)
	{
		FontInfoMap::const_iterator it = _fontInfoMap.find(fontName);
		return it == _fontInfoMap.end() ? ATOM_GUIFont::invalid_handle : it->second.handle;
	}
	return ATOM_GUIFont::invalid_handle;
}

const ATOM_HASHMAP<ATOM_STRING, ATOM_GUIFont::GUIFontInfo> &ATOM_GUIFont::getFontInfoMap(void)
{
	return _fontInfoMap;
}

