#ifndef __ATOM_maxexporter_scene_h__
#define __ATOM_maxexporter_scene_h__

struct SceneEntry
{
	char *name;
	INode *node;
	INode *tnode;
	Object *object;
	int type;
	int id;
	SceneEntry *next;
	SceneEntry (INode *n, Object *o, int t);
	void setID (int id);
};

class SceneEnumProc : public ITreeEnumProc
{
public:
	Interface *i;
	SceneEntry *head;
	SceneEntry *tail;
	IScene *theScene;
	int count;
	TimeValue time;

	SceneEnumProc (IScene *scene, TimeValue t, Interface *i);
	~SceneEnumProc (void);

	int getCount (void) const;
	void append (INode *node, Object *obj, int type);
	int callback (INode *node);
	Box3 bound (void);
	SceneEntry *find (INode *node);
	SceneEntry *operator [] (int index);
};

#endif // __ATOM_maxexporter_scene_h__
