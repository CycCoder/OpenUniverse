/********************************************************************************
 *             DOM Plus for Application - Version 1.1.8.202012250001
 ********************************************************************************
 * Copyright (C) 2002-2021 by Tangram Team.   All Rights Reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
 *
 * CONTACT INFORMATION:
 * mailto:tangramteam@outlook.com or mailto:sunhuizlz@yeah.net
 * https://www.tangram.dev
 *
 *******************************************************************************/

#pragma once
#include "XZip.h"
namespace Utilities
{
	class CComponentInstaller
	{
	public:
		CComponentInstaller(void);
		~CComponentInstaller(void);

		CString m_strHostFile;
		//ѹ��һ���ļ�
		BOOL Zip(LPCTSTR lpszZipArchive, LPCTSTR lpszSrcFile);
		//��ѹѹ�����ڵ��ļ�
		void VerifyZip(HZIP hz, LPCTSTR lpszFile, LPCTSTR lpszPath);
		//�����ļ��Ƚ�
		BOOL Compare(LPCTSTR lpszFile1, LPCTSTR lpszFile2, BOOL *pbResult);
		//��ѹѹ�����ڵ��ļ���֧��һ���ļ�,����strName����,strName��·��
		BOOL UnZip(CString strZip, CString strName);
		//��ѹѹ�����ڵ��ļ���֧�ֶ���ļ�����������
		BOOL UnMultiZip(CString strZip, CString strPath);
		BOOL UnMultiZip2(CString strZip, CString strPath);
		BOOL InstallComponent(CString strXml);

		BOOL RegComDll(const CString& strDllLib);
		BOOL UnRegComDll(const CString& strDllLib);
		BOOL SetDirectory(CString strFile);
	};
}