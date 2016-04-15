#include "tester.h"

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.VC90.CRT' version='9.0.30729.1' processorArchitecture='X86' publicKeyToken='1fc8b3b9a1e18e3b' language='*'\"")

int main(int argc, const char ** argv)
{
  if (argc!=2)
  {
    printf ("Usage: webertest [xml-file]");
    return 1;
  }

  FILE* file = fopen(argv[1], "rb");
  if (!file)
  {
    printf("File does not exist, program exiting\n");
    return 2;
  }

  Tester test;
  test.Run(argv[1]);
  return 0;
};
