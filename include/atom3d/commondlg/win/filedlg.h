//This utility can only be used in M$ Windows platform
//To use this utility, just include this header"l3_filedlg.h",
//and link lib"l3_filedlgd.lib" for debug mode, lib"l3_filedlg.lib" for retail mode,


#ifndef ATOM_UTILITY_FILEDLG_H_
#define ATOM_UTILITY_FILEDLG_H_

#if _MSC_VER > 1000
# pragma once
#endif

#undef _WIN32_IE
#define _WIN32_IE	0x0400

#include <list>
#include <vector>
#include <map>
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>

#include "../../ATOM_kernel.h"
#include "../basedefs.h"


class ATOM_COMMONDLG_API ATOM_FileDlg
{
public:
	enum MODE_
	{
		MODE_SAVEFILE = 0x0001,
		MODE_MULTISELECT = 0x0002,
		MODE_OVERWRITEPROMPT = 0x0004,
		MODE_OPENDIR = 0x0008,
	};
public:
	ATOM_FileDlg(unsigned dwMode = 0,
				const char* szInitPath = NULL, const char* szDefaultExt = NULL, const char*szDefaultFileName= NULL,
				const char* szFilter = NULL, HWND hParent = NULL);
	~ATOM_FileDlg(void);

	void setTitle (const char *title);
	int doModal();
	int getNumFilesSelected (void) const;
	const char *getSelectedFileName (int index) const;
	const char *getSelectedFileDesc (void) const;

private:
	void listFile(bool showFile);
	BOOL getShellIcon(int nIndex, HICON& hSmall, HICON& hLarge);
private:
	ATOM_STRING				m_szTitle;
	ATOM_STRING				m_szSelectedPath;
	ATOM_VECTOR<ATOM_STRING>	m_ltSelectedFileName;
	unsigned				m_dwMode;
	ATOM_STRING				m_szInitPath;
	ATOM_STRING				m_szDefaultExt;
	ATOM_STRING				m_szDefaultFileName;

	typedef std::pair<ATOM_STRING, ATOM_VECTOR<ATOM_STRING> > _StringVectorPair;
	typedef ATOM_VECTOR<_StringVectorPair> FileFilter;
	FileFilter				m_Filter;
	void createFilter(const char* szFilter);
	int						m_nCurrFilter;

	HWND					m_hParent;
	HWND					m_hWnd;

	HWND					m_hLCFile;
	HWND					m_hCBRange;
	HWND					m_hEdtFileName;
	HWND					m_hCBFileType;
	HWND					m_hBtnParentDir;
	HWND					m_hBtnListStyle;
	HWND					m_hBtnOK;

	HIMAGELIST				m_hImageListType;	//Large
	HIMAGELIST				m_hImageListSmall;	//ImageListSmall
	HIMAGELIST				m_hImageListLarge;	//ImageListLarge
	int						m_nImageFileDefault;
	int						m_nImageFolderClose;
	int						m_nImageFolderOpen;
	ATOM_MAP<int,int>		m_mpImageFile;		//Index of the icon image within the system image list
												//-> Index of icon in m_hImageListSmall/m_hImageListLarge.
	int lookupImage(int nSysImageIndex);
	void insertImage(int nSysImageIndex, int nMyImageIndex);


	char*					m_szBuffer;
	int						m_nBufferLen;
	char*					m_szDesc;
	int						m_nDescBufferLen;

private:
	static INT_PTR CALLBACK l3FileDlgProc(HWND hwnd,UINT message,WPARAM wParam,LPARAM lParam);
	static BOOL OnInitDialog(HWND hWnd, ATOM_FileDlg* lpDlg);
	BOOL OnButtonListViewFileStyle();
	BOOL OnButtonNewDir();
	BOOL OnButtonParentDir();
	BOOL OnCBRangeSelChange();
	BOOL OnCBFileTypeSelChange();
	BOOL OnLBFileSelChange(LPNMLISTVIEW lpNMListView);
	BOOL OnLBFileDBClick(void *lpNMItemActive);
	BOOL OnLBFileFilenameChange(NMLVDISPINFO* lpInfo);
	BOOL OnOK();
};

#endif // ATOM_UTILITY_FILEDLG_H_
