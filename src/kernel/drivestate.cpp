#include "stdafx.h"

const char *ATOM_DriveStats::getName(void) const
{
  return m_strName.c_str();
}

const char *ATOM_DriveStats::getType(void) const
{
  return m_strType.c_str();
}

const char *ATOM_DriveStats::getTotalSpaceStr(void) const
{
  return m_strTotalSpace.c_str();
}

const char *ATOM_DriveStats::getFreeSpaceStr(void) const
{
  return m_strFreeSpace.c_str();
}

