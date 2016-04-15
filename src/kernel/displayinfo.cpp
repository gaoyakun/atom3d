#include "stdafx.h"
#include "displayinfo.h"

#undef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

#undef TERM_ARRAY
#define TERM_ARRAY(x) (x)[(sizeof(x)/sizeof(x[0]))-1] = 0

#undef SAFE_RELEASE
#define SAFE_RELEASE(p)  { if(p) { (p)->Release(); (p)=NULL; } }

struct DisplayDeviceInfo
{
  char m_strDescription[200];
  char m_strManufacturer[200];
  char m_strChipType[100];
  char m_strDisplayMemory[100];
  char m_strATOM_DisplayMode[100];
  char m_strDriverName[100];
  char m_strDriverVersion[100];
  char m_strDriverDate[100];
  char m_strVendorId[100];
  char m_strDeviceId[100];
  char m_strSubSysId[100];
  char m_strRevisionId[100];
  bool m_bDDAccelerationEnabled;
  bool m_b3DAccelerationExists;
  bool m_b3DAccelerationEnabled;
  bool m_bAGPExists;
  bool m_bAGPEnabled;

  DisplayDeviceInfo (void)
  {
    strcpy (m_strDescription, "n/a");
    strcpy (m_strManufacturer, "n/a");
    strcpy (m_strChipType, "n/a");
    strcpy (m_strDisplayMemory, "n/a");
    strcpy (m_strATOM_DisplayMode, "n/a");
    strcpy (m_strDriverName, "n/a");
    strcpy (m_strDriverVersion, "n/a");
    strcpy (m_strDriverDate, "n/a");
	strcpy (m_strVendorId, "n/a");
	strcpy (m_strDeviceId, "n/a");
	strcpy (m_strSubSysId, "n/a");
	strcpy (m_strRevisionId, "n/a");
    m_bDDAccelerationEnabled = false;
    m_b3DAccelerationExists = false;
    m_b3DAccelerationEnabled = false;
    m_bAGPExists = false;
    m_bAGPEnabled = false;
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

static HRESULT GetIntValue( IDxDiagContainer* pObject, WCHAR* wstrName, int *nValue )
{
  HRESULT hr;
  VARIANT var;
  VariantInit( &var );

  if( FAILED( hr = pObject->GetProp( wstrName, &var ) ) )
    return hr;

  if( var.vt != VT_UI4 )
    return E_INVALIDARG;

  *nValue = var.ulVal;
  VariantClear( &var );

  return S_OK;
}


ATOM_DisplayInfo::ATOM_DisplayInfo (void)
{
  IDxDiagProvider*  pDxDiagProvider = 0;
  IDxDiagContainer* pDxDiagRoot = 0;
  BOOL bComInitialized = FALSE;
  IDxDiagContainer* pContainer = NULL;
  IDxDiagContainer* pObject = NULL;
  IDxDiagContainer* pDxDiagSystemInfo = NULL;
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

  if( FAILED( hr = pDxDiagRoot->GetChildContainer( L"DxDiag_DisplayDevices", &pContainer ) ) )
    goto LCleanup;

  if( FAILED( hr = pContainer->GetNumberOfChildContainers( &nInstanceCount ) ) )
    goto LCleanup;

  WCHAR wszContainer[256];

  for (nItem = 0; nItem < nInstanceCount; nItem++)
  {
    DisplayDeviceInfo *di = ATOM_NEW(DisplayDeviceInfo);
    devices.push_back (di);

    hr = pContainer->EnumChildContainerNames( nItem, wszContainer, 256 );
    if(FAILED(hr))
      goto LCleanup;

    hr = pContainer->GetChildContainer( wszContainer, &pObject );
    if( FAILED( hr ) || pObject == NULL )
      goto LCleanup;

    if( FAILED( hr = GetStringValue( pObject, L"szDescription", di->m_strDescription, ARRAY_SIZE(di->m_strDescription))))
      goto LCleanup;

    if( FAILED( hr = GetStringValue( pObject, L"szManufacturer", di->m_strManufacturer, ARRAY_SIZE(di->m_strManufacturer))))
      goto LCleanup;

    if( FAILED( hr = GetStringValue( pObject, L"szChipType", di->m_strChipType, ARRAY_SIZE(di->m_strChipType))))
      goto LCleanup;

    if( FAILED( hr = GetStringValue( pObject, L"szDisplayMemoryLocalized", di->m_strDisplayMemory, ARRAY_SIZE(di->m_strDisplayMemory))))
      goto LCleanup;

    if( FAILED( hr = GetStringValue( pObject, L"szDisplayModeLocalized", di->m_strATOM_DisplayMode, ARRAY_SIZE(di->m_strATOM_DisplayMode))))
      goto LCleanup;

    if( FAILED( hr = GetStringValue( pObject, L"szDriverName", di->m_strDriverName, ARRAY_SIZE(di->m_strDriverName))))
      goto LCleanup;

    if( FAILED( hr = GetStringValue( pObject, L"szDriverVersion", di->m_strDriverVersion, ARRAY_SIZE(di->m_strDriverVersion))))
      goto LCleanup;

    if( FAILED( hr = GetStringValue( pObject, L"szDriverDateLocalized", di->m_strDriverDate, ARRAY_SIZE(di->m_strDriverDate))))
      goto LCleanup;

	if( FAILED( hr = GetStringValue( pObject, L"szVendorId", di->m_strVendorId, ARRAY_SIZE(di->m_strVendorId))))
	  goto LCleanup;

	if( FAILED( hr = GetStringValue( pObject, L"szDeviceId", di->m_strDeviceId, ARRAY_SIZE(di->m_strDeviceId))))
	  goto LCleanup;

	if( FAILED( hr = GetStringValue( pObject, L"szSubSysId", di->m_strSubSysId, ARRAY_SIZE(di->m_strSubSysId))))
	  goto LCleanup;

	if( FAILED( hr = GetStringValue( pObject, L"szRevisionId", di->m_strRevisionId, ARRAY_SIZE(di->m_strRevisionId))))
	  goto LCleanup;

    if( FAILED( hr = GetBoolValue( pObject, L"bDDAccelerationEnabled", &di->m_bDDAccelerationEnabled)))
      goto LCleanup;

    if( FAILED( hr = GetBoolValue( pObject, L"b3DAccelerationExists", &di->m_b3DAccelerationExists)))
      goto LCleanup;

    if( FAILED( hr = GetBoolValue( pObject, L"b3DAccelerationEnabled", &di->m_b3DAccelerationEnabled)))
      goto LCleanup;

    if( FAILED( hr = GetBoolValue( pObject, L"bAGPEnabled", &di->m_bAGPEnabled)))
      goto LCleanup;

    if( FAILED( hr = GetBoolValue( pObject, L"bAGPExists", &di->m_bAGPExists)))
      goto LCleanup;

    SAFE_RELEASE(pObject);
  }

  //get dx version
  if( FAILED( hr = pDxDiagRoot->GetChildContainer( L"DxDiag_SystemInfo", &pDxDiagSystemInfo ) ) )
    goto LCleanup;

  if( FAILED( hr = GetIntValue( pDxDiagSystemInfo, L"dwDirectXVersionMajor", &dxverion.nDXVersionMajor)))
    goto LCleanup;

  if( FAILED( hr = GetIntValue( pDxDiagSystemInfo, L"dwDirectXVersionMinor", &dxverion.nDXVersionMinor)))
    goto LCleanup;

  if( FAILED( hr = GetStringValue( pDxDiagSystemInfo, L"szDirectXVersionLetter", dxverion.sDXVersionLetter, ARRAY_SIZE(dxverion.sDXVersionLetter))))
    goto LCleanup;

LCleanup:
  SAFE_RELEASE(pObject);
  SAFE_RELEASE(pContainer);
  SAFE_RELEASE(pDxDiagSystemInfo);
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

ATOM_DisplayInfo::~ATOM_DisplayInfo ()
{
  for (unsigned i = 0; i < devices.size(); ++i)
  {
    ATOM_DELETE(devices[i]);
  }
}

unsigned ATOM_DisplayInfo::getNumDevices (void) const
{
  return devices.size();
}

const char *ATOM_DisplayInfo::getDeviceDescription (unsigned device) const
{
  return devices[device]->m_strDescription;
}

const char *ATOM_DisplayInfo::getDeviceManufacturer (unsigned device) const
{
  return devices[device]->m_strManufacturer;
}

const char *ATOM_DisplayInfo::getDeviceChipType (unsigned device) const
{
  return devices[device]->m_strChipType;
}

const char *ATOM_DisplayInfo::getDeviceMemory (unsigned device) const
{
  return devices[device]->m_strDisplayMemory;
}

const char *ATOM_DisplayInfo::getDeviceDisplayMode (unsigned device) const
{
  return devices[device]->m_strATOM_DisplayMode;
}

const char *ATOM_DisplayInfo::getDeviceDriverName (unsigned device) const
{
  return devices[device]->m_strDriverName;
}

const char *ATOM_DisplayInfo::getDeviceDriverVersion (unsigned device) const
{
  return devices[device]->m_strDriverVersion;
}

const char *ATOM_DisplayInfo::getDeviceDriverDate (unsigned device) const
{
  return devices[device]->m_strDriverDate;
}

unsigned ATOM_DisplayInfo::getDeviceVendorId (unsigned device) const
{
	return strtol (devices[device]->m_strVendorId, 0, 16);
}

unsigned ATOM_DisplayInfo::getDeviceId (unsigned device) const
{
	return strtol (devices[device]->m_strDeviceId, 0, 16);
}

unsigned ATOM_DisplayInfo::getDeviceSubSystemId (unsigned device) const
{
	return strtol (devices[device]->m_strSubSysId, 0, 16);
}

unsigned ATOM_DisplayInfo::getDeviceRevisionId (unsigned device) const
{
	return strtol (devices[device]->m_strRevisionId, 0, 16);
}

bool ATOM_DisplayInfo::isDDrawAccelerationEnabled (unsigned device) const
{
  return devices[device]->m_bDDAccelerationEnabled;
}

bool ATOM_DisplayInfo::is3DAccelerationExists (unsigned device) const
{
  return devices[device]->m_b3DAccelerationExists;
}

bool ATOM_DisplayInfo::is3DAccelerationEnabled (unsigned device) const
{
  return devices[device]->m_b3DAccelerationEnabled;
}

bool ATOM_DisplayInfo::isAGPExists (unsigned device) const
{
  return devices[device]->m_bAGPExists;
}

bool ATOM_DisplayInfo::isAGPEnabled (unsigned device) const
{
  return devices[device]->m_bAGPEnabled;
}

const ATOM_DisplayInfo::DxVersionInfo *ATOM_DisplayInfo::getDxVersionInfo (void) const
{
  return &dxverion;
}

const char *ATOM_DisplayInfo::getDXVersionString (void) const
{
	if (dxversionStr.empty ())
	{
		char buffer[256];
		sprintf (buffer, "Direct3D %d.%d%s", dxverion.nDXVersionMajor, dxverion.nDXVersionMinor, dxverion.sDXVersionLetter);
		dxversionStr = buffer;
	}
	return dxversionStr.c_str();
}

