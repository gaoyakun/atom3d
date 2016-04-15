#include <string>
#include <vector>
#include <cassert>
#include <tinyxml.h>

struct GeodeInfo
{
	std::string modelFileName;
	std::string transform;
};

std::vector<GeodeInfo> geodeRecord;
std::vector<TiXmlElement*> removeList;

void usage (void)
{
	printf ("Usage: ATOM_dupremove <3sg filename>\n");
}

void doNodes (TiXmlElement *nodeElement)
{
	TiXmlElement *p = 0;

	for (p = nodeElement->FirstChildElement ("node"); p; p = p->NextSiblingElement ("node"))
	{
		const char *attrib = p->Attribute ("class");
		if (!attrib)
		{
			continue;
		}

		if (!strcmp (attrib, "ATOM_OctreeNode"))
		{
			doNodes (p);
		}
		else if (!strcmp (attrib, "ATOM_Geode"))
		{
			const char *modelFileName = p->Attribute ("ModelFileName");
			const char *transform = p->Attribute ("matrix");
			if (modelFileName && transform)
			{
				bool exists = false;
				for (unsigned i = 0; i < geodeRecord.size(); ++i)
				{
					if (geodeRecord[i].modelFileName == modelFileName && geodeRecord[i].transform == transform)
					{
						exists = true;
						removeList.push_back (p);
						break;
					}
				}
				if (!exists)
				{
					geodeRecord.resize (geodeRecord.size() + 1);
					geodeRecord.back().modelFileName = modelFileName;
					geodeRecord.back().transform = transform;
				}
			}
		}
	}
}

int main (int argc, char *argv [])
{
	if (argc != 2)
	{
		usage ();
		return 1;
	}

	const char *filename = argv[1];

	TiXmlDocument doc(filename);
	if (!doc.LoadFile ())
	{
		printf ("ERR: The file is not a valid XML file.\n");
		return 1;
	}

	TiXmlElement *eRoot = doc.RootElement ();

	if (!eRoot)
	{
		return 0;
	}

	for (TiXmlElement *eNodes = eRoot->FirstChildElement ("Nodes"); eNodes; eNodes = eNodes->NextSiblingElement ("Nodes"))
	{
		const char *attrib = eNodes->Attribute ("Layer");
		if (attrib && !strcmp (attrib, "FWImp"))
		{
			doNodes (eNodes);
			break;
		}
	}

	if (removeList.size() > 0)
	{
		for (unsigned i = 0; i < removeList.size(); ++i)
		{
			TiXmlNode *parent = removeList[i]->Parent();
			assert (parent);
			parent->RemoveChild (removeList[i]);
		}

		if (!doc.SaveFile (filename))
		{
			printf ("ERR: couldn't write file\n");
			return 1;
		}

		printf ("%d duplicate entries removed\n", removeList.size());
	}

	return 0;
}
