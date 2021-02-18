/********************************************************************************
 *           Web Runtime for Application - Version 1.0.0.202102180033           *
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
 * Use of this source code is governed by a BSD-style license that
 * can be found in the LICENSE file.
 *
 * CONTACT INFORMATION:
 * mailto:tangramteam@outlook.com or mailto:sunhuizlz@yeah.net
 * https://www.tangram.dev
 *******************************************************************************/

#pragma once

namespace Browser {
	using namespace ATL;

	class CWebPage;

	class ATL_NO_VTABLE CBrowser :
		public CBrowserImpl, 
		public CWindowImpl<CBrowser, CWindow>,
		public CComObjectRootEx<CComSingleThreadModel>,
		public IDispatchImpl<IBrowser, &_uuidof(IBrowser), &LIBID_Universe, 1, 0>
	{
		friend class CXobj;
	public:
		CBrowser();
		~CBrowser() override;

		bool				m_bDestroy = false;
		bool				m_bTabChange;
		bool				m_bSZMode = false;

		float				m_fdevice_scale_factor;
		HWND				m_hDrawWnd;
		HWND				m_hOldTab;
		CString				m_strCurKey;
		CWebPage*			m_pVisibleWebWnd;
		IXobj*				m_pRemoteXobj;
		CXobj*				m_pParentXobj;
		CGalaxy*			m_pClientGalaxy = nullptr;
		CosmosFrameWndInfo* m_pCosmosFrameWndInfo = nullptr;

		map<HWND, CWebPage*> m_mapChildPage;

		STDMETHOD(get_RemoteXobj)(IXobj** pVal);
		STDMETHOD(put_RemoteXobj)(IXobj* newVal);
		STDMETHOD(AddURLs)(BSTR bstrURLs);
		STDMETHOD(OpenURL)(BSTR bstrURL, BrowserWndOpenDisposition nDisposition, BSTR bstrKey, BSTR bstrXml);
		BEGIN_MSG_MAP(CBrowser)
			MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
			MESSAGE_HANDLER(WM_ACTIVATE, OnActivate)
			MESSAGE_HANDLER(WM_COSMOSMSG, OnCosmosMsg)
			MESSAGE_HANDLER(WM_EXITSIZEMOVE, OnExitSZ)
			MESSAGE_HANDLER(WM_ENTERSIZEMOVE, OnEnterSZ)
			MESSAGE_HANDLER(WM_TABCHANGE, OnChromeTabChange)
			MESSAGE_HANDLER(WM_MOUSEACTIVATE, OnMouseActivate)
			MESSAGE_HANDLER(WM_BROWSERLAYOUT, OnBrowserLayout)
			MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, OnWindowPosChanging)
			MESSAGE_HANDLER(WM_DEVICESCALEFACTORCHANGED, OnDeviceScaleFactorChanged)
		END_MSG_MAP()

		BEGIN_COM_MAP(CBrowser)
			COM_INTERFACE_ENTRY(IBrowser)
			COM_INTERFACE_ENTRY(IDispatch)
		END_COM_MAP()
	protected:
		ULONG InternalAddRef() { return 1; }
		ULONG InternalRelease() { return 1; }
	private:
		void UpdateContentRect(HWND hContentWnd, RECT& rc, int nTopFix) override;
		void ActiveChromeTab(HWND hActive, HWND hOldWnd) override;

		void OnFinalMessage(HWND hWnd) override;
		LRESULT OnExitSZ(UINT, WPARAM, LPARAM, BOOL&);
		LRESULT OnEnterSZ(UINT, WPARAM, LPARAM, BOOL&);
		LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&);
		LRESULT OnActivate(UINT, WPARAM, LPARAM, BOOL&);
		LRESULT OnCosmosMsg(UINT, WPARAM, LPARAM, BOOL&);
		LRESULT OnBrowserLayout(UINT, WPARAM, LPARAM, BOOL&);
		LRESULT OnChromeTabChange(UINT, WPARAM, LPARAM, BOOL&);
		LRESULT OnMouseActivate(UINT, WPARAM, LPARAM, BOOL&);
		LRESULT OnWindowPosChanging(UINT, WPARAM, LPARAM, BOOL&);
		LRESULT OnDeviceScaleFactorChanged(UINT, WPARAM, LPARAM, BOOL&);
	};
};