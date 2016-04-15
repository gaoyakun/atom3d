#include "main.h"
#include "dcgen.h"
#include "lmgen.h"
#include "misc.h"
#include "dcvisitor.h"

ATOM_BEGIN_EVENT_MAP(DCGenApp, ATOM_Application)
  ATOM_EVENT_HANDLER(DCGenApp, ATOM_AppInitEvent, onAppInit)
  ATOM_EVENT_HANDLER(DCGenApp, ATOM_AppExitEvent, onAppExit)
ATOM_END_EVENT_MAP

void DCGenApp::usage (void)
{
	printf ("Usage: dcgen -i <InputSceneFileName> [-o <OutpuSceneFileName>] [-t <TerrainLightmapFileName>] [-d <MeshLightmapPath>]\n");
}

void DCGenApp::onAppInit (ATOM_AppInitEvent *event)
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

	ATOM_RenderWindowDesc desc;
	desc.width = 800;
	desc.height = 600;
	desc.naked = false;
	desc.resizable = false;
	desc.multisample = 0;
	desc.title = "t_terrain";
	desc.windowid = 0;
	view = ATOM_InitializeGfx (0, desc, false);
	if (!view)
	{
		ATOM_LOGGER::error ("Initialize GFX failed.\n");
		return;
	}

	ATOM_GetRenderDevice()->setClearColor (view.get(), 0, 0, 0, 0);

	ATOM_STRING inputFileName;
	ATOM_STRING outputFileName;
	ATOM_STRING terrainLightmapFileName;
	ATOM_STRING meshLightmapPath;

	int argc = event->argc;
	char **argv = event->argv;
	for (int i = 0; i < argc; ++i)
	{
		if (!strcmp (argv[i], "-i"))
		{
			if (i == argc - 1 || !inputFileName.empty())
			{
				usage ();
				return;
			}
			inputFileName = argv[++i];
		}
		if (!strcmp (argv[i], "-o"))
		{
			if (i == argc - 1 || !outputFileName.empty())
			{
				usage ();
				return;
			}
			outputFileName = argv[++i];
		}
		if (!strcmp (argv[i], "-t"))
		{
			if (i == argc - 1 || !terrainLightmapFileName.empty())
			{
				usage ();
				return;
			}
			terrainLightmapFileName = argv[++i];
		}
		if (!strcmp (argv[i], "-d"))
		{
			if (i == argc - 1 || !meshLightmapPath.empty())
			{
				usage ();
				return;
			}
			meshLightmapPath = argv[++i];
		}
	}

	if (inputFileName.empty())
	{
		usage ();
		return;
	}

	if (outputFileName.empty())
	{
		outputFileName = inputFileName;
	}

	ATOM_SDLScene scene;

	printf ("Loading scene <%s>...", inputFileName.c_str());
	if (!scene.load (inputFileName.c_str()))
	{
		printf ("Failed.\n");
		return;
	}
	printf ("OK\n");

	if (ATOM_GetRenderDevice()->beginFrame ())
	{
		SceneLightmapGen lightmapGen;
		lightmapGen.setScene (&scene);
		lightmapGen.setTerrainLightmapFileName (terrainLightmapFileName.c_str());
		lightmapGen.setTerrainLightmapSize (1024);
		lightmapGen.setMeshLightmapPath (meshLightmapPath.c_str());
		lightmapGen.setMeshLightmapSize (16);
		if (!lightmapGen.generate ())
		{
			printf ("Generation failed.\n");
		}
		else
		{
			printf ("Writing scene...");
			scene.save (outputFileName.c_str());
			printf ("OK\n");
		}

		ATOM_GetRenderDevice()->endFrame ();
	}

	event->success = true;
}

void DCGenApp::onAppExit (ATOM_AppExitEvent *event)
{
    ATOM_DoneGfx ();
}

int main (int argc, char *argv [])
{
	DCGenApp app;
	return app.run ();
}
