#include "stdafx.h"

#undef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#undef TERM_ARRAY
#define TERM_ARRAY(x) (x)[(sizeof(x)/sizeof(x[0]))-1] = 0

#undef SAFE_RELEASE
#define SAFE_RELEASE(p)  { if(p) { (p)->Release(); (p)=NULL; } }

struct SoundDeviceInfo
{
  char m_strDescription[200];
  char m_strDriverName[100];

  SoundDeviceInfo (void)
  {
    strcpy (m_strDescription, "n/a");
    strcpy (m_strDriverName, "n/a");
  }
};

static HRESULT GetBoolValue( IDxDiagContainer* pObject, WCHAR* wstrName, bool* pbValue )
{
  HRESULT hr;
  VARIANT var;
  VariantInit( &var );

  if( FAILED( hr = pObject->GetProp( wstrName, &var ) ) )
      return hr;

  if( var.vt != VT_BOOL )
      return E_INVALIDARG;

  *pbValue = ( var.boolVal != 0 );
  VariantClear( &var );

  return S_OK;
}

static HRESULT GetStringValue( IDxDiagContainer* pObject, WCHAR* wstrName, TCHAR* strValue, int nStrLen )
{
  HRESULT hr;
  VARIANT var;
  VariantInit( &var );

  if( FAILED( hr = pObject->GetProp( wstrName, &var ) ) )
      return hr;

  if( var.vt != VT_BSTR )
      return E_INVALIDARG;
  
#ifdef _UNICODE
  wcsncpy( strValue, var.bstrVal, nStrLen-1 );
#else
  wcstombs( strValue, var.bstrVal, nStrLen );   
#endif
  strValue[nStrLen-1] = TEXT('\0');
  VariantClear( &var );

  return S_OK;
}

ATOM_SoundInfo::ATOM_SoundInfo (void)
{
  IDxDiagProvider*  pDxDiagProvider = 0;
  IDxDiagContainer* pDxDiagRoot = 0;
  BOOL bComInitialized = FALSE;
  IDxDiagContainer* pContainer = NULL;
  IDxDiagContainer* pObject = NULL;
  DWORD nInstanceCount = 0;
  DWORD nItem = 0;

  HRESULT hr = CoInitialize (NULL);
  bComInitialized = SUCCEEDED (hr);

  hr = CoCreateInstance (CLSID_DxDiagProvider, NULL, CLSCTX_INPROC_SERVER, IID_IDxDiagProvider, (LPVOID*)&pDxDiagProvider);
  if (FAILED(hr))
    goto LCleanup;

  if (!pDxDiagProvider)
  {
    hr = E_POINTER;
    goto LCleanup;
  }

  DXDIAG_INIT_PARAMS dxDiagInitParam;
  memset(&dxDiagInitParam, 0, sizeof(DXDIAG_INIT_PARAMS));

  dxDiagInitParam.dwSize = sizeof(DXDIAG_INIT_PARAMS);
  dxDiagInitParam.dwDxDiagHeaderVersion = DXDIAG_DX9_SDK_VERSION;
  dxDiagInitParam.bAllowWHQLChecks = false;
  dxDiagInitParam.pReserved = NULL;

  hr = pDxDiagProvider->Initialize( &dxDiagInitParam );
  if( FAILED(hr) )
    goto LCleanup;

  hr = pDxDiagProvider->GetRootContainer( &pDxDiagRoot );
  if( FAILED(hr) )
    goto LCleanup;

  if( FAILED( hr = pDxDiagRoot->GetChildContainer( L"DxDiag_DirectSound.DxDiag_SoundDevices", &pContainer ) ) )
    goto LCleanup;

  if( FAILED( hr = pContainer->GetNumberOfChildContainers( &nInstanceCount ) ) )
    goto LCleanup;

  WCHAR wszContainer[256];

  for (nItem = 0; nItem < nInstanceCount; nItem++)
  {
    SoundDeviceInfo *di = ATOM_NEW(SoundDeviceInfo);
    devices.push_back (di);

    hr = pContainer->EnumChildContainerNames( nItem, wszContainer, 256 );
    if(FAILED(hr))
      goto LCleanup;

    hr = pContainer->GetChildContainer( wszContainer, &pObject );
    if( FAILED( hr ) || pObject == NULL )
      goto LCleanup;

    if( FAILED( hr = GetStringValue( pObject, L"szDescription", di->m_strDescription, ARRAY_SIZE(di->m_strDescription))))
      goto LCleanup;

    if( FAILED( hr = GetStringValue( pObject, L"szDriverName", di->m_strDriverName, ARRAY_SIZE(di->m_strDriverName))))
      goto LCleanup;

    SAFE_RELEASE(pObject);
  }

LCleanup:
  SAFE_RELEASE(pObject);
  SAFE_RELEASE(pContainer);
  SAFE_RELEASE(pDxDiagRoot);
  SAFE_RELEASE(pDxDiagProvider);

  if (FAILED(hr) && !devices.empty())
  {
    ATOM_DELETE(devices.back ());
    devices.pop_back ();
  }

  if (bComInitialized)
    CoUninitialize ();
}

ATOM_SoundInfo::~ATOM_SoundInfo ()
{
  for (unsigned i = 0; i < devices.size(); ++i)
  {
    ATOM_DELETE(devices[i]);
  }
}

unsigned ATOM_SoundInfo::getNumDevices (void) const
{
  return devices.size();
}

const char *ATOM_SoundInfo::getDeviceDescription (unsigned device) const
{
  return devices[device]->m_strDescription;
}

const char *ATOM_SoundInfo::getDeviceDriverName (unsigned device) const
{
  return devices[device]->m_strDriverName;
}

