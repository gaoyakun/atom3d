#include "StdAfx.h"
#include "dlgloadnpc.h"
#include "heightgen.h"

ATOM_BEGIN_EVENT_MAP(DlgLoadNPC, ATOM_EventTrigger)
	ATOM_EVENT_HANDLER(DlgLoadNPC, ATOM_WidgetCommandEvent, onCommand)
ATOM_END_EVENT_MAP

DlgLoadNPC::DlgLoadNPC (const char *sceneFileName, ATOM_Widget *parent, ATOM_Node *npcRoot)
{
	_npcRoot = npcRoot;
	_sceneFileName = sceneFileName;

	_dlgLoadNPC = (ATOM_Dialog*)parent->loadHierarchy ("/editor/ui/loadnpc.ui");
	if (_dlgLoadNPC)
	{
		_dlgLoadNPC->setEventTrigger (this);
		_dlgLoadNPC->show (ATOM_Widget::Hide);
		initControls (_dlgLoadNPC);
		m_edNPCConfig->setString ("/configure/npc.xml");
		m_edMapConfig->setString ("/map/mapinfo.xml");
		m_edModelConfig->setString ("/configure/model.xml");
		m_edMonsterConfig->setString ("/configure/monsterset.xml");
	}
}

DlgLoadNPC::~DlgLoadNPC (void)
{
	ATOM_DELETE(_dlgLoadNPC);
}

void DlgLoadNPC::showModal (HeightFieldBBoxTree *hf)
{
	if (_dlgLoadNPC)
	{
		_heightField = hf;
		_dlgLoadNPC->showModal ();
	}
}

int DlgLoadNPC::getCurrentSceneId (void) const
{
	ATOM_AutoFile f(m_edMapConfig->getString().c_str(), ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		return -1;
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
		return -1;
	}

	char buffer1[ATOM_VFS::max_filename_length];
	char buffer2[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName (_sceneFileName.c_str(), buffer1);
	
	for (ATOM_TiXmlElement *e = doc.RootElement()->FirstChildElement("element"); e; e = e->NextSiblingElement("element"))
	{
		const char *path = e->Attribute("path");
		ATOM_GetNativePathName (path, buffer2);
		if (!strnicmp (buffer1, buffer2, strlen(buffer2)))
		{
			int id = -1;
			e->QueryIntAttribute ("id", &id);
			return id;
		}
	}

	return -1;
}

void DlgLoadNPC::onCommand (ATOM_WidgetCommandEvent *event)
{
	switch (event->id)
	{
	case ID_BTNOK:
		{
			int sceneId = getCurrentSceneId ();
			if (sceneId < 0)
			{
				return;
			}
			load (m_edNPCConfig->getString().c_str(), sceneId);
			break;
		}
	case ID_BTNCANCEL:
		{
			break;
		}
	default:
		return;
	}

	ATOM_Dialog *dlg = (ATOM_Dialog*)getHost();
	dlg->endModal (0);
}

bool DlgLoadNPC::getMonsterInfo (int monsterId, int &modelId, float &scale) const
{
	ATOM_AutoFile f(m_edMonsterConfig->getString().c_str(), ATOM_VFS::read|ATOM_VFS::text);
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

	for (ATOM_TiXmlElement *e = doc.RootElement()->FirstChildElement("element"); e; e = e->NextSiblingElement("element"))
	{
		int id = -1;
		e->QueryIntAttribute ("id", &id);
		if (id == monsterId)
		{
			modelId = -1;
			e->QueryIntAttribute ("model_id", &modelId);

			scale = 1.f;
			e->QueryFloatAttribute ("model_scale", &scale);

			return true;
		}
	}

	return false;
}

bool DlgLoadNPC::getModelFileName (int modelId, ATOM_STRING &s) const
{
	ATOM_AutoFile f(m_edModelConfig->getString().c_str(), ATOM_VFS::read|ATOM_VFS::text);
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

	for (ATOM_TiXmlElement *e = doc.RootElement()->FirstChildElement("elem"); e; e = e->NextSiblingElement("elem"))
	{
		int id = -1;
		e->QueryIntAttribute ("id", &id);
		if (id == modelId)
		{
			const char *fn = e->Attribute("filename");
			if (!fn)
			{
				return false;
			}
			s = fn;
			return true;
		}
	}

	return false;
}

static ATOM_STRING genPartName (ATOM_Actor *actor)
{
	int id = 1;
	char buffer[256];

	for (;;)
	{
		sprintf (buffer, "component%d", id++);
		if (!actor->getPartDefine().getPartByName(buffer))
		{
			return buffer;
		}
	}
}

static ATOM_AUTOREF(ATOM_Node) createCPNode (const char *filename)
{
	ATOM_AutoFile fCP(filename, ATOM_VFS::read|ATOM_VFS::text);
	if (!fCP)
	{
		return nullptr;
	}
	unsigned size = fCP->size ();
	char *p = ATOM_NEW_ARRAY(char, size);
	size = fCP->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument document;
	document.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (document.Error())
	{
		return nullptr;
	}

	ATOM_TiXmlElement *root = document.RootElement ();
	if (!root)
	{
		return nullptr;
	}

	const char *coreFileName = root->Attribute ("CoreFile");
	if (!coreFileName)
	{
		return nullptr;
	}

	ATOM_AutoFile fCCP(coreFileName, ATOM_VFS::read|ATOM_VFS::text);
	if (!fCCP)
	{
		return nullptr;
	}
	size = fCCP->size ();
	p = ATOM_NEW_ARRAY(char, size);
	size = fCCP->read (p, size);
	p[size] = '\0';
	ATOM_TiXmlDocument documentCCP;
	documentCCP.Parse (p);
	ATOM_DELETE_ARRAY(p);
	if (documentCCP.Error())
	{
		return nullptr;
	}

	ATOM_TiXmlElement *rootCCP = documentCCP.RootElement ();
	if (!rootCCP)
	{
		return nullptr;
	}

	ATOM_HARDREF(ATOM_Actor) actor;
	actor->load (ATOM_GetRenderDevice());

	const char *skeletonFileName = rootCCP->Attribute ("skeleton");
	if (skeletonFileName)
	{
		ATOM_ActorComponentsPart *part = actor->getPartDefine ().newComponentsPart ("skeleton");
		part->addCandidate (1, skeletonFileName);
		part->setCurrentCandidate (1);
	}

	for (ATOM_TiXmlElement *eAction = rootCCP->FirstChildElement("action"); eAction; eAction = eAction->NextSiblingElement("action"))
	{
		const char *filename = eAction->Attribute ("file");
		if (filename)
		{
			const char *p1 = strrchr (filename, '/');
			const char *p2 = strrchr (filename, '.');
			if (p1 && p2)
			{
				ATOM_STRING s(p1+1, p2-p1-1);
				ATOM_ActorComponentsPart *part = actor->getPartDefine ().newComponentsPart (s.c_str());
				part->addCandidate (1, filename);
				part->setCurrentCandidate (1);
			}
		}
	}

	for (ATOM_TiXmlElement *eComponent = rootCCP->FirstChildElement("component"); eComponent; eComponent = eComponent->NextSiblingElement("component"))
	{
		const char *name = eComponent->Attribute ("name");
		ATOM_STRING s;
		if (!name || actor->getPartDefine().getPartByName (name))
		{
			s = genPartName (actor.get());
			name = s.c_str();
		}
		ATOM_ActorComponentsPart *part = actor->getPartDefine ().newComponentsPart (name);

		for (ATOM_TiXmlElement *eMesh = eComponent->FirstChildElement("mesh"); eMesh; eMesh = eMesh->NextSiblingElement("mesh"))
		{
			int id = -1;
			eMesh->QueryIntAttribute ("id", &id);
			if (id < 0)
			{
				return false;
			}
			if (part->getCandidateIndexById (id) >= 0)
			{
				return false;
			}

			const char *filename = eMesh->Attribute ("file");
			if (filename)
			{
				part->addCandidate (id, filename);
			}
		}

		if (part->getNumCandidates() > 0)
		{
			part->setCurrentCandidate (part->getCandidateId (0));
		}
	}

	return actor;
}

ATOM_Node *DlgLoadNPC::loadModel (const char *filename, ATOM_Node *parent) const
{
	ATOM_AUTOREF(ATOM_Node) node;
	const char *ext = strrchr(filename, '.');
	if (ext && !stricmp(ext, ".cp"))
	{
		node = createCPNode(filename);
	}
	else
	{
		node = ATOM_Node::loadNodeFromFile (filename);
	}

	if (node)
	{
		parent->appendChild (node.get());
	}
	return node.get();
}

static void generateNPCName (int npcId, char *buffer)
{
	sprintf (buffer, "$NPC%d", npcId);
}

ATOM_Node *DlgLoadNPC::findNPC (int npcId)
{
	char buffer[256];
	generateNPCName (npcId, buffer);

	for (unsigned i = 0; i < _npcRoot->getNumChildren(); ++i)
	{
		ATOM_Node *node = _npcRoot->getChild(i);
		if (node && node->getDescribe() == buffer)
		{
			return node;
		}
	}
	return 0;
}

bool DlgLoadNPC::save (void)
{
	ATOM_TiXmlDocument doc;

	{
		ATOM_AutoFile f(_npcConfigFileName.c_str(), ATOM_VFS::read|ATOM_VFS::text);
		if (!f)
		{
			return false;
		}
		unsigned size = f->size ();
		char *p = ATOM_NEW_ARRAY(char, size);
		size = f->read (p, size);
		p[size] = '\0';
		doc.Parse (p);
		ATOM_DELETE_ARRAY(p);
		if (doc.Error())
		{
			return false;
		}
	}

	int mapId = getCurrentSceneId ();
	if (mapId < 0)
	{
		return false;
	}

	for (ATOM_TiXmlElement *e = doc.RootElement()->FirstChildElement("element"); e; e = e->NextSiblingElement("element"))
	{
		int map_id = -1;
		e->QueryIntAttribute ("map_id", &map_id);
		if (map_id != mapId)
		{
			continue;
		}

		int id = -1;
		e->QueryIntAttribute ("id", &id);

		ATOM_Node *nodeNPC = findNPC (id);
		if (nodeNPC)
		{
			const ATOM_Matrix4x4f &m = nodeNPC->getO2T ();
			e->SetAttribute("pos_x", int(m.m30));
			e->SetAttribute("pos_y", int(m.m32));

			ATOM_Matrix4x4f rot;
			ATOM_Vector3f t, s;
			m.decompose (t, rot, s);
			ATOM_Quaternion q;
			q.fromMatrix (rot);

			char buffer[256];
			sprintf (buffer, "%f,%f,%f,%f", q.x, q.y, q.z, q.w);
			e->SetAttribute ("rotate", buffer);
		}
	}

	char buffer[ATOM_VFS::max_filename_length];
	ATOM_GetNativePathName(_npcConfigFileName.c_str(), buffer);
	return doc.SaveFile (buffer);
}

void DlgLoadNPC::load (const char *npcConfigFileName, int sceneId)
{
	ATOM_AutoFile f(npcConfigFileName, ATOM_VFS::read|ATOM_VFS::text);
	if (!f)
	{
		return;
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
		return;
	}

	for (ATOM_TiXmlElement *e = doc.RootElement()->FirstChildElement("element"); e; e = e->NextSiblingElement("element"))
	{
		int mapId = -1;
		e->QueryIntAttribute ("map_id", &mapId);

		int id = -1;
		e->QueryIntAttribute ("id", &id);

		if (mapId == sceneId)
		{
			float x = 0.f, y = 0.f;
			e->QueryFloatAttribute ("pos_x", &x);
			e->QueryFloatAttribute ("pos_y", &y);

			ATOM_Quaternion rot;
			const char *rotate = e->Attribute("rotate");
			if (!rotate || 4 != sscanf(rotate, "%f,%f,%f,%f", &rot.x, &rot.y, &rot.z, &rot.w))
			{
				rot.identity ();
			}

			int monsterId = -1;
			e->QueryIntAttribute ("monsterid", &monsterId);
			
			int modelId;
			float scale;
			ATOM_STRING modelFileName;
			if (getMonsterInfo (monsterId, modelId, scale) && getModelFileName(modelId, modelFileName))
			{
				ATOM_Node *npcNode = loadModel (modelFileName.c_str(), _npcRoot.get());
				if (npcNode)
				{
					char buffer[256];
					generateNPCName (id, buffer);
					npcNode->setDescribe (buffer);

					ATOM_Ray ray(ATOM_Vector3f(x, 5000.f, y), ATOM_Vector3f(0.f, -1.f, 0.f));
					ATOM_Vector3f pos (x, 0.f, y);
					float d;
					if (_heightField->pick (ray, d))
					{
						pos = ray.getOrigin() + ray.getDirection() * d;
					}

					ATOM_Matrix4x4f rotMatrix;
					rot.toMatrix (rotMatrix);

					ATOM_Vector3f s(scale);

					ATOM_Matrix4x4f m = rotMatrix;
					m.m30 = pos.x;
					m.m31 = pos.y;
					m.m32 = pos.z;
					m >>= ATOM_Matrix4x4f::getScaleMatrix(ATOM_Vector3f(scale, scale, scale));

					npcNode->setO2T (m);
				}
			}
		}
	}
	_npcConfigFileName = npcConfigFileName;
}
