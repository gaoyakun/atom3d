#include <ATOM.h>
#include "fwconverter.h"

class FWConvApp: public ATOM_Application
{
public:
  ATOM_AUTOREF(ATOM_RenderWindow) view;

public:
  FWConvApp (void)
  {
  }

  void usage (void)
  {
	  printf ("ATOM_fwconv -i|--input <fwfile> -o|--output <scenefile> -tp|--terrainpath <VFSPath> [-aw|--alphawidth w] [-ah|--alphaheight h] [-as|--aomapsize size].\n");
  }

  static void callback (void *, FWConverter::ProcessStage stage, int percent)
  {
	  if (stage == FWConverter::PS_FINISHED)
	  {
		  printf ("\nFinished.\n");
	  }
	  else
	  {
		  const char *jobTitle;
		  static FWConverter::ProcessStage lastStage = FWConverter::PS_UNKNOWN;
		  if (stage != lastStage)
		  {
			  printf ("\n");
			  lastStage = stage;
		  }

		  switch (stage)
		  {
		  case FWConverter::PS_READING_FREEWORD_FILE:
			  {
				  jobTitle = "Reading FREEWORLD file";
				  break;
			  }
		  case FWConverter::PS_BUILDING_ALPHA_MAPS:
			  {
				  jobTitle = "Building alpha maps";
				  break;
			  }
		  case FWConverter::PS_BUILDING_GLOBAL_MAP:
			  {
				  jobTitle = "Building global map";
				  break;
			  }
		  case FWConverter::PS_COLLAPSING_ALPHA_MAPS:
			  {
				  jobTitle = "Collapsing alpha maps";
				  break;
			  }
		  case FWConverter::PS_BUILDING_GRASS:
			  {
				  jobTitle = "Building grass datas";
				  break;
			  }
		  case FWConverter::PS_WRITING_TERRAIN_FILE_1:
			  {
				  jobTitle = "Writing terrain file";
				  break;
			  }
		  case FWConverter::PS_WRITING_SCENE_FILE:
			  {
				  jobTitle = "Writing scene file";
				  break;
			  }
		  case FWConverter::PS_BUILDING_AO_MAP:
			  {
				  jobTitle = "Computing illumination";
				  break;
			  }
		  case FWConverter::PS_WRITING_TERRAIN_FILE_2:
			  {
				  jobTitle = "Writing terrain file";
				  break;
			  }
		  }

		  printf ("\r%s (%d%%)", jobTitle, percent);
	  }
  }

  void onAppInit (ATOM_AppInitEvent *event)
  {
	callParentHandler (event);
    if (!event->success)
    {
      return;
    }

	event->success = false;
	ATOM_STRING input;
	ATOM_STRING output;
	ATOM_STRING terrainPath;
	int w = 512, h = 512, aosize = 512;
	int argc = event->argc;
	char **argv = event->argv;

	for (int i = 0; i < argc; ++i)
	{
	  if (!strcmp (argv[i], "-i") || !strcmp (argv[i], "--input"))
	  {
		  if (i == argc - 1 || !input.empty())
		  {
			  usage ();
			  return;
		  }
		  input = argv[++i];
	  }
	  else if (!strcmp (argv[i], "-o") || !strcmp (argv[i], "--output"))
	  {
		  if (i == argc - 1 || !output.empty())
		  {
			  usage ();
			  return;
		  }
		  output = argv[++i];
	  }
	  else if (!strcmp (argv[i], "-tp") || !strcmp (argv[i], "--terrainpath"))
	  {
		  if (i == argc - 1 || !terrainPath.empty ())
		  {
			  usage ();
			  return;
		  }
		  terrainPath = argv[++i];
	  }
	  else if (!strcmp (argv[i], "-aw") || !strcmp (argv[i], "--alphawidth"))
	  {
		  if (i == argc - 1)
		  {
			  usage ();
			  return;
		  }
		  w = atoi (argv[++i]);
	  }
	  else if (!strcmp (argv[i], "-ah") || !strcmp (argv[i], "--alphaheight"))
	  {
		  if (i == argc - 1)
		  {
			  usage ();
			  return;
		  }
		  h = atoi (argv[++i]);
	  }
	  else if (!strcmp (argv[i], "-as") || !strcmp (argv[i], "--aomapsize"))
	  {
		  if (i == argc - 1)
		  {
			  usage ();
			  return;
		  }
		  aosize = atoi (argv[++i]);
	  }
	}

	if (input.empty() || output.empty ())
	{
		usage ();
		return;
	}

	if (w == 0 || h == 0 || !ATOM_ispo2(w) || !ATOM_ispo2(h))
	{
		printf ("Invalid alpha map size.\n");
		return;
	}

	if (aosize == 0 || !ATOM_ispo2(aosize))
	{
		printf ("Invalid ambient occlusion map size.\n");
		return;
	}

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
		return;
    }

	ATOM_GetRenderDevice()->setClearColor (view.get(), 0, 0, 0, 0);

	FWConverter converter (input.c_str(), output.c_str(), terrainPath.c_str(), w, h, aosize);
	converter.setProcessCallback (&callback, 0);
	converter.convert ();

	ATOM_APP->postQuitEvent (0);
	event->success = true;
  }

  void onAppExit (ATOM_AppExitEvent *event)
  {
    ATOM_DoneGfx();
  }

  ATOM_DECLARE_EVENT_MAP(FWConvApp, ATOM_Application)
};

ATOM_BEGIN_EVENT_MAP(FWConvApp, ATOM_Application)
  ATOM_EVENT_HANDLER(FWConvApp, ATOM_AppInitEvent, onAppInit)
  ATOM_EVENT_HANDLER(FWConvApp, ATOM_AppExitEvent, onAppExit)
ATOM_END_EVENT_MAP

static LONG WINAPI ExceptionFilter (struct _EXCEPTION_POINTERS *ExceptionInfo)
{
	ATOM_WriteMiniDump ("crash.dmp", 0, ExceptionInfo);
	printf ("发生程序异常，请发送crash.dmp给高雅昆.\n");

	return EXCEPTION_EXECUTE_HANDLER;
}

int main (int argc, char *argv [])
{
	::SetUnhandledExceptionFilter (&ExceptionFilter);

	FWConvApp app;
	return app.run ();
}
