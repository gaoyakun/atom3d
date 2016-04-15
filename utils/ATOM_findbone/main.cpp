#include <ATOM.h>

class FindBoneApp: public ATOM_Application
{
public:
	void onAppInit (ATOM_AppInitEvent *event);
	void onAppExit (ATOM_AppExitEvent *event);
	void usage (void);

	ATOM_DECLARE_EVENT_MAP(FindBoneApp, ATOM_Application)
};

ATOM_BEGIN_EVENT_MAP(FindBoneApp, ATOM_Application)
  ATOM_EVENT_HANDLER(FindBoneApp, ATOM_AppInitEvent, onAppInit)
  ATOM_EVENT_HANDLER(FindBoneApp, ATOM_AppExitEvent, onAppExit)
ATOM_END_EVENT_MAP

struct CRC32
{
	unsigned crc_table[256];

	CRC32 (void)
	{
		unsigned poly = 0xEDB88320;
		for (unsigned i = 0; i < 256; ++i)
		{
			unsigned crc = i;
			for (unsigned j = 8; j > 0; --j)
			{
				if (crc & 1)
				{
					crc = (crc >> 1) ^ poly;
				}
				else
				{
					crc >>= 1;
				}
			}
			crc_table[i] = crc;
		}
	}

	unsigned generate (const char *block, unsigned len) const
	{
		register unsigned crc = 0xFFFFFFFF;

		for (unsigned i = 0; i < len; ++i)
		{
			crc = ((crc >> 8) & 0x00FFFFFF) ^ crc_table[(crc ^ *block++) & 0xFF];
		}

		return crc ^ 0xFFFFFFFF;
	}
};

void FindBoneApp::usage (void)
{
	printf ("Usage: findbone -f <SkeletonFileName> -i <BoneId>\n");
}

void FindBoneApp::onAppInit (ATOM_AppInitEvent *event)
{
	callParentHandler (event);
	if (!event->success)
	{
		return;
	}
	ATOM_APP->postQuitEvent (0);

	ATOM_LoadPlugin ("ATOM_engine");
	ATOM_LoadPlugin ("ATOM_render");
	ATOM_LoadPlugin ("ATOM_image");

	ATOM_STRING inputFileName;
	unsigned boneId = 0;

	int argc = event->argc;
	char **argv = event->argv;

	for (int i = 0; i < argc; ++i)
	{
		if (!strcmp (argv[i], "-f"))
		{
			if (i == argc - 1 || !inputFileName.empty())
			{
				usage ();
				return;
			}
			inputFileName = argv[++i];
		}
		if (!strcmp (argv[i], "-i"))
		{
			if (i == argc - 1 || boneId != 0)
			{
				usage ();
				return;
			}
			boneId = _atoi64(argv[++i]);
		}
	}

	if (inputFileName.empty())
	{
		usage ();
		return;
	}

	if (boneId == 0)
	{
		usage ();
		return;
	}

	ATOM_Components components(inputFileName.c_str());
	ATOM_Skeleton *skeleton = components.getSkeleton ();

	if (skeleton)
	{
		const ATOM_HASHMAP<ATOM_STRING, unsigned> &attachPoints = skeleton->getAttachPoints ();
		for (ATOM_HASHMAP<ATOM_STRING, unsigned>::const_iterator it = attachPoints.begin(); it != attachPoints.end(); ++it)
		{
			CRC32 crc32;
			unsigned hash = crc32.generate (it->first.c_str(), it->first.length());
			if (hash == boneId)
			{
				printf ("Found bone: <%s>\n", it->first.c_str());
				return;
			}
		}
	}

	printf ("Not found.\n");

	event->success = true;
}

void FindBoneApp::onAppExit (ATOM_AppExitEvent *event)
{
    ATOM_DoneGfx ();
}

int main (int argc, char *argv [])
{
	FindBoneApp app;
	return app.run ();
}
