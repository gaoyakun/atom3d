/**	\file send_error.h
 *	ATOM_ErrorSender�������.
 *
 *	\author ������
 *	\addtogroup dbghlp
 *	@{
 */

#ifndef __ATOM3D_DBGHLP_SEND_ERROR_H
#define __ATOM3D_DBGHLP_SEND_ERROR_H

#pragma comment(lib, "wininet.lib")

#include "basedefs.h"

//! \class ATOM_ErrorSender
//! ���ʹ��󱨸�֧����
//! ATOM_ErrorSender�ṩ�˽�ָ���ļ��ϴ����ض���FTP�������Ĺ���
//! \author ������
//! \ingroup dbghlp
class ATOM_DBGHLP_API ATOM_ErrorSender
{
public:
	//! ����FTP����������
	//! \param ftp_url FTP�������ĵ�ַ,ǰ�治Ҫ��"ftp://"ǰ׺
	//! \param port FTP�������˿ں�
	//! \param username FTP��������¼�û��������ΪNULL��ʹ��"anonymous"
	//! \param password FTP��������¼����
	//! \return true ���óɹ� false ��������
	static bool setFtpSite (const char *ftp_url, int port = 21, const char *username = 0, const char *password = 0);

	//! �����������
	//! \param smtp_svr SMTP�������ĵ�ַ
	//! \param port SMTP�������˿ں�
	//! \param username �����½�û���
	//! \param password �����½����
	//! \return true ���óɹ� false ��������
	static void setMailServer (const char *smtp_svr, int port, const char *username, const char *password);

	//! �ϴ��ļ����趨��FTP��������ĳ��Ŀ¼����
	//! \param fileName Ҫ���͵ı����ļ�·��
	//! \param destDir FTP�������ϵ�Ŀ¼���ƣ��ļ������ϴ�����Ŀ¼��
	//! \param text true��ʾҪ�����ı��ļ���false��ʾ���Ͷ������ļ�
	//! \return true ���ͳɹ� false ����ʧ��
	static bool sendFile (const char *fileName, const char *destDir, bool text);

	//! �ʼ����ʹ����ļ�
	static bool sendFileViaEmail (const char *fileName);
};

#endif // __ATOM3D_DBGHLP_SEND_ERROR_H

/*! @} */
