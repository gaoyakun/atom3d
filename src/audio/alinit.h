#ifndef __ATOM3D_AUDIO_ALINIT_H
#define __ATOM3D_AUDIO_ALINIT_H

#define AL_NO_PROTOTYPES
#include <al/al.h>
#include <al/alc.h>

extern LPALBUFFERDATA pfn_alBufferData;
extern LPALDELETEBUFFERS pfn_alDeleteBuffers;
extern LPALDELETESOURCES pfn_alDeleteSources;
extern LPALDISTANCEMODEL pfn_alDistanceModel;
extern LPALGENBUFFERS pfn_alGenBuffers;
extern LPALGENSOURCES pfn_alGenSources;
extern LPALGETENUMVALUE pfn_alGetEnumValue;
extern LPALGETERROR pfn_alGetError;
extern LPALGETSOURCEI pfn_alGetSourcei;
extern LPALSOURCEPLAY pfn_alSourcePlay;
extern LPALSOURCEQUEUEBUFFERS pfn_alSourceQueueBuffers;
extern LPALSOURCESTOP pfn_alSourceStop;
extern LPALSOURCEUNQUEUEBUFFERS pfn_alSourceUnqueueBuffers;
extern LPALSOURCEF pfn_alSourcef;
extern LPALSOURCEI pfn_alSourcei;
extern LPALCCLOSEDEVICE pfn_alcCloseDevice;
extern LPALCCREATECONTEXT pfn_alcCreateContext;
extern LPALCDESTROYCONTEXT pfn_alcDestroyContext;
extern LPALCGETCONTEXTSDEVICE pfn_alcGetContextsDevice;
extern LPALCGETERROR pfn_alcGetError;
extern LPALCMAKECONTEXTCURRENT pfn_alcMakeContextCurrent;
extern LPALCOPENDEVICE pfn_alcOpenDevice;

bool initOpenALLib (void);

#endif // __ATOM3D_AUDIO_ALINIT_H
