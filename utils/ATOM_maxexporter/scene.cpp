#include "StdAfx.h"
#include "scene.h"

SceneEntry::SceneEntry (INode *n, Object *o, int t)
{
	node = n;
	object = o;
	type = t;
}

SceneEnumProc::SceneEnumProc (IScene *scene, TimeValue t, Interface *i)
{
	time = t;
	theScene = scene;
	count = 0;
	head = 0;
	tail = 0;
	this->i = i;
	theScene->EnumTree (this);
}

SceneEnumProc::~SceneEnumProc (void)
{
	while (head)
	{
		SceneEntry *next = head->next;
		delete head;
		head = next;
	}

	head = 0;
	tail = 0;
	count = 0;
}

int SceneEnumProc::callback (INode *node)
{
	if (node->Selected () && !node->IsTarget ())
	{
		Object *obj = node->EvalWorldState (time).obj;

		if (obj && obj->ClassID() != Class_ID(TARGET_CLASS_ID, 0) && obj->CanConvertToType (Class_ID(TRIOBJ_CLASS_ID, 0)))
		{
			append (node, obj, 0);
		}
	}

	return TREE_CONTINUE;
}

void SceneEnumProc::append (INode *node, Object *obj, int type)
{
	SceneEntry *entry = new SceneEntry (node, obj, type);
	entry->next = 0;
	if (tail) tail->next = entry;
	tail = entry;
	if (!head) head = entry;
	++count;
}

Box3 SceneEnumProc::bound (void)
{
	Box3 bound;
	bound.Init ();
	SceneEntry *e = head;
	ViewExp *vpt = i->GetViewport (0);
	while (e)
	{
		Box3 bb;
		e->object->GetWorldBoundBox (time, e->node, vpt, bb);
		bound += bb;
		e = e->next;
	}
	return bound;
}

SceneEntry *SceneEnumProc::find (INode *node)
{
	SceneEntry *e = head;
	while (e)
	{
		if (e->node == node) return e;
		e = e->next;
	}
	return 0;
}

SceneEntry *SceneEnumProc::operator [] (int index)
{
	if (index >= count)
	{
		return 0;
	}

	SceneEntry *ptr = head;
	while (index)
	{
		ptr = ptr->next;
		--index;
	}

	return ptr;
}

int SceneEnumProc::getCount (void) const
{
	return count;
}
