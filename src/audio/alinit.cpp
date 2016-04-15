#include <windows.h>
#include "alinit.h"

LPALBUFFERDATA pfn_alBufferData = nullptr;
LPALDELETEBUFFERS pfn_alDeleteBuffers = nullptr;
LPALDELETESOURCES pfn_alDeleteSources = nullptr;
LPALDISTANCEMODEL pfn_alDistanceModel = nullptr;
LPALGENBUFFERS pfn_alGenBuffers = nullptr;
LPALGENSOURCES pfn_alGenSources = nullptr;
LPALGETENUMVALUE pfn_alGetEnumValue = nullptr;
LPALGETERROR pfn_alGetError = nullptr;
LPALGETSOURCEI pfn_alGetSourcei = nullptr;
LPALSOURCEPLAY pfn_alSourcePlay = nullptr;
LPALSOURCEQUEUEBUFFERS pfn_alSourceQueueBuffers = nullptr;
LPALSOURCESTOP pfn_alSourceStop = nullptr;
LPALSOURCEUNQUEUEBUFFERS pfn_alSourceUnqueueBuffers = nullptr;
LPALSOURCEF pfn_alSourcef = nullptr;
LPALSOURCEI pfn_alSourcei = nullptr;
LPALCCLOSEDEVICE pfn_alcCloseDevice = nullptr;
LPALCCREATECONTEXT pfn_alcCreateContext = nullptr;
LPALCDESTROYCONTEXT pfn_alcDestroyContext = nullptr;
LPALCGETCONTEXTSDEVICE pfn_alcGetContextsDevice = nullptr;
LPALCGETERROR pfn_alcGetError = nullptr;
LPALCMAKECONTEXTCURRENT pfn_alcMakeContextCurrent = nullptr;
LPALCOPENDEVICE pfn_alcOpenDevice = nullptr;

#define LOAD_AL_FUNC(protoco, func) \
	pfn_##func = (protoco)::GetProcAddress(hDll, #func); \
	if (!pfn_##func) \
	{ \
		return false; \
	}

bool loadAlFunctions (void)
{
	HMODULE hDll = ::LoadLibraryA ("OpenAL32.dll");
	if (!hDll)
	{
		return false;
	}

	LOAD_AL_FUNC(LPALBUFFERDATA, alBufferData);
	LOAD_AL_FUNC(LPALDELETEBUFFERS, alDeleteBuffers);
	LOAD_AL_FUNC(LPALDELETESOURCES, alDeleteSources);
	LOAD_AL_FUNC(LPALDISTANCEMODEL, alDistanceModel);
	LOAD_AL_FUNC(LPALGENBUFFERS, alGenBuffers);
	LOAD_AL_FUNC(LPALGENSOURCES, alGenSources);
	LOAD_AL_FUNC(LPALGETENUMVALUE, alGetEnumValue);
	LOAD_AL_FUNC(LPALGETERROR, alGetError);
	LOAD_AL_FUNC(LPALGETSOURCEI, alGetSourcei);
	LOAD_AL_FUNC(LPALSOURCEPLAY, alSourcePlay);
	LOAD_AL_FUNC(LPALSOURCEQUEUEBUFFERS, alSourceQueueBuffers);
	LOAD_AL_FUNC(LPALSOURCESTOP, alSourceStop);
	LOAD_AL_FUNC(LPALSOURCEUNQUEUEBUFFERS, alSourceUnqueueBuffers);
	LOAD_AL_FUNC(LPALSOURCEF, alSourcef);
	LOAD_AL_FUNC(LPALSOURCEI, alSourcei);
	LOAD_AL_FUNC(LPALCCLOSEDEVICE, alcCloseDevice);
	LOAD_AL_FUNC(LPALCCREATECONTEXT, alcCreateContext);
	LOAD_AL_FUNC(LPALCDESTROYCONTEXT, alcDestroyContext);
	LOAD_AL_FUNC(LPALCGETCONTEXTSDEVICE, alcGetContextsDevice);
	LOAD_AL_FUNC(LPALCGETERROR, alcGetError);
	LOAD_AL_FUNC(LPALCMAKECONTEXTCURRENT, alcMakeContextCurrent);
	LOAD_AL_FUNC(LPALCOPENDEVICE, alcOpenDevice);

	return true;
}

bool initOpenALLib (void)
{
	if (!loadAlFunctions())
	{
		pfn_alBufferData = nullptr;
		pfn_alDeleteBuffers = nullptr;
		pfn_alDeleteSources = nullptr;
		pfn_alDistanceModel = nullptr;
		pfn_alGenBuffers = nullptr;
		pfn_alGenSources = nullptr;
		pfn_alGetEnumValue = nullptr;
		pfn_alGetError = nullptr;
		pfn_alGetSourcei = nullptr;
		pfn_alSourcePlay = nullptr;
		pfn_alSourceQueueBuffers = nullptr;
		pfn_alSourceStop = nullptr;
		pfn_alSourceUnqueueBuffers = nullptr;
		pfn_alSourcef = nullptr;
		pfn_alSourcei = nullptr;
		pfn_alcCloseDevice = nullptr;
		pfn_alcCreateContext = nullptr;
		pfn_alcDestroyContext = nullptr;
		pfn_alcGetContextsDevice = nullptr;
		pfn_alcGetError = nullptr;
		pfn_alcMakeContextCurrent = nullptr;
		pfn_alcOpenDevice = nullptr;

		return false;
	}

	return true;
}

