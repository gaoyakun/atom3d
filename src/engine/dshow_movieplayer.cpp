#include "StdAfx.h"
#include "dshow_movieplayer.h"

bool DShowMoviePlayer::_oleInitialized = false;

DShowMoviePlayer::DShowMoviePlayer (void)
{
	_graphBuilder = 0;
	_control = 0;
	_event = 0;
	_grabberFilter = 0;
	_grabber = 0;
	_seeking = 0;
	_window = 0;
	_videoWidth = 0;
	_videoHeight = 0;
}

DShowMoviePlayer::~DShowMoviePlayer (void)
{
	unloadMovie ();
}

void DShowMoviePlayer::setFileName (const char *filename)
{
	_fileName = filename ? filename : "";
}

const char *DShowMoviePlayer::getFileName (void) const
{
	return _fileName.c_str();
}

void DShowMoviePlayer::pause (void)
{
	if (_control)
	{
		_control->Pause ();
	}
}

void DShowMoviePlayer::play (void)
{
	if (_control)
	{
		_control->Run ();
	}
}

void DShowMoviePlayer::rewind (void)
{
	if (_seeking)
	{
		LONGLONG p1 = 0;
		LONGLONG p2 = 0;
		_seeking->SetPositions (&p1, AM_SEEKING_AbsolutePositioning, &p2, AM_SEEKING_NoPositioning);
	}
}

void DShowMoviePlayer::stop (void)
{
	if (isPlaying ())
	{
		if (_control)
		{
			_control->Stop ();
		}
	}
}

bool DShowMoviePlayer::isPlaying (void)
{
	OAFilterState pfs;
	HRESULT hr;

	if (_event)
	{
		long ev;
		LONG_PTR p1, p2;

		while (E_ABORT != _event->GetEvent (&ev, &p1, &p2, 0))
		{
			if (ev == EC_COMPLETE)
			{
				pause ();
				return false;
			}

			hr = _event->FreeEventParams (ev, p1, p2);
			if (FAILED(hr))
			{
				pause ();
				return false;
			}
		}
	}

	if (_control)
	{
		hr = _control->GetState (0, &pfs);
		if (FAILED(hr))
		{
			pause ();
			return false;
		}

		return pfs == State_Running;
	}

	return false;
}

static bool convertCStringToWString (const char *str, wchar_t *dstStr, unsigned size)
{
	if (::MultiByteToWideChar (CP_ACP, 0, str, -1, dstStr, size) == 0)
	{
		return false;
	}

	return true;
}

HRESULT SaveGraphFile(IGraphBuilder *pGraph, WCHAR *wszPath) 
{
    const WCHAR wszStreamName[] = L"ActiveMovieGraph"; 
    HRESULT hr;
    
    IStorage *pStorage = NULL;
    hr = StgCreateDocfile(
        wszPath,
        STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE,
        0, &pStorage);
    if(FAILED(hr)) 
    {
        return hr;
    }

    IStream *pStream;
    hr = pStorage->CreateStream(
        wszStreamName,
        STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE,
        0, 0, &pStream);
    if (FAILED(hr)) 
    {
        pStorage->Release();    
        return hr;
    }

    IPersistStream *pPersist = NULL;
    pGraph->QueryInterface(IID_IPersistStream, (void**)&pPersist);
    hr = pPersist->Save(pStream, TRUE);
    pStream->Release();
    pPersist->Release();
    if (SUCCEEDED(hr)) 
    {
        hr = pStorage->Commit(STGC_DEFAULT);
    }
    pStorage->Release();
    return hr;
}

void FindPin(IBaseFilter* baseFilter, PIN_DIRECTION direction, int pinNumber, IPin** destPin) 
{
    IEnumPins *enumPins = 0;

    *destPin = NULL;

    if (SUCCEEDED(baseFilter->EnumPins(&enumPins))) {
        ULONG numFound;
        IPin* tmpPin;

        while (SUCCEEDED(enumPins->Next(1, &tmpPin, &numFound))) {
            PIN_DIRECTION pinDirection;

            tmpPin->QueryDirection(&pinDirection);
            if (pinDirection == direction) {
                if (pinNumber == 0) {
                    // Return the pin's interface
                    *destPin = tmpPin;
                    break;
                }
                pinNumber--;
            }
            tmpPin->Release();
        }
    }

	if (enumPins)
	{
		enumPins->Release();
	}
}

bool ConnectPins(IGraphBuilder *filterGraph, IBaseFilter* outputFilter, unsigned int outputNum, IBaseFilter* inputFilter, unsigned int inputNum) 
{
    IPin *inputPin = 0;
    IPin *outputPin = 0;

    if (!outputFilter || !inputFilter) {
        return false;
    }

    FindPin(outputFilter, PINDIR_OUTPUT, outputNum, &outputPin);
    FindPin(inputFilter, PINDIR_INPUT, inputNum, &inputPin);

	bool ret = false;
    if (inputPin && outputPin) 
	{
		IPin *tmp = 0;
		outputPin->ConnectedTo (&tmp);
		if (tmp)
		{
			outputPin->Disconnect ();
			tmp->Release();
		}
		inputPin->ConnectedTo (&tmp);
		if (tmp)
		{
			inputPin->Disconnect ();
			tmp->Release();
		}

		HRESULT hr = filterGraph->Connect (outputPin, inputPin);
        ret = SUCCEEDED(hr);
	}

	if (inputPin)
	{
		inputPin->Release();
	}

	if (outputPin)
	{
		outputPin->Release();
	}

	return ret;
}



bool DShowMoviePlayer::loadMovie (void)
{
	if (!_oleInitialized)
	{
		::CoInitialize (0);
	}

	char physicFileName[512];
	wchar_t physicFileNameW[512];
	ATOM_GetNativePathName (_fileName.c_str(), physicFileName);
	if (!convertCStringToWString (physicFileName, physicFileNameW, 512))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "Error converting C string filename to UNICODE");
		return false;
	}

#if 1
	HRESULT hr = ::CoCreateInstance (CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&_graphBuilder);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error creating graph");
		return false;
	}

	hr = ::CoCreateInstance (CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&_grabberFilter);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error creating sample grabber");
		return false;
	}

	hr = _grabberFilter->QueryInterface (IID_ISampleGrabber, (void**)&_grabber);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error querying sample grabber");
		return false;
	}

	AM_MEDIA_TYPE mt;
	memset (&mt, 0, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB32;
	mt.formattype = FORMAT_VideoInfo;
	hr = _grabber->SetMediaType (&mt);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error setting smaple grabber media type");
		return false;
	}

	hr = _graphBuilder->AddFilter (_grabberFilter, L"Sample Grabber");
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error adding filter");
		return false;
	}

	hr = _graphBuilder->QueryInterface (IID_IMediaControl, (void**)&_control);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error querying media control");
		return false;
	}

	IBaseFilter *render = 0;
	hr = ::CoCreateInstance (CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&render);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error creating render filter");
		return false;
	}

	hr = _graphBuilder->AddFilter (render, L"Null renderer");
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error adding null-renderer filter");
		return false;
	}

	hr = _control->RenderFile (physicFileNameW);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error rendering file");
		return false;
	}
#if 0
	IPin *grabberOut = 0;
	FindPin (_grabberFilter, PINDIR_OUTPUT, 0, &grabberOut);
	if (grabberOut)
	{
		grabberOut->Disconnect ();
		grabberOut->Release ();
	}
#endif

	SaveGraphFile (_graphBuilder, L"C:\\test.grf");
	hr = _graphBuilder->QueryInterface (IID_IVideoWindow, (void**)&_window);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error querying video window");
		return false;
	}

	_window->put_AutoShow (OAFALSE);

#if 0
	if (!ConnectPins (_graphBuilder, _grabberFilter, 0, render, 0))
	{
		return false;
	}
	_control->RenderFile (physicFileNameW);
#endif

	memset(&mt, 0, sizeof(mt));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB32;
	mt.formattype = FORMAT_VideoInfo;

	hr = _grabber->GetConnectedMediaType (&mt);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error getting connected media type");
		return false;
	}

	VIDEOINFOHEADER *vih = (VIDEOINFOHEADER*)mt.pbFormat;
	_videoWidth = vih->bmiHeader.biWidth;
	_videoHeight = vih->bmiHeader.biHeight;
	_bufferSize = mt.lSampleSize;
	if (mt.cbFormat != 0)
	{
		::CoTaskMemFree ((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = 0;
	}
	if (mt.pUnk != NULL)
	{
		mt.pUnk->Release ();
		mt.pUnk = NULL;
	}

	_grabber->SetOneShot (FALSE);
	_grabber->SetBufferSamples (TRUE);

	hr = _graphBuilder->QueryInterface (IID_IMediaEvent, (void**)&_event);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error querying media event");
		return false;
	}

	hr = _graphBuilder->QueryInterface (IID_IMediaSeeking, (void**)&_seeking);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error querying media seeking");
		return false;
	}

#else
	HRESULT hr = ::CoCreateInstance (CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void**)&_graphBuilder);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error creating graph");
		return false;
	}

	hr = _graphBuilder->QueryInterface (IID_IMediaControl, (void**)&_control);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error querying media control");
		return false;
	}

	hr = _graphBuilder->QueryInterface (IID_IMediaEvent, (void**)&_event);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error querying media event");
		return false;
	}

	hr = _graphBuilder->QueryInterface (IID_IMediaSeeking, (void**)&_seeking);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error querying media seeking");
		return false;
	}

	hr = ::CoCreateInstance (CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&_grabberFilter);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error creating sample grabber");
		return false;
	}

	hr = _graphBuilder->AddFilter (_grabberFilter, L"Sample Grabber");
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error adding filter");
		return false;
	}

	hr = _grabberFilter->QueryInterface (IID_ISampleGrabber, (void**)&_grabber);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error querying sample grabber");
		return false;
	}

	AM_MEDIA_TYPE mt;
	memset (&mt, 0, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
	mt.formattype = FORMAT_VideoInfo;
	hr = _grabber->SetMediaType (&mt);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error setting smaple grabber media type");
		return false;
	}

	IBaseFilter *srcFilter = 0;
	hr = _graphBuilder->AddSourceFilter (physicFileNameW, L"Source", &srcFilter);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error adding source filter");
		return false;
	}

	hr = connectFilters (_graphBuilder, srcFilter, _grabberFilter);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error connecting filters");
		return false;
	}

	IBaseFilter *render = 0;
	hr = ::CoCreateInstance (CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&render);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error creating render filter");
		return false;
	}

	_graphBuilder->AddFilter (render, L"Render");

	hr = connectFilters (_graphBuilder, _grabberFilter, render);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error connect filters");
		return false;
	}

	hr = _graphBuilder->RenderFile (physicFileNameW, NULL);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error rendering file");
		return false;
	}

	hr = _graphBuilder->QueryInterface (IID_IVideoWindow, (void**)&_window);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error querying video window");
		return false;
	}

	_window->put_AutoShow (OAFALSE);

	AM_MEDIA_TYPE mtt;
	hr = _grabber->GetConnectedMediaType (&mtt);
	if (FAILED(hr))
	{
		ATOM_LOGGER::error ("%s() %s\n", __FUNCTION__, "[DSHOW] Error getting connected media type info");
		return false;
	}
	
	VIDEOINFOHEADER *vih = (VIDEOINFOHEADER*)mtt.pbFormat;
	_videoWidth = vih->bmiHeader.biWidth;
	_videoHeight = vih->bmiHeader.biHeight;
	if (mtt.cbFormat != 0)
	{
		::CoTaskMemFree ((PVOID)mtt.pbFormat);
		mtt.cbFormat = 0;
		mtt.pbFormat = 0;
	}
	if (mtt.pUnk != NULL)
	{
		mtt.pUnk->Release ();
		mtt.pUnk = NULL;
	}

	_grabber->SetOneShot (FALSE);
	_grabber->SetBufferSamples (TRUE);
#endif

	return true;
}

bool DShowMoviePlayer::aquireFrame (ATOM_Texture *texture)
{
	if (!texture || texture->getWidth() != _videoWidth || texture->getHeight() != _videoHeight)
	{
		return false;
	}

	ATOM_Texture::LockedRect lrc;
	if (!texture->lock (0, &lrc))
	{
		return false;
	}

	if (_bufferSize != lrc.pitch * texture->getHeight())
	{
		return false;
	}

	bool ret = true;

	long size = _bufferSize;
	HRESULT hr = _grabber->GetCurrentBuffer (&size, (long*)lrc.bits);
	if (FAILED(hr))
	{
		ret = false;
		ATOM_LOGGER::error ("%s(0x%08X) %s\n", __FUNCTION__, texture, "Error getting current frame, may be the video is not playing");
	}

	texture->unlock (0);

	return ret;
}

void DShowMoviePlayer::unloadMovie (void)
{
	stop ();

	if (_grabber)
	{
		_grabber->Release();
		_grabber = 0;
	}

	if (_grabberFilter)
	{
		_grabberFilter->Release ();
		_grabberFilter = 0;
	}

	if (_window)
	{
		_window->Release ();
		_window = 0;
	}

	if (_seeking)
	{
		_seeking->Release ();
		_seeking = 0;
	}

	if (_control)
	{
		_control->Release ();
		_control = 0;
	}

	if (_event)
	{
		_event->Release ();
		_event = 0;
	}

	if (_graphBuilder)
	{
		_graphBuilder->Release ();
		_graphBuilder = 0;
	}

	_videoWidth = 0;
	_videoHeight = 0;
}

unsigned DShowMoviePlayer::getVideoWidth (void) const
{
	return _videoWidth;
}

unsigned DShowMoviePlayer::getVideoHeight (void) const
{
	return _videoHeight;
}
