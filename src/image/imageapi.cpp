#include "stdafx.h"
#ifdef __CODEGEARC__
# pragma hdrstop
#endif
#include "imageapi.h"
#include <squish.h>

int dxt_quality = squish::kColourMetricPerceptual;

void ATOM_IMAGE_API ATOM_SetDXTCompressQuality (int quality)
{
  ATOM_STACK_TRACE(ATOM_SetDXTCompressQuality);
  switch (quality)
  {
  case ATOM_IMAGE_DXT_COMPRESS_QUALITY_FASTEST:
	dxt_quality = squish::kColourRangeFit;
	break;
  case ATOM_IMAGE_DXT_COMPRESS_QUALITY_NORMAL:
	dxt_quality = squish::kColourMetricPerceptual;
	break;
  case ATOM_IMAGE_DXT_COMPRESS_QUALITY_PRODUCTION:
	dxt_quality = squish::kColourClusterFit;
    break;
  case ATOM_IMAGE_DXT_COMPRESS_QUALITY_HIGHEST:
	dxt_quality = squish::kColourIterativeClusterFit;
    break;
  default:
    break;
  }
}

int ATOM_IMAGE_API ATOM_GetDXTCompressQuality (void)
{
  ATOM_STACK_TRACE(ATOM_GetDXTCompressQuality);
  switch (dxt_quality)
  {
  case squish::kColourRangeFit:
    return ATOM_IMAGE_DXT_COMPRESS_QUALITY_FASTEST;
  case squish::kColourMetricPerceptual:
    return ATOM_IMAGE_DXT_COMPRESS_QUALITY_NORMAL;
  case squish::kColourClusterFit:
    return ATOM_IMAGE_DXT_COMPRESS_QUALITY_PRODUCTION;
  case squish::kColourIterativeClusterFit:
    return ATOM_IMAGE_DXT_COMPRESS_QUALITY_HIGHEST;
  default:
    return ATOM_IMAGE_DXT_COMPRESS_QUALITY_NORMAL;
  }
}

