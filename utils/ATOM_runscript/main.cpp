#include <ATOM_engine.h>
#include <ATOM_script.h>

static ATOM_ScriptManager scriptMngr;

static void output (const char *s, bool error)
{
	fprintf (stderr, s);
}

static void ATOM_CALL ScriptCallback (ATOM_Script *script)
{
	ATOM_BindToScript (script);
}

int main (int argc, char * argv[])
{
#if 0
#define BUFFER_SIZE 1024
#define BYTE_TO_MB (1.0/(1024.0*1024.0))
#define TEST_COUNT 10
#define TEST_LOOP 1024*1024

	unsigned *pDest[4];
	unsigned *pSource[4];
	unsigned *srcIndices;
	unsigned *destIndices;

	printf ("Initializing memcpy test...\n");
	srand (ATOM_GetTick ());
	for (unsigned i = 0; i < 4; ++i)
	{
		pDest[i] = (unsigned*)ATOM_NEW_ARRAY(unsigned, BUFFER_SIZE);
		pSource[i] = (unsigned*)ATOM_NEW_ARRAY(unsigned, BUFFER_SIZE);
		for (unsigned j = 0; j < BUFFER_SIZE; ++j)
		{
			pSource[i][j] = rand ();
		}
	}
	srcIndices = ATOM_NEW_ARRAY(unsigned, TEST_LOOP);
	destIndices = ATOM_NEW_ARRAY(unsigned, TEST_LOOP);
	for (unsigned i = 0; i < TEST_LOOP; ++i)
	{
		srcIndices[i] = rand() & 0x3;
		destIndices[i] = rand() & 0x3;
	}
	unsigned totalSize = BUFFER_SIZE*TEST_LOOP;

	printf ("Starting MMX memcpy test...\n");
	for (unsigned nTest = 0; nTest < TEST_COUNT; ++nTest)
	{
		printf ("* memcpy test case %d: ", nTest);

		unsigned tick0 = ATOM_GetTick();
		for (unsigned loop = 0; loop < TEST_LOOP; ++loop)
		{
			unsigned srcIndex = srcIndices[loop];
			unsigned destIndex = destIndices[loop];
			ATOM_memcpy (pDest[destIndex], pSource[srcIndex], sizeof(unsigned)*BUFFER_SIZE);
		}
		unsigned tick1 = ATOM_GetTick();
		float sec = (tick1-tick0)*0.001f;
		float MB = totalSize * BYTE_TO_MB / sec;

		printf ("%fsec %dMB/sec\n", (tick1-tick0)*0.001f, int(MB));
	}

	printf ("Starting memcpy test...\n");
	for (unsigned nTest = 0; nTest < TEST_COUNT; ++nTest)
	{
		printf ("* memcpy test case %d: ");

		unsigned tick0 = ATOM_GetTick();
		for (unsigned loop = 0; loop < TEST_LOOP; ++loop)
		{
			unsigned srcIndex = srcIndices[loop];
			unsigned destIndex = destIndices[loop];
			memcpy (pDest[destIndex], pSource[srcIndex], sizeof(unsigned)*BUFFER_SIZE);
		}
		unsigned tick1 = ATOM_GetTick();
		float sec = (tick1-tick0)*0.001f;
		float MB = totalSize * BYTE_TO_MB / sec;

		printf ("%fsec %dMB/sec\n", (tick1-tick0)*0.001f, int(MB));
	}

	printf ("Ending memcpy test...\n");
	for (unsigned i = 0; i < 4; ++i)
	{
		ATOM_DELETE_ARRAY(pDest[i]);
		ATOM_DELETE_ARRAY(pSource[i]);
	}
	ATOM_DELETE_ARRAY(srcIndices);
	ATOM_DELETE_ARRAY(destIndices);

	return 0;
#endif

	if (argc != 2 && argc != 3)
	{
		fprintf (stdout, "Usage: ATOM_runscript <filename> [entryfunc]\n");
		return 1;
	}

	const char *filename = argv[1];
	const char *entry = argc == 3 ? argv[2] : "main";

	ATOM_Script *script = scriptMngr.createScript (&ScriptCallback);
	if (!script)
	{
		return 1;
	}
	script->setErrorOutputCallback (&output);

	FILE *fp = fopen (filename, "rt");
	if (!fp)
	{
		fprintf (stderr, "File not found.");
		return 1;
	}

	fseek (fp, 0, SEEK_END);
	unsigned size = ftell (fp);
	fseek (fp, 0, SEEK_SET);
	if (!size)
	{
		fprintf (stderr, "Not a valid script file.");
		return 1;
	}

	char *str = ATOM_NEW_ARRAY(char, size + 1);
	if (!str)
	{
		fprintf (stderr, "Out of memory.");
		return 1;
	}

	size = fread (str, 1, size, fp);
	str[size] = '\0';
	fclose (fp);

	ATOM_LOGGER::setLogMethods (ATOM_LOGGER::getLogMethods()|ATOM_LOGGER::CONSOLE);

	if (!script->setSource (str, false))
	{
		fprintf (stderr, "Load script failed.");
		ATOM_DELETE_ARRAY(str);
		return 1;
	}

	ATOM_DELETE_ARRAY(str);

	char buffer[512];
	sprintf (buffer, "%s()", entry);
	script->executeString (buffer);

	scriptMngr.destroyScript (script);

	return 0;
}

