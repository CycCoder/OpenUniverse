/********************************************************************************
*					Open Universe - version 1.1.1.21							*
*********************************************************************************
* Copyright (C) 2002-2020 by Tangram Team.   All Rights Reserved.				*
*
* THIS SOURCE FILE IS THE PROPERTY OF TANGRAM TEAM AND IS NOT TO
* BE RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED
* WRITTEN CONSENT OF TANGRAM TEAM.
*
* THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS
* OUTLINED IN THE MIT LICENSE AGREEMENT.TANGRAM TEAM
* GRANTS TO YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE
* THIS SOFTWARE ON A SINGLE COMPUTER.
*
* CONTACT INFORMATION:
* mailto:tangramteam@outlook.com or mailto:sunhuizlz@yeah.net
* https://www.tangram.dev
*
********************************************************************************/
//https://github.com/eclipse/rt.equinox.framework/tree/master/features/org.eclipse.equinox.executable.feature

#include "../stdafx.h"
#include "../UniverseApp.h"
#include "../Grid.h"
#include "../Galaxy.h"
#include "../HubbleCtrl.h"
#include "../GridWnd.h"
#include "EclipseAddin.h"

#include "atlcom.h"

extern jstring newJavaString(JNIEnv *env, _TCHAR * str);

CEclipseExtender::CEclipseExtender()
{
}

CEclipseExtender::~CEclipseExtender()
{
}

STDMETHODIMP CEclipseExtender::Close()
{
	delete this;
	return S_OK;
}

STDMETHODIMP CEclipseExtender::get_ActiveWorkBenchWindow(BSTR bstrID, IWorkBenchWindow** pVal)
{
	CString strID = OLE2T(bstrID);
	strID.Trim();
	if (strID==_T(""))
	{
		if (g_pHubble->m_pActiveEclipseWnd)
		{
			*pVal = (IWorkBenchWindow*)g_pHubble->m_pActiveEclipseWnd;
			(*pVal)->AddRef();
		}
	}
	else
	{
		IHubble* pHubble = nullptr;
		g_pHubble->get_RemoteHubble(bstrID, &pHubble);
		if (pHubble)
		{
			IWorkBenchWindow* pRet = nullptr;
			IHubbleExtender* pExtender = nullptr;
			pHubble->get_Extender(&pExtender);
			if (pExtender)
			{
				CComQIPtr<IEclipseExtender> pEclipse(pExtender);
				if(pEclipse)
					pEclipse->get_ActiveWorkBenchWindow(bstrID, &pRet);
				if (pRet)
				{
					*pVal = pRet;
					(*pVal)->AddRef();
				}
			}
		}
	}

	return S_OK;
}

CEclipseWnd::CEclipseWnd(void)
{
	m_pDoc = nullptr;
	m_pGalaxy = nullptr;
	m_pCurGrid = nullptr;
	m_pHostGrid = nullptr;
	m_strXml = _T("");
	m_strPath = _T("");
	m_strAppProxyID = _T("");
	m_strNodeName = _T("");
	m_strDocKey = _T("");
	m_strFrameID = _T("");
	m_pAppProxy = nullptr;
	m_pGalaxyCluster = nullptr;
	g_pHubble->m_pActiveEclipseWnd = this;
}

CEclipseWnd::~CEclipseWnd(void) 
{
	if (m_pGalaxyCluster)
	{
		HWND hWnd = m_pGalaxyCluster->m_hWnd;
		auto it = g_pHubble->m_mapWindowPage.find(hWnd);
		if (it != g_pHubble->m_mapWindowPage.end())
		delete m_pGalaxyCluster;
	}
	ATLTRACE(_T("delete CEclipseWnd:%x\n"), this);
}

void CEclipseWnd::OnFinalMessage(HWND hWnd)
{
	auto it = g_pHubble->m_mapWorkBenchWnd.find(hWnd);
	if (it != g_pHubble->m_mapWorkBenchWnd.end())
		g_pHubble->m_mapWorkBenchWnd.erase(it);
	if (g_pHubble->m_pCLRProxy)
	{
		g_pHubble->m_pCLRProxy->ReleaseHubbleObj((IWorkBenchWindow*)this);
	}
	CWindowImpl::OnFinalMessage(hWnd);
	delete this;
}

void CEclipseWnd::CreatePage(BOOL bSaveToConfigFile)
{
	if (m_pGalaxyCluster == nullptr)
	{
		IGalaxyCluster* pGalaxyCluster = nullptr;
		g_pHubble->CreateGalaxyCluster((LONGLONG)m_hWnd, &pGalaxyCluster);
		if (pGalaxyCluster == nullptr)
			return;
		m_pGalaxyCluster = (CGalaxyCluster*)pGalaxyCluster;
		HMENU hMenu = ::GetMenu(m_hWnd);
		if (hMenu)
		{
			MENUITEMINFO mii;
			mii.fMask = MIIM_STRING;
			mii.cbSize = sizeof(MENUITEMINFO);
			int nCount = ::GetMenuItemCount(hMenu);
			for (int i = nCount - 1; i >0; i--)
			{
				mii.cch = 256;
				mii.fType = MFT_STRING;
				mii.dwTypeData = g_pHubble->m_szBuffer;
				GetMenuItemInfo(hMenu, i, true, &mii);
				CString str = mii.dwTypeData;
				if (str == _T("&Window"))
				{
					HMENU hSubMenu = ::GetSubMenu(hMenu, i);
					int nCount = ::GetMenuItemCount(hSubMenu);
					for (int i = 0; i<nCount; i++)
					{
						mii.fMask = MIIM_STRING | MIIM_ID;
						mii.cch = 256;
						mii.dwTypeData = g_pHubble->m_szBuffer;
						GetMenuItemInfo(hSubMenu, i, true, &mii);
						str = mii.dwTypeData;
						if (str == _T("&New Window"))
						{
							m_nNewWinCmdID = mii.wID;
							break;
						}
					}
					break;
				}
			}
		}
		if (::IsWindowVisible(m_hWnd))
		{
			m_strDocKey = g_pHubble->m_strCurrentEclipsePagePath;
			g_pHubble->m_strCurrentEclipsePagePath = _T("");
			Show(m_strDocKey);
			auto it2 = g_pHubble->m_mapValInfo.find(_T("newmdtframe"));
			if (it2 != g_pHubble->m_mapValInfo.end())
			{
				m_strFrameID = OLE2T(it2->second.bstrVal);
				g_pHubble->m_mapValInfo.erase(it2);
			}
		}
	}
}

STDMETHODIMP CEclipseWnd::get_Count(long* pCount)
{
	*pCount = (long)m_mapCtrl.size();
	return S_OK;
}

STDMETHODIMP CEclipseWnd::get_Ctrl(VARIANT vIndex, IEclipseCtrl **ppCtrl)
{
	if (vIndex.vt == VT_I4)
	{
		long lCount = m_mapCtrl.size();
		int iIndex = vIndex.lVal;
		if (iIndex < 0 || iIndex >= lCount) return E_INVALIDARG;

		auto it = m_mapCtrl.begin();
		int iPos = 0;
		while (it != m_mapCtrl.end())
		{
			if (iPos == iIndex) break;
			iPos++;
			it++;
		}

		*ppCtrl = it->second;
		return S_OK;
	}

	return S_OK;
}

STDMETHODIMP CEclipseWnd::CloseTangramUI()
{
	if (m_pGalaxy)
	{
		HWND _hWnd = ::CreateWindowEx(NULL, _T("Hubble Grid Class"), L"", WS_CHILD, 0, 0, 0, 0, g_pHubble->m_hHostWnd, NULL, AfxGetInstanceHandle(), NULL);
		HWND _hChildWnd = ::CreateWindowEx(NULL, _T("Hubble Grid Class"), L"", WS_CHILD, 0, 0, 0, 0, (HWND)_hWnd, NULL, AfxGetInstanceHandle(), NULL);
		if (::IsWindow(m_hWnd))
		{
			m_pGalaxy->ModifyHost((LONGLONG)_hChildWnd);
			::DestroyWindow(_hWnd);
		}
		m_pGalaxy = nullptr;
		delete m_pGalaxyCluster;
		m_pGalaxyCluster = nullptr;
	}

	return S_OK;
}

STDMETHODIMP CEclipseWnd::get__NewEnum(IUnknown** ppVal)
{
	typedef CComEnum<IEnumVARIANT, &IID_IEnumVARIANT, VARIANT, _Copy<VARIANT>>
		CComEnumVariant;

	CComObject<CComEnumVariant> *pe = 0;
	HRESULT hr = pe->CreateInstance(&pe);

	if (SUCCEEDED(hr))
	{
		pe->AddRef();
		long nLen = (long)m_mapCtrl.size();
		VARIANT* rgvar = new VARIANT[nLen];
		ZeroMemory(rgvar, sizeof(VARIANT)*nLen);
		VARIANT* pItem = rgvar;
		for (auto it : m_mapCtrl)
		{
			IUnknown* pDisp = nullptr;
			CEclipseCtrl* pObj = it.second;
			hr = pObj->QueryInterface(IID_IUnknown, (void**)&pDisp);
			pItem->vt = VT_UNKNOWN;
			pItem->punkVal = pDisp;
			if (pItem->punkVal != nullptr)
				pItem->punkVal->AddRef();
			pItem++;
			pDisp->Release();
		}

		hr = pe->Init(rgvar, &rgvar[nLen], 0, AtlFlagTakeOwnership);
		if (SUCCEEDED(hr))
			hr = pe->QueryInterface(IID_IUnknown, (void**)ppVal);
		pe->Release();
	}
	return S_OK;
}

STDMETHODIMP CEclipseWnd::get_Handle(LONGLONG* pVal)
{
	*pVal = (LONGLONG)m_hClient;
	return S_OK;
}

STDMETHODIMP CEclipseWnd::get_GalaxyCluster(IGalaxyCluster** pVal)
{
	if (m_pGalaxyCluster)
		*pVal = m_pGalaxyCluster;

	return S_OK;
}

STDMETHODIMP CEclipseWnd::get_Galaxy(IGalaxy** pVal)
{
	if (m_pGalaxy)
		*pVal = m_pGalaxy;
	return S_OK;
}

STDMETHODIMP CEclipseWnd::Observe(BSTR bstrKey, BSTR bstrXml, IGrid** ppGrid)
{
	CString strKey = OLE2T(bstrKey);
	strKey.Trim();
	HRESULT hr = S_FALSE;
	if (m_hClient == NULL)
		return S_FALSE;
	if (m_pGalaxyCluster == nullptr)
	{
		IGalaxyCluster* pGalaxyCluster = nullptr;
		g_pHubble->CreateGalaxyCluster((LONGLONG)m_hWnd, &pGalaxyCluster);
		if (pGalaxyCluster == nullptr)
			return S_FALSE;
		m_pGalaxyCluster = (CGalaxyCluster*)pGalaxyCluster;
		if (m_pGalaxy == nullptr)
		{
			IGalaxy* pGalaxy = nullptr;
			m_pGalaxyCluster->CreateGalaxy(CComVariant(0), CComVariant((LONGLONG)m_hClient), CComBSTR(L"default"), &pGalaxy);
			if (pGalaxy == nullptr)
			{
				delete m_pGalaxyCluster;
				m_pGalaxyCluster = nullptr;
			}
			m_pGalaxy = (CGalaxy*)pGalaxy;
		}
	}
	else
	{
		m_pGalaxy = (CGalaxy*)::SendMessage(m_hClient, WM_HUBBLE_DATA, 0, 1992);
		if (m_pGalaxy == nullptr)
		{
			auto it = m_pGalaxyCluster->m_mapGalaxy.find(m_hClient);
			if (it != m_pGalaxyCluster->m_mapGalaxy.end())
				m_pGalaxy = it->second;
			else
			{
				IGalaxy* pGalaxy = nullptr;
				m_pGalaxyCluster->CreateGalaxy(CComVariant(0), CComVariant((LONGLONG)m_hClient), CComBSTR(L"default"), &pGalaxy);
				if (pGalaxy == nullptr)
				{
					delete m_pGalaxyCluster;
					m_pGalaxyCluster = nullptr;
				}
				m_pGalaxy = (CGalaxy*)pGalaxy;
			}
		}
	}
	if (m_pGalaxy)
	{
		hr = m_pGalaxy->Observe(bstrKey, bstrXml, ppGrid);
		if (*ppGrid)
		{
			m_pCurGrid = (CGrid*)*ppGrid;
		}
	}
	return hr;
} 

STDMETHODIMP CEclipseWnd::ObserveEx(BSTR bstrKey, BSTR bstrXml, IGrid** ppGrid)
{
	HRESULT hr = Observe(bstrKey, bstrXml, ppGrid);
	if (*ppGrid)
	{
		m_pCurGrid = (CGrid*)*ppGrid;
		m_pCurGrid->put_SaveToConfigFile(true);
	}
	return hr;
} 

STDMETHODIMP CEclipseWnd::ObserveInView(int nIndex, BSTR bstrKey, BSTR bstrXml, IGrid** ppRetGrid)
{
	IEclipseCtrl* pCtrl = nullptr;
	get_Ctrl(CComVariant((int)nIndex), &pCtrl);
	if (pCtrl)
	{
		IGrid* pGrid = nullptr;
		return pCtrl->Observe(CComBSTR(L"EclipseView"), bstrKey, bstrXml, ppRetGrid);
	}

	return S_OK;
}

STDMETHODIMP CEclipseWnd::Active()
{
	SetFocus();

	return S_OK;
}

STDMETHODIMP CEclipseWnd::get_HubbleCtrl(LONGLONG hWnd, IEclipseCtrl** pVal)
{
	auto it = m_mapCtrl.find((HWND)hWnd);
	if (it != m_mapCtrl.end())
	{
		*pVal = it->second;
		(*pVal)->AddRef();
	}
	return S_OK;
}

void CEclipseWnd::Show(CString strID)
{
	if (m_pDoc)
		return;
	LONG_PTR data = 0;
	if(::IsWindow(m_hClient))
		data = ::GetWindowLongPtr(m_hClient, GWLP_USERDATA);
	if (g_pHubble->m_bIsEclipseInit == false)
		g_pHubble->m_bIsEclipseInit = true;
	if (data==0&&g_pHubble->m_pUniverseAppProxy)
	{
		m_strPath = strID;
		if (::IsChild(m_hWnd, m_hClient))
		{
			m_strAppProxyID = _T("");
			auto it = g_pHubble->m_mapWindowPage.find(m_hWnd);
			if (m_pGalaxyCluster == nullptr)
			{
				if (it != g_pHubble->m_mapWindowPage.end())
					m_pGalaxyCluster = (CGalaxyCluster*)it->second;
				else
				{
					m_pGalaxyCluster = new CComObject<CGalaxyCluster>();
					m_pGalaxyCluster->m_hWnd = m_hWnd;
					g_pHubble->m_mapWindowPage[m_hWnd] = m_pGalaxyCluster;

					for (auto it : g_pHubble->m_mapHubbleAppProxy)
					{
						CGalaxyClusterProxy* pHubbleProxy = it.second->OnGalaxyClusterCreated(m_pGalaxyCluster);
						if (pHubbleProxy)
							m_pGalaxyCluster->m_mapGalaxyClusterProxy[it.second] = pHubbleProxy;
					}
				}
			}

			if (m_strPath == _T("")||::PathFileExists(m_strPath)==false)
			{
				auto it = g_pHubble->m_mapCreatingWorkBenchInfo.find(strID);
				if (it != g_pHubble->m_mapCreatingWorkBenchInfo.end())
				{
					m_strDocKey = m_strXml = it->second;
					g_pHubble->m_mapCreatingWorkBenchInfo.erase(it);
				}
				else
					m_strDocKey = m_strXml = m_strPath = g_pHubble->m_strDefaultWorkBenchXml;
			}
			else
			{
				m_strDocKey = m_strXml = m_strPath;
				CTangramXmlParse m_Parse;
				if (m_Parse.LoadFile(m_strDocKey))
				{
					m_strAppProxyID = m_Parse.attr(_T("styleproxyid"), _T(""));
				}
			}
			if (m_strAppProxyID != _T(""))
			{
				m_pAppProxy = nullptr;
				auto it = g_pHubble->m_mapHubbleAppProxy.find(m_strAppProxyID.MakeLower());
				if (it != g_pHubble->m_mapHubbleAppProxy.end())
					m_pAppProxy = it->second;
				else
				{
					CString strPath = g_pHubble->m_strAppPath;
					HMODULE hHandle = nullptr;
					CString strAppName = _T("");
					int nPos = m_strAppProxyID.Find(_T("."));
					if (nPos != -1)
						strAppName = m_strAppProxyID.Left(nPos);
					CString strdll = strPath + m_strAppProxyID + _T("\\") + strAppName + _T(".dll");
					if (::PathFileExists(strdll))
						hHandle = ::LoadLibrary(strdll);
					if (hHandle == nullptr)
					{
						strdll = g_pHubble->m_strAppCommonDocPath2 + m_strAppProxyID + _T("\\") + strAppName + _T(".dll");
						if (::PathFileExists(strdll))
							hHandle = ::LoadLibrary(strdll);
					}
					if (hHandle)
					{
						it = g_pHubble->m_mapHubbleAppProxy.find(m_strAppProxyID.MakeLower());
						if (it != g_pHubble->m_mapHubbleAppProxy.end())
						{
							m_pAppProxy = it->second;
						}
					}
				}
				if (m_pAppProxy)
				{
					g_pHubble->m_pActiveAppProxy = m_pAppProxy;
					m_pAppProxy->m_hCreatingView = m_hClient;
					::SetWindowText(m_hClient, g_pHubble->m_strAppKey);
					::SetWindowLongPtr(m_hClient, GWLP_USERDATA, (LONG_PTR)1);
					m_pDoc = (CHubbleDoc*)m_pAppProxy->NewDoc();
					if (m_pDoc)
					{
						auto it = m_pGalaxyCluster->m_mapGalaxy.find(m_hClient);
						if (it != m_pGalaxyCluster->m_mapGalaxy.end())
							m_pGalaxy = it->second;
					}
				}
			}
			else if(strID !=_T(""))
			{
				IGalaxy* pGalaxy = nullptr;
				m_pGalaxyCluster->CreateGalaxy(CComVariant(0), CComVariant((LONGLONG)m_hClient), CComBSTR(L"default"), &pGalaxy);
				if (pGalaxy)
				{
					m_pGalaxy = (CGalaxy*)pGalaxy;
					m_pGalaxyCluster->m_mapNeedSaveGalaxy[m_hClient] = m_pGalaxy;
					CString strKey = _T("default");
					if (m_strDocKey == _T(""))
					{
						m_strDocKey = m_strXml = m_strPath = g_pHubble->m_strAppDataPath + _T("default.workbench");
					}
					else
					{
						strKey = strID;
						if (g_pHubble->m_nAppType == APP_ECLIPSE)
							m_strDocKey = g_pHubble->m_strStartupURL;
					}

					IGrid* pGrid = nullptr;
					pGalaxy->Observe(CComBSTR(strKey), CComBSTR(m_strDocKey), &pGrid);
					if (pGrid == nullptr)
					{
						delete m_pGalaxy;
						m_pGalaxy = nullptr;
						m_pGalaxyCluster = nullptr;
					}
				}
			}
		}
	}
}

LRESULT CEclipseWnd::OnShowWindow(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& )
{
	if (m_pDoc == nullptr)
	{
		if (g_pHubble->m_strWorkBenchStrs != _T(""))
		{
			int nPos = g_pHubble->m_strWorkBenchStrs.Find(_T("|"));
			m_strDocKey = g_pHubble->m_strWorkBenchStrs.Left(nPos);
			g_pHubble->m_strWorkBenchStrs = g_pHubble->m_strWorkBenchStrs.Mid(nPos + 1);
		}

		if (g_pHubble->m_strCurrentEclipsePagePath != _T(""))
		{
			m_strDocKey = g_pHubble->m_strCurrentEclipsePagePath;
			g_pHubble->m_strCurrentEclipsePagePath = _T("");
		}
		if (::IsWindow(m_hClient)/*&& m_strDocKey!=_T("")*/)
		{
			Show(m_strDocKey);
		}
	}
	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CEclipseWnd::OnCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&)
{
	if (wParam == m_nNewWinCmdID)
	{
		if (g_pHubble->m_strCurrentEclipsePagePath == _T(""))
		{
			CComBSTR bstrTemplate(L"");
			g_pHubble->GetDocTemplateXml(CComBSTR("Please select New Eclipse Window Bench Template:"), CComBSTR(g_pHubble->m_strAppDataPath+_T("workbench\\")), _T(".eclipse"), &bstrTemplate);
			g_pHubble->m_strCurrentEclipsePagePath = OLE2T(bstrTemplate);
		}
		if (g_pHubble->m_strCurrentEclipsePagePath == _T("")&& m_pGalaxy)
		{
			IGrid* pGrid = nullptr;
			m_pGalaxy->Observe(CComBSTR(L"newdocument"), g_pHubble->m_strNewDocXml.AllocSysString(), &pGrid);
			if (pGrid)
				return 0;
		}
	}
	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CEclipseWnd::OnHubbleGetXml(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&)
{
	CString strGalaxyName = (LPCTSTR)wParam;
	CString currentKey = (LPCTSTR)lParam;
	CString strIndex = strGalaxyName + L"_" + currentKey;
	CTangramXmlParse parse;
	if (parse.LoadXml(m_strXml) || parse.LoadFile(m_strXml))
	{
		CTangramXmlParse* pParse = parse.GetChild(strGalaxyName);
		if (pParse)
		{
			CTangramXmlParse* pParse2 = pParse->GetChild(currentKey);
			if (pParse2)
			{
				auto it = g_pHubble->m_mapValInfo.find(strIndex);
				if (it != g_pHubble->m_mapValInfo.end())
				{
					g_pHubble->m_mapValInfo.erase(it);
				}
				g_pHubble->m_mapValInfo[strIndex] = CComVariant(pParse2->xml());
				return 1;
			}
		}
	}
	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CEclipseWnd::OnHubbleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& )
{
	switch (wParam)
	{
	case 1963:
		{
			return ::SendMessage(::GetParent(m_hClient), WM_QUERYAPPPROXY, wParam, 0);
		}
		break;
	case 20190305:
		{
			return (LRESULT)this;
		}
		break;
	case 19820911:
		{
			if (lParam == 0)
			{
				::PostMessage(m_hWnd, WM_COSMOSMSG, 19820911, 1);
			}
			if (lParam == 1)
			{
				CGalaxy* pGalaxy = (CGalaxy*)::SendMessage(m_hClient, WM_HUBBLE_DATA, 0, 1992);
				if (pGalaxy)
				{
					pGalaxy->m_nGalaxyType = EclipseWorkBenchGalaxy;
					pGalaxy->m_pWorkBenchFrame = this;
				}
				else
				{
					if (m_pDoc == nullptr)
					{
						if (g_pHubble->m_strWorkBenchStrs != _T(""))
						{
							int nPos = g_pHubble->m_strWorkBenchStrs.Find(_T("|"));
							m_strDocKey = g_pHubble->m_strWorkBenchStrs.Left(nPos);
							g_pHubble->m_strWorkBenchStrs = g_pHubble->m_strWorkBenchStrs.Mid(nPos + 1);
						}
						if (g_pHubble->m_strCurrentEclipsePagePath != _T(""))
						{
							m_strDocKey = g_pHubble->m_strCurrentEclipsePagePath;
							g_pHubble->m_strCurrentEclipsePagePath = _T("");
						}
						LONG_PTR data = 0;
						if (::IsWindow(m_hClient)/*&& m_strDocKey!=_T("")*/)
						{
							data = ::GetWindowLongPtr(m_hClient, GWLP_USERDATA);
							if (data == 0)
							{
								Show(m_strDocKey);
								break;
							}
						}
					}
				}
			}
		}
		break;
	}

	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CEclipseWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& )
{
	m_strPath = g_pHubble->m_strAppDataPath + _T("default.workbench");
	if (m_strPath != _T(""))
	{
		LRESULT lRes = ::SendMessage(::GetParent(m_hClient),WM_QUERYAPPPROXY,0,19631222);
		if (lRes == 0)
		{
			CTangramXmlParse m_Parse;
			if (m_pGalaxyCluster)
			{
				m_strXml = _T("<eclipseplus>");
				for (auto it : m_pGalaxyCluster->m_mapGalaxy)
				{
					CGalaxy* pGalaxy = it.second;
					for (auto it2 : pGalaxy->m_mapGrid)
					{
						CGrid* pWndGrid = (CGrid*)it2.second;
						if (pWndGrid)
						{
							if (pWndGrid->m_pWindow)
							{
								if (pWndGrid->m_nActivePage > 0)
								{
									CString strVal = _T("");
									strVal.Format(_T("%d"), pWndGrid->m_nActivePage);
									pWndGrid->m_pHostParse->put_attr(_T("activepage"), strVal);
								}
								pWndGrid->m_pWindow->Save();
							}
							if (pWndGrid->m_nViewType == Grid)
							{
								((CGridWnd*)pWndGrid->m_pHostWnd)->Save();
							}
							g_pHubble->UpdateGrid(pWndGrid);
							for (auto it2 : pWndGrid->m_vChildNodes)
							{
								g_pHubble->UpdateGrid(it2);
							}
						}
					}
				}
				for (auto it : m_pGalaxyCluster->m_mapNeedSaveGalaxy)
				{
					CGalaxy* pGalaxy = it.second;
					auto it2 = pGalaxy->m_mapGrid.find(_T("default"));
					if (it2 != pGalaxy->m_mapGrid.end())
					{
						CGrid* _pGrid = (CGrid*)it2->second;
						if (pGalaxy->m_strGalaxyName.Find(_T("@")) == -1)
						{
							CString strXml = _pGrid->m_pGridShareData->m_pHubbleParse->xml();
							strXml.Replace(_T("/><"), _T("/>\r\n<"));
							strXml.Replace(_T("/>"), _T("></grid>"));
							CString s = _T("");
							s.Format(_T("<%s>%s</%s>"), pGalaxy->m_strGalaxyName, strXml, pGalaxy->m_strGalaxyName);
							m_strXml += s;
						}
					}
				}
				m_strXml += _T("</eclipseplus>");
			}
			bool bSave = false;
			if (m_Parse.LoadXml(m_strXml))
			{
				if (m_pAppProxy)
				{
					m_Parse.put_attr(_T("styleproxyid"), m_strAppProxyID);
				}
				bSave = m_Parse.SaveFile(m_strPath);
			}
		}
	}
	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
	return lRes;
}

LRESULT CEclipseWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& )
{
	CTangramXmlParse xmlParse;
	if (xmlParse.LoadFile(g_pHubble->m_strConfigDataFile))
	{
		CTangramXmlParse* pParse = xmlParse.GetChild(_T("openedworkbench"));
		if (pParse == nullptr)
		{
			pParse = xmlParse.AddNode(_T("openedworkbench"));
		}
		if (pParse)
		{
			CString strWorkBenchStrs = m_strDocKey + _T("|");
			strWorkBenchStrs += pParse->text();
			pParse->put_text(strWorkBenchStrs);
			xmlParse.SaveFile(g_pHubble->m_strConfigDataFile);
		}
	}
	if (g_pHubble->m_pActiveEclipseWnd == this)
		g_pHubble->m_pActiveEclipseWnd = nullptr;
	auto it = g_pHubble->m_mapWorkBenchWnd.find(m_hWnd);
	if (it != g_pHubble->m_mapWorkBenchWnd.end())
	{
		g_pHubble->m_mapWorkBenchWnd.erase(it);
	}

	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);

	if (g_pHubble->m_mapWorkBenchWnd.size() == 0)
	{
		if (::GetModuleHandle(L"chrome_elf.dll"))
		{
			if (g_pHubble)
			{
				if (g_pHubble->m_mapBrowserWnd.size())
				{
					g_pHubble->m_bChromeNeedClosed = true;
					auto it = g_pHubble->m_mapBrowserWnd.begin();
					((CBrowser*)it->second)->SendMessageW(WM_CLOSE, 0, 0);
				}
			}
		}
		if (::IsWindow(g_pHubble->m_hHostWnd))
			::DestroyWindow(g_pHubble->m_hHostWnd);
	}
	return lRes;
}

LRESULT CEclipseWnd::OnActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& )
{
	LRESULT lRes = DefWindowProc(uMsg, wParam, lParam);
	if (LOWORD(wParam) != WA_INACTIVE)
	{
		g_pHubble->m_pActiveEclipseWnd = this;
		g_pHubble->m_pActiveAppProxy = m_pAppProxy;
	}
	return lRes;
}

LRESULT CEclipseWnd::OnQueryAppProxy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& )
{
	LRESULT lRes = ::SendMessage(::GetParent(m_hClient), uMsg, wParam, 19631992);
	if (lRes)
	{
		if (m_pAppProxy==nullptr)
			m_pAppProxy = (IUniverseAppProxy*)lRes;

		return lRes;
	}
	
	lRes = DefWindowProc(uMsg, wParam, lParam);
	return lRes;
}

CEclipseCtrl::CEclipseCtrl()
{
	m_varTag.vt = VT_BSTR;
	m_varTag.bstrVal = ::SysAllocString(L"");
	m_hEclipseViewWnd = NULL;
	m_pCurGrid = nullptr;
	m_pEclipseWnd = nullptr;
	m_pGalaxyClusterProxy = nullptr;
}

STDMETHODIMP CEclipseCtrl::get_AppKeyValue(BSTR bstrKey, VARIANT* pVal)
{
	if (g_pHubble)
	{
		return g_pHubble->get_AppKeyValue(bstrKey, pVal);
	}
	return S_FALSE;
}

STDMETHODIMP CEclipseCtrl::put_AppKeyValue(BSTR bstrKey, VARIANT newVal)
{
	if (g_pHubble)
	{
		return g_pHubble->put_AppKeyValue(bstrKey, newVal);
	}

	return S_OK;
}

STDMETHODIMP CEclipseCtrl::get_tag(VARIANT* pVal)
{
	*pVal = m_varTag;
	return S_OK;
}

STDMETHODIMP CEclipseCtrl::put_tag(VARIANT newVal)
{
	::VariantClear(&m_varTag);
	m_varTag = newVal;
	return S_OK;
}

HRESULT CEclipseCtrl::Fire_GalaxyClusterLoaded(IDispatch* sender, BSTR url)
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		CComVariant avarParams[2];
		avarParams[1] = sender;
		avarParams[1].vt = VT_DISPATCH;
		avarParams[0] = url;
		avarParams[0].vt = VT_BSTR;
		DISPPARAMS params = { avarParams, NULL, 2, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pHubble->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			g_pHubble->Unlock();
			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);
			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(1, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}
	if (m_pGalaxyClusterProxy)
		m_pGalaxyClusterProxy->OnGalaxyClusterLoaded(sender, OLE2T(url));
	return hr;
}

HRESULT CEclipseCtrl::Fire_NodeCreated(IGrid * pGridCreated)
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		CComVariant avarParams[1];
		avarParams[0] = pGridCreated;
		avarParams[0].vt = VT_DISPATCH;
		DISPPARAMS params = { avarParams, NULL, 1, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pHubble->Lock();
			IUnknown* punkConnection = m_vec.GetAt(iConnection);
			g_pHubble->Unlock();
			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection);
			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(2, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}
	if (m_pGalaxyClusterProxy)
		m_pGalaxyClusterProxy->OnGridCreated(pGridCreated);
	return hr;
}

HRESULT CEclipseCtrl::Fire_AddInCreated(IGrid * pRootGrid, IDispatch * pAddIn, BSTR bstrID, BSTR bstrAddInXml)
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		CComVariant avarParams[4];
		avarParams[3] = pRootGrid;
		avarParams[2] = pAddIn;
		avarParams[2].vt = VT_DISPATCH;
		avarParams[1] = bstrID;
		avarParams[1].vt = VT_BSTR;
		avarParams[0] = bstrAddInXml;
		avarParams[0].vt = VT_BSTR;
		DISPPARAMS params = { avarParams, NULL, 4, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pHubble->Lock();
			IUnknown* punkConnection = m_vec.GetAt(iConnection);
			g_pHubble->Unlock();
			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection);
			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(3, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}
	if (m_pGalaxyClusterProxy)
		m_pGalaxyClusterProxy->OnAddInCreated(pRootGrid, pAddIn, OLE2T(bstrID), OLE2T(bstrAddInXml));
	return hr;
}

HRESULT CEclipseCtrl::Fire_BeforeOpenXml(BSTR bstrXml, LONGLONG hWnd)
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		CComVariant avarParams[2];
		avarParams[1] = bstrXml;
		avarParams[1].vt = VT_BSTR;
		avarParams[0] = hWnd;
		avarParams[0].vt = VT_I8;
		DISPPARAMS params = { avarParams, NULL, 2, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pHubble->Lock();
			IUnknown* punkConnection = m_vec.GetAt(iConnection);
			g_pHubble->Unlock();
			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection);
			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(4, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}
	if (m_pGalaxyClusterProxy)
		m_pGalaxyClusterProxy->OnBeforeOpenXml(OLE2T(bstrXml), (HWND)hWnd);
	return hr;
}

HRESULT CEclipseCtrl::Fire_OpenXmlComplete(BSTR bstrXml, LONGLONG hWnd, IGrid * pRetRootNode)
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		CComVariant avarParams[3];
		avarParams[2] = bstrXml;
		avarParams[2].vt = VT_BSTR;
		avarParams[1] = hWnd;
		avarParams[1].vt = VT_I8;
		avarParams[0] = pRetRootNode;
		avarParams[0].vt = VT_DISPATCH;
		DISPPARAMS params = { avarParams, NULL, 3, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pHubble->Lock();
			IUnknown* punkConnection = m_vec.GetAt(iConnection);
			g_pHubble->Unlock();
			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection);
			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(5, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}
	if (m_pGalaxyClusterProxy)
		m_pGalaxyClusterProxy->OnOpenXmlComplete(OLE2T(bstrXml), (HWND)hWnd, pRetRootNode);

	return hr;
}

HRESULT CEclipseCtrl::Fire_Destroy()
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		DISPPARAMS params = { NULL, NULL, 0, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pHubble->Lock();
			IUnknown* punkConnection = m_vec.GetAt(iConnection);
			g_pHubble->Unlock();
			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection);
			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(6, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}
	if (m_pGalaxyClusterProxy)
		m_pGalaxyClusterProxy->OnDestroy();

	return hr;
}

HRESULT CEclipseCtrl::Fire_NodeMouseActivate(IGrid * pActiveNode)
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		CComVariant avarParams[1];
		avarParams[0] = pActiveNode;
		avarParams[0].vt = VT_DISPATCH;
		DISPPARAMS params = { avarParams, NULL, 1, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pHubble->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			g_pHubble->Unlock();
			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);
			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(7, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}

	if (m_pGalaxyClusterProxy)
		m_pGalaxyClusterProxy->OnNodeMouseActivate(pActiveNode);
	return hr;
}

HRESULT CEclipseCtrl::Fire_ClrControlCreated(IGrid * Node, IDispatch * Ctrl, BSTR CtrlName, LONGLONG CtrlHandle)
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		CComVariant avarParams[4];
		avarParams[3] = Node;
		avarParams[3].vt = VT_DISPATCH;
		avarParams[2] = Ctrl;
		avarParams[2].vt = VT_DISPATCH;
		avarParams[1] = CtrlName;
		avarParams[1].vt = VT_BSTR;
		avarParams[0] = CtrlHandle;
		avarParams[0].vt = VT_I8;
		DISPPARAMS params = { avarParams, NULL, 4, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pHubble->Lock();
			IUnknown* punkConnection = m_vec.GetAt(iConnection);
			g_pHubble->Unlock();
			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection);
			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(8, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}

	if (m_pGalaxyClusterProxy)
		m_pGalaxyClusterProxy->OnClrControlCreated(Node, Ctrl, OLE2T(CtrlName), (HWND)CtrlHandle);
	return hr;
}

HRESULT CEclipseCtrl::Fire_TabChange(IGrid* sender, LONG ActivePage, LONG OldPage)
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		CComVariant avarParams[3];
		avarParams[2] = sender;
		avarParams[2].vt = VT_DISPATCH;
		avarParams[1] = ActivePage;
		avarParams[1].vt = VT_I4;
		avarParams[0] = OldPage;
		avarParams[0].vt = VT_I4;
		DISPPARAMS params = { avarParams, NULL, 3, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pHubble->Lock();
			IUnknown* punkConnection = m_vec.GetAt(iConnection);
			g_pHubble->Unlock();
			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection);
			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(9, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}

	if (m_pGalaxyClusterProxy)
		m_pGalaxyClusterProxy->OnTabChange(sender, ActivePage, OldPage);
	return hr;
}

HRESULT CEclipseCtrl::Fire_Event(IGrid* sender, IDispatch* EventArg)
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		CComVariant avarParams[2];
		avarParams[1] = sender;
		avarParams[1].vt = VT_DISPATCH;
		avarParams[0] = EventArg;
		avarParams[0].vt = VT_DISPATCH;
		DISPPARAMS params = { avarParams, NULL, 2, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pHubble->Lock();
			IUnknown* punkConnection = m_vec.GetAt(iConnection);
			g_pHubble->Unlock();
			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection);
			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(10, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}

	return hr;
}

HRESULT CEclipseCtrl::Fire_ControlNotify(IGrid * sender, LONG NotifyCode, LONG CtrlID, LONGLONG CtrlHandle, BSTR CtrlClassName)
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		CComVariant avarParams[5];
		avarParams[4] = sender;
		avarParams[4].vt = VT_DISPATCH;
		avarParams[3] = NotifyCode;
		avarParams[3].vt = VT_I4;
		avarParams[2] = CtrlID;
		avarParams[2].vt = VT_I4;
		avarParams[1] = CtrlHandle;
		avarParams[1].vt = VT_I8;
		avarParams[0] = CtrlClassName;
		avarParams[0].vt = VT_BSTR;
		DISPPARAMS params = { avarParams, NULL, 5, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pHubble->Lock();
			CComPtr<IUnknown> punkConnection = m_vec.GetAt(iConnection);
			g_pHubble->Unlock();

			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection.p);
			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(11, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}
	if (m_pGalaxyClusterProxy)
		m_pGalaxyClusterProxy->OnControlNotify(sender, NotifyCode, CtrlID, (HWND)CtrlHandle, OLE2T(CtrlClassName));
	return hr;
}

HRESULT CEclipseCtrl::Fire_HubbleEvent(IHubbleEventObj* pEventObj)
{
	HRESULT hr = S_OK;
	int cConnections = m_vec.GetSize();
	if (cConnections)
	{
		CComVariant avarParams[1];
		avarParams[0] = pEventObj;
		avarParams[0].vt = VT_DISPATCH;
		DISPPARAMS params = { avarParams, NULL, 1, 0 };
		for (int iConnection = 0; iConnection < cConnections; iConnection++)
		{
			g_pHubble->Lock();
			IUnknown* punkConnection = m_vec.GetAt(iConnection);
			g_pHubble->Unlock();
			IDispatch * pConnection = static_cast<IDispatch *>(punkConnection);
			if (pConnection)
			{
				CComVariant varResult;
				hr = pConnection->Invoke(12, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &params, &varResult, NULL, NULL);
			}
		}
	}
	if (m_pGalaxyClusterProxy)
		m_pGalaxyClusterProxy->OnHubbleEvent(pEventObj);
	return hr;
}

STDMETHODIMP CEclipseCtrl::InitCtrl(BSTR bstrXml)
{
	CString strXml = OLE2T(bstrXml);
	if (strXml != _T(""))
	{
		CTangramXmlParse m_Parse;
		if (!m_Parse.LoadXml(strXml))
		{
			CString strPath = g_pHubble->m_strAppPath + strXml;
			if (m_Parse.LoadFile(strPath) == false)
				return S_OK;
		}
		int nCount = m_Parse.GetCount();
		CTangramXmlParse* pChild = nullptr;
		for (int i = 0; i < nCount; i++)
		{
			pChild = m_Parse.GetChild(i);
			CString strName = pChild->name();
			strName.MakeLower();
			auto it = m_mapHubbleInfo.find(strName);
			if (it == m_mapHubbleInfo.end())
			{
				m_mapHubbleInfo[strName] = pChild->xml();
			}
		}
	}

	return S_OK;
}

STDMETHODIMP CEclipseCtrl::put_Handle(BSTR bstrHandleName, LONGLONG newVal)
{
	CString strName = OLE2T(bstrHandleName);
	HWND hWnd = (HWND)newVal;
	if (strName != _T("") && ::IsWindow(hWnd))
	{
		auto it = m_mapHubbleHandle.find(strName);
		if (it != m_mapHubbleHandle.end())
			m_mapHubbleHandle.erase(strName);
		m_mapHubbleHandle[strName] = hWnd;
	}

	return S_OK;
}

STDMETHODIMP CEclipseCtrl::Observe(BSTR bstrGalaxyName, BSTR bstrKey, BSTR bstrXml, IGrid** ppGrid)
{
	CString strGalaxyName = OLE2T(bstrGalaxyName);
	if (strGalaxyName == _T(""))
		strGalaxyName = _T("EclipseView");
	if (strGalaxyName.CompareNoCase(_T("TopView")) == 0)
	{
		return m_pEclipseWnd->Observe(bstrKey, bstrXml, ppGrid);
	}
	auto it = m_mapHubbleHandle.find(strGalaxyName);
	if (it == m_mapHubbleHandle.end())
		return S_FALSE;
	HWND hWnd = it->second;
	CString strKey = OLE2T(bstrKey);
	if (strKey == _T(""))
		strKey= _T("default");
	CString strXml = OLE2T(bstrXml);
	IGrid* pGrid = nullptr;
	auto it2 = m_mapGalaxy.find(strGalaxyName);
	if (it2 == m_mapGalaxy.end())
	{
		HWND hTop = ::GetAncestor(m_hWnd, GA_ROOT);
		IGalaxyCluster* pGalaxyCluster = nullptr;
		g_pHubble->CreateGalaxyCluster((LONGLONG)hTop, &pGalaxyCluster);
		if (pGalaxyCluster == nullptr)
			return S_OK;

		IGalaxy* pGalaxy = nullptr;
		pGalaxyCluster->CreateGalaxy(CComVariant(0), CComVariant((long)hWnd), bstrGalaxyName, &pGalaxy);
		if (pGalaxy == nullptr)
		{
			return S_FALSE;
		}
		m_mapGalaxy[strGalaxyName] = (CGalaxy*)pGalaxy;
		HRESULT hr = pGalaxy->Observe(bstrKey, bstrXml, ppGrid);
		if (hr == S_OK&&strGalaxyName.CompareNoCase(_T("EclipseView")) == 0)
		{
			if (strGalaxyName.CompareNoCase(_T("EclipseView")) == 0)
			{
				((CGalaxy*)pGalaxy)->m_nGalaxyType = EclipseViewGalaxy;
			}
			else
				((CGalaxy*)pGalaxy)->m_nGalaxyType = EclipseSWTGalaxy;
			(*ppGrid)->put_SaveToConfigFile(true);
		}
		return hr;
	}
	HRESULT hr = it2->second->Observe(bstrKey, bstrXml, ppGrid);
	if(hr==S_OK&&strGalaxyName.CompareNoCase(_T("EclipseView")) == 0)
		(*ppGrid)->put_SaveToConfigFile(true);
	return hr;
}
	
STDMETHODIMP CEclipseCtrl::ObserveEx(BSTR bstrGalaxyName, BSTR bstrKey, BSTR bstrXml, IGrid** ppGrid)
{
	HRESULT hr = Observe(bstrGalaxyName, bstrKey, bstrXml, ppGrid);
	if (hr == S_OK&&*ppGrid != nullptr)
		(*ppGrid)->put_SaveToConfigFile(true);
	return S_OK;
}

void CEclipseCtrl::OnFinalMessage(HWND hWnd)
{
	::VariantClear(&m_varTag);
	if (m_pEclipseWnd)
	{
		auto it = m_pEclipseWnd->m_mapCtrl.find(hWnd);
		if (it != m_pEclipseWnd->m_mapCtrl.end())
			m_pEclipseWnd->m_mapCtrl.erase(it);
	}
	__super::OnFinalMessage(hWnd);
	if (g_pHubble->m_bEclipse)
		Release();
}

STDMETHODIMP CEclipseCtrl::get_EclipseViewHandle(LONGLONG* pVal)
{
	*pVal = (LONGLONG)m_hEclipseViewWnd;
	return S_OK;
}

LRESULT CEclipseCtrl::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&)
{
	LRESULT lr = DefWindowProc(uMsg, wParam, lParam);
	HWND hTop = ::GetAncestor(m_hWnd, GA_ROOT);
	HWND hClient = ::GetWindow(hTop, GW_CHILD);
	m_mapHubbleHandle[_T("TopView")] = hClient;
	m_hEclipseViewWnd = ::GetParent(::GetParent(::GetParent(m_hWnd)));
	m_mapHubbleHandle[_T("EclipseView")] = m_hEclipseViewWnd;
	::PostMessage(m_hWnd, WM_COSMOSMSG, 0, 0);
	return lr;
}

LRESULT CEclipseCtrl::OnHubbleMsg(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL&)
{
	LRESULT lr = DefWindowProc(uMsg, wParam, lParam);
	HWND hTop = ::GetAncestor(m_hWnd, GA_ROOT);

	auto it = g_pHubble->m_mapWorkBenchWnd.find(hTop);
	if (it != g_pHubble->m_mapWorkBenchWnd.end())
	{
		m_pEclipseWnd = (CEclipseWnd*)it->second;
		auto it2 = m_pEclipseWnd->m_mapCtrl.find(m_hWnd);
		if (it2 == m_pEclipseWnd->m_mapCtrl.end())
			m_pEclipseWnd->m_mapCtrl[m_hWnd] = this;
		if (m_pEclipseWnd->m_pGalaxyCluster)
		{
			auto it = m_pEclipseWnd->m_pGalaxyCluster->m_mapNotifyCtrl.find(m_hWnd);
			if (it == m_pEclipseWnd->m_pGalaxyCluster->m_mapNotifyCtrl.end())
				m_pEclipseWnd->m_pGalaxyCluster->m_mapNotifyCtrl[m_hWnd] = this;
		}
	}
	return lr;
}

STDMETHODIMP CEclipseCtrl::get_HWND(LONGLONG* pVal)
{
	*pVal = (LONGLONG)m_hWnd;
	return S_OK;
}

STDMETHODIMP CEclipseCtrl::get_Hubble(IHubble** pVal)
{
	*pVal = g_pHubble;
	return S_OK;
}

STDMETHODIMP CEclipseCtrl::get_GalaxyCluster(IGalaxyCluster** pVal)
{
	if (m_pEclipseWnd->m_pGalaxyCluster)
		*pVal = m_pEclipseWnd->m_pGalaxyCluster;
	return S_OK;
}

STDMETHODIMP CEclipseCtrl::get_WorkBenchWindow(IWorkBenchWindow** pVal)
{
	if (m_pEclipseWnd)
		*pVal = m_pEclipseWnd;
	return S_OK;
}

STDMETHODIMP CEclipseCtrl::get_TopGalaxyCluster(IGalaxyCluster** pVal)
{
	if (m_pEclipseWnd)
	{
		*pVal = m_pEclipseWnd->m_pGalaxyCluster;
	}

	return S_OK;
}

STDMETHODIMP CEclipseCtrl::get_TopGalaxy(IGalaxy** pVal)
{
	if (m_pEclipseWnd)
	{
		*pVal = m_pEclipseWnd->m_pGalaxy;
	}

	return S_OK;
}

STDMETHODIMP CEclipseCtrl::get_ActiveTopGrid(IGrid** pVal)
{
	if (m_pEclipseWnd)
	{
		*pVal = m_pEclipseWnd->m_pGalaxy->m_pWorkGrid;
	}

	return S_OK;
}

HRESULT CEclipseCtrl::FinalConstruct()
{
	CString strKey = _T("startdata");
	auto it = g_pHubble->m_mapValInfo.find(strKey);
	if (it != g_pHubble->m_mapValInfo.end())
	{
		CString strData = OLE2T(it->second.bstrVal);
		int nPos = strData.Find(_T("|"));
		if (nPos != -1)
		{
			g_pHubble->m_strStartView = strData.Left(nPos);
			g_pHubble->m_strStartXml = strData.Mid(nPos + 1);
			if (::IsWindow(g_pHubble->m_hHostWnd))
			{
				::PostMessage(g_pHubble->m_hHostWnd, WM_HUBBLE_APPQUIT, 0, 0);
			}
		}
		::VariantClear(&it->second);
		g_pHubble->m_mapValInfo.erase(it);
	}

	return S_OK;
}
