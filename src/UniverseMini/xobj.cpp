/********************************************************************************
 *           Web Runtime for Application - Version 1.0.0.202102050024
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

// Grid.cpp : Implementation of CXobj

#include "stdafx.h"
#include "UniverseApp.h"
#include "Cosmos.h"
#include "XobjHelper.h"
#include "GridWnd.h"
#include "Xobj.h"
#include "Galaxy.h"
#include "CosmosEvents.h"
#include "universe.c"

CXobj::CXobj()
{
	g_pCosmos->m_nTangramObj++;
	m_nID = 0;
	m_nRow = 0;
	m_nCol = 0;
	m_nRows = 1;
	m_nCols = 1;
	m_nViewType = BlankView;
	m_bTopObj = false;
	m_bCreated = false;
	m_varTag.vt = VT_EMPTY;
	m_strKey = _T("");
	m_strURL = _T("");
	m_strNodeName = _T("");
	m_hHostWnd = NULL;
	m_hChildHostWnd = NULL;
	m_pDisp = nullptr;
	m_pHostGalaxy = nullptr;
	m_pRootObj = nullptr;
	m_pHostWnd = nullptr;
	m_pExtender = nullptr;
	m_pParentObj = nullptr;
	m_pObjClsInfo = nullptr;
	m_pCLREventConnector = nullptr;
	m_pChildNodeCollection = nullptr;
	m_pCurrentExNode = nullptr;
	m_pHostParse = nullptr;
	m_pXobjShareData = nullptr;
	g_pCosmos->m_pActiveXobj = this;
}


void CXobj::InitWndXobj()
{
	m_pParentWinFormWnd = nullptr;
	m_pXobjShareData = m_pRootObj->m_pXobjShareData;
	ASSERT(m_pXobjShareData != nullptr);
	m_nHeigh = m_pHostParse->attrInt(TGM_HEIGHT, 0);
	m_nWidth = m_pHostParse->attrInt(TGM_WIDTH, 0);

	m_strObjTypeID = m_pHostParse->attr(TGM_OBJ_ID, _T(""));
	if (m_strObjTypeID.CompareNoCase(TGM_NUCLEUS) == 0)
		m_strID = TGM_NUCLEUS;
	m_nRows = m_pHostParse->attrInt(TGM_ROWS, 0);
	m_nCols = m_pHostParse->attrInt(TGM_COLS, 0);
	if (m_nRows * m_nCols>1)
	{
		m_strID = _T("xobj");
		m_nViewType = Grid;
	}
	else
	{
		if (m_pHostParse->GetChild(TGM_XOBJ))
			m_nViewType = TabGrid;
		if (m_strID != TGM_NUCLEUS)
		{
			m_strID = m_pHostParse->attr(TGM_GRID_TYPE, _T(""));
			m_strID.MakeLower();
			m_strID.Trim();
		}
		if(m_strID==_T(""))
		{ 
			if (m_strObjTypeID.Find(_T(",")) != -1)
			{
				m_strID = _T("clrctrl");
				m_nViewType = CLRCtrl;
			}
		}
	}
	m_strName = m_pHostParse->attr(_T("id"), _T(""));
	if (m_strName == _T(""))
	{
		switch (m_nViewType)
		{
		case Grid:
		{
			if (this == m_pRootObj)
			{
				m_strName = _T("Grid");
			}
		}
		break;
		case TabGrid:
			if (this == m_pRootObj)
			{
				if (m_pObjClsInfo)
					m_strName = m_pObjClsInfo->m_lpszClassName;
				else
					m_strName = _T("Tabs");
			}
			break;
		case CLRCtrl:
			if (this == m_pRootObj)
			{
				int nPos = m_strObjTypeID.Find(_T(","));
				if (nPos != -1)
				{
					m_strName = m_strObjTypeID.Left(nPos);
					m_strName.Replace(_T("."), _T("_"));
				}
			}
			break;
		default:
			if (m_pObjClsInfo && this == m_pRootObj)
				m_strName = m_pObjClsInfo->m_lpszClassName;
			break;
		}
		if (m_strName == _T(""))
		{
			if (m_pParentObj)
			{
				CString _strName = m_pParentObj->m_strName;
				if (m_pParentObj->m_nViewType == Grid)
				{
					if (m_nViewType == CLRCtrl)
					{
						int nPos = m_strObjTypeID.Find(_T(","));
						if (nPos != -1)
						{
							CString _strCtrlName = m_strObjTypeID.Left(nPos);
							_strCtrlName.Replace(_T("."), _T("_"));
							m_strName.Format(_T("%s_%02d%02d_%s"), _strName, m_nRow, m_nCol, _strCtrlName);
						}
					}
					else
						m_strName.Format(_T("%s_%02d%02d"), _strName, m_nRow, m_nCol);
				}
				else
				{
					if (m_nViewType == CLRCtrl)
					{
						int nPos = m_strObjTypeID.Find(_T(","));
						if (nPos != -1)
						{
							CString _strCtrlName = m_strObjTypeID.Left(nPos);
							_strCtrlName.Replace(_T("."), _T("_"));
							m_strName.Format(_T("%s_%s%d"), _strName, _strCtrlName, m_nCol);
						}
					}
					else
						m_strName.Format(_T("%s_Page%d"), _strName, m_nCol);
				}
			}
			else
				m_strName.Format(_T("Xobj_%p"), (LONGLONG)this);
		}
		m_pHostParse->put_attr(_T("id"), m_strName);
	}
	m_strObjTypeID.MakeLower();
	m_strObjTypeID.Trim();
	m_pRootObj->m_mapChildXobj[m_strName] = this;
	m_nActivePage = m_pHostParse->attrInt(TGM_ACTIVE_PAGE, 0);
	m_strCaption = m_pHostParse->attr(TGM_CAPTION, _T(""));
	if (m_pXobjShareData->m_pGalaxy && m_pXobjShareData->m_pGalaxy->m_pGalaxyCluster)
	{
		m_strNodeName = m_strName + _T("@") + g_pCosmos->m_strCurrentKey + _T("@") + m_pXobjShareData->m_pGalaxy->m_strGalaxyName;
		auto it2 = m_pXobjShareData->m_pGalaxyCluster->m_mapXobj.find(m_strNodeName);
		if (it2 == m_pXobjShareData->m_pGalaxyCluster->m_mapXobj.end())
		{
			m_pXobjShareData->m_pGalaxyCluster->m_mapXobj[m_strNodeName] = this;
		}
	}

	auto it = g_pCosmos->m_TabWndClassInfoDictionary.find(m_strID);
	if (it != g_pCosmos->m_TabWndClassInfoDictionary.end())
		m_pObjClsInfo = it->second;
	else
		m_pObjClsInfo = RUNTIME_CLASS(CXobjHelper);

	for (auto it : g_pCosmos->m_mapCosmosAppProxy)
	{
		CXobjProxy* pCosmosWndXobjProxy = it.second->OnXobjInit(this);
		if (pCosmosWndXobjProxy)
			m_mapWndXobjProxy[it.second] = pCosmosWndXobjProxy;
	}
}

CXobj::~CXobj()
{
	if (g_pCosmos->m_pActiveXobj == this)
		g_pCosmos->m_pActiveXobj = nullptr;
	if (m_pXobjShareData->m_pOldGalaxy)
		m_pXobjShareData->m_pGalaxy = m_pXobjShareData->m_pOldGalaxy;
	CGalaxy * pGalaxy = m_pXobjShareData->m_pGalaxy;
	if (pGalaxy->m_pGalaxyCluster)
	{
		auto it = pGalaxy->m_pGalaxyCluster->m_mapXobj.find(m_strNodeName);
		if (it != pGalaxy->m_pGalaxyCluster->m_mapXobj.end())
			pGalaxy->m_pGalaxyCluster->m_mapXobj.erase(it);
	}
	if (pGalaxy->m_pWorkXobj == this)
		pGalaxy->m_pWorkXobj = nullptr;
	if (m_strKey != _T(""))
	{
		auto it = pGalaxy->m_mapXobj.find(m_strKey);
		if (it != pGalaxy->m_mapXobj.end())
		{
			//BOOL bDeleteFrame = FALSE;
			pGalaxy->m_mapXobj.erase(it);
			if (pGalaxy->m_mapXobj.size() == 0)
			{
				if (::IsWindow(pGalaxy->m_hWnd))
				{
					pGalaxy->UnsubclassWindow(true);
					pGalaxy->m_hWnd = NULL;
				}
				delete pGalaxy;
			}
			delete m_pXobjShareData;
			m_pXobjShareData = nullptr;
		}
	}

	if (m_pCLREventConnector)
	{
		delete m_pCLREventConnector;
		m_pCLREventConnector = nullptr;
	}

	g_pCosmos->m_nTangramObj--;
#ifdef _DEBUG
#endif
	HRESULT hr = S_OK;
	DWORD dw = 0;

	if (m_pExtender)
	{
		dw = m_pExtender->Release();
		m_pExtender = nullptr;
	}

	if (m_nViewType != TangramTreeView && m_nViewType != Grid && m_pDisp)
		CCommonFunction::ClearObject<IUnknown>(m_pDisp);

	m_vChildNodes.clear();

	if (m_pChildNodeCollection != nullptr)
	{
		dw = m_pChildNodeCollection->Release();
		while (dw)
			dw = m_pChildNodeCollection->Release();
		m_pChildNodeCollection = nullptr;
	}
	for (auto it : m_mapWndXobjProxy)
	{
		if (it.second->m_bAutoDelete)
			delete it.second;
	}
	m_mapWndXobjProxy.clear();
	ATLTRACE(_T("delete CXobj:%x\n"), this);
}

CString CXobj::GetNames()
{
	return _T("");
}

void CXobj::NodeCreated() 
{
	CosmosInfo* pInfo = new CosmosInfo();
	pInfo->m_pXobj = this;
	pInfo->m_pGalaxy = this->m_pXobjShareData->m_pGalaxy;
	pInfo->m_pGalaxyCluster = this->m_pXobjShareData->m_pGalaxyCluster;
	pInfo->m_strName = this->m_strName;
	pInfo->m_strNodeName = this->m_strNodeName;
	::SetProp(m_pHostWnd->m_hWnd, _T("CosmosInfo"), pInfo);
}

BOOL CXobj::PreTranslateMessage(MSG * pMsg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (m_pHostWnd && pMsg->message != WM_MOUSEMOVE)
	{
		m_pHostWnd->PreTranslateMessage(pMsg);

		for (auto it : m_vChildNodes)
		{
			it->PreTranslateMessage(pMsg);
		}
	}
	return true;
}

STDMETHODIMP CXobj::LoadXML(int nType, BSTR bstrXML)
{
	return S_OK;
}

STDMETHODIMP CXobj::ActiveTabPage(IXobj * _pXobj)
{
	g_pCosmos->m_pActiveXobj = this;
	HWND hWnd = m_pHostWnd->m_hWnd;
	if (::IsWindow(hWnd))
	{
		::SetFocus(hWnd);
		m_pXobjShareData->m_pGalaxy->HostPosChanged();
	}
	return S_OK;
}

STDMETHODIMP CXobj::GetChildXobjByName(BSTR bstrName, IXobj** ppXobj)
{
	CString strName = OLE2T(bstrName);
	auto it = m_pRootObj->m_mapChildXobj.find(strName);
	if (it != m_pRootObj->m_mapChildXobj.end())
	{
		*ppXobj = it->second;
		return S_OK;
	}
	return S_FALSE;
}

STDMETHODIMP CXobj::Observe(BSTR bstrKey, BSTR bstrXml, IXobj * *ppRetXobj)
{
	switch (m_nViewType)
	{
	case CLRCtrl:
	case BlankView:
	{
		if (m_nViewType == BlankView)
		{
			if (m_pParentObj && m_pParentObj->m_nViewType == Grid)
			{
				HRESULT hr =  m_pParentObj->ObserveEx(m_nRow, m_nCol, bstrKey, bstrXml, ppRetXobj);
				return hr;
			}
		}
		if (m_pXobjShareData->m_pGalaxyCluster)
		{
			if (m_nViewType == BlankView && m_pParentObj && m_pParentObj->m_nViewType == Grid)
			{
				return m_pParentObj->ObserveEx(m_nRow, m_nCol, bstrKey, bstrXml, ppRetXobj);
			}
			if (m_pHostGalaxy == nullptr)
			{
				CString strName = m_strNodeName;
				strName += _T("_Frame");

				if (m_nViewType == BlankView)
				{
					RECT rc;
					::GetClientRect(m_pHostWnd->m_hWnd, &rc);
					m_hHostWnd = ::CreateWindowEx(NULL, L"Cosmos Xobj Class", NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, rc.right, rc.bottom, m_pHostWnd->m_hWnd, NULL, AfxGetInstanceHandle(), NULL);
				}
				else
				{
					m_hHostWnd = ::GetWindow(m_pHostWnd->m_hWnd, GW_CHILD);
				}
				IGalaxy* pGalaxy = nullptr;
				m_pXobjShareData->m_pGalaxyCluster->CreateGalaxy(CComVariant(0), CComVariant((long)m_hHostWnd), strName.AllocSysString(), &pGalaxy);
				if (pGalaxy)
					m_pHostGalaxy = (CGalaxy*)pGalaxy;
			}
			if (m_pHostGalaxy && ::IsWindow(m_hHostWnd))
			{
				m_pHostGalaxy->m_pContainerNode = this;
				if (m_pXobjShareData->m_pGalaxy->m_pParentXobj)
				{
					m_pHostGalaxy->m_pParentXobj = m_pXobjShareData->m_pGalaxy->m_pParentXobj;
				}
				HRESULT hr = m_pHostGalaxy->Observe(bstrKey, bstrXml, ppRetXobj);
				return hr;
			}
		}
	}
	break;
	case Grid:
		break;
	}
	return S_OK;
}

STDMETHODIMP CXobj::ObserveEx(int nRow, int nCol, BSTR bstrKey, BSTR bstrXml, IXobj * *ppRetXobj)
{
	if (m_pXobjShareData->m_pGalaxyCluster && m_nViewType == Grid)
	{
		IXobj* pXobj = nullptr;
		GetXobj(nRow, nCol, &pXobj);
		if (pXobj == nullptr)
			return S_OK;
		CXobj * pWndXobj = (CXobj*)pXobj;
		if (pWndXobj->m_pHostGalaxy == nullptr)
		{
			CString strName = pWndXobj->m_strNodeName;
			strName += _T("_Frame");

			::SetWindowLong(pWndXobj->m_pHostWnd->m_hWnd, GWL_ID, 1);
			IGalaxy* pGalaxy = nullptr;
			m_pXobjShareData->m_pGalaxyCluster->CreateGalaxy(CComVariant(0), CComVariant((long)pWndXobj->m_pHostWnd->m_hWnd), strName.AllocSysString(), &pGalaxy);
			pWndXobj->m_pHostGalaxy = (CGalaxy*)pGalaxy;
			CGalaxy* _pGalaxy = pWndXobj->m_pHostGalaxy;
			if (m_pXobjShareData->m_pGalaxy->m_pParentXobj)
			{
				_pGalaxy->m_pParentXobj = m_pXobjShareData->m_pGalaxy->m_pParentXobj;
			}
		}

		if (pWndXobj->m_pHostGalaxy)
		{
			if (pWndXobj->m_pCurrentExNode)
			{
				::SetWindowLong(pWndXobj->m_pCurrentExNode->m_pHostWnd->m_hWnd, GWL_ID, 1);
			}

			HRESULT hr = pWndXobj->m_pHostGalaxy->Observe(bstrKey, bstrXml, ppRetXobj);

			long dwID = AFX_IDW_PANE_FIRST + nRow * 16 + nCol;
			if (hr != S_OK)
			{
				if (pWndXobj->m_pCurrentExNode)
					::SetWindowLong(pWndXobj->m_pCurrentExNode->m_pHostWnd->m_hWnd, GWL_ID, dwID);
				else
					::SetWindowLong(pWndXobj->m_pHostWnd->m_hWnd, GWL_ID, dwID);
				return S_OK;
			}
			CXobj* pRootXobj = (CXobj*)* ppRetXobj;
			CString strKey = OLE2T(bstrKey);
			strKey.MakeLower();
			m_mapExtendNode[pWndXobj] = strKey;
			pWndXobj->m_pCurrentExNode = pRootXobj;
			::SetWindowLongPtr(pRootXobj->m_pHostWnd->m_hWnd, GWLP_ID, dwID);
			HWND h = ::GetParent(m_pHostWnd->m_hWnd);
			if (m_nViewType==Grid)
			{
				CGalaxy* pGalaxy = m_pXobjShareData->m_pGalaxy;
				pGalaxy->HostPosChanged();
			}
			return hr;
		}
	}
	return S_OK;
}

STDMETHODIMP CXobj::get_Tag(VARIANT * pVar)
{
	*pVar = m_varTag;

	if (m_varTag.vt == VT_DISPATCH && m_varTag.pdispVal)
		m_varTag.pdispVal->AddRef();
	return S_OK;
}

STDMETHODIMP CXobj::put_Tag(VARIANT var)
{
	m_varTag = var;
	return S_OK;
}

STDMETHODIMP CXobj::get_XObject(VARIANT * pVar)
{
	pVar->vt = VT_EMPTY;
	if (m_pDisp)
	{
		pVar->vt = VT_DISPATCH;
		pVar->pdispVal = m_pDisp;
		pVar->pdispVal->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CXobj::get_AxPlugIn(BSTR bstrPlugInName, IDispatch * *pVal)
{
	return S_OK;
}

STDMETHODIMP CXobj::get_Name(BSTR * pVal)
{
	*pVal = m_strName.AllocSysString();
	return S_OK;
}

STDMETHODIMP CXobj::put_Name(BSTR bstrNewName)
{
	//CString strName = OLE2T(bstrNewName);
	//strName.Trim();
	//strName.Replace(_T(","), _T(""));
	//if (m_pHostParse != NULL)
	//{
	//	CString _strName = _T(",");
	//	_strName += GetNames();
	//	CString _strName2 = _T(",");
	//	_strName2 += strName;
	//	_strName2 += _T(",");
	//	int nPos = _strName.Find(_strName2);
	//	if (nPos == -1)
	//	{
	//		m_pHostParse->put_attr(L"id", strName);
	//		m_strName = strName;
	//	}
	//	else
	//	{
	//		::MessageBox(NULL, _T("Modify name failed!"), _T("Tangram"), MB_OK);
	//	}
	//}
	return S_OK;
}

STDMETHODIMP CXobj::get_Attribute(BSTR bstrKey, BSTR * pVal)
{
	if (m_pHostParse != nullptr)
	{
		CString strVal = m_pHostParse->attr(OLE2T(bstrKey), _T(""));
		*pVal = strVal.AllocSysString();
	}
	return S_OK;
}

STDMETHODIMP CXobj::put_Attribute(BSTR bstrKey, BSTR bstrVal)
{
	if (m_pHostParse != nullptr)
	{
		CString strID = OLE2T(bstrKey);
		CString strVal = OLE2T(bstrVal);
		if (strID.CompareNoCase(TGM_GRID_TYPE))
			m_strID = strVal;
		ATLTRACE(_T("Modify CXobj Attribute: ID: %s Value: %s\n"), strID, strVal);
		CGalaxy* pGalaxy = nullptr;
		if (strVal.CompareNoCase(TGM_NUCLEUS) == 0 && g_pCosmos->m_pDesignXobj)
		{
			pGalaxy = g_pCosmos->m_pDesignXobj->m_pRootObj->m_pXobjShareData->m_pGalaxy;
			if (g_pCosmos->m_pDesignXobj && pGalaxy->m_pBindingXobj)
			{
				CXobj* pOldNode = pGalaxy->m_pBindingXobj;
				if (pOldNode->m_pXobjShareData->m_pOldGalaxy)
				{
					pOldNode->m_pXobjShareData->m_pGalaxy = pOldNode->m_pXobjShareData->m_pOldGalaxy;
					pOldNode->m_pXobjShareData->m_pOldGalaxy = nullptr;
				}
				CXobj* pParent = pOldNode->m_pParentObj;
				if (pParent && pParent->m_nViewType == Grid)
				{
					if (pOldNode != this)
					{
						CGridWnd* pWnd = (CGridWnd*)pParent->m_pHostWnd;
						pWnd->m_pHostXobj = nullptr;
						if (m_pParentObj == pParent)
							pWnd->m_pHostXobj = this;
					}
				}
				if (m_pParentObj && m_pParentObj->m_nViewType == Grid)
				{
					CGridWnd* pWnd = (CGridWnd*)m_pParentObj->m_pHostWnd;
					pWnd->m_pHostXobj = this;
				}
				pOldNode->m_strID = _T("");
				if (pOldNode->m_pRootObj == g_pCosmos->m_pDesignXobj->m_pRootObj)
					pOldNode->m_pHostParse->put_attr(TGM_GRID_TYPE, _T(""));
				ATLTRACE(_T("Modify CXobj HostView Attribute: ID:%s Value: %s\n"), strID, strVal);
				pOldNode->m_pHostWnd->Invalidate();
			}

			m_strID = TGM_NUCLEUS;
			CXobj* pTopXobj = m_pRootObj;
			pTopXobj->m_pXobjShareData->m_pHostClientView = (CXobjHelper*)m_pHostWnd;
			while (pTopXobj != pTopXobj->m_pRootObj)
			{
				pTopXobj->m_pXobjShareData->m_pGalaxy->m_pBindingXobj = this;
				pTopXobj->m_pXobjShareData->m_pHostClientView = pTopXobj->m_pXobjShareData->m_pHostClientView;
				pTopXobj = pTopXobj->m_pRootObj;
			}
			m_pHostParse->put_attr(TGM_GRID_TYPE, TGM_NUCLEUS);
			if (g_pCosmos->m_pDesignXobj)
			{
				pGalaxy->m_pBindingXobj = this;
				g_pCosmos->m_pDesignXobj->m_pXobjShareData->m_pOldGalaxy = g_pCosmos->m_pDesignXobj->m_pXobjShareData->m_pGalaxy;
				g_pCosmos->m_pDesignXobj->m_pXobjShareData->m_pGalaxy = m_pRootObj->m_pXobjShareData->m_pGalaxy;
				g_pCosmos->m_pDesignXobj->m_pXobjShareData->m_pHostClientView = m_pRootObj->m_pXobjShareData->m_pHostClientView;
			}

			if (m_pParentObj && m_pParentObj->m_nViewType == Grid)
				m_pHostWnd->ModifyStyleEx(WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE, 0);
			m_pXobjShareData->m_pGalaxy->HostPosChanged();
		}
		m_pHostParse->put_attr(strID, strVal);
	}
	return S_OK;
}

STDMETHODIMP CXobj::get_Caption(BSTR * pVal)
{
	*pVal = m_strCaption.AllocSysString();
	return S_OK;
}

STDMETHODIMP CXobj::put_Caption(BSTR bstrCaption)
{
	CString str(bstrCaption);

	m_strCaption = str;

	if (m_pParentObj != nullptr && m_pParentObj->m_pHostWnd != nullptr)
	{
		m_pParentObj->m_pHostWnd->SendMessage(WM_TGM_SET_CAPTION, m_nCol, (LPARAM)str.GetBuffer());
	}

	if (m_pHostParse != nullptr)
	{
		m_pHostParse->put_attr(L"caption", str);
	}
	return S_OK;
}

STDMETHODIMP CXobj::get_Handle(LONGLONG * pVal)
{
	if (m_pHostWnd)
		* pVal = (LONGLONG)m_pHostWnd->m_hWnd;
	return S_OK;
}

STDMETHODIMP CXobj::get_OuterXml(BSTR * pVal)
{
	*pVal = m_pXobjShareData->m_pCosmosParse->xml().AllocSysString();
	return S_OK;
}

STDMETHODIMP CXobj::get_Key(BSTR * pVal)
{
	*pVal = m_pRootObj->m_strKey.AllocSysString();
	return S_OK;
}

STDMETHODIMP CXobj::get_XML(BSTR * pVal)
{
	*pVal = m_pHostParse->xml().AllocSysString();
	return S_OK;
}

BOOL CXobj::Create(DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT nID, CCreateContext * pContext)
{
	BOOL bRet = false;

	HWND hWnd = 0;
	int nCol = m_pHostParse->GetCount();
	if (nCol && m_strID == _T("") && m_strObjTypeID == _T("") && m_pHostParse->GetChild(TGM_XOBJ))
		m_strObjTypeID = _T("tabbedwnd");
	CXobjHelper* pCosmosDesignView = (CXobjHelper*)m_pHostWnd;
	BOOL isAppWnd = false;
	if ( m_strID == _T("clrctrl"))
	{
		g_pCosmos->LoadCLR();
		m_nViewType = CLRCtrl;

		hWnd = CreateView(pParentWnd->m_hWnd, m_strObjTypeID);
		bRet = true;
	}
	else
	{
		if (hWnd == NULL)
		{
			int nPos = m_strID.Find(_T("@"));
			if (nPos != -1)
			{
				IUniverseAppProxy* pProxy = nullptr;
				CString strKey = m_strID.Mid(nPos + 1);
				auto it = g_pCosmos->m_mapCosmosAppProxy.find(strKey);
				if (it != g_pCosmos->m_mapCosmosAppProxy.end())
				{
					pProxy = it->second;
				}
				else
				{
					CString strPath = g_pCosmos->m_strAppPath;
					nPos = strKey.Find(_T("."));
					CString strAppName = strKey.Left(nPos);
					HMODULE hHandle = nullptr;
					CString strdll = strPath + strKey + _T("\\") + strAppName + _T(".dll");
					if (::PathFileExists(strdll))
						hHandle = ::LoadLibrary(strdll);
					if (hHandle)
					{
						it = g_pCosmos->m_mapCosmosAppProxy.find(strKey.MakeLower());
						if (it != g_pCosmos->m_mapCosmosAppProxy.end())
						{
							pProxy = it->second;
						}
					}
				}
			}

			if (m_nViewType!=CLRCtrl&&::IsWindow(hWnd) == false&&m_strObjTypeID != _T(""))
			{
				ICosmosWindowProvider* pViewFactoryDisp = nullptr;
				auto it = g_pCosmos->m_mapWindowProvider.find(m_strObjTypeID);
				if (it != g_pCosmos->m_mapWindowProvider.end())
				{
					pViewFactoryDisp = it->second;
				}
				else
				{
					if (it == g_pCosmos->m_mapWindowProvider.end())
					{
						if (m_strID.CompareNoCase(_T("TreeView")))
						{
							CString strLib = g_pCosmos->m_strAppPath + _T("TabbedWnd.dll");
							if (::PathFileExists(strLib))
							{
								::LoadLibrary(strLib);
								auto it = g_pCosmos->m_mapWindowProvider.find(m_strObjTypeID);
								if (it != g_pCosmos->m_mapWindowProvider.end())
								{
									pViewFactoryDisp = it->second;
								}
							}
							if (pViewFactoryDisp == nullptr)
							{
								CString strLib = _T("");
								CString strPath = g_pCosmos->m_strAppPath + _T("wincomponent\\") + m_strObjTypeID + _T(".component");
								CTangramXmlParse m_Parse;
								if (::PathFileExists(strPath))
								{
									if (m_Parse.LoadFile(strPath))
									{
										strLib = g_pCosmos->m_strAppPath + _T("wincomponent\\") + m_Parse.attr(_T("lib"), _T(""));
									}
								}
								if (::PathFileExists(strLib)&&::LoadLibrary(strLib))
								{
									auto it = g_pCosmos->m_mapWindowProvider.find(m_strObjTypeID);
									if (it != g_pCosmos->m_mapWindowProvider.end())
									{
										pViewFactoryDisp = it->second;
									}
								}
							}
						}
					}
				}

				if (pViewFactoryDisp)
				{
					hWnd = pViewFactoryDisp->Create(pParentWnd ? pParentWnd->m_hWnd : 0, this);
					//::PostMessage(hWnd, WM_XOBJCREATED, (WPARAM)((IXobj*)this), 10000);
					m_nID = ::GetWindowLong(hWnd, GWL_ID);
				}
			}
		}
	}

	if (!::IsWindow(m_pHostWnd->m_hWnd) && hWnd && pCosmosDesignView->SubclassWindow(hWnd))
	{
		if (isAppWnd == false)
			::SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_CHILD | /*WS_VISIBLE | */WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		else
		{
			if (m_pParentObj && m_pParentObj->m_nViewType == Grid)
				m_pHostWnd->ModifyStyleEx(0, WS_EX_CLIENTEDGE);
		}
		::SetWindowLong(hWnd, GWL_ID, nID);

		pCosmosDesignView->m_bCreateExternal = true;
		if(m_nViewType==BlankView)
			m_nViewType = TabGrid;
		bRet = true;
	}

	if (hWnd == 0)
	{
		hWnd = CreateWindow(L"Cosmos Xobj Class", NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0, pParentWnd->m_hWnd, (HMENU)nID, AfxGetInstanceHandle(), NULL);
		if (::IsWindow(m_pHostWnd->m_hWnd) == false)
			bRet = m_pHostWnd->SubclassWindow(hWnd);
	}

	bRet = true;

	//Very important:
	if (m_pHostWnd && ::IsWindow(m_pHostWnd->m_hWnd))
		m_pHostWnd->SendMessage(WM_INITIALUPDATE);

	////////////////////////////////////////////////////////////////////////////////////////////////
	if (m_strID.CompareNoCase(_T("treeview")))
	{
		m_nRows = 1;
		m_nCols = nCol;

		if (nCol && m_pHostParse->GetChild(TGM_XOBJ))
		{
			m_nViewType = TabGrid;
			if (m_nActivePage<0 || m_nActivePage>nCol - 1)
				m_nActivePage = 0;
			CWnd * pView = nullptr;
			CXobj * pObj = nullptr;
			int j = 0;
			for (int i = 0; i < nCol; i++)
			{
				CTangramXmlParse* pChild = m_pHostParse->GetChild(i);
				CString _strName = pChild->name();
				CString strName = pChild->attr(_T("id"), _T(""));
				if (_strName.CompareNoCase(TGM_XOBJ) == 0)
				{
					strName.Trim();
					strName.MakeLower();

					pObj = new CComObject<CXobj>;
					pObj->m_pRootObj = m_pRootObj;
					pObj->m_pHostParse = pChild;
					AddChildNode(pObj);
					pObj->m_nCol = j;
					pObj->InitWndXobj();

					if (pObj->m_pObjClsInfo)
					{
						pContext->m_pNewViewClass = pObj->m_pObjClsInfo;
						pView = (CWnd*)pContext->m_pNewViewClass->CreateObject();
						pView->Create(NULL, _T(""), WS_CHILD, rect, m_pHostWnd, 0, pContext);
						HWND m_hChild = (HWND)::SendMessage(m_pHostWnd->m_hWnd, WM_CREATETABPAGE, (WPARAM)pView->m_hWnd, (LPARAM)LPCTSTR(pObj->m_strCaption));
					}
					j++;
				}
			}
			::SendMessage(m_pHostWnd->m_hWnd, WM_ACTIVETABPAGE, (WPARAM)m_nActivePage, (LPARAM)1);
			Fire_TabChange(m_nActivePage, -1);
		}
	}

	m_pHostWnd->SetWindowText(m_strNodeName);
	if (m_nViewType == TabGrid && m_pParentObj && m_pParentObj->m_nViewType == Grid)
	{
		if (m_pHostWnd)
			m_pHostWnd->ModifyStyleEx(WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE, 0);
	}

	return bRet;
}

HWND CXobj::CreateView(HWND hParentWnd, CString strTag)
{
	BOOL bWebCtrl = false;
	CString strURL = _T("");
	CString strID = strTag;
	CComBSTR bstr2;
	get_Attribute(CComBSTR("id"), &bstr2);
	CString strName = OLE2T(bstr2);

	HWND _hWnd = m_pXobjShareData->m_pGalaxy->m_hWnd;

	switch (m_nViewType)
	{
	case ActiveX:
	{
	}
	break;
	case CLRCtrl:
	{
		g_pCosmos->m_pActiveXobj = this;
		
		CString strUIKey = strTag;
		if (g_pCosmos->m_pCLRProxy)
		{
			m_pDisp = g_pCosmos->m_pCLRProxy->CreateObject(strTag.AllocSysString(), hParentWnd, this);
			if (m_pDisp == nullptr)
			{
				((CXobjHelper*)m_pHostWnd)->m_bCreateExternal = false;
				m_nViewType = BlankView;
			}
		}
	}
	break;
	}
	if (m_pDisp)
	{
		if (m_nViewType == CLRCtrl)
		{
			if (g_pCosmos->m_hFormNodeWnd && (::GetWindowLongPtr(g_pCosmos->m_hFormNodeWnd, GWL_STYLE) & WS_CHILD))
			{
				HWND hCtrl = NULL;
				if (g_pCosmos->m_pCLRProxy)
					hCtrl = g_pCosmos->m_pCLRProxy->GetCtrlHandle(m_pDisp);
				if (hCtrl == g_pCosmos->m_hFormNodeWnd)
				{
					HWND hWnd = g_pCosmos->m_hFormNodeWnd;
					g_pCosmos->m_hFormNodeWnd = nullptr;
					return hWnd;
				}
			}
		}
		auto hWnd = ::CreateWindowEx(NULL, L"Cosmos Xobj Class", NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, 0, 0, 0, hParentWnd, NULL, AfxGetInstanceHandle(), NULL);
		CAxWindow m_Wnd;
		m_Wnd.Attach(hWnd);
		CComPtr<IUnknown> pUnk;
		m_Wnd.AttachControl(m_pDisp, &pUnk);

		CComQIPtr<IOleInPlaceActiveObject> pIOleInPlaceActiveObject(m_pDisp);
		if (pIOleInPlaceActiveObject)
			((CXobjHelper*)m_pHostWnd)->m_pOleInPlaceActiveObject = pIOleInPlaceActiveObject.Detach();
		m_Wnd.Detach();
		return hWnd;
	}

	return 0;
}

STDMETHODIMP CXobj::get_ChildNodes(IXobjCollection * *pXobjColletion)
{
	if (m_pChildNodeCollection == nullptr)
	{
		CComObject<CXobjCollection>::CreateInstance(&m_pChildNodeCollection);
		m_pChildNodeCollection->AddRef();
		m_pChildNodeCollection->m_pXobjs = &m_vChildNodes;
	}
	return m_pChildNodeCollection->QueryInterface(IID_IXobjCollection, (void**)pXobjColletion);
}

int CXobj::_getNodes(CXobj * pXobj, CString & strName, CXobj * *ppRetXobj, CXobjCollection * pXobjs)
{
	int iCount = 0;
	if (pXobj->m_strName.CompareNoCase(strName) == 0)
	{
		if (pXobjs != nullptr)
			pXobjs->m_pXobjs->push_back(pXobj);

		if (ppRetXobj != nullptr && (*ppRetXobj) == nullptr)
			* ppRetXobj = pXobj;
		return 1;
	}

	for (auto it : pXobj->m_vChildNodes)
	{
		iCount += _getNodes(it, strName, ppRetXobj, pXobjs);
	}
	return iCount;
}

STDMETHODIMP CXobj::Show()
{
	CXobj* pChild = this;
	CXobj* pParent = pChild->m_pParentObj;

	while (pParent != nullptr)
	{
		pParent->m_pHostWnd->SendMessage(WM_ACTIVETABPAGE, (WPARAM)pChild->m_nCol, (LPARAM)1);

		pChild = pParent;
		pParent = pChild->m_pParentObj;
	}
	return S_OK;
}

STDMETHODIMP CXobj::get_RootXobj(IXobj * *ppXobj)
{
	if (m_pRootObj != nullptr)
		* ppXobj = m_pRootObj;
	return S_OK;
}

STDMETHODIMP CXobj::get_ParentXobj(IXobj * *ppXobj)
{
	*ppXobj = nullptr;
	if (m_pParentObj != nullptr)
		* ppXobj = m_pParentObj;

	return S_OK;
}

STDMETHODIMP CXobj::get_XobjType(XobjType* nType)
{
	*nType = m_nViewType;
	return S_OK;
}

//void CXobj::_get_Objects(CXobj * pXobj, UINT32 & nType, CXobjCollection * pXobjColletion)
//{
//	if (pXobj->m_nViewType & nType)
//	{
//		pXobjColletion->m_pXobjs->push_back(pXobj);
//	}
//
//	CXobj* pChildNode = nullptr;
//	for (auto it : pXobj->m_vChildNodes)
//	{
//		pChildNode = it;
//		_get_Objects(pChildNode, nType, pXobjColletion);
//	}
//}

STDMETHODIMP CXobj::get_Objects(long nType, IXobjCollection * *ppXobjColletion)
{
	//CComObject<CXobjCollection>* pXobjs = nullptr;
	//CComObject<CXobjCollection>::CreateInstance(&pXobjs);

	//pXobjs->AddRef();

	//UINT32 u = nType;
	//_get_Objects(this, u, pXobjs);
	//HRESULT hr = pXobjs->QueryInterface(IID_IXobjCollection, (void**)ppXobjColletion);

	//pXobjs->Release();

	return S_OK;
}

STDMETHODIMP CXobj::get_Rows(long* nRows)
{
	*nRows = m_nRows;
	return S_OK;
}

STDMETHODIMP CXobj::get_Cols(long* nCols)
{
	*nCols = m_nCols;
	if (m_vChildNodes.size() && m_nRows * m_nCols == 0)
	{
		*nCols = m_vChildNodes.size();
	}
	return S_OK;
}

STDMETHODIMP CXobj::get_Row(long* nRow)
{
	*nRow = m_nRow;
	return S_OK;
}

STDMETHODIMP CXobj::get_Col(long* nCol)
{
	*nCol = m_nCol;
	return S_OK;
}

STDMETHODIMP CXobj::GetXobj(long nRow, long nCol, IXobj * *ppXobj)
{
	CXobj* pRet = nullptr;

	*ppXobj = nullptr;
	if (nRow < 0 || nCol < 0 || nRow >= m_nRows || nCol >= m_nCols) return E_INVALIDARG;
	//if (m_nViewType == Grid)
	//{
	//	CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
	//	HWND hWnd = ::GetDlgItem(pSplitter->m_hWnd, pSplitter->IdFromRowCol(nRow, nCol));
	//	LRESULT lRes = ::SendMessage(hWnd, WM_HUBBLE_GETNODE, 0, 0);
	//	if (lRes)
	//	{
	//		pRet = (CXobj*)lRes;
	//		pRet->QueryInterface(IID_IXobj, (void**)ppXobj);
	//		return S_OK;
	//	}
	//	return S_FALSE;
	//}

	for (auto it : m_vChildNodes)
	{
		pRet = it;
		if (pRet->m_nCol == nCol && pRet->m_nRow == nRow)
		{
			break;
		}
	}

	HRESULT hr = S_OK;
	if (pRet)
	{
		hr = pRet->QueryInterface(IID_IXobj, (void**)ppXobj);
	}
	return hr;
}

STDMETHODIMP CXobj::GetXobjByName(BSTR bstrName, IXobjCollection * *ppXobjs)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strName(bstrName);

	CXobj* pRetNode = nullptr;

	CComObject<CXobjCollection>* pXobjs = nullptr;
	if (ppXobjs != nullptr)
	{
		*ppXobjs = nullptr;
		CComObject<CXobjCollection>::CreateInstance(&pXobjs);
		pXobjs->AddRef();
	}

	int iCount = _getNodes(this, strName, &pRetNode, pXobjs);

	if (ppXobjs != nullptr)
		pXobjs->QueryInterface(IID_IXobjCollection, (void**)ppXobjs);

	if (pXobjs != nullptr)
		pXobjs->Release();

	return S_OK;
}

STDMETHODIMP CXobj::GetXobjs(BSTR bstrName, IXobj * *ppXobj, IXobjCollection * *ppXobjs, long* pCount)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strName(bstrName);

	CXobj* pRetNode = nullptr;

	if (ppXobj != nullptr)
		* ppXobj = nullptr;

	CComObject<CXobjCollection> * pXobjs = nullptr;
	if (ppXobjs != nullptr)
	{
		*ppXobjs = nullptr;
		CComObject<CXobjCollection>::CreateInstance(&pXobjs);
		pXobjs->AddRef();
	}

	int iCount = _getNodes(this, strName, &pRetNode, pXobjs);

	*pCount = iCount;

	if ((iCount > 0) && (ppXobj != nullptr))
		pRetNode->QueryInterface(IID_IXobj, (void**)ppXobj);

	if (ppXobjs != nullptr)
		pXobjs->QueryInterface(IID_IXobjCollection, (void**)ppXobjs);

	if (pXobjs != nullptr)
		pXobjs->Release();

	return S_OK;
}

BOOL CXobj::AddChildNode(CXobj * pXobj)
{
	m_vChildNodes.push_back(pXobj);
	pXobj->m_pParentObj = this;
	pXobj->m_pRootObj = m_pRootObj;
	return true;
}

BOOL CXobj::RemoveChildNode(CXobj * pXobj)
{
	auto it = find(m_vChildNodes.begin(), m_vChildNodes.end(), pXobj);
	if (it != m_vChildNodes.end())
	{
		m_vChildNodes.erase(it);
		return true;
	}
	return false;
}

STDMETHODIMP CXobj::get_Galaxy(IGalaxy * *pVal)
{
	if (m_pXobjShareData->m_pGalaxy)
		* pVal = m_pXobjShareData->m_pGalaxy;

	return S_OK;
}

STDMETHODIMP CXobj::get_HostGalaxy(IGalaxy * *pVal)
{
	if (m_pHostGalaxy)
		* pVal = m_pHostGalaxy;

	return S_OK;
}

STDMETHODIMP CXobj::get_Height(LONG * pVal)
{
	RECT rc;
	::GetClientRect(m_pHostWnd->m_hWnd, &rc);
	*pVal = rc.bottom;
	return S_OK;
}

STDMETHODIMP CXobj::get_Width(LONG * pVal)
{
	RECT rc;
	::GetClientRect(m_pHostWnd->m_hWnd, &rc);
	*pVal = rc.right;

	return S_OK;
}

STDMETHODIMP CXobj::get_OfficeObj(IDispatch * *pVal)
{
	return S_OK;
}

STDMETHODIMP CXobj::get_Extender(IDispatch * *pVal)
{
	if (m_pExtender)
	{
		*pVal = m_pExtender;
		(*pVal)->AddRef();
	}
	return S_OK;
}

STDMETHODIMP CXobj::put_Extender(IDispatch * newVal)
{
	if (m_pExtender)
		m_pExtender->Release();
	m_pExtender = newVal;
	m_pExtender->AddRef();

	return S_OK;
}

STDMETHODIMP CXobj::get_GalaxyCluster(IGalaxyCluster * *pVal)
{
	*pVal = (IGalaxyCluster*)m_pXobjShareData->m_pGalaxy->m_pGalaxyCluster;
	return S_OK;
}

STDMETHODIMP CXobj::get_NameAtWindowPage(BSTR * pVal)
{
	*pVal = m_strNodeName.AllocSysString();
	return S_OK;
}

STDMETHODIMP CXobj::GetCtrlByName(BSTR bstrName, VARIANT_BOOL bFindInChild, IDispatch * *ppRetDisp)
{
	if (g_pCosmos->m_pCLRProxy && m_nViewType == CLRCtrl && m_pDisp)
		* ppRetDisp = g_pCosmos->m_pCLRProxy->GetCtrlByName(m_pDisp, bstrName, bFindInChild ? true : false);

	return S_OK;
}

STDMETHODIMP CXobj::GetCtrlValueByName(BSTR bstrName, VARIANT_BOOL bFindInChild, BSTR * bstrVal)
{
	//if (g_pCosmos->m_pCLRProxy && m_nViewType == CLRCtrl && m_pDisp)
	//{
	//	*bstrVal = g_pCosmos->m_pCLRProxy->GetCtrlValueByName(m_pDisp, bstrName, bFindInChild ? true : false);
	//}
	return S_OK;
}

STDMETHODIMP CXobj::SetCtrlValueByName(BSTR bstrName, VARIANT_BOOL bFindInChild, BSTR bstrVal)
{
	//if (g_pCosmos->m_pCLRProxy && m_nViewType == CLRCtrl && m_pDisp)
	//{
	//	g_pCosmos->m_pCLRProxy->SetCtrlValueByName(m_pDisp, bstrName, bFindInChild ? true : false, bstrVal);
	//}
	return S_OK;
}

CXobjCollection::CXobjCollection()
{
	m_pXobjs = &m_vXobjs;
	g_pCosmos->m_mapWndXobjCollection[(__int64)this] = this;
}

CXobjCollection::~CXobjCollection()
{
	auto it = g_pCosmos->m_mapWndXobjCollection.find((__int64)this);
	if (it != g_pCosmos->m_mapWndXobjCollection.end())
	{
		g_pCosmos->m_mapWndXobjCollection.erase(it);
	}
	m_vXobjs.clear();
}

STDMETHODIMP CXobjCollection::get_XobjCount(long* pCount)
{
	*pCount = (int)m_pXobjs->size();
	return S_OK;
}

STDMETHODIMP CXobjCollection::get_Item(long iIndex, IXobj * *ppXobj)
{
	if (iIndex < 0 || iIndex >= (int)m_pXobjs->size()) return E_INVALIDARG;

	CXobj * pXobj = m_pXobjs->operator [](iIndex);

	return pXobj->QueryInterface(IID_IXobj, (void**)ppXobj);
}

STDMETHODIMP CXobjCollection::get__NewEnum(IUnknown * *ppVal)
{
	*ppVal = nullptr;

	struct _CopyVariantFromIUnkown
	{
		static HRESULT copy(VARIANT* p1, CXobj* const* p2)
		{
			CXobj* pXobj = *p2;
			p1->vt = VT_UNKNOWN;
			return pXobj->QueryInterface(IID_IUnknown, (void**) & (p1->punkVal));
		}

		static void init(VARIANT* p) { VariantInit(p); }
		static void destroy(VARIANT* p) { VariantClear(p); }
	};

	typedef CComEnumOnSTL<IEnumVARIANT, & IID_IEnumVARIANT, VARIANT, _CopyVariantFromIUnkown, CXobjVector>
		CComEnumVariantOnVector;

	CComObject<CComEnumVariantOnVector>* pe = 0;
	HRESULT hr = CComObject<CComEnumVariantOnVector>::CreateInstance(&pe);

	if (SUCCEEDED(hr))
	{
		hr = pe->AddRef();
		hr = pe->Init(GetUnknown(), *m_pXobjs);

		if (SUCCEEDED(hr))
			hr = pe->QueryInterface(ppVal);

		hr = pe->Release();
	}

	return hr;
}

STDMETHODIMP CXobj::get_DocXml(BSTR * pVal)
{
	return S_OK;
}

STDMETHODIMP CXobj::get_rgbMiddle(OLE_COLOR * pVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		*pVal = OLE_COLOR(pSplitter->rgbMiddle);
	}
	else
	{
		*pVal = OLE_COLOR(RGB(240, 240, 240));
	}
	return S_OK;
}

STDMETHODIMP CXobj::put_rgbMiddle(OLE_COLOR newVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		OleTranslateColor(newVal, NULL, &pSplitter->rgbMiddle);
		BYTE Red = GetRValue(pSplitter->rgbMiddle);
		BYTE Green = GetGValue(pSplitter->rgbMiddle);
		BYTE Blue = GetBValue(pSplitter->rgbMiddle);
		CString strRGB = _T("");
		strRGB.Format(_T("RGB(%d,%d,%d)"), Red, Green, Blue);
		put_Attribute(CComBSTR(L"middlecolor"), strRGB.AllocSysString());
		pSplitter->Invalidate();
	}
	return S_OK;
}

STDMETHODIMP CXobj::get_rgbLeftTop(OLE_COLOR * pVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		*pVal = OLE_COLOR(pSplitter->rgbLeftTop);
	}
	else
	{
		*pVal = OLE_COLOR(RGB(240, 240, 240));
	}
	return S_OK;
}

STDMETHODIMP CXobj::put_rgbLeftTop(OLE_COLOR newVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		OleTranslateColor(newVal, NULL, &pSplitter->rgbLeftTop);
		CString strRGB = _T("");
		strRGB.Format(_T("RGB(%d,%d,%d)"), GetRValue(pSplitter->rgbLeftTop), GetGValue(pSplitter->rgbLeftTop), GetBValue(pSplitter->rgbLeftTop));
		put_Attribute(CComBSTR(L"lefttopcolor"), strRGB.AllocSysString());
		pSplitter->Invalidate();
	}
	return S_OK;
}

STDMETHODIMP CXobj::get_rgbRightBottom(OLE_COLOR * pVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		*pVal = OLE_COLOR(pSplitter->rgbRightBottom);
	}
	else
		*pVal = OLE_COLOR(RGB(240, 240, 240));

	return S_OK;
}

STDMETHODIMP CXobj::put_rgbRightBottom(OLE_COLOR newVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		OleTranslateColor(newVal, NULL, &pSplitter->rgbRightBottom);
		BYTE Red = GetRValue(pSplitter->rgbRightBottom);
		BYTE Green = GetGValue(pSplitter->rgbRightBottom);
		BYTE Blue = GetBValue(pSplitter->rgbRightBottom);
		CString strRGB = _T("");
		strRGB.Format(_T("RGB(%d,%d,%d)"), Red, Green, Blue);
		put_Attribute(CComBSTR(L"rightbottomcolor"), strRGB.AllocSysString());
		pSplitter->Invalidate();
	}
	return S_OK;
}

STDMETHODIMP CXobj::get_Hmin(int* pVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		*pVal = pSplitter->m_Hmin;
	}
	return S_OK;
}

STDMETHODIMP CXobj::put_Hmin(int newVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		pSplitter->m_Hmin = min(pSplitter->m_Hmax, newVal);
		CString strVal = _T("");
		strVal.Format(_T("%d"), pSplitter->m_Hmin);
		put_Attribute(CComBSTR(L"hmin"), strVal.AllocSysString());
	}

	return S_OK;
}

STDMETHODIMP CXobj::get_Hmax(int* pVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		*pVal = pSplitter->m_Hmax;
	}
	return S_OK;
}

STDMETHODIMP CXobj::put_Hmax(int newVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		pSplitter->m_Hmax = max(pSplitter->m_Hmin, newVal);
		CString strVal = _T("");
		strVal.Format(_T("%d"), pSplitter->m_Hmax);
		put_Attribute(CComBSTR(L"hmax"), strVal.AllocSysString());
	}

	return S_OK;
}

STDMETHODIMP CXobj::get_Vmin(int* pVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		*pVal = pSplitter->m_Vmin;
	}

	return S_OK;
}

STDMETHODIMP CXobj::put_Vmin(int newVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		pSplitter->m_Vmin = min(pSplitter->m_Vmax, newVal);
		CString strVal = _T("");
		strVal.Format(_T("%d"), pSplitter->m_Vmin);
		put_Attribute(CComBSTR(L"vmin"), strVal.AllocSysString());
	}

	return S_OK;
}

STDMETHODIMP CXobj::get_Vmax(int* pVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		*pVal = pSplitter->m_Vmax;
	}

	return S_OK;
}

STDMETHODIMP CXobj::put_Vmax(int newVal)
{
	if (m_nViewType == Grid)
	{
		CGridWnd* pSplitter = (CGridWnd*)m_pHostWnd;
		pSplitter->m_Vmax = max(pSplitter->m_Vmin, newVal);
		CString strVal = _T("");
		strVal.Format(_T("%d"), pSplitter->m_Vmax);
		put_Attribute(CComBSTR(L"vmax"), strVal.AllocSysString());
	}

	return S_OK;
}


STDMETHODIMP CXobj::get_HostXobj(IXobj * *pVal)
{
	if (m_pXobjShareData->m_pHostClientView)
		* pVal = m_pXobjShareData->m_pHostClientView->m_pXobj;

	return S_OK;
}


STDMETHODIMP CXobj::put_HostXobj(IXobj * newVal)
{
	return S_OK;
}


STDMETHODIMP CXobj::get_ActivePage(int* pVal)
{
	if (this->m_nViewType == TabGrid)
	{
		CComBSTR bstr(L"");
		get_Attribute(CComBSTR(L"activepage"), &bstr);
		*pVal = _wtoi(OLE2T(bstr));
	}
	return S_OK;
}


STDMETHODIMP CXobj::put_ActivePage(int newVal)
{
	if (this->m_nViewType == TabGrid && newVal < m_nCols)
	{
		HWND hwnd = nullptr;
		int nOldPage = 0;
		get_ActivePage(&nOldPage);
		if (nOldPage == newVal)
			return S_OK;
		IXobj * pOldNode = nullptr;
		GetXobj(0, newVal, &pOldNode);
		if (pOldNode)
		{
			LONGLONG h = 0;
			pOldNode->get_Handle(&h);
			hwnd = (HWND)h;
			if (::IsWindow(hwnd))
			{
				::ShowWindow(hwnd, SW_HIDE);
			}
		}
		m_pHostWnd->SendMessage(WM_ACTIVETABPAGE, (WPARAM)newVal, (LPARAM)1);
		IXobj* pXobj = nullptr;
		this->GetXobj(0, newVal, &pXobj);
		if (pXobj)
		{
			::ShowWindow(hwnd, SW_HIDE);
			ActiveTabPage(pXobj);
		}
	}

	return S_OK;
}

STDMETHODIMP CXobj::get_MasterRow(int* pVal)
{
	return S_OK;
}

STDMETHODIMP CXobj::put_MasterRow(int newVal)
{
	return S_OK;
}

STDMETHODIMP CXobj::get_MasterCol(int* pVal)
{
	return S_OK;
}

STDMETHODIMP CXobj::put_MasterCol(int newVal)
{
	return S_OK;
}

HRESULT CXobj::Fire_ObserveComplete()
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		DISPPARAMS params = { NULL, NULL, 0, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pCosmos->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			g_pCosmos->Unlock();

			IDispatch* pConnection = static_cast<IDispatch*>(punkConnection.p);

			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}

	for (auto it : m_mapWndXobjProxy)
	{
		it.second->OnObserverComplete();
	}

	return hr;
}

HRESULT CXobj::Fire_Destroy()
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		DISPPARAMS params = { NULL, NULL, 0, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pCosmos->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			g_pCosmos->Unlock();

			IDispatch* pConnection = static_cast<IDispatch*>(punkConnection.p);

			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(2, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}
	for (auto it : m_mapWndXobjProxy)
	{
		it.second->OnDestroy();
	}

	if (g_pCosmos->m_pCLRProxy)
	{
		g_pCosmos->m_pCLRProxy->ReleaseCosmosObj((IXobj*)this);
	}
	return hr;
}

HRESULT CXobj::Fire_TabChange(LONG ActivePage, LONG OldPage)
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		CComVariant avarParams[2];
		avarParams[1] = ActivePage;
		avarParams[1].vt = VT_I4;
		avarParams[0] = OldPage;
		avarParams[0].vt = VT_I4;
		DISPPARAMS params = { avarParams, NULL, 2, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pCosmos->Lock();
			IUnknown* punkConnection = m_vec.GetAt(iConnection);
			g_pCosmos->Unlock();
			IDispatch* pConnection = static_cast<IDispatch*>(punkConnection);

			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(7, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}
	for (auto it : m_mapWndXobjProxy)
	{
		it.second->OnTabChange(ActivePage, OldPage);
	}
	return hr;
}

HRESULT CXobj::Fire_IPCMessageReceived(BSTR bstrFrom, BSTR bstrTo, BSTR bstrMsgId, BSTR bstrPayload, BSTR bstrExtra)
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		CComVariant avarParams[5];
		avarParams[4] = bstrFrom;
		avarParams[4].vt = VT_BSTR;
		avarParams[3] = bstrTo;
		avarParams[3].vt = VT_BSTR;
		avarParams[2] = bstrMsgId;
		avarParams[2].vt = VT_BSTR;
		avarParams[1] = bstrPayload;
		avarParams[1].vt = VT_BSTR;
		avarParams[0] = bstrExtra;
		avarParams[0].vt = VT_BSTR;
		DISPPARAMS params = { avarParams, NULL, 5, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pCosmos->Lock();
			IUnknown* punkConnection = m_vec.GetAt(iConnection);
			g_pCosmos->Unlock();
			IDispatch* pConnection = static_cast<IDispatch*>(punkConnection);

			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(8, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}
	return hr;
}

STDMETHODIMP CXobj::put_SaveToConfigFile(VARIANT_BOOL newVal)
{
	return S_OK;
}

STDMETHODIMP CXobj::get_DockObj(BSTR bstrName, LONGLONG * pVal)
{
	return S_OK;
}

STDMETHODIMP CXobj::put_DockObj(BSTR bstrName, LONGLONG newVal)
{
	return S_OK;
}

STDMETHODIMP CXobj::NavigateURL(BSTR bstrURL, IDispatch * dispObjforScript)
{
	return S_OK;
}

STDMETHODIMP CXobj::get_URL(BSTR * pVal)
{
	if (m_pHostParse != nullptr)
	{
		CString strVal = m_pHostParse->attr(_T("url"), _T(""));
		*pVal = strVal.AllocSysString();
		strVal.ReleaseBuffer();
	}
	return S_OK;
}

STDMETHODIMP CXobj::put_URL(BSTR newVal)
{
	return S_OK;
}

STDMETHODIMP CXobj::SendIPCMessage(BSTR bstrTo, BSTR bstrPayload, BSTR bstrExtra, BSTR bstrMsgId, BSTR* bstrRet)
{
	return S_OK;
}

STDMETHODIMP CXobj::GetUIScript(BSTR bstrCtrlName, BSTR* bstrVal)
{
	*bstrVal = CComBSTR(m_pHostParse->xml());
	return S_OK;
}
