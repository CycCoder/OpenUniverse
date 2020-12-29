/********************************************************************************
 *           Web Runtime for Application - Version 1.0.0.202012290004
 ********************************************************************************
 * Copyright (C) 2002-2021 by Tangram Team.   All Rights Reserved.
 * There are Three Key Features of Webruntime:
 * 1. Built-in Modern Web Browser: Independent Browser Window and Browser Window
 *    as sub windows of other windows are supported in the application process;
 * 2. DOM Plus: DOMPlus is a natural extension of the standard DOM system.
 *    It allows the application system to support a kind of generalized web pages,
 *    which are composed of standard DOM elements and binary components supported
 *    by the application system;
 * 3. JavaScript for Application: Similar to VBA in MS office, JavaScript will
 *    become a built-in programmable language in the application system, so that
 *    the application system can be expanded and developed for the Internet based
 *    on modern javscript/Web technology.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
 *
 * CONTACT INFORMATION:
 * mailto:tangramteam@outlook.com or mailto:sunhuizlz@yeah.net
 * https://www.tangram.dev
 *
 *******************************************************************************/

#pragma once
#include "../OfficeAddin.h"
#include "ExcelPlusEvents.h"
using namespace OfficePlus;
using namespace OfficePlus::ExcelPlus::ExcelPlusEvent;

namespace OfficePlus
{
	namespace ExcelPlus
	{
		class CExcelObject;
		class CExcelAddin;
		class CExcelWorkBook;
		class CExcelWorkSheet;
		typedef map<HWND, CExcelObject*> ExcelWorkBookWndMap;
		typedef ExcelWorkBookWndMap::iterator ExcelWorkBookWndMapIT;

		class CExcelWorkBook : 
			public COfficeDocument,
			public CExcelWorkbookEvents,
			public map<_Worksheet*, CExcelWorkSheet*>
		{
		public:
			CExcelWorkBook(void);
			~CExcelWorkBook(void);
			BOOL						m_bCreating;
			CString						m_strDefaultSheetXml;
			_Workbook*					m_pWorkBook;
			ExcelWorkBookWndMap			m_mapExcelWorkBookWnd;

			CXobj*						m_pSheetNode;
			map<CString, CString>		m_mapWorkSheetInfo;

			void InitWorkBook();
			//void ModifySheetForTangram(IDispatch* Sh, CString strSheetXml, CString strTaskPaneXml);
			//void AddPropertyToSheet()
		private:
			void __stdcall OnNewSheet(IDispatch* Sh);
			void __stdcall OnSheetActivate(IDispatch* Sh);
			void __stdcall OnSheetBeforeDelete(IDispatch* Sh);
			void __stdcall OnBeforeSave(VARIANT_BOOL SaveAsUI, VARIANT_BOOL* Cancel);
		};

		class CExcelWorkSheet : public map<CString, CString>
		{
		public:
			CExcelWorkSheet(void);
			~CExcelWorkSheet(void);
			CString m_strKey;
			CString m_strSheetName;
			_Worksheet* m_pSheet;
			map<CString, CXobj*> m_mapXobjMap;
		};
	}
}
