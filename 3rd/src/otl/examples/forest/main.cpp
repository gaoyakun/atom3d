#include <renderer/sdlframework.h>

#include "forestgamelet.h"

//#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.VC90.CRT' version='9.0.30729.1 SP' processorArchitecture='X86' publicKeyToken='1fc8b3b9a1e18e3b' language='*'\"")

int OTL_main (int argc, char ** argv)
{
  return SDL_FrameWork(new ForestGamelet()).run();
}


