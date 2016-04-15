/**	\file displayinfo.h
 *	ATOM_DisplayInfo类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_DISPLAYINFO_H_
#define __ATOM_KERNEL_DISPLAYINFO_H_

#include "basedefs.h"

//! \class ATOM_DisplayInfo
//! 获取当前计算机系统中显卡信息的类
//! \author 高雅昆
class ATOM_KERNEL_API ATOM_DisplayInfo
{
public:
  //! DirectX版本信息结构
  struct DxVersionInfo
  {
	//! 主版本号
    int nDXVersionMajor;

	//! 次版本号
    int nDXVersionMinor;

	//! 微版本号
    char sDXVersionLetter[128];
  };

  //! 构造函数
  ATOM_DisplayInfo (void);

  //! 析构函数
  ~ATOM_DisplayInfo (void);

  //! 获取系统中显示设备的数量
  //! \return 数量
  unsigned getNumDevices (void) const;

  //! 获取显示设备的描述字符串
  //! \param device 显示设备的索引
  //! \return 描述
  const char *getDeviceDescription (unsigned device) const;

  //! 获取显示设备的制造商信息
  //! \param device 显示设备的索引
  //! \return 制造商信息
  const char *getDeviceManufacturer (unsigned device) const;

  //! 获取显示设备的芯片类型
  //! \param device 显示设备的索引
  //! \return 芯片类型信息
  const char *getDeviceChipType (unsigned device) const;

  //! 获取显示设备的内存大小
  //! \param device 显示设备的索引
  //! \return 内存大小信息
  const char *getDeviceMemory (unsigned device) const;

  //! 获取显示设备的当前显示模式
  //! \param device 显示设备的索引
  //! 显示模式信息
  const char *getDeviceDisplayMode (unsigned device) const;

  //! 获取显示设备的驱动程序名称
  //! \param device 显示设备的索引
  //! \return 驱动名称
  const char *getDeviceDriverName (unsigned device) const;

  //! 获取显示设备的驱动版本
  //! \param device 显示设备的索引
  //! \return 驱动版本信息
  const char *getDeviceDriverVersion (unsigned device) const;

  // 获取显示设备的驱动发布日期
  //! \param device 显示设备的索引
  //! \return 驱动日期信息
  const char *getDeviceDriverDate (unsigned device) const;

  //! 获取显示设备提供商索引
  //! \param device 显示设备的索引
  //! \return 提供商索引
  unsigned getDeviceVendorId (unsigned device) const;

  //! 获取设备的产品索引号
  //! \param device 显示设备的索引
  //! \return 产品索引号
  unsigned getDeviceId (unsigned device) const;

  //! 获取设备的子系统索引
  //! \param device 显示设备的索引
  //! \return 子系统索引
  unsigned getDeviceSubSystemId (unsigned device) const;

  //! 获取设备的修订索引
  //! \param device 显示设备的索引
  //! \return 修订索引
  unsigned getDeviceRevisionId (unsigned device) const;

  //! 查询设备是否支持DDraw加速
  //! \param device 显示设备的索引
  //! \return true支持，false不支持
  bool isDDrawAccelerationEnabled (unsigned device) const;

  //! 查询设备是否支持3D加速
  //! \param device 显示设备的索引
  //! \return true支持 false不支持
  bool is3DAccelerationExists (unsigned device) const;

  //! 查询设备的3D加速支持是否被打开
  //! \param device 显示设备的索引
  //! \return true打开 false不支持3D加速或者未打开3D加速功能
  bool is3DAccelerationEnabled (unsigned device) const;

  //! 查询设备是否具有AGP内存
  //! \param device 显示设备的索引
  //! \return true有 false没有
  bool isAGPExists (unsigned device) const;

  //! 查询设备的AGP内存是否被允许
  //! \param device 显示设备的索引
  //! \return true允许 false无AGP内存或者未打开AGP内存功能
  bool isAGPEnabled (unsigned device) const;

  //! 查询当前安装的DirectX的版本信息
  //! \return 版本信息
  //! \sa ATOM_DisplayInfo::DxVersionInfo
  const DxVersionInfo *getDxVersionInfo (void) const;

  //! 查询当前安装的DirectX版本，用字符串表示
  //! \return DirectX版本字符串
  const char *getDXVersionString (void) const;

private:       // attributes
  ATOM_VECTOR<struct DisplayDeviceInfo*> devices;
  DxVersionInfo dxverion;
  mutable ATOM_STRING dxversionStr;
};

#endif // __ATOM_KERNEL_DISPLAYINFO_H_
/*! @} */
