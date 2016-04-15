#include "propedit.h"
#include "resource.h"
#include "selectprop.h"

#include <commctrl.h>

extern "C" HINSTANCE hModelHandle;
extern BOOL chooseDir (HWND hWndParent, const char *title, char *initialDir);

#define STR_STR 0
#define STR_INT 1
#define STR_FLT 2

struct StringEditInfo
{
	std::string *result;
	int mode;
};

bool validateStrOfInt (char *s)
{
	if (!s || !s[0])
	{
		return false;
	}

	char *start = s;
	while (*start == ' ' || *start == '\t')
	{
		start++;
	}

	char *end = s + strlen(s) - 1;
	while (end > start && (*end == ' ' || *end == '\t'))
	{
		--end;
	}

	end[1] = '\0';
	if (start == end)
	{
		return false;
	}

	bool hasNeg = false;

	for (const char *ch = start; ch <= end; ++ch)
	{
		if (*ch == '-')
		{
			if (ch != start || hasNeg)
			{
				return false;
			}
			else
			{
				hasNeg = true;
			}
		}
		else if (*ch < '0' || *ch > '9')
		{
			return false;
		}

		*s++ = *ch;
	}

	*s = '\0';

	return true;
}

bool validateStrOfFloat (char *s)
{
	if (!s || !s[0])
	{
		return false;
	}

	char *t = s;

	char *start = s;
	while (*start == ' ' || *start == '\t')
	{
		start++;
	}

	char *end = s + strlen(s) - 1;
	while (end > start && (*end == ' ' || *end == '\t'))
	{
		--end;
	}

	end[1] = '\0';
	if (start == end)
	{
		return false;
	}

	bool hasNeg = false;
	bool hasDot = false;
	bool hasDigit = false;

	for (const char *ch = start; ch <= end; ++ch)
	{
		if (*ch == '-')
		{
			if (hasNeg || hasDigit)
			{
				return false;
			}
			else
			{
				hasNeg = true;
			}
		}
		else if (*ch == '.')
		{
			if (hasDot)
			{
				return false;
			}
			hasDot = true;
		}
		else if (*ch < '0' || *ch > '9')
		{
			return false;
		}
		else
		{
			hasDigit = true;
		}

		*s++ = *ch;
	}

	if (!hasDigit)
	{
		return false;
	}

	*s = '\0';

	float f = atof (t);
	sprintf (t, "%.3f", f);

	return true;
}

INT_PTR CALLBACK StrEditDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	static std::string *resultStr = 0;
	static int mode = STR_STR;

	switch(message) 
	{
	case WM_INITDIALOG:
		{
			StringEditInfo *sei = (StringEditInfo*)lParam;
			resultStr = sei->result;
			mode = sei->mode;
			::SetWindowText (::GetDlgItem (hWnd, IDC_STRINGEDIT), resultStr->c_str());
			::SetFocus (::GetDlgItem (hWnd, IDC_STRINGEDIT));
			::SendMessage (::GetDlgItem (hWnd, IDC_STRINGEDIT), EM_SETSEL, (WPARAM)0, (LPARAM)-1);
			return FALSE;
		}
	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDOK:
				{
					int n = ::GetWindowTextLengthA (::GetDlgItem (hWnd, IDC_STRINGEDIT));
					std::vector<char> buffer(n+10);
					::GetWindowTextA (::GetDlgItem (hWnd, IDC_STRINGEDIT), &buffer[0], buffer.size() + 1);
					char *s = &buffer[0];

					switch (mode)
					{
					case STR_INT:
						if (!validateStrOfInt (s))
						{
							::MessageBox (hWnd, "整数输入不合法!", "编辑属性", MB_OK|MB_ICONHAND);
							return FALSE;
						}
						break;
					case STR_FLT:
						if (!validateStrOfFloat (s))
						{
							::MessageBox (hWnd, "浮点数输入不合法!", "编辑属性", MB_OK|MB_ICONHAND);
							return FALSE;
						}
						break;
					}
					*resultStr = s;
					::EndDialog (hWnd, IDOK);
					return FALSE;
				}
			case IDCANCEL:
				{
					::EndDialog (hWnd, IDCANCEL);
					return FALSE;
				}
			default:
				{
					break;
				}
			}
			break;
		}
	default:
		{
			break;
		}
	}
	return FALSE;
}

INT_PTR CALLBACK PropEditDlgProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) 
{
	static PropertyEditor *editor = NULL;
	static PropertyEditor *copy = NULL;
	static int itemIndex = -1;
	static bool modified = false;

	switch(message) 
	{
	case WM_INITDIALOG:
		{
			editor = (PropertyEditor*)lParam;
			copy = new PropertyEditor(*editor);
			modified = false;

			HWND hWndListView = ::GetDlgItem (hWnd, IDC_PROPLIST);
			LVCOLUMN lvc;
			lvc.mask = LVCF_TEXT|LVCF_WIDTH|LVCF_FMT|LVCF_SUBITEM;
			lvc.fmt = LVCFMT_LEFT;
			lvc.cx = 100;
			lvc.pszText = "属性";
			lvc.cchTextMax = 20;
			lvc.iSubItem = 0;
			ListView_InsertColumn (hWndListView, 0, &lvc);
			lvc.cx = 200;
			lvc.pszText = "值";
			lvc.iSubItem = 1;
			ListView_InsertColumn (hWndListView, 1, &lvc);

			copy->syncUI (hWnd);

			return TRUE;
		}

	case WM_NOTIFY:
		{
			LPNMHDR pnmh = (LPNMHDR)lParam;
			if (pnmh->hwndFrom == ::GetDlgItem(hWnd, IDC_PROPLIST))
			{
				switch (pnmh->code)
				{
				case LVN_ENDLABELEDIT:
					{
						NMLVDISPINFO *pdi = (NMLVDISPINFO*)lParam;
						int item = pdi->item.iItem;
						if (pdi->item.pszText)
						{
							copy->setPropertyName (item, pdi->item.pszText);
							ListView_SetItemText (::GetDlgItem (hWnd, IDC_PROPLIST), item, 0, pdi->item.pszText);
							modified = true;
							return TRUE;
						}
						return FALSE;
					}
				case NM_RCLICK:
					{
						DWORD pos = ::GetMessagePos ();
						POINT point;
						point.x = LOWORD(pos);
						point.y = HIWORD(pos);
						::ScreenToClient (::GetDlgItem(hWnd, IDC_PROPLIST), &point);
						LVHITTESTINFO hti;
						hti.pt = point;
						itemIndex = ListView_HitTest (::GetDlgItem(hWnd, IDC_PROPLIST), &hti); 
						if (itemIndex >= 0)
						{
							ListView_SetItemState (::GetDlgItem(hWnd, IDC_PROPLIST), itemIndex, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						}
						copy->trackMenu (hWnd, LOWORD(pos), HIWORD(pos), itemIndex >= 0);
						break;
					}
				default:
					break;
				}
			}
			break;
		}
	case WM_COMMAND:
		{
			if (HIWORD(wParam) != 0)
			{
				break;
			}
			switch (LOWORD(wParam))
			{
			case ID_COLORPROP:
				{
					std::string result;

					if (copy->editColorValue (hWnd, 0xFFFFFFFF, result))
					{
						const char *key = "颜色";
						copy->newProperty (key, result.c_str());
						copy->syncUI (hWnd);

						int item = copy->getNumProperties()-1;
						ListView_SetItemState (::GetDlgItem(hWnd, IDC_PROPLIST), item, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						ListView_EditLabel (::GetDlgItem (hWnd, IDC_PROPLIST), item);
						modified = true;
					}
					break;
				}
			case ID_FILENAMEPROP:
				{
					std::string result;

					if (copy->editFileNameValue (hWnd, result))
					{
						copy->newProperty ("文件", result.c_str());
						copy->syncUI (hWnd);

						int item = copy->getNumProperties()-1;
						ListView_SetItemState (::GetDlgItem(hWnd, IDC_PROPLIST), item, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						ListView_EditLabel (::GetDlgItem (hWnd, IDC_PROPLIST), item);
						modified = true;
					}
					break;
				}
			case ID_FOLDERPROP:
				{
					std::string result;

					if (copy->editFolderValue (hWnd, result))
					{
						copy->newProperty ("文件夹", result.c_str());
						copy->syncUI (hWnd);

						int item = copy->getNumProperties()-1;
						ListView_SetItemState (::GetDlgItem(hWnd, IDC_PROPLIST), item, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						ListView_EditLabel (::GetDlgItem (hWnd, IDC_PROPLIST), item);
						modified = true;
					}
					break;
				}
			case ID_BOOLPROP_YES:
				{
					copy->newProperty ("布尔", "True");
					copy->syncUI (hWnd);
					int item = copy->getNumProperties()-1;
					ListView_SetItemState (::GetDlgItem(hWnd, IDC_PROPLIST), item, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
					ListView_EditLabel (::GetDlgItem (hWnd, IDC_PROPLIST), item);
					modified = true;
					break;
				}
			case ID_BOOLPROP_NO:
				{
					copy->newProperty ("布尔", "False");
					copy->syncUI (hWnd);
					int item = copy->getNumProperties()-1;
					ListView_SetItemState (::GetDlgItem(hWnd, IDC_PROPLIST), item, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
					ListView_EditLabel (::GetDlgItem (hWnd, IDC_PROPLIST), item);
					modified = true;
					break;
				}
			case ID_INTPROP:
				{
					std::string result;

					if (copy->editIntValue (hWnd, result))
					{
						copy->newProperty ("属性", result.c_str());
						copy->syncUI (hWnd);

						int item = copy->getNumProperties()-1;
						ListView_SetItemState (::GetDlgItem(hWnd, IDC_PROPLIST), item, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						ListView_EditLabel (::GetDlgItem (hWnd, IDC_PROPLIST), item);
						modified = true;
					}
					break;
				}
			case ID_FLOATPROP:
				{
					std::string result;

					if (copy->editFloatValue (hWnd, result))
					{
						copy->newProperty ("属性", result.c_str());
						copy->syncUI (hWnd);

						int item = copy->getNumProperties()-1;
						ListView_SetItemState (::GetDlgItem(hWnd, IDC_PROPLIST), item, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						ListView_EditLabel (::GetDlgItem (hWnd, IDC_PROPLIST), item);
						modified = true;
					}
					break;
				}
			case ID_LIGHTN_START:
				{
					copy->newProperty ("LightnStart", "0");
					copy->syncUI (hWnd);
					modified = true;
					break;
				}
			case ID_LIGHTN_END:
				{
					copy->newProperty ("LightnEnd", "0");
					copy->syncUI (hWnd);
					modified = true;
					break;
				}
			case ID_STRINGPROP:
				{
					std::string result;

					if (copy->editStringValue (hWnd, result))
					{
						copy->newProperty ("属性", result.c_str());
						copy->syncUI (hWnd);

						int item = copy->getNumProperties()-1;
						ListView_SetItemState (::GetDlgItem(hWnd, IDC_PROPLIST), item, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
						ListView_EditLabel (::GetDlgItem (hWnd, IDC_PROPLIST), item);
						modified = true;
					}
					break;
				}
			case ID_BLENDMODE_ADD:
			case ID_BLENDMODE_PARTICLE:
			case ID_BLENDMODE_MODULATE:
			case ID_BLENDMODE_MODULATEB:
			case ID_BLENDMODE_IMODULATE:
			case ID_BLENDMODE_IMODULATEB:
			case ID_BLENDMODE_INORMAL:
				{
					const char *value;

					switch (LOWORD(wParam))
					{
					case ID_BLENDMODE_ADD:
						value = "Add";
						break;
					case ID_BLENDMODE_PARTICLE:
						value = "AlphaAdd";
						break;
					case ID_BLENDMODE_MODULATE:
						value = "Modulate";
						break;
					case ID_BLENDMODE_MODULATEB:
						value = "ModulateB";
						break;
					case ID_BLENDMODE_IMODULATE:
						value = "InvModulate";
						break;
					case ID_BLENDMODE_IMODULATEB:
						value = "InvModulateB";
						break;
					case ID_BLENDMODE_INORMAL:
						value = "InvNormal";
						break;
					}

					copy->newProperty ("BlendMode", value);
					copy->syncUI (hWnd);
					modified = true;

					break;
				}
			case ID_MATERIALSTYLE_METAL:
			case ID_MATERIALSTYLE_PLASTIC:
			case ID_MATERIALSTYLE_SKIN:
				{
					const char *value;
					switch (LOWORD(wParam))
					{
					case ID_MATERIALSTYLE_METAL:
						value = "Metal";
						break;
					case ID_MATERIALSTYLE_PLASTIC:
						value = "Plastic";
						break;
					case ID_MATERIALSTYLE_SKIN:
						value = "Skin";
						break;
					}

					copy->newProperty ("MaterialStyle", value);
					copy->syncUI (hWnd);
					modified = true;

					break;
				}
			case ID_NOLIGHTING:
				{
					copy->newProperty ("LightingMode", "None");
					copy->syncUI (hWnd);
					modified = true;

					break;
				}
			case ID_NOMIPMAP:
				{
					copy->newProperty ("MipmapMode", "None");
					copy->syncUI (hWnd);
					modified = true;
					break;
				}
			case ID_BILINEARFILTER:
				{
					copy->newProperty ("MipmapMode", "Bilinear");
					copy->syncUI (hWnd);
					modified = true;
					break;
				}
			case ID_TRILINEARFILTER:
				{
					copy->newProperty ("MipmapMode", "Trilinear");
					copy->syncUI (hWnd);
					modified = true;
					break;
				}
			case ID_M_COLORPROP:
				{
					std::string value = copy->getPropertyValue (itemIndex);
					ATOM_ColorARGB color = copy->str2color (value.c_str());
					if (copy->editColorValue (hWnd, color, value))
					{
						copy->setPropertyValue (itemIndex, value.c_str());
						copy->syncUI (hWnd);
						modified = true;
					}
					break;
				}
			case ID_M_FILENAMEPROP:
				{
					std::string value = copy->getPropertyValue (itemIndex);
					if (copy->editFileNameValue (hWnd, value))
					{
						copy->setPropertyValue (itemIndex, value.c_str());
						copy->syncUI (hWnd);
						modified = true;
					}
					break;
				}
			case ID_M_FOLDERPROP:
				{
					std::string value = copy->getPropertyValue (itemIndex);
					if (copy->editFolderValue (hWnd, value))
					{
						copy->setPropertyValue (itemIndex, value.c_str());
						copy->syncUI (hWnd);
						modified = true;
					}
					break;
				}
			case ID_M_BOOLPROP_YES:
				{
					copy->setPropertyValue (itemIndex, "True");
					copy->syncUI (hWnd);
					modified = true;
					break;
				}
			case ID_M_BOOLPROP_NO:
				{
					copy->setPropertyValue (itemIndex, "False");
					copy->syncUI (hWnd);
					modified = true;
					break;
				}
			case ID_M_INTPROP:
				{
					std::string value = copy->getPropertyValue (itemIndex);
					if (copy->editIntValue (hWnd, value))
					{
						copy->setPropertyValue (itemIndex, value.c_str());
						copy->syncUI (hWnd);
						modified = true;
					}
					break;
				}
			case ID_M_FLOATPROP:
				{
					std::string value = copy->getPropertyValue (itemIndex);
					if (copy->editFloatValue (hWnd, value))
					{
						copy->setPropertyValue (itemIndex, value.c_str());
						copy->syncUI (hWnd);
						modified = true;
					}
					break;
				}
			case ID_M_LIGHTN_START:
				{
					copy->setPropertyName (itemIndex, "LightnStart");
					copy->syncUI (hWnd);
					modified = true;
					break;
				}
			case ID_M_LIGHTN_END:
				{
					copy->setPropertyName (itemIndex, "LightnEnd");
					copy->syncUI (hWnd);
					modified = true;
					break;
				}
			case ID_M_STRINGPROP:
				{
					std::string value = copy->getPropertyValue (itemIndex);
					if (copy->editStringValue (hWnd, value))
					{
						copy->setPropertyValue (itemIndex, value.c_str());
						copy->syncUI (hWnd);
						modified = true;
					}
					break;
				}
			case ID_M_BLENDMODE_ADD:
			case ID_M_BLENDMODE_PARTICLE:
			case ID_M_BLENDMODE_MODULATE:
			case ID_M_BLENDMODE_MODULATEB:
			case ID_M_BLENDMODE_IMODULATE:
			case ID_M_BLENDMODE_IMODULATEB:
			case ID_M_BLENDMODE_INORMAL:
				{
					const char *value;

					switch (LOWORD(wParam))
					{
					case ID_M_BLENDMODE_ADD:
						value = "Add";
						break;
					case ID_M_BLENDMODE_PARTICLE:
						value = "AlphaAdd";
						break;
					case ID_M_BLENDMODE_MODULATE:
						value = "Modulate";
						break;
					case ID_M_BLENDMODE_MODULATEB:
						value = "ModulateB";
						break;
					case ID_M_BLENDMODE_IMODULATE:
						value = "InvModulate";
						break;
					case ID_M_BLENDMODE_IMODULATEB:
						value = "InvModulateB";
						break;
					case ID_M_BLENDMODE_INORMAL:
						value = "InvNormal";
						break;
					}

					copy->setPropertyValue (itemIndex, value);
					copy->syncUI (hWnd);
					modified = true;

					break;
				}
			case ID_M_MATERIALSTYLE_METAL:
			case ID_M_MATERIALSTYLE_PLASTIC:
			case ID_M_MATERIALSTYLE_SKIN:
				{
					const char *value;
					switch (LOWORD(wParam))
					{
					case ID_M_MATERIALSTYLE_METAL:
						value = "Metal";
						break;
					case ID_M_MATERIALSTYLE_PLASTIC:
						value = "Plastic";
						break;
					case ID_M_MATERIALSTYLE_SKIN:
						value = "Skin";
						break;
					}

					copy->setPropertyValue(itemIndex, value);
					copy->syncUI (hWnd);
					modified = true;

					break;
				}
			case ID_M_NOLIGHTING:
				{
					copy->setPropertyName (itemIndex, "LightingMode");
					copy->setPropertyValue (itemIndex, "None");
					copy->syncUI (hWnd);
					modified = true;

					break;
				}
			case ID_M_NOMIPMAP:
				{
					copy->setPropertyName (itemIndex, "MipmapMode");
					copy->setPropertyValue (itemIndex, "None");
					copy->syncUI (hWnd);
					modified = true;

					break;
				}
			case ID_M_BILINEARFILTER:
				{
					copy->setPropertyName (itemIndex, "MipmapMode");
					copy->setPropertyValue (itemIndex, "Bilinear");
					copy->syncUI (hWnd);
					modified = true;

					break;
				}
			case ID_M_TRILINEARFILTER:
				{
					copy->setPropertyName (itemIndex, "MipmapMode");
					copy->setPropertyValue (itemIndex, "Trilinear");
					copy->syncUI (hWnd);
					modified = true;

					break;
				}
			case ID_USE_EMISSIVE:
				{
					copy->newProperty ("Emissive", "True");
					copy->syncUI (hWnd);
					modified = true;

					break;
				}
			case ID_NO_EMISSIVE:
				{
					copy->newProperty ("Emissive", "False");
					copy->syncUI (hWnd);
					modified = true;

					break;
				}
			case ID_PROP_DELETE:
				{
					copy->removeProperty (itemIndex);
					copy->syncUI (hWnd);
					modified = true;
					break;
				}
			case ID_SELECTPROP:
				{
					SelectProp sp;
					const char *prop = sp.select (hWnd);
					if (prop)
					{
						copy->setPropertyName (itemIndex, prop);
						copy->syncUI (hWnd);
						modified = true;
					}
					break;
				}
			case IDOK:
				{
					if (!copy->validateProperties ())
					{
						::MessageBox (hWnd, "属性名称重复!", "编辑属性", MB_OK|MB_ICONHAND);
						break;
					}
					*editor = *copy;
					::EndDialog (hWnd, IDOK);
					break;
				}
			case IDCANCEL:
				{
					delete copy;
					copy = 0;
					::EndDialog (hWnd, IDCANCEL);
					break;
				}
			default:
				{
					break;
				}
			}
			break;
		}
	case WM_CLOSE:
		if (modified)
		{
			int r = ::MessageBox (hWnd, "要保存修改吗?", "编辑属性", MB_YESNOCANCEL);
			if (r == IDCANCEL)
			{
				return TRUE;
			}
			if (r == IDYES)
			{
				if (!copy->validateProperties ())
				{
					::MessageBox (hWnd, "属性名称重复!", "编辑属性", MB_OK|MB_ICONHAND);
					break;
				}
				*editor = *copy;
				delete copy;
				copy = 0;
				::EndDialog (hWnd, IDOK);
			}
			else
			{
				delete copy;
				copy = 0;
				::EndDialog (hWnd, IDCANCEL);
			}
		}
		else
		{
			delete copy;
			copy = 0;
			::EndDialog (hWnd, IDCANCEL);
		}
		break;
	}
	return FALSE;
}

PropertyEditor::PropertyEditor (void)
{
}

PropertyEditor::~PropertyEditor (void)
{
}

unsigned PropertyEditor::getNumProperties (void) const
{
	return _properties.size ();
}

const char *PropertyEditor::getPropertyName (unsigned index) const
{
	return _properties[index].first.c_str();
}

const char *PropertyEditor::getPropertyValue (unsigned index) const
{
	return _properties[index].second.c_str();
}

int PropertyEditor::getPropertyValueAsInt (unsigned index) const
{
	return str2int (_properties[index].second.c_str());
}

float PropertyEditor::getPropertyValueAsFloat (unsigned index) const
{
	return str2float (_properties[index].second.c_str());
}

ATOM_ColorARGB PropertyEditor::getPropertyValueAsColor (unsigned index) const
{
	return str2color (_properties[index].second.c_str());
}

void PropertyEditor::setPropertyName (unsigned index, const char *name)
{
	_properties[index].first = name ? name : "";
}

void PropertyEditor::setPropertyValue (unsigned index, const char *value)
{
	_properties[index].second = value ? value : "";
}

void PropertyEditor::setPropertyValueAsInt (unsigned index, int value)
{
	_properties[index].second = int2str (value);
}

void PropertyEditor::setPropertyValueAsFloat (unsigned index, float value)
{
	_properties[index].second = float2str (value);
}

void PropertyEditor::setPropertyValueAsColor (unsigned index, ATOM_ColorARGB color)
{
	_properties[index].second = color2str (color);
}

void PropertyEditor::newProperty (const char *name, const char *value)
{
	_properties.resize (_properties.size()+1);
	setPropertyName (_properties.size()-1, name ? name : "");
	setPropertyValue (_properties.size()-1, value ? value : "");
}

void PropertyEditor::removeProperty (unsigned index)
{
	_properties.erase (_properties.begin() + index);
}

bool PropertyEditor::validateProperties (void) const
{
	if (_properties.size() > 0)
	{
		for (unsigned i = 0; i < _properties.size()-1; ++i)
		{
			for (unsigned j = i + 1; j < _properties.size(); ++j)
			{
				if (!_stricmp (_properties[j].first.c_str(), _properties[i].first.c_str()))
				{
					return false;
				}
			}
		}
	}
	return true;
}

bool PropertyEditor::edit (HWND parentWnd)
{
	return IDOK == ::DialogBoxParam(hModelHandle, MAKEINTRESOURCE(IDD_PROPEDIT), parentWnd, PropEditDlgProc, (LPARAM)this);
}

std::string PropertyEditor::float2str (float value)
{
	char buffer[256];
	sprintf (buffer, "%.3f", value);
	return buffer;
}

std::string PropertyEditor::int2str (int value)
{
	char buffer[256];
	sprintf (buffer, "%d", value);
	return buffer;
}

std::string PropertyEditor::color2str (ATOM_ColorARGB value)
{
	char buffer[256];
	sprintf (buffer, "%.3f, %.3f, %.3f, %.3f", value.getFloatR(), value.getFloatG(), value.getFloatB(), value.getFloatA());
	return buffer;
}

float PropertyEditor::str2float (const char *str) const
{
	return atof (str);
}

int PropertyEditor::str2int (const char *str) const
{
	return atoi (str);
}

ATOM_ColorARGB PropertyEditor::str2color (const char *str) const
{
	float r = 1.f, g = 1.f, b = 1.f, a = 1.f;
	sscanf (str, "%f, %f, %f, %f", &r, &g, &b, &a);
	return ATOM_ColorARGB(r, g, b, a);
}

void PropertyEditor::syncUI (HWND hWnd)
{
	HWND hWndListView = ::GetDlgItem (hWnd, IDC_PROPLIST);
	assert (hWndListView);

	ListView_DeleteAllItems (hWndListView);

	for (unsigned i = 0; i < _properties.size(); ++i)
	{
		LV_ITEM item;
		item.mask = LVIF_TEXT;
		item.iItem = i;
		item.iSubItem = 0;
		item.pszText = (LPSTR)_properties[i].first.c_str();
		item.cchTextMax = _properties[i].first.length() * 2 + 2;
		ListView_InsertItem (hWndListView, &item);

		item.iSubItem = 1;
		item.pszText = (LPSTR)_properties[i].second.c_str();
		item.cchTextMax = _properties[i].second.length() * 2 + 2;
		ListView_SetItem (hWndListView, &item);
	}
}

void PropertyEditor::trackMenu (HWND hWnd, int x, int y, bool selected)
{
	HMENU hMenu = ::LoadMenu (hModelHandle, MAKEINTRESOURCE(IDR_PROPMENU));
	HMENU hSubMenu = ::GetSubMenu (hMenu, 0);
	if (hSubMenu)
	{
		::EnableMenuItem (hSubMenu, ID_M_INTPROP, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_FLOATPROP, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_COLORPROP, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_FILENAMEPROP, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_FOLDERPROP, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_STRINGPROP, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_BOOLPROP_YES, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_BOOLPROP_NO, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_BLENDMODE_NORMAL, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_BLENDMODE_ADD, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_BLENDMODE_PARTICLE, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_BLENDMODE_MODULATE, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_BLENDMODE_MODULATEB, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_BLENDMODE_IMODULATE, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_BLENDMODE_IMODULATEB, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_BLENDMODE_INORMAL, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_MATERIALSTYLE_METAL, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_MATERIALSTYLE_PLASTIC, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_MATERIALSTYLE_SKIN, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_NOLIGHTING, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_LIGHTN_START, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_LIGHTN_END, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_NOMIPMAP, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_BILINEARFILTER, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_M_TRILINEARFILTER, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_PROP_DELETE, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::EnableMenuItem (hSubMenu, ID_SELECTPROP, selected ? MF_ENABLED : MF_DISABLED|MF_GRAYED);
		::TrackPopupMenu (hSubMenu, TPM_LEFTBUTTON, x, y, 0, hWnd, 0);
	}
	::DestroyMenu (hMenu);

}

bool PropertyEditor::editColorValue (HWND hWnd, ATOM_ColorARGB initValue, std::string &result)
{
	static COLORREF custColors[16] = {
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255),
		RGB(255, 255, 255)
	};

	CHOOSECOLOR cc;
	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hwndOwner = hWnd;
	cc.hInstance = NULL;
	cc.rgbResult = RGB(initValue.getByteR(), initValue.getByteG(), initValue.getByteB());
	cc.lpCustColors = custColors;
	cc.Flags = CC_FULLOPEN|CC_RGBINIT;
	cc.lCustData = NULL;
	cc.lpfnHook = NULL;
	cc.lpTemplateName = NULL;
	if (::ChooseColor (&cc))
	{
		ATOM_ColorARGB color(
			float(GetRValue(cc.rgbResult))/255.f,
			float(GetGValue(cc.rgbResult))/255.f,
			float(GetBValue(cc.rgbResult))/255.f,
			1.f);

		result = color2str(color);
		return true;
	}
	return false;
}

bool PropertyEditor::editFileNameValue (HWND hWnd, std::string &result)
{
	OPENFILENAME ofn;
	char fileNameBuffer[MAX_PATH];

	strcpy (fileNameBuffer, result.c_str());

	memset (&ofn, 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = fileNameBuffer;
	ofn.nMaxFile = sizeof(fileNameBuffer);

	char filterBuffer[MAX_PATH];
	strcpy (filterBuffer, "*.*");
	char *s2 = filterBuffer+strlen(filterBuffer)+1;
	strcpy (s2, filterBuffer);
	*(s2 + strlen(s2) + 1) = '\0';

	ofn.lpstrFilter = filterBuffer;
	ofn.Flags = 0;

	if (::GetOpenFileName (&ofn))
	{
		result = fileNameBuffer;
		return true;
	}

	return false;
}

bool PropertyEditor::editFolderValue (HWND hWnd, std::string &result)
{
	char buffer[MAX_PATH];
	strcpy (buffer, result.c_str());

	if (chooseDir (hWnd, "选择文件夹", buffer))
	{
		result = buffer;
		return true;
	}
	return false;
}

bool PropertyEditor::editStringValue (HWND hWnd, std::string &result)
{
	StringEditInfo sei;
	sei.result = &result;
	sei.mode = STR_STR;

	if (IDOK == ::DialogBoxParam(hModelHandle, MAKEINTRESOURCE(IDD_EDITSTRING), hWnd, StrEditDlgProc, (LPARAM)&sei))
	{
		return true;
	}
	return false;
}

bool PropertyEditor::editIntValue (HWND hWnd, std::string &result)
{
	StringEditInfo sei;
	sei.result = &result;
	sei.mode = STR_INT;

	if (IDOK == ::DialogBoxParam(hModelHandle, MAKEINTRESOURCE(IDD_EDITSTRING), hWnd, StrEditDlgProc, (LPARAM)&sei))
	{
		return true;
	}
	return false;
}

bool PropertyEditor::editFloatValue (HWND hWnd, std::string &result)
{
	StringEditInfo sei;
	sei.result = &result;
	sei.mode = STR_FLT;

	if (IDOK == ::DialogBoxParam(hModelHandle, MAKEINTRESOURCE(IDD_EDITSTRING), hWnd, StrEditDlgProc, (LPARAM)&sei))
	{
		return true;
	}
	return false;
}

void PropertyEditor::clear (void)
{
	_properties.resize (0);
}
