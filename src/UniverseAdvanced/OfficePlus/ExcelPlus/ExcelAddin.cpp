/********************************************************************************
 *           Web Runtime for Application - Version 1.0.0.202104110061
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

#include "../../stdafx.h"
#include "../../UniverseApp.h"
#include "../../XobjWnd.h"
#include "../../Xobj.h"
#include "../../Galaxy.h"
#include "../../TangramHtmlTreeWnd.h"
#include "fm20.h"
#include "ExcelAddin.h"
#include "ExcelPlusWnd.h"
#include "ExcelPlusEvents.cpp"

namespace OfficePlus
{
	namespace ExcelPlus
	{
		CExcelAddin::CExcelAddin() :COfficeAddin()
		{
			m_pExcelAppCtrl = nullptr;
			m_pActiveExcelObject = nullptr;
			m_pExcelAppObjEvents = nullptr;
			m_bOldVer = false;
			CString strVer = theApp.GetFileVer();
			int nPos = strVer.Find(_T("."));
			strVer = strVer.Left(nPos);
			int nVer = _wtoi(strVer);
			if (nVer < 15)
				m_bOldVer = true;
		}

		CExcelAddin::~CExcelAddin()
		{
			if (m_pExcelAppObjEvents)
				delete m_pExcelAppObjEvents;
		}

		void CExcelAddin::AddDocXml(IDispatch* pDocdisp, BSTR bstrXml, BSTR bstrKey)
		{
			CComQIPtr<_Workbook> pDoc(pDocdisp);
			if (pDoc)
			{
				CComPtr<_CustomXMLParts> pCustomXMLParts;
				pDoc->get_CustomXMLParts(&pCustomXMLParts);
				_AddDocXml(pCustomXMLParts.p, bstrXml, bstrKey);
			}
		}

		STDMETHODIMP CExcelAddin::StartApplication(BSTR bstrAppID, BSTR bstrXml)
		{
			CString strID = OLE2T(bstrAppID);
			strID.MakeLower();
			if (strID.Find(_T("excel.application")) == 0)
			{
				auto it = m_mapValInfo.find(_T("exceldesignstate"));
				if (it == m_mapValInfo.end())
					m_mapValInfo[_T("exceldesignstate")] = CComVariant((VARIANT_BOOL)true);
				return put_AppKeyValue(CComBSTR(L"doctemplate"), CComVariant(bstrXml));
			}
			return CCosmos::StartApplication(bstrAppID, bstrXml);
		}

		STDMETHODIMP CExcelAddin::AttachObjEvent(IDispatch* pDisp, int nEventIndex)
		{
			if (m_pExcelAppObjEvents == nullptr)
			{
				m_pExcelAppObjEvents = new CExcelAppObjEvents();
				m_pExcelAppObjEvents->DispEventAdvise(m_pExcelApplication);
			}
			return CCosmos::AttachObjEvent(pDisp, nEventIndex);
		}

		CString CExcelAddin::GetDocXmlByKey(IDispatch* pDocdisp, BSTR bstrKey)
		{
			CComQIPtr<_Workbook> pDoc(pDocdisp);
			if (pDoc)
			{
				CComPtr<_CustomXMLParts> pCustomXMLParts;
				pDoc->get_CustomXMLParts(&pCustomXMLParts);
				CString strRet = _GetDocXmlByKey(pCustomXMLParts.p, bstrKey);
				return strRet;
			}
			return _T("");
		}

		void CExcelAddin::OnWorkBookActivate(CExcelObject* pExcelObject)
		{
			CExcelWorkBook* pWorkBook = pExcelObject->m_pWorkBook;
			if (pWorkBook == nullptr)
				return;
			HWND hWnd = (HWND)pExcelObject->m_hForm;
			if (pWorkBook->m_pGalaxy)
				pWorkBook->m_pGalaxy->ModifyHost((LONGLONG)pExcelObject->m_hChildClient);
			if (pWorkBook->m_pTaskPaneGalaxy)
			{
				if (pExcelObject->m_hTaskPane)
				{
					pWorkBook->m_pTaskPaneGalaxy->ModifyHost((LONGLONG)pExcelObject->m_hTaskPane);
				}
				else
					pWorkBook->m_pTaskPaneGalaxy->ModifyHost((LONGLONG)pExcelObject->m_hTaskPaneChildWnd);
			}

			if (pExcelObject->m_strActiveSheetName != _T(""))
			{
				CString strName = pExcelObject->m_strActiveSheetName;
				auto it = pWorkBook->m_mapWorkSheetInfo.find(strName);
				CString strXml = _T("");
				if (it != pWorkBook->m_mapWorkSheetInfo.end())
				{
					strXml = it->second;
				}
				else
				{
					strXml = pWorkBook->m_mapWorkSheetInfo[_T("Default")];
				}

				IXobj* pXobj = nullptr;
				pWorkBook->m_pGalaxy->Observe(strName.AllocSysString(), CComBSTR(strXml), &pXobj);
			}
		}

		CString CExcelAddin::GetFormXml(CString strFormName)
		{
			CExcelWorkBook* pWorkBook = m_pActiveExcelObject->m_pWorkBook;
			auto it = pWorkBook->m_mapUserFormScript.find(strFormName);
			if (it != pWorkBook->m_mapUserFormScript.end())
				return it->second;
			return _T("");
		}

		void CExcelAddin::OnOpenDoc(WPARAM wParam)
		{
			CExcelObject* pObj = (CExcelObject*)wParam;
			if (pObj)
			{
				pObj->OnOpenDoc();
			}
		}

		void CExcelAddin::SetFocus(HWND hWnd)
		{
			if (m_pActiveExcelObject == nullptr)
				return;
			if (hWnd == m_pActiveExcelObject->m_hExcelEdit)
			{
				m_pActiveXobj = nullptr;
				if (::IsWindowVisible(m_pActiveExcelObject->m_hChildClient) == FALSE)
				{
					::PostMessage(m_pActiveExcelObject->m_hChildClient, WM_SETWNDFOCUSE, 0, 0);
				}
			}
		}

		void CExcelAddin::ProcessMsg(LPMSG lpMsg)
		{
			if (m_pActiveExcelObject == nullptr)
				return;

			switch (lpMsg->message)
			{
			case WM_LBUTTONDOWN:
			case WM_RBUTTONDOWN:
			case WM_POINTERDOWN:
			{
				::GetClassName(lpMsg->hwnd, g_pCosmos->m_szBuffer, MAX_PATH);
				CString strClassName = CString(g_pCosmos->m_szBuffer);
				if (strClassName.Find(_T("Afx:ToolBar:")) == 0)
				{
					ATLTRACE(_T("Afx:ToolBar:%x\n"), lpMsg->hwnd);
					if (::GetWindowLong(::GetParent(lpMsg->hwnd), GWL_STYLE) & WS_POPUP)
					{
						TranslateMessage(lpMsg);
						DispatchMessage(lpMsg);//
						break;
					}
				}
				if (m_pActiveXobj == nullptr)
					::SetFocus(lpMsg->hwnd);
			}
			m_pActiveExcelObject->ProcessMouseDownMsg(lpMsg->hwnd);
			break;
			case WM_SETWNDFOCUSE:
				if (lpMsg->wParam)
				{
					if (::IsWindowEnabled(m_pActiveExcelObject->m_hExcelEdit))
						::EnableWindow(m_pActiveExcelObject->m_hExcelEdit, false);
					::SendMessage((HWND)lpMsg->wParam, WM_MOUSEACTIVATE, (WPARAM)::GetActiveWindow(), 0);
				}
				else
				{
					::ShowWindow(m_pActiveExcelObject->m_hExcelEdit2, SW_HIDE);
				}

				break;
			}
		}

		void CExcelAddin::OnVbaFormInit(HWND hWnd, CGalaxy* pGalaxy)
		{
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)m_pActiveExcelObject->m_pWorkBook);
		}

		void CExcelAddin::UpdateOfficeObj(IDispatch* pObj, CString strXml, CString _strName)
		{
			CComQIPtr<MSForm::_UserForm> pForm(pObj);
			if (pForm)
			{
				CComPtr<_Workbook> pWb;
				m_pExcelApplication->get_ActiveWorkbook(&pWb);
				if (pWb)
				{
					CComPtr<_CustomXMLParts> pCustomXMLParts;
					pWb->get_CustomXMLParts(&pCustomXMLParts);
					CString strName = _T("");
					CTangramXmlParse m_Parse;
					if (m_Parse.LoadXml(_GetDocXmlByKey(pCustomXMLParts.p, CComBSTR(L"tangram"))))
					{
						DISPID dispID = 0x80010000;
						DISPPARAMS dispParams = { NULL, NULL, 0, 0 };
						VARIANT result = { 0 };
						EXCEPINFO excepInfo;
						memset(&excepInfo, 0, sizeof excepInfo);
						UINT nArgErr = (UINT)-1;
						HRESULT hr = pObj->Invoke(dispID, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &dispParams, &result, &excepInfo, &nArgErr);
						if (S_OK == hr && VT_BSTR == result.vt)
						{
							CComBSTR bstrName = result.bstrVal;
							strName = OLE2T(bstrName);
							::VariantClear(&result);
						}

						CTangramXmlParse* pParse = m_Parse.GetChild(_T("userforms"));
						if (pParse)
						{
							CTangramXmlParse* pParse2 = pParse->GetChild(strName);
							CString strXml2 = _T("");
							if (pParse2)
							{
								CTangramXmlParse* pParse3 = pParse2->GetChild(TGM_CLUSTER);
								CString strGUID = GetNewGUID();
								pParse2->RemoveNode(pParse3);
								pParse2->put_text(strGUID);
								strXml2 = m_Parse.xml();
								strXml2.Replace(strGUID, strXml);
							}
							else
							{
								strXml2.Format(_T("<%s>%s</%s>"), strName, strXml, strName);
								CTangramXmlParse m_Parse2;
								m_Parse2.LoadXml(strXml2);
								pParse->AddNode(&m_Parse2, _T(""));
								strXml2 = m_Parse.xml();
							}
							AddDocXml(pWb, CComBSTR(strXml2), CComBSTR(L"tangram"));
						}
					}
				}
				return;
			}
			CComQIPtr<_Worksheet> pSheet(pObj);
			if (pSheet)
			{
				CComPtr<CustomProperties> pCustomProperties;
				pSheet->get_CustomProperties(&pCustomProperties);
				ICustomProperties* pProps = (ICustomProperties*)pCustomProperties.p;
				long nCount = 0;
				CComBSTR bstrName(L"");
				if (pProps == nullptr)
					return;
				pProps->get_Count(&nCount);
				for (int i = 1; i <= nCount; i++)
				{
					CComPtr<CustomProperty> pProp;
					pProps->get_Item(CComVariant(i), &pProp);
					ICustomProperty* _pProp = (ICustomProperty*)pProp.p;
					_pProp->get_Name(&bstrName);
					CString strName = OLE2T(bstrName);
					if (strName == _T("Tangram"))
					{
						CComVariant var;
						_pProp->get_Value(&var);
						CString _strXml = OLE2T(var.bstrVal);
						::VariantClear(&var);
						CString strKey = _T("</") + _strName + _T(">");
						CString strName = _T("");
						int nPos = _strXml.Find(_T(" "));
						strName = _strXml.Left(nPos);
						nPos = _strXml.Find(strKey);
						CString str1 = _strXml.Mid(nPos);
						strKey = _T("<") + _strName + _T(" ");
						CString str2 = _T("");
						nPos = _strXml.Find(strKey);
						if (nPos == -1)
						{
							strKey = _T("<") + _strName + _T(">");
							nPos = _strXml.Find(strKey);
							str2 = _strXml.Left(nPos);
							str2 += strKey;
						}
						else
						{
							str2 = _strXml.Left(nPos);
							str2 += _T("<") + _strName + _T(">");
						}
						_strXml = str2 + strXml + str1;
						//Fix Bug for xml 20190112
						nPos = _strXml.Find(strName);
						if (nPos != -1)
							_strXml = _strXml.Mid(nPos);
						_pProp->put_Value(CComVariant(_strXml));
						return;
					}
				}
			}

			if (pObj)
			{
				CComBSTR szMember(L"Parent");
				DISPID dispid = -1;
				HRESULT hr = pObj->GetIDsOfNames(IID_NULL, &szMember, 1, LOCALE_USER_DEFAULT, &dispid);
				if (hr == S_OK)
				{
					DISPPARAMS dispParams = { NULL, NULL, 0, 0 };
					VARIANT result = { 0 };
					EXCEPINFO excepInfo;
					memset(&excepInfo, 0, sizeof excepInfo);
					UINT nArgErr = (UINT)-1;
					HRESULT hr = pObj->Invoke(dispid, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &dispParams, &result, &excepInfo, &nArgErr);
					if (S_OK == hr && VT_DISPATCH == result.vt)
					{
						CComQIPtr<Excel::_Workbook> pWb(result.pdispVal);
						if (pWb)
						{
							CComPtr<Office::_CustomXMLParts> pCustomXMLParts;
							pWb->get_CustomXMLParts(&pCustomXMLParts);
							CTangramXmlParse m_Parse;
							if (m_Parse.LoadXml(_GetDocXmlByKey(pCustomXMLParts.p, CComBSTR(L"tangram"))))
							{
								CTangramXmlParse* pParse = m_Parse.GetChild(_T("default"));
								if (pParse)
								{
									pParse = pParse->GetChild(_T("default"));
									if (pParse)
									{
										CTangramXmlParse* pParse2 = pParse->GetChild(_strName);
										if (pParse2)
										{
											CTangramXmlParse* pParse3 = pParse2->GetChild(TGM_CLUSTER);
											CString strGUID = GetNewGUID();
											pParse2->RemoveNode(pParse3);
											pParse2->put_text(strGUID);
											CString str = m_Parse.xml();
											str.Replace(strGUID, strXml);
											AddDocXml(pWb, CComBSTR(str), CComBSTR(L"tangram"));
										}
									}
								}
							}
						}
					}
					::VariantClear(&result);
				}
			}

		}

		STDMETHODIMP CExcelAddin::CosmosCommand(IDispatch* RibbonControl)
		{
			if (m_spRibbonUI)
				m_spRibbonUI->Invalidate();
			CComBSTR bstrTag(L"");
			CComBSTR bstrID(L"");
			CComQIPtr<IRibbonControl> pIRibbonControl(RibbonControl);
			if (pIRibbonControl)
			{
				pIRibbonControl->get_Id(&bstrID);
				pIRibbonControl->get_Tag(&bstrTag);
				CString strTag = OLE2T(bstrTag);
			}

			if (m_pActiveExcelObject == nullptr)
				return S_OK;
			CString strTag = OLE2T(bstrTag);
			int nPos = strTag.Find(_T("TangramButton.Cmd"));
			strTag.Replace(_T("TangramButton.Cmd."), _T(""));
			int nCmdIndex = _wtoi(strTag);
			CExcelObject* pWnd = m_pActiveExcelObject;
			CExcelWorkBook* m_pWorkBook = pWnd->m_pWorkBook;
			switch (nCmdIndex)
			{
			case 100:
			{
			}
			break;
			case 101:
				//case 102:
			{
				auto iter = m_mapTaskPaneMap.find((long)m_pActiveExcelObject->m_hForm);
				if (iter != m_mapTaskPaneMap.end())
					iter->second->put_Visible(true);
				else
				{
					IDispatch* pDisp = nullptr;
					m_pWorkBook->m_pWorkBook->get_ActiveSheet(&pDisp);
					CComQIPtr<_Worksheet> pSheet(pDisp);

					CString strName = _T("");
					CString strXml = m_pWorkBook->m_strDefaultSheetXml;
					if (pSheet)
					{
						CComPtr<CustomProperties> pCustomProperties;
						pSheet->get_CustomProperties(&pCustomProperties);
						ICustomProperties* pProps = (ICustomProperties*)pCustomProperties.p;
						long nCount = 0;
						CComBSTR bstrName(L"");
						pProps->get_Count(&nCount);
						for (int i = 1; i <= nCount; i++)
						{
							CComPtr<CustomProperty> pProp;
							pProps->get_Item(CComVariant(i), &pProp);
							ICustomProperty* _pProp = (ICustomProperty*)pProp.p;
							_pProp->get_Name(&bstrName);
							strName = OLE2T(bstrName);
							if (strName == _T("Tangram"))
							{
								CComVariant var;
								_pProp->get_Value(&var);
								strXml = OLE2T(var.bstrVal);
								::VariantClear(&var);
								break;
							}
						}
					}
					if (strXml == _T(""))
						return 0;
					CTangramXmlParse m_Parse;
					if (m_Parse.LoadXml(strXml))
					{
						CTangramXmlParse* pParse = m_Parse.GetChild(_T("default"));
						if (pParse)
						{
							pParse = pParse->GetChild(_T("taskpane"));
							if (pParse)
							{
								strName += _T("_taskpane");
								_CustomTaskPane* _pCustomTaskPane = nullptr;
								CString strSheetName = _T("");
								auto it = m_mapTaskPaneMap.find((long)m_pActiveExcelObject->m_hForm);
								if (it == m_mapTaskPaneMap.end())
								{
									VARIANT vWindow;
									vWindow.vt = VT_DISPATCH;
									vWindow.pdispVal = nullptr;
									HRESULT hr = m_pCTPFactory->CreateCTP(L"Tangram.Ctrl.1", m_pWorkBook->m_strTaskPaneTitle.AllocSysString(), vWindow, &_pCustomTaskPane);
									_pCustomTaskPane->AddRef();
									m_mapTaskPaneMap[(long)m_pActiveExcelObject->m_hForm] = _pCustomTaskPane;
									CComPtr<ICosmosCtrl> pCtrlDisp;
									_pCustomTaskPane->get_ContentControl((IDispatch**)&pCtrlDisp);
									if (pCtrlDisp)
									{
										LONGLONG hWnd = 0;
										pCtrlDisp->get_HWND(&hWnd);
										pWnd->m_hTaskPane = (HWND)hWnd;
										if (m_pWorkBook->m_pTaskPaneGalaxyCluster == nullptr)
										{
											HWND hPWnd = ::GetParent((HWND)hWnd);
											CWindow m_Wnd;
											m_Wnd.Attach(hPWnd);
											m_Wnd.ModifyStyle(0, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
											m_Wnd.Detach();
											m_pWorkBook->m_pTaskPaneGalaxyCluster = new CComObject<CGalaxyCluster>();
											m_pWorkBook->m_pTaskPaneGalaxyCluster->m_hWnd = hPWnd;
											g_pCosmos->m_mapWindowPage[hPWnd] = m_pWorkBook->m_pTaskPaneGalaxyCluster;
											IGalaxy* pTaskPaneFrame = nullptr;
											m_pWorkBook->m_pTaskPaneGalaxyCluster->CreateGalaxy(CComVariant(0), CComVariant((long)hWnd), CComBSTR(L"TaskPane"), &pTaskPaneFrame);
											m_pWorkBook->m_pTaskPaneGalaxy = (CGalaxy*)pTaskPaneFrame;
											if (m_pWorkBook->m_pTaskPaneGalaxy)
											{
												IXobj* pXobj = nullptr;
												m_pWorkBook->m_pTaskPaneGalaxy->Observe(CComBSTR(strName), pParse->xml().AllocSysString(), &pXobj);
												CXobj* _pXobj = (CXobj*)pXobj;
												if (_pXobj->m_pXobjShareData->m_pOfficeObj == nullptr && pDisp)
												{
													_pXobj->m_pXobjShareData->m_pOfficeObj = pDisp;
													_pXobj->m_pXobjShareData->m_pOfficeObj->AddRef();
												}
											}
											_pCustomTaskPane->put_DockPosition(m_pWorkBook->m_nMsoCTPDockPosition);
											_pCustomTaskPane->put_DockPositionRestrict(m_pWorkBook->m_nMsoCTPDockPositionRestrict);
											_pCustomTaskPane->put_Width(m_pWorkBook->m_nWidth);
											_pCustomTaskPane->put_Height(m_pWorkBook->m_nHeight);
											_pCustomTaskPane->put_Visible(true);
										}
										else
											m_pWorkBook->m_pTaskPaneGalaxy->ModifyHost(hWnd);
									}
								}
								else
								{
									_pCustomTaskPane = it->second;
									_pCustomTaskPane->put_Visible(true);
									if (m_pWorkBook->m_pTaskPaneGalaxy)
									{
										IXobj* pXobj = nullptr;
										m_pWorkBook->m_pTaskPaneGalaxy->Observe(CComBSTR(strName), pParse->xml().AllocSysString(), &pXobj);
									}
								}
							}
						}
					}
				}
				if (nCmdIndex == 102)
				{
					CGalaxy* pGalaxy = m_pWorkBook->m_pTaskPaneGalaxy;
					if (pGalaxy == nullptr)return S_OK;
				}
			}
			break;
			case 102:
			{
			}
			break;
			}
			CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
			pObj->Init(_T("ExcelPlus"), 0, m_pWorkBook->m_pWorkBook);
			pObj->m_mapVar[0] = CComVariant(bstrID);
			pObj->m_mapVar[1] = CComVariant(bstrTag);

			g_pCosmos->FireAppEvent(pObj);

			return S_OK;
		}

		HRESULT CExcelAddin::OnConnection(IDispatch* pHostApp, int ConnectMode)
		{
			COfficeAddin::OnConnection(pHostApp, ConnectMode);
			if (m_pExcelApplication == nullptr)
			{
				pHostApp->QueryInterface(__uuidof(IDispatch), (LPVOID*)&m_pExcelApplication);
			}
			if (m_strRibbonXml == _T(""))
			{
				CTangramXmlParse m_Parse;
				if (m_Parse.LoadXml(m_strConfigFile) || m_Parse.LoadFile(m_strConfigFile))
				{
					m_strRibbonXml = m_Parse[_T("RibbonUI")][_T("customUI")].xml();
				}
			}
			if (m_pExcelAppObjEvents != nullptr)
			{
				m_pExcelAppObjEvents->DispEventAdvise(m_pExcelApplication);
			}
			if (g_pCosmos->m_hForegroundIdleHook == NULL)
				g_pCosmos->m_hForegroundIdleHook = SetWindowsHookEx(WH_FOREGROUNDIDLE, CUniverse::ForegroundIdleProc, NULL, ::GetCurrentThreadId());
			BSTR bstrCap = ::SysAllocString(L"");
			m_pExcelApplication->get_Caption(&bstrCap);
			CString strCaption = OLE2T(bstrCap);
			strCaption += _T(" Plus");
			m_pExcelApplication->put_Caption(strCaption.AllocSysString());
			::SysFreeString(bstrCap);
			return S_OK;
		}

		STDMETHODIMP CExcelAddin::GetCustomUI(BSTR RibbonID, BSTR* RibbonXml)
		{
			if (!RibbonXml)
				return E_POINTER;
			*RibbonXml = m_strRibbonXml.AllocSysString();
			return (*RibbonXml ? S_OK : E_OUTOFMEMORY);
		}

		void CExcelAddin::WindowCreated(CString strClassName, LPCTSTR strName, HWND hPWnd, HWND hWnd)
		{
			if (strClassName == _T("EXCEL7"))
			{
				m_pActiveExcelObject = new CComObject<CExcelObject>;
				m_pActiveExcelObject->m_hClient = hPWnd;
				m_pActiveExcelObject->m_hChildClient = hWnd;
				m_pActiveExcelObject->m_hForm = ::GetParent(hPWnd);
				m_pActiveExcelObject->m_hExcelEdit = ::FindWindowEx(::GetParent(hPWnd), NULL, _T("EXCEL<"), NULL);;
				m_pActiveExcelObject->m_hExcelEdit2 = ::FindWindowEx(hPWnd, NULL, _T("EXCEL6"), NULL);
				m_mapOfficeObjects[hWnd] = m_pActiveExcelObject;
				m_mapOfficeObjects2[m_pActiveExcelObject->m_hForm] = m_pActiveExcelObject;
				::PostMessage(m_hCosmosWnd, WM_OFFICEOBJECTCREATED, (WPARAM)hWnd, 0);
			}
		}

		void CExcelAddin::WindowDestroy(HWND hWnd)
		{
			::GetClassName(hWnd, m_szBuffer, MAX_PATH);
			CString strClassName = CString(m_szBuffer);
			if (strClassName == _T("ThunderDFrame"))
			{
				CExcelWorkBook* pWorkBook = (CExcelWorkBook*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
				if (pWorkBook)
				{

				}
				auto it = m_mapVBAForm.find(hWnd);
				if (it != m_mapVBAForm.end())
					m_mapVBAForm.erase(it);
			}
			else if (strClassName == _T("EXCEL="))
			{
				m_pActiveExcelObject->SheetNameChanged();
			}
			else if (strClassName == _T("EXCEL7"))
			{
				OnCloseOfficeObj(strClassName, hWnd);
				return;
			}
		}

		STDMETHODIMP CExcelAddin::ExportOfficeObjXml(IDispatch* OfficeObject, BSTR* bstrXml)
		{
			return S_OK;
		}

		STDMETHODIMP CExcelAddin::put_AppKeyValue(BSTR bstrKey, VARIANT newVal)
		{
			CString strKey = OLE2T(bstrKey);

			if (strKey == _T(""))
				return S_OK;
			strKey.Trim();
			strKey.MakeLower();
			if (strKey == _T("doctemplate"))
			{
				auto it = g_pCosmos->m_mapValInfo.find(_T("doctemplate"));
				if (it != g_pCosmos->m_mapValInfo.end())
				{
					::VariantClear(&it->second);
					g_pCosmos->m_mapValInfo.erase(it);
				}
				CComPtr<Workbooks> pWorkBooks;
				m_pExcelApplication->get_Workbooks(&pWorkBooks);
				if (pWorkBooks)
				{
					CString strXml = OLE2T(newVal.bstrVal);
					g_pCosmos->m_mapValInfo[strKey] = newVal;
					CComPtr<_Workbook> pWorkbook;
					CComVariant varTemplate(L"");
					pWorkBooks->Add(varTemplate, 0, &pWorkbook);
				}
			}
			return CCosmos::put_AppKeyValue(bstrKey, newVal);
		}

		HRESULT CExcelAddin::CreateCosmosCtrl(void* pv, REFIID riid, LPVOID* ppv)
		{
			return CExcelAppCtrl::_CreatorClass::CreateInstance(pv, riid, ppv);
		}

		bool CExcelAddin::OnActiveOfficeObj(HWND hWnd)
		{
			auto it = m_mapOfficeObjects2.find(hWnd);
			if (it != m_mapOfficeObjects2.end())
			{
				if (m_pActiveExcelObject != it->second)
				{
					m_pActiveExcelObject = (CExcelObject*)it->second;
				}
				OnWorkBookActivate((CExcelObject*)it->second);
				return true;
			}
			return false;
		}

		void CExcelAddin::ConnectOfficeObj(HWND hWnd)
		{
			if (m_pExcelApplication == nullptr)
				return;
			auto it = m_mapOfficeObjects.find(hWnd);
			CExcelObject* pExcelObj = (CExcelObject*)it->second;
			Excel::_Workbook* pWorkBook = nullptr;
			m_pExcelApplication->get_ActiveWorkbook(&pWorkBook);
			pExcelObj->m_pOfficeObj = pWorkBook;
			pExcelObj->m_pOfficeObj->AddRef();
			pExcelObj->m_hTaskPaneWnd = ::CreateWindowEx(NULL, _T("Tangram Remote Helper Window"), L"", WS_CHILD, 0, 0, 0, 0, pExcelObj->m_hChildClient, NULL, theApp.m_hInstance, NULL);
			pExcelObj->m_hTaskPaneChildWnd = ::CreateWindowEx(NULL, _T("Tangram Remote Helper Window"), L"", WS_CHILD, 0, 0, 0, 0, (HWND)pExcelObj->m_hTaskPaneWnd, NULL, theApp.m_hInstance, NULL);
			bool bNewWindow = false;
			CExcelWorkBook* _pWorkBook = nullptr;
			auto it1 = find(pWorkBook);
			if (it1 != end())
			{
				_pWorkBook = it1->second;
				bNewWindow = true;
			}
			else
			{
				_pWorkBook = new CExcelWorkBook();
				(*this)[pWorkBook] = _pWorkBook;
				HRESULT hr = _pWorkBook->DispEventAdvise(pWorkBook);
				_pWorkBook->m_pWorkBook = pWorkBook;
				_pWorkBook->m_pWorkBook->AddRef();
			}
			pExcelObj->m_pWorkBook = _pWorkBook;
			_pWorkBook->m_mapExcelWorkBookWnd[pExcelObj->m_hChildClient] = pExcelObj;
			if (bNewWindow)
				return;
			CComBSTR bstrPath(L"");
			pWorkBook->get_Path(0, &bstrPath);
			CString strPath = OLE2T(bstrPath);
			if (strPath == _T(""))
			{
				CString strTemplate = GetDocTemplateXml(_T("Please select WorkBook Template:"), m_strExeName, _T(""));
				CTangramXmlParse m_Parse;
				if (m_Parse.LoadXml(strTemplate) == false && m_Parse.LoadFile(strTemplate) == false)
					return;

				CComPtr<Sheets> pSheets;
				pWorkBook->get_Sheets(&pSheets);
				CComPtr<IDispatch> pActive;
				pWorkBook->get_ActiveSheet(&pActive);
				_pWorkBook->m_bCreating = TRUE;
				_pWorkBook->m_strTaskPaneTitle = m_Parse.attr(_T("title"), _T("TaskPane"));
				_pWorkBook->m_nWidth = m_Parse.attrInt(_T("width"), 200);
				_pWorkBook->m_nHeight = m_Parse.attrInt(_T("height"), 300);
				_pWorkBook->m_nMsoCTPDockPosition = (MsoCTPDockPosition)m_Parse.attrInt(_T("dockposition"), 4);
				_pWorkBook->m_nMsoCTPDockPositionRestrict = (MsoCTPDockPositionRestrict)m_Parse.attrInt(_T("dockpositionrestrict"), 3);
				vector<CTangramXmlParse*> vec;
				int nCount = m_Parse.GetCount();
				for (int i = nCount - 1; i >= 0; i--)
				{
					CTangramXmlParse* pChild = m_Parse.GetChild(i);
					CString strName = pChild->name();
					if (strName.CompareNoCase(_T("userforms")) && strName.CompareNoCase(_T("default")))
					{
						CString strType = pChild->attr(_T("type"), _T(""));
						XlSheetType nType = xlWorksheet;
						if (strType.CompareNoCase(_T("Chart")) == 0)
						{
							nType = xlChart;
						}
						CTangramXmlParse* pChild2 = pChild->GetChild(_T("default"));
						if (pChild2)
						{
							pChild2 = pChild2->GetChild(_T("sheet"));
							CComPtr<IDispatch> pDisp;
							if (pActive && nType == xlWorksheet)
							{
								pDisp.Attach(pActive);
								pActive.Detach();
							}
							else
								pSheets->Add(vtMissing, vtMissing, vtMissing, CComVariant(nType), 0, &pDisp);

							CComQIPtr<_Worksheet> pSh(pDisp);
							CString _strName = pChild2->attr(_T("caption"), _T(""));
							if (_strName == _T(""))
								_strName = pChild2->name();

							if (pSh)
							{
								pSh->put_Name(_strName.AllocSysString());
								CComPtr<CustomProperties> _pProps;
								pSh->get_CustomProperties(&_pProps);
								ICustomProperties* pProps = (ICustomProperties*)_pProps.p;
								CComPtr<CustomProperty> pProp;
								pProps->Add(CComBSTR(L"Tangram"), CComVariant(pChild->xml()), &pProp);
								vec.push_back(pChild);

								auto it = pExcelObj->m_pWorkBook->find(pSh.p);
								if (it == pExcelObj->m_pWorkBook->end())
								{
									CExcelWorkSheet* pExcelWorkSheet = new CExcelWorkSheet();
									pExcelWorkSheet->m_strSheetName = _strName;
									pExcelWorkSheet->m_pSheet = pSh.p;
									pExcelWorkSheet->m_pSheet->AddRef();
									(*_pWorkBook)[pExcelWorkSheet->m_pSheet] = pExcelWorkSheet;

									int nCount = pChild->GetCount();
									for (int i = 0; i < nCount; i++)
									{
										CTangramXmlParse* _pChild = pChild->GetChild(i);
										(*pExcelWorkSheet)[_pChild->name()] = _pChild->xml();
									}
								}
							}
							else
							{
								CComQIPtr<_Chart> pChart(pDisp);
								if (pChart)
								{
									pChart->put_Name(_strName.AllocSysString());
								}
							}
						}
					}
					else if (strName.CompareNoCase(_T("userforms")) == 0)
					{
						int nCount = pChild->GetCount();
						for (int i = 0; i < nCount; i++)
						{
							CTangramXmlParse* _pChild = pChild->GetChild(i);
							_pWorkBook->m_mapUserFormScript[_pChild->name()] = _pChild->xml();
						}
					}
					else
					{
						if (pChild)
						{
							_pWorkBook->m_strDefaultSheetXml = pChild->xml();
						}
					}
				}

				while (vec.size())
				{
					m_Parse.RemoveNode(*vec.begin());
					vec.erase(vec.begin());
				}

				_pWorkBook->m_strDocXml = m_Parse.xml();
				AddDocXml(pWorkBook, CComBSTR(_pWorkBook->m_strDocXml), CComBSTR(L"tangram"));
				_pWorkBook->m_bCreating = FALSE;
				//::PostMessage(m_hHostWnd, WM_OPENDOCUMENT, (WPARAM)pExcelObj, 0);
			}
			else
			{
				_pWorkBook->InitWorkBook();
			}
		}

		CExcelObject::CExcelObject(void)
		{
			m_hExcelEdit = NULL;
			m_hExcelEdit2 = NULL;
			m_pWorkBook = nullptr;
		}

		CExcelObject::~CExcelObject(void)
		{
		}

		void CExcelObject::SheetNameChanged()
		{
			CComPtr<IDispatch> pDisp;
			m_pWorkBook->m_pWorkBook->get_ActiveSheet(&pDisp);
			CString strName = g_pCosmos->GetPropertyFromObject(pDisp, _T("Name"));
			if (strName != m_strActiveSheetName)
			{
				ATLTRACE(_T("namechanged: oleName: %s, newName:%s\n"), m_strActiveSheetName, strName);
			}
		}

		void CExcelObject::OnOpenDoc()
		{
			if (m_pWorkBook->m_strDocXml == _T(""))
				return;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;

			auto it = g_pCosmos->m_mapWindowPage.find(m_hClient);
			if (it != g_pCosmos->m_mapWindowPage.end())
				m_pWorkBook->m_pDocGalaxyCluster = (CGalaxyCluster*)it->second;
			else
			{
				m_pWorkBook->m_pDocGalaxyCluster = new CComObject<CGalaxyCluster>();
				m_pWorkBook->m_pDocGalaxyCluster->m_hWnd = m_hClient;
				g_pCosmos->m_mapWindowPage[m_hClient] = m_pWorkBook->m_pDocGalaxyCluster;
			}

			if (m_pWorkBook->m_pDocGalaxyCluster)
			{
				IGalaxy* pGalaxy = nullptr;
				m_pWorkBook->m_pDocGalaxyCluster->CreateGalaxy(CComVariant(0), CComVariant((long)m_hChildClient), CComBSTR(L"Document"), &pGalaxy);
				m_pWorkBook->m_pGalaxy = (CGalaxy*)pGalaxy;
				if (m_pWorkBook->m_pGalaxy)
				{
					//if (m_bOldVer) 
					//	pExcelWorkBookWnd->m_pWorkBook->m_pGalaxy->ModifyHost((LONGLONG)m_hClientWnd);
					IDispatch* pDisp = nullptr;
					m_pWorkBook->m_pWorkBook->get_ActiveSheet(&pDisp);
					CComQIPtr<Excel::_Worksheet> pSheet(pDisp);

					CString strName = _T("");
					CString strXml = m_pWorkBook->m_strDefaultSheetXml;
					if (pSheet)
					{
						CComPtr<Excel::CustomProperties> pCustomProperties;
						pSheet->get_CustomProperties(&pCustomProperties);
						Excel::ICustomProperties* pProps = (Excel::ICustomProperties*)pCustomProperties.p;
						long nCount = 0;
						CComBSTR bstrName(L"");
						pProps->get_Count(&nCount);
						for (int i = 1; i <= nCount; i++)
						{
							CComPtr<Excel::CustomProperty> pProp;
							pProps->get_Item(CComVariant(i), &pProp);
							Excel::ICustomProperty* _pProp = (ICustomProperty*)pProp.p;
							_pProp->get_Name(&bstrName);
							strName = OLE2T(bstrName);
							if (strName == _T("Tangram"))
							{
								CComVariant var;
								_pProp->get_Value(&var);
								strXml = OLE2T(var.bstrVal);
								::VariantClear(&var);
								break;
							}
						}
					}

					if (strXml == _T(""))
						return;
					CTangramXmlParse m_Parse;
					if (m_Parse.LoadXml(strXml))
					{
						CTangramXmlParse* pParse = m_Parse.GetChild(_T("default"));
						if (pParse)
						{
							strName = m_Parse.name();
							CString strKey = strName;
							strName += _T("_default");
							CTangramXmlParse* pParse2 = pParse->GetChild(_T("sheet"));
							IXobj* pXobj = nullptr;
							m_pWorkBook->m_pGalaxy->Observe(strName.AllocSysString(), pParse2->xml().AllocSysString(), &pXobj);
							CXobj* _pXobj = (CXobj*)pXobj;
							if (_pXobj->m_pXobjShareData->m_pOfficeObj == nullptr && pDisp)
							{
								_pXobj->m_pXobjShareData->m_pOfficeObj = pDisp;
								_pXobj->m_pXobjShareData->m_pOfficeObj->AddRef();
							}
						}
						pParse = pParse->GetChild(_T("taskpane"));
						if (pParse)
						{
							strName += _T("_taskpane");
							_CustomTaskPane* _pCustomTaskPane = nullptr;
							CString strSheetName = _T("");
							auto it = pAddin->m_mapTaskPaneMap.find((long)m_hForm);
							if (it == pAddin->m_mapTaskPaneMap.end())
							{
								VARIANT vWindow;
								vWindow.vt = VT_DISPATCH;
								vWindow.pdispVal = nullptr;
								HRESULT hr = pAddin->m_pCTPFactory->CreateCTP(L"Tangram.Ctrl.1", m_pWorkBook->m_strTaskPaneTitle.AllocSysString(), vWindow, &_pCustomTaskPane);
								_pCustomTaskPane->AddRef();
								pAddin->m_mapTaskPaneMap[(long)m_hForm] = _pCustomTaskPane;
								CComPtr<ICosmosCtrl> pCtrlDisp;
								_pCustomTaskPane->get_ContentControl((IDispatch**)&pCtrlDisp);
								if (pCtrlDisp)
								{
									LONGLONG hWnd = 0;
									pCtrlDisp->get_HWND(&hWnd);
									m_hTaskPane = (HWND)hWnd;
									if (m_pWorkBook->m_pTaskPaneGalaxyCluster == nullptr)
									{
										HWND hPWnd = ::GetParent((HWND)hWnd);
										CWindow m_Wnd;
										m_Wnd.Attach(hPWnd);
										m_Wnd.ModifyStyle(0, WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
										m_Wnd.Detach();
										m_pWorkBook->m_pTaskPaneGalaxyCluster = new CComObject<CGalaxyCluster>();
										m_pWorkBook->m_pTaskPaneGalaxyCluster->m_hWnd = hPWnd;
										g_pCosmos->m_mapWindowPage[hPWnd] = m_pWorkBook->m_pTaskPaneGalaxyCluster;

										IGalaxy* pTaskPaneFrame = nullptr;
										m_pWorkBook->m_pTaskPaneGalaxyCluster->CreateGalaxy(CComVariant(0), CComVariant((long)hWnd), CComBSTR(L"TaskPane"), &pTaskPaneFrame);
										m_pWorkBook->m_pTaskPaneGalaxy = (CGalaxy*)pTaskPaneFrame;
										_pCustomTaskPane->put_DockPosition(m_pWorkBook->m_nMsoCTPDockPosition);
										_pCustomTaskPane->put_DockPositionRestrict(m_pWorkBook->m_nMsoCTPDockPositionRestrict);
										_pCustomTaskPane->put_Width(m_pWorkBook->m_nWidth);
										_pCustomTaskPane->put_Height(m_pWorkBook->m_nHeight);
										_pCustomTaskPane->put_Visible(true);
										if (m_pWorkBook->m_pTaskPaneGalaxy)
										{
											IXobj* pXobj = nullptr;
											m_pWorkBook->m_pTaskPaneGalaxy->Observe(CComBSTR(strName), pParse->xml().AllocSysString(), &pXobj);
											CXobj* _pXobj = (CXobj*)pXobj;
											if (_pXobj->m_pXobjShareData->m_pOfficeObj == nullptr && pDisp)
											{
												_pXobj->m_pXobjShareData->m_pOfficeObj = pDisp;
												_pXobj->m_pXobjShareData->m_pOfficeObj->AddRef();
											}
										}
									}
									else
										m_pWorkBook->m_pTaskPaneGalaxy->ModifyHost(hWnd);
								}
							}
							else
							{
								_pCustomTaskPane = it->second;
								_pCustomTaskPane->put_Visible(true);
								if (m_pWorkBook->m_pTaskPaneGalaxy)
								{
									IXobj* pXobj = nullptr;
									m_pWorkBook->m_pTaskPaneGalaxy->Observe(CComBSTR(strName), pParse->xml().AllocSysString(), &pXobj);
								}
							}
						}
					}
				}
			}
		}

		void CExcelObject::ProcessMouseDownMsg(HWND hWnd)
		{
			HWND hActiveWnd = ::GetActiveWindow();
			if (hActiveWnd == m_hForm)
			{
				if (g_pCosmos->m_pActiveXobj && (::IsChild(g_pCosmos->m_pActiveXobj->m_pHostWnd->m_hWnd, hWnd) || hWnd == g_pCosmos->m_pActiveXobj->m_pHostWnd->m_hWnd))
				{
					if (g_pCosmos->m_pActiveXobj->m_nViewType == Grid || g_pCosmos->m_pActiveXobj->m_nViewType == TangramTreeView)
						return;
					if (::IsWindowEnabled(m_hExcelEdit))
						::EnableWindow(m_hExcelEdit, false);
					if (::IsWindowVisible(m_hExcelEdit2) || ::IsWindowEnabled(m_hExcelEdit2))
					{
						::PostMessage(m_hChildClient, WM_KEYDOWN, VK_TAB, 0);
						::PostMessage(m_hChildClient, WM_KEYDOWN, VK_LEFT, 0);
						::EnableWindow(m_hExcelEdit2, false);
						::PostMessage(g_pCosmos->m_pActiveXobj->m_pHostWnd->m_hWnd, WM_SETWNDFOCUSE, (WPARAM)hWnd, (LPARAM)m_hChildClient);
					}
				}
				else
				{
					::EnableWindow(m_hExcelEdit, true);
					::EnableWindow(m_hExcelEdit2, true);
					if (::IsWindowVisible(m_hChildClient) == false && g_pCosmos->m_pActiveXobj)
					{
						::PostMessage(g_pCosmos->m_pActiveXobj->m_pHostWnd->m_hWnd, WM_SETWNDFOCUSE, (WPARAM)hWnd, (LPARAM)m_hChildClient);
					}
				}
			}
			else
			{
				::EnableWindow(m_hExcelEdit, false);
				if (::IsWindowVisible(m_hExcelEdit2))
				{
					::PostMessage(m_hChildClient, WM_KEYDOWN, VK_TAB, 0);
					::PostMessage(m_hChildClient, WM_KEYDOWN, VK_LEFT, 0);
					::EnableWindow(m_hExcelEdit2, false);
				}
			}
		}

		void CExcelObject::OnObjDestory()
		{
			if (m_pOfficeObj != nullptr)
			{
				CExcelAddin* pAddin = ((CExcelAddin*)g_pCosmos);

				if (pAddin->m_pActiveExcelObject == this)
				{
					pAddin->m_pActiveExcelObject = nullptr;
					g_pCosmos->m_pActiveXobj = nullptr;
				}

				auto it2 = m_pWorkBook->m_mapExcelWorkBookWnd.find(m_hChildClient);
				if (it2 != m_pWorkBook->m_mapExcelWorkBookWnd.end())
				{
					m_pWorkBook->m_mapExcelWorkBookWnd.erase(it2);
				}

				size_t nCount = m_pWorkBook->m_mapExcelWorkBookWnd.size();
				if (nCount > 0 && m_pWorkBook->m_pGalaxy)
				{
					it2 = m_pWorkBook->m_mapExcelWorkBookWnd.begin();
					m_pWorkBook->m_pGalaxy->ModifyHost((LONGLONG)::GetParent(it2->second->m_hChildClient));
					if (m_hTaskPane)
					{
						m_hTaskPane = NULL;
						m_pWorkBook->m_pTaskPaneGalaxy->ModifyHost((LONGLONG)it2->second->m_hTaskPaneChildWnd);
						::DestroyWindow(m_hTaskPaneWnd);
					}
				}

				auto it = pAddin->m_mapTaskPaneMap.find((long)m_hForm);
				if (it != pAddin->m_mapTaskPaneMap.end())
				{
					it->second->Delete();
					pAddin->m_mapTaskPaneMap.erase(it);
				}
				if (nCount == 0)
				{
					for (auto it : *m_pWorkBook)
					{
						delete it.second;
					}
					m_pWorkBook->erase(m_pWorkBook->begin(), m_pWorkBook->end());
					m_pWorkBook->DispEventUnadvise(m_pWorkBook->m_pWorkBook);
					auto it2 = pAddin->find(m_pWorkBook->m_pWorkBook);
					if (it2 != pAddin->end())
						pAddin->erase(it2);
					ULONG dw = m_pWorkBook->m_pWorkBook->Release();
					while (dw > 1)
						dw = m_pWorkBook->m_pWorkBook->Release();

					delete m_pWorkBook;
				}
				auto it3 = pAddin->m_mapOfficeObjects2.find(m_hForm);
				if (it3 != pAddin->m_mapOfficeObjects2.end())
					pAddin->m_mapOfficeObjects2.erase(it3);
			}
		}

		CExcelAppCtrl::CExcelAppCtrl()
		{
			OutputDebugString(_T("------------------Create CExcelAppCtrl------------------------\n"));
		}

		void CExcelAppCtrl::OnFinalMessage(HWND hWnd)
		{
			__super::OnFinalMessage(hWnd);
		}

		STDMETHODIMP CExcelAppCtrl::get_tag(VARIANT* pVal)
		{
			return S_OK;
		}


		STDMETHODIMP CExcelAppCtrl::put_tag(VARIANT newVal)
		{
			return S_OK;
		}

		STDMETHODIMP CExcelAppCtrl::put_AppCtrl(VARIANT_BOOL newVal)
		{
			g_pCosmos->m_pCosmosAppCtrl = this;

			return S_OK;
		}

		STDMETHODIMP CExcelAppCtrl::get_HWND(LONGLONG* pVal)
		{
			*pVal = (long)m_hWnd;
			return S_OK;
		}

		STDMETHODIMP CExcelAppCtrl::get_Cosmos(ICosmos** pVal)
		{
			*pVal = g_pCosmos;
			return S_OK;
		}

		LRESULT ExcelPlus::CExcelAppCtrl::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&)
		{
			return DefWindowProc(uMsg, wParam, lParam);
		}

		void CExcelAppObjEvents::OnNewWorkbook(_Workbook* Wb)
		{
			int nIndex = 0x0000061d;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("NewWorkbook"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetSelectionChange(IDispatch* Sh, Excel::Range* Target)
		{
			int nIndex = 0x00000616;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetSelectionChange"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Target;
					pObj->m_mapDisp[1]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetBeforeDoubleClick(IDispatch* Sh, Excel::Range* Target, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000617;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetBeforeDoubleClick"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var;
					var.vt = VT_BOOL | VT_BYREF;
					var.pboolVal = Cancel;
					pObj->m_mapVar[0] = var;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetBeforeRightClick(IDispatch* Sh, Excel::Range* Target, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000618;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetBeforeRightClick"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Target;
					pObj->m_mapDisp[1]->AddRef();
					CComVariant var;
					var.vt = VT_BOOL | VT_BYREF;
					var.pboolVal = Cancel;
					pObj->m_mapVar[0] = var;

					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetActivate(IDispatch* Sh)
		{
			int nIndex = 0x00000619;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetActivate"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetDeactivate(IDispatch* Sh)
		{
			int nIndex = 0x0000061a;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetDeactivate"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetCalculate(IDispatch* Sh)
		{
			int nIndex = 0x0000061b;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetCalculate"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetChange(IDispatch* Sh, Excel::Range* Target)
		{
			int nIndex = 0x0000061c;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetChange"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Target;
					pObj->m_mapDisp[1]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookOpen(_Workbook* Wb)
		{
			int nIndex = 0x0000061f;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = g_pCosmos->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != g_pCosmos->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookOpen"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookActivate(_Workbook* Wb)
		{
			int nIndex = 0x00000620;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookActivate"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookDeactivate(_Workbook* Wb)
		{
			int nIndex = 0x00000621;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookDeactivate"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookBeforeClose(_Workbook* Wb, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000622;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookBeforeClose"), nIndex, pAddin->m_pExcelApplication.p);
					CComVariant var;
					var.vt = VT_BOOL | VT_BYREF;
					var.pboolVal = Cancel;
					pObj->m_mapVar[0] = var;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookBeforeSave(_Workbook* Wb, VARIANT_BOOL SaveAsUI, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000623;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookBeforeSave"), nIndex, pAddin->m_pExcelApplication.p);
					CComVariant var;
					var.vt = VT_BOOL;
					var.boolVal = SaveAsUI;
					pObj->m_mapVar[0] = var;
					CComVariant var1;
					var1.vt = VT_BOOL | VT_BYREF;
					var1.pboolVal = Cancel;
					pObj->m_mapVar[1] = var1;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookBeforePrint(_Workbook* Wb, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000624;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookBeforePrint"), nIndex, pAddin->m_pExcelApplication.p);
					CComVariant var1;
					var1.vt = VT_BOOL | VT_BYREF;
					var1.pboolVal = Cancel;
					pObj->m_mapVar[0] = var1;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookNewSheet(_Workbook* Wb, IDispatch* Sh)
		{
			int nIndex = 0x00000625;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookNewSheet"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Sh;
					pObj->m_mapDisp[1]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookAddinInstall(_Workbook* Wb)
		{
			int nIndex = 0x00000626;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookAddinInstall"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookAddinUninstall(_Workbook* Wb)
		{
			int nIndex = 0x00000627;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookAddinUninstall"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWindowResize(_Workbook* Wb, Excel::Window* Wn)
		{
			int nIndex = 0x00000612;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WindowResize"), nIndex, pAddin->m_pExcelApplication.p);

					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Wn;
					pObj->m_mapDisp[1]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWindowActivate(_Workbook* Wb, Excel::Window* Wn)
		{
			int nIndex = 0x00000614;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WindowActivate"), nIndex, pAddin->m_pExcelApplication.p);

					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Wn;
					pObj->m_mapDisp[1]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWindowDeactivate(_Workbook* Wb, Excel::Window* Wn)
		{
			int nIndex = 0x00000615;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WindowDeactivate"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Wn;
					pObj->m_mapDisp[1]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetFollowHyperlink(IDispatch* Sh, Excel::Hyperlink* Target)
		{
			int nIndex = 0x0000073e;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetFollowHyperlink"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Target;
					pObj->m_mapDisp[1]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetPivotTableUpdate(IDispatch* Sh, PivotTable* Target)
		{
			int nIndex = 0x0000086d;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetPivotTableUpdate"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Target;
					pObj->m_mapDisp[1]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookPivotTableCloseConnection(_Workbook* Wb, PivotTable* Target)
		{
			int nIndex = 0x00000870;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookPivotTableCloseConnection"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Target;
					pObj->m_mapDisp[1]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookPivotTableOpenConnection(_Workbook* Wb, PivotTable* Target)
		{
			int nIndex = 0x00000871;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookPivotTableOpenConnection"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Target;
					pObj->m_mapDisp[1]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookSync(_Workbook* Wb, MsoSyncEventType SyncEvent)
		{
			int nIndex = 0x000008f1;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookSync"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var;
					var.vt = VT_INT;
					var.intVal = SyncEvent;
					pObj->m_mapVar[0] = var;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookBeforeXmlImport(_Workbook* Wb, XmlMap* Map, BSTR Url, VARIANT_BOOL IsRefresh, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x000008f2;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookBeforeXmlImport"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Map;
					pObj->m_mapDisp[1]->AddRef();
					CComVariant var;
					var.vt = VT_BSTR;
					var.bstrVal = Url;
					pObj->m_mapVar[0] = var;
					CComVariant var1;
					var1.vt = VT_BOOL;
					var1.boolVal = IsRefresh;
					pObj->m_mapVar[1] = var1;
					CComVariant var2;
					var2.vt = VT_BOOL | VT_BYREF;
					var2.pboolVal = Cancel;
					pObj->m_mapVar[2] = var2;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookAfterXmlImport(_Workbook* Wb, XmlMap* Map, VARIANT_BOOL IsRefresh, XlXmlImportResult Result)
		{
			int nIndex = 0x000008f3;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookAfterXmlImport"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Map;
					pObj->m_mapDisp[1]->AddRef();
					CComVariant var1;
					var1.vt = VT_BOOL;
					var1.boolVal = IsRefresh;
					pObj->m_mapVar[0] = var1;
					CComVariant var2;
					var2.vt = VT_INT;
					var2.intVal = Result;
					pObj->m_mapVar[1] = var2;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookBeforeXmlExport(_Workbook* Wb, XmlMap* Map, BSTR Url, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x000008f4;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookBeforeXmlExport"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Map;
					pObj->m_mapDisp[1]->AddRef();
					CComVariant var;
					var.vt = VT_BSTR;
					var.bstrVal = Url;
					pObj->m_mapVar[0] = var;
					CComVariant var2;
					var2.vt = VT_BOOL | VT_BYREF;
					var2.pboolVal = Cancel;
					pObj->m_mapVar[1] = var2;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookAfterXmlExport(_Workbook* Wb, XmlMap* Map, BSTR Url, XlXmlExportResult Result)
		{
			int nIndex = 0x000008f5;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookAfterXmlExport"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Map;
					pObj->m_mapDisp[1]->AddRef();
					CComVariant var;
					var.vt = VT_BSTR;
					var.bstrVal = Url;
					pObj->m_mapVar[0] = var;
					CComVariant var2;
					var2.vt = VT_INT;
					var2.intVal = Result;
					pObj->m_mapVar[1] = var2;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookRowsetComplete(_Workbook* Wb, BSTR Description, BSTR Sheet, VARIANT_BOOL Success)
		{
			int nIndex = 0x00000a33;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookRowsetComplete"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var;
					var.vt = VT_BSTR;
					var.bstrVal = Description;
					pObj->m_mapVar[0] = var;
					CComVariant var1;
					var1.vt = VT_BSTR;
					var1.bstrVal = Sheet;
					pObj->m_mapVar[1] = var1;
					CComVariant var2;
					var2.vt = VT_BOOL;
					var2.boolVal = Success;
					pObj->m_mapVar[2] = var2;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnAfterCalculate()
		{
			int nIndex = 0x00000a34;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("AfterCalculate"), nIndex, pAddin->m_pExcelApplication.p);
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetPivotTableAfterValueChange(IDispatch* Sh, PivotTable* TargetPivotTable, Excel::Range* TargetRange)
		{
			int nIndex = 0x00000b4f;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetPivotTableAfterValueChange"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = TargetPivotTable;
					pObj->m_mapDisp[1]->AddRef();
					pObj->m_mapDisp[2] = TargetRange;
					pObj->m_mapDisp[2]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetPivotTableBeforeAllocateChanges(IDispatch* Sh, PivotTable* TargetPivotTable, long ValueChangeStart, long ValueChangeEnd, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000b50;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetPivotTableBeforeAllocateChanges"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = TargetPivotTable;
					pObj->m_mapDisp[1]->AddRef();
					CComVariant var;
					var.vt = VT_I4;
					var.lVal = ValueChangeStart;
					pObj->m_mapVar[0] = var;
					CComVariant var1;
					var1.vt = VT_I4;
					var1.lVal = ValueChangeEnd;
					pObj->m_mapVar[1] = var1;
					CComVariant var2;
					var2.vt = VT_BOOL | VT_BYREF;
					var2.pboolVal = Cancel;
					pObj->m_mapVar[2] = var2;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetPivotTableBeforeCommitChanges(IDispatch* Sh, PivotTable* TargetPivotTable, long ValueChangeStart, long ValueChangeEnd, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000b51;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetPivotTableBeforeCommitChanges"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = TargetPivotTable;
					pObj->m_mapDisp[1]->AddRef();
					CComVariant var;
					var.vt = VT_I4;
					var.lVal = ValueChangeStart;
					pObj->m_mapVar[0] = var;
					CComVariant var1;
					var1.vt = VT_I4;
					var1.lVal = ValueChangeEnd;
					pObj->m_mapVar[1] = var1;
					CComVariant var2;
					var2.vt = VT_BOOL | VT_BYREF;
					var2.pboolVal = Cancel;
					pObj->m_mapVar[2] = var2;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetPivotTableBeforeDiscardChanges(IDispatch* Sh, PivotTable* TargetPivotTable, long ValueChangeStart, long ValueChangeEnd)
		{
			int nIndex = 0x00000b52;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetPivotTableBeforeDiscardChanges"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = TargetPivotTable;
					pObj->m_mapDisp[1]->AddRef();
					CComVariant var;
					var.vt = VT_I4;
					var.lVal = ValueChangeStart;
					pObj->m_mapVar[0] = var;
					CComVariant var1;
					var1.vt = VT_I4;
					var1.lVal = ValueChangeEnd;
					pObj->m_mapVar[1] = var1;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnProtectedViewWindowOpen(Excel::ProtectedViewWindow* Pvw)
		{
			int nIndex = 0x00000b57;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("ProtectedViewWindowOpen"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Pvw;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnProtectedViewWindowBeforeEdit(Excel::ProtectedViewWindow* Pvw, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000b59;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("ProtectedViewWindowBeforeEdit"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Pvw;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var2;
					var2.vt = VT_BOOL | VT_BYREF;
					var2.pboolVal = Cancel;
					pObj->m_mapVar[0] = var2;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnProtectedViewWindowBeforeClose(Excel::ProtectedViewWindow* Pvw, XlProtectedViewCloseReason Reason, VARIANT_BOOL* Cancel)
		{
			int nIndex = 0x00000b5a;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("ProtectedViewWindowBeforeClose"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Pvw;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var;
					var.vt = VT_I4;
					var.lVal = Reason;
					pObj->m_mapVar[0] = var;
					CComVariant var2;
					var2.vt = VT_BOOL | VT_BYREF;
					var2.pboolVal = Cancel;
					pObj->m_mapVar[1] = var2;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnProtectedViewWindowResize(Excel::ProtectedViewWindow* Pvw)
		{
			int nIndex = 0x00000b5c;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("ProtectedViewWindowResize"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Pvw;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnProtectedViewWindowActivate(Excel::ProtectedViewWindow* Pvw)
		{
			int nIndex = 0x00000b5d;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("ProtectedViewWindowActivate"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Pvw;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnProtectedViewWindowDeactivate(Excel::ProtectedViewWindow* Pvw)
		{
			int nIndex = 0x00000b5e;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("ProtectedViewWindowDeactivate"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Pvw;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookAfterSave(_Workbook* Wb, VARIANT_BOOL Success)
		{
			int nIndex = 0x00000b5f;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookAfterSave"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					CComVariant var;
					var.vt = VT_BOOL;
					var.boolVal = Success;
					pObj->m_mapVar[0] = var;
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookNewChart(_Workbook* Wb, Excel::Chart* Ch)
		{
			int nIndex = 0x00000b60;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookNewChart"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = (_Chart*)Ch;
					pObj->m_mapDisp[1]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetLensGalleryRenderComplete(IDispatch* Sh)
		{
			int nIndex = 0x00000c03;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetLensGalleryRenderComplete"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetTableUpdate(IDispatch* Sh, TableObject* Target)
		{
			int nIndex = 0x00000c04;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetTableUpdate"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Target;
					pObj->m_mapDisp[1]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnWorkbookModelChange(_Workbook* Wb, ModelChanges* Changes)
		{
			int nIndex = 0x00000c08;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("WorkbookModelChange"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Wb;
					pObj->m_mapDisp[0]->AddRef();
					pObj->m_mapDisp[1] = Changes;
					pObj->m_mapDisp[1]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}

		void CExcelAppObjEvents::OnSheetBeforeDelete(IDispatch* Sh)
		{
			int nIndex = 0x00000c07;
			CExcelAddin* pAddin = (CExcelAddin*)g_pCosmos;
			auto it2 = pAddin->m_mapObjEventDic.find(pAddin->m_pExcelApplication.p);
			if (it2 != pAddin->m_mapObjEventDic.end())
			{
				CString strEventIndexs = it2->second;
				CString strIndex = _T("");
				strIndex.Format(_T(",%d,"), nIndex);
				if (strEventIndexs.Find(strIndex) != -1)
				{
					CCosmosEvent* pObj = new CComObject<CCosmosEvent>;
					pObj->Init(_T("SheetBeforeDelete"), nIndex, pAddin->m_pExcelApplication.p);
					pObj->m_mapDisp[0] = Sh;
					pObj->m_mapDisp[0]->AddRef();
					pAddin->FireAppEvent(pObj);
				}
			}
		}
	}
}

