#ifndef __ATOM3D_ENGINE_DSHOW_MOVIEPLAYER_H
#define __ATOM3D_ENGINE_DSHOW_MOVIEPLAYER_H

#if _MSC_VER > 1000
# pragma once
#endif

//#include <qedit.h>
#include <dshow.h>

EXTERN_C const CLSID CLSID_SampleGrabber;
class DECLSPEC_UUID("C1F400A0-3F08-11d3-9F0B-006008039E37")
	SampleGrabber;
 
EXTERN_C const CLSID CLSID_NullRenderer;
class DECLSPEC_UUID("C1F400A4-3F08-11d3-9F0B-006008039E37")
	NullRenderer;
 
EXTERN_C const IID IID_ISampleGrabberCB;
MIDL_INTERFACE("0579154A-2B53-4994-B0D0-E773148EFF85")
ISampleGrabberCB : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE SampleCB( 
		double SampleTime,
		IMediaSample *pSample) = 0;
 
	virtual HRESULT STDMETHODCALLTYPE BufferCB( 
		double SampleTime,
		BYTE *pBuffer,
		long BufferLen) = 0;
 
};
 
EXTERN_C const IID IID_ISampleGrabber;
MIDL_INTERFACE("6B652FFF-11FE-4fce-92AD-0266B5D7C78F")
ISampleGrabber : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE SetOneShot( 
		BOOL OneShot) = 0;
 
	virtual HRESULT STDMETHODCALLTYPE SetMediaType( 
		const AM_MEDIA_TYPE *pType) = 0;
 
	virtual HRESULT STDMETHODCALLTYPE GetConnectedMediaType( 
		AM_MEDIA_TYPE *pType) = 0;
 
	virtual HRESULT STDMETHODCALLTYPE SetBufferSamples( 
		BOOL BufferThem) = 0;
 
	virtual HRESULT STDMETHODCALLTYPE GetCurrentBuffer( 
		/* [out][in] */ long *pBufferSize,
		/* [out] */ long *pBuffer) = 0;
 
	virtual HRESULT STDMETHODCALLTYPE GetCurrentSample( 
		/* [retval][out] */ IMediaSample **ppSample) = 0;
 
	virtual HRESULT STDMETHODCALLTYPE SetCallback( 
		ISampleGrabberCB *pCallback,
		long WhichMethodToCallback) = 0; 
};

class DShowMoviePlayer
{
public:
	DShowMoviePlayer (void);
	~DShowMoviePlayer (void);

public:
	void setFileName (const char *filename);
	const char *getFileName (void) const;
	void pause (void);
	void play (void);
	void rewind (void);
	void stop (void);
	bool isPlaying (void);
	bool loadMovie (void);
	void unloadMovie (void);
	bool aquireFrame (ATOM_Texture *texture);
	unsigned getVideoWidth (void) const;
	unsigned getVideoHeight (void) const;

private:
	IGraphBuilder *_graphBuilder;
	IMediaControl *_control;
	IMediaEvent *_event;
	IBaseFilter *_grabberFilter;
	ISampleGrabber *_grabber;
	IMediaSeeking *_seeking;
	IVideoWindow *_window;
	int _videoWidth;
	int _videoHeight;
	unsigned _bufferSize;
	ATOM_STRING _fileName;

	static bool _oleInitialized;
};

#endif // __ATOM3D_ENGINE_DSHOW_MOVIEPLAYER_H
