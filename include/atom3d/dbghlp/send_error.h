/**	\file send_error.h
 *	ATOM_ErrorSender类的声明.
 *
 *	\author 高雅昆
 *	\addtogroup dbghlp
 *	@{
 */

#ifndef __ATOM3D_DBGHLP_SEND_ERROR_H
#define __ATOM3D_DBGHLP_SEND_ERROR_H

#pragma comment(lib, "wininet.lib")

#include "basedefs.h"

//! \class ATOM_ErrorSender
//! 发送错误报告支持类
//! ATOM_ErrorSender提供了将指定文件上传到特定的FTP服务器的功能
//! \author 高雅昆
//! \ingroup dbghlp
class ATOM_DBGHLP_API ATOM_ErrorSender
{
public:
	//! 设置FTP服务器参数
	//! \param ftp_url FTP服务器的地址,前面不要加"ftp://"前缀
	//! \param port FTP服务器端口号
	//! \param username FTP服务器登录用户名，如果为NULL则使用"anonymous"
	//! \param password FTP服务器登录密码
	//! \return true 设置成功 false 参数错误
	static bool setFtpSite (const char *ftp_url, int port = 21, const char *username = 0, const char *password = 0);

	//! 设置邮箱参数
	//! \param smtp_svr SMTP服务器的地址
	//! \param port SMTP服务器端口号
	//! \param username 邮箱登陆用户名
	//! \param password 邮箱登陆密码
	//! \return true 设置成功 false 参数错误
	static void setMailServer (const char *smtp_svr, int port, const char *username, const char *password);

	//! 上传文件到设定的FTP服务器的某个目录当中
	//! \param fileName 要发送的本地文件路径
	//! \param destDir FTP服务器上的目录名称，文件将被上传至此目录中
	//! \param text true表示要发送文本文件，false表示发送二进制文件
	//! \return true 发送成功 false 发送失败
	static bool sendFile (const char *fileName, const char *destDir, bool text);

	//! 邮件发送错误文件
	static bool sendFileViaEmail (const char *fileName);
};

#endif // __ATOM3D_DBGHLP_SEND_ERROR_H

/*! @} */
