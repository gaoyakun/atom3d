#include "main.h"

class MyApp: public ATOM_Application
{
public:
	void usage (void)
	{
		printf ("Usage: dumpskeleton foo.nm\n");
	}

	void onInit(ATOM_AppInitEvent *event)
	{
		event->success = false;
		event->errorcode = 1;

		if (event->argc != 2)
		{
			usage ();
		}
		else
		{
			registerPluginsImage ();
			registerPluginsRender ();
			registerPluginsEngine ();

			ATOM_KernelConfig::initialize (0);

			ATOM_RenderWindowDesc desc;
			desc.width = 1;
			desc.height = 1;
			desc.naked = true;
			desc.fullscreen = false;
			desc.iconid = NULL;
			desc.resizable = false;
			desc.multisample = 0;
			desc.title = "";
			desc.windowid = 0;
			desc.parentWindowId = 0;

			if (!ATOM_InitializeGfx (desc, ATOM_RenderWindow::SHOWMODE_HIDE, false, NULL))
			{
				printf ("Init graphics failed\n");
				return;
			}

			ATOM_RenderDevice *device = ATOM_GetRenderDevice();
			if (!device)
			{
				printf ("Init graphics failed\n");
				ATOM_DoneGfx ();
				return;
			}

			ATOM_Components c(event->argv[1]);
			ATOM_Skeleton *skeleton = c.getSkeleton();
			if (!skeleton)
			{
				printf ("Load skeleton failed\n");
				ATOM_DoneGfx ();
				return;
			}

			skeleton->dump ();

			ATOM_DoneGfx ();
			event->errorcode = 0;
		}
	}

	ATOM_DECLARE_EVENT_MAP(MyApp, ATOM_Application)
};

ATOM_BEGIN_EVENT_MAP(MyApp, ATOM_Application)
	ATOM_EVENT_HANDLER(MyApp, ATOM_AppInitEvent, onInit)
ATOM_END_EVENT_MAP

int main (int argc, char *argv [])
{
	ATOM_DownloadContext *downloadContext = ATOM_NewDownload ("http://192.168.18.122/gamedata/pkg/first_packet.pkg");
	ATOM_StartDownloading (downloadContext, "F:\\DownloadTest.tmp");
	ATOM_WaitForDownloading (downloadContext, INFINITE);
	ATOM_DestroyDownload (downloadContext);

	return MyApp().run ();
}
