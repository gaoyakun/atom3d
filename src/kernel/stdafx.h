#ifndef __ATOM3D_KERNEL_STDAFX_H
#define __ATOM3D_KERNEL_STDAFX_H

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN	
// ���������ʹ��������ָ����ƽ̨֮ǰ��ƽ̨�����޸�����Ķ��塣
// �йز�ͬƽ̨����Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER				// ����ʹ�� Windows 95 �� Windows NT 4 ����߰汾���ض����ܡ�
#define WINVER 0x0400		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_WINNT		// ����ʹ�� Windows NT 4 ����߰汾���ض����ܡ�
#define _WIN32_WINNT 0x0501		//Ϊ Windows98 �� Windows 2000 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif						

#ifndef _WIN32_WINDOWS		// ����ʹ�� Windows 98 ����߰汾���ض����ܡ�
#define _WIN32_WINDOWS 0x0410 //Ϊ Windows Me �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#ifndef _WIN32_IE			// ����ʹ�� IE 4.0 ����߰汾���ض����ܡ�
#define _WIN32_IE 0x0400	//Ϊ IE 5.0 �����°汾�ı�Ϊ�ʵ���ֵ��
#endif

#include <windows.h>
#include <tchar.h>
#include <dxdiag.h>
#include <initguid.h>


#endif



#include <string>
#include <cstdlib>
#include <stack>
#include <ctime>
#include <time.h>
#include <stdlib.h>
#include <cstdio>
#include <ctype.h>
#include <process.h>
#include <vector>
#include <map>
#include <stdio.h>
#include <malloc.h>
#include <algorithm>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <signal.h>
#include <psapi.h>
#include <tlhelp32.h>

#include <ATOM_dbghlp.h>
#include "ATOM_kernel.h"
#include "profiler.h"

#endif // __ATOM3D_KERNEL_STDAFX_H
