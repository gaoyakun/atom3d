#ifndef __ATOM3D_STUDIO_TTFVIEWER_H
#define __ATOM3D_STUDIO_TTFVIEWER_H

class TTFViewer
{
public:
	TTFViewer (void);
	~TTFViewer (void);

public:
	void showDialog (HWND hWnd, const char *fontFileName);

private:
	bool updateViewerDialog (HWND hDlg, const char *fontFileName);
	void updateFaceInfo (HWND hDlg, int faceIndex);
	static INT_PTR CALLBACK DlgProc (HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

private:
	ATOM_VECTOR<ATOM_FaceInfo> _FaceInfos;
};

#endif // __ATOM3D_STUDIO_TTFVIEWER_H
