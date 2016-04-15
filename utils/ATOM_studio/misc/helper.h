#ifndef __ATOM3D_STUDIO_HELPER_H
#define __ATOM3D_STUDIO_HELPER_H

ATOM_STRING findEarthSculptorPluginDir (void);
bool IsProjectDirectory (const char *path);
bool MakeProjectDirectory (const char *path, bool forceCreate);
void GetDocumentForOpen (int argc, char **argv, ATOM_STRING &project, ATOM_STRING &file);
bool ChooseDir (HWND hWndParent, const char *title, char *initialDir);

class Config
{
private:
	Config (void);
	~Config (void);

public:
	static Config *getInstance (void);

public:
	unsigned getVersion (void) const;
	void setLastProject (const char *proj);
	const char *getLastProject (void) const;
	unsigned getNumRecentProjects (void) const;
	const char *getRecentProject (unsigned index) const;
	void clearRecentProjects (void);
	void addRecentProject (const char *proj);
	const char *getUpdateServer (void) const;
	bool load (void);
	void save (void) const;

private:
	ATOM_STRING _updateServer;
	ATOM_STRING _lastProject;
	ATOM_STRING _configFileName;
	unsigned _version;
	ATOM_VECTOR<ATOM_STRING> _recentProjects;
};

#endif // __ATOM3D_STUDIO_HELPER_H
