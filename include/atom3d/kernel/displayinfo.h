/**	\file displayinfo.h
 *	ATOM_DisplayInfo�������.
 *
 *	\author ������
 *	\addtogroup kernel
 *	@{
 */

#ifndef __ATOM_KERNEL_DISPLAYINFO_H_
#define __ATOM_KERNEL_DISPLAYINFO_H_

#include "basedefs.h"

//! \class ATOM_DisplayInfo
//! ��ȡ��ǰ�����ϵͳ���Կ���Ϣ����
//! \author ������
class ATOM_KERNEL_API ATOM_DisplayInfo
{
public:
  //! DirectX�汾��Ϣ�ṹ
  struct DxVersionInfo
  {
	//! ���汾��
    int nDXVersionMajor;

	//! �ΰ汾��
    int nDXVersionMinor;

	//! ΢�汾��
    char sDXVersionLetter[128];
  };

  //! ���캯��
  ATOM_DisplayInfo (void);

  //! ��������
  ~ATOM_DisplayInfo (void);

  //! ��ȡϵͳ����ʾ�豸������
  //! \return ����
  unsigned getNumDevices (void) const;

  //! ��ȡ��ʾ�豸�������ַ���
  //! \param device ��ʾ�豸������
  //! \return ����
  const char *getDeviceDescription (unsigned device) const;

  //! ��ȡ��ʾ�豸����������Ϣ
  //! \param device ��ʾ�豸������
  //! \return ��������Ϣ
  const char *getDeviceManufacturer (unsigned device) const;

  //! ��ȡ��ʾ�豸��оƬ����
  //! \param device ��ʾ�豸������
  //! \return оƬ������Ϣ
  const char *getDeviceChipType (unsigned device) const;

  //! ��ȡ��ʾ�豸���ڴ��С
  //! \param device ��ʾ�豸������
  //! \return �ڴ��С��Ϣ
  const char *getDeviceMemory (unsigned device) const;

  //! ��ȡ��ʾ�豸�ĵ�ǰ��ʾģʽ
  //! \param device ��ʾ�豸������
  //! ��ʾģʽ��Ϣ
  const char *getDeviceDisplayMode (unsigned device) const;

  //! ��ȡ��ʾ�豸��������������
  //! \param device ��ʾ�豸������
  //! \return ��������
  const char *getDeviceDriverName (unsigned device) const;

  //! ��ȡ��ʾ�豸�������汾
  //! \param device ��ʾ�豸������
  //! \return �����汾��Ϣ
  const char *getDeviceDriverVersion (unsigned device) const;

  // ��ȡ��ʾ�豸��������������
  //! \param device ��ʾ�豸������
  //! \return ����������Ϣ
  const char *getDeviceDriverDate (unsigned device) const;

  //! ��ȡ��ʾ�豸�ṩ������
  //! \param device ��ʾ�豸������
  //! \return �ṩ������
  unsigned getDeviceVendorId (unsigned device) const;

  //! ��ȡ�豸�Ĳ�Ʒ������
  //! \param device ��ʾ�豸������
  //! \return ��Ʒ������
  unsigned getDeviceId (unsigned device) const;

  //! ��ȡ�豸����ϵͳ����
  //! \param device ��ʾ�豸������
  //! \return ��ϵͳ����
  unsigned getDeviceSubSystemId (unsigned device) const;

  //! ��ȡ�豸���޶�����
  //! \param device ��ʾ�豸������
  //! \return �޶�����
  unsigned getDeviceRevisionId (unsigned device) const;

  //! ��ѯ�豸�Ƿ�֧��DDraw����
  //! \param device ��ʾ�豸������
  //! \return true֧�֣�false��֧��
  bool isDDrawAccelerationEnabled (unsigned device) const;

  //! ��ѯ�豸�Ƿ�֧��3D����
  //! \param device ��ʾ�豸������
  //! \return true֧�� false��֧��
  bool is3DAccelerationExists (unsigned device) const;

  //! ��ѯ�豸��3D����֧���Ƿ񱻴�
  //! \param device ��ʾ�豸������
  //! \return true�� false��֧��3D���ٻ���δ��3D���ٹ���
  bool is3DAccelerationEnabled (unsigned device) const;

  //! ��ѯ�豸�Ƿ����AGP�ڴ�
  //! \param device ��ʾ�豸������
  //! \return true�� falseû��
  bool isAGPExists (unsigned device) const;

  //! ��ѯ�豸��AGP�ڴ��Ƿ�����
  //! \param device ��ʾ�豸������
  //! \return true���� false��AGP�ڴ����δ��AGP�ڴ湦��
  bool isAGPEnabled (unsigned device) const;

  //! ��ѯ��ǰ��װ��DirectX�İ汾��Ϣ
  //! \return �汾��Ϣ
  //! \sa ATOM_DisplayInfo::DxVersionInfo
  const DxVersionInfo *getDxVersionInfo (void) const;

  //! ��ѯ��ǰ��װ��DirectX�汾�����ַ�����ʾ
  //! \return DirectX�汾�ַ���
  const char *getDXVersionString (void) const;

private:       // attributes
  ATOM_VECTOR<struct DisplayDeviceInfo*> devices;
  DxVersionInfo dxverion;
  mutable ATOM_STRING dxversionStr;
};

#endif // __ATOM_KERNEL_DISPLAYINFO_H_
/*! @} */
