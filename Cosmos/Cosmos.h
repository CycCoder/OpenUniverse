/********************************************************************************
*					Open Universe - version 0.9.9								*
*********************************************************************************
* Copyright (C) 2002-2020 by Tangram Team.   All Rights Reserved.				*
*
* This SOURCE CODE is governed by a BSD - style license that can be
* found in the LICENSE file.
*
* CONTACT INFORMATION:
* mailto:tangramteam@outlook.com
* https://www.tangram.dev
********************************************************************************/

#pragma once
#include "CosmosProxy.h"
#include <map>

using namespace std;
using namespace System;
using namespace System::IO;
using namespace System::Windows;
using namespace System::Reflection;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Runtime::InteropServices;
using namespace System::ComponentModel;
using namespace System::Collections::Generic;
using namespace System::Threading;
using namespace System::Threading::Tasks;

using System::Runtime::InteropServices::Marshal;

class CGridCLREvent;
class CCosmosGridEvent;
extern CCosmosProxy theAppProxy;

namespace Cosmos
{
	/// <summary>
	/// Summary for Hubble
	/// </summary>

	ref class Grid;
	ref class Quasar;
	ref class Browser;

	public enum class CosmosAppType
	{
		APPWIN32 = APP_WIN32,
		APPBROWSER = APP_BROWSER,
		APPECLIPSE = APP_ECLIPSE,
		APPBROWSER_ECLIPSE = APP_BROWSER_ECLIPSE,
		APPBROWSERAPP = APP_BROWSERAPP,
		APPOTHER = 0
	};

	public ref class Grid
	{
	public:
		Grid(IGrid* pGrid);
		~Grid();

		IGrid* m_pGrid;
		Object^ m_pHostObj = nullptr;
		CCosmosGridEvent* m_pGridEvent;
		CGridCLREvent* m_pGridCLREvent;
		Cosmos::Wormhole^ m_pWormhole = nullptr;
		CBrowserImpl* m_pChromeBrowserProxy;

		delegate void GridAddInCreated(Grid^ sender, Object^ pAddIndisp, String^ bstrAddInID, String^ bstrAddInXml);
		event GridAddInCreated^ OnGridAddInCreated;
		void Fire_GridAddInCreated(Grid^ sender, Object^ pAddIndisp, String^ bstrAddInID, String^ bstrAddInXml)
		{
			OnGridAddInCreated(sender, pAddIndisp, bstrAddInID, bstrAddInXml);
		}

		delegate void GridAddInsCreated(Grid^ sender);
		event GridAddInsCreated^ OnGridAddInsCreated;
		void Fire_GridAddInsCreated(Grid^ sender)
		{
			OnGridAddInsCreated(sender);
		}

		// A notification has been created for all nodes in the current layout.
		delegate void RootNodeCreated(IntPtr nHandle, String^ strUrl, Grid^ pRootGrid);
		event RootNodeCreated^ OnRootNodeCreated;
		void Fire_RootNodeCreated(IntPtr nHandle, String^ strUrl, Grid^ pRootGrid)
		{
			OnRootNodeCreated(nHandle, strUrl, pRootGrid);
		}

		delegate void OpenComplete(Grid^ sender);
		event OpenComplete^ OnOpenComplete;
		void Fire_OpenComplete(Grid^ sender)
		{
			OnOpenComplete(sender);
		}

		delegate void Destroy(Grid^ sender);
		event Destroy^ OnDestroy;
		void Fire_OnDestroy(Grid^ sender)
		{
			OnDestroy(sender);
		}

		delegate void DocumentComplete(Grid^ sender, Object^ pHtmlDoc, String^ strURL);
		event DocumentComplete^ OnDocumentComplete;
		void Fire_OnDocumentComplete(Grid^ sender, Object^ pHtmlDoc, String^ strURL)
		{
			OnDocumentComplete(sender, pHtmlDoc, strURL);
		}

		delegate void TabChange(Grid^ ActiveGrid, Grid^ OldGrid);
		event TabChange^ OnTabChange;
		void Fire_OnTabChange(Grid^ ActiveGrid, Grid^ OldGrid);

		delegate void MessageHandle(String^ strFrom, String^ strTo, String^ strMsgId, String^ strPayload, String^ strExtra);
		event MessageHandle^ OnIPCMessageReceived;
		void Fire_OnIPCMessageReceived(String^ strFrom, String^ strTo, String^ strMsgId, String^ strPayload, String^ strExtra)
		{
			OnIPCMessageReceived(strFrom, strTo, strMsgId, strPayload, strExtra);
		}

		delegate void CloudMessageHandle(Wormhole^ cloudSession);
		event CloudMessageHandle^ OnCloudMessageReceived;
		void Fire_OnCloudMessageReceived(Wormhole^ cloudSession)
		{
			OnCloudMessageReceived(cloudSession);
		}

		delegate void BindCLRObjToWebPage(Object^ SourceObj, Cosmos::Wormhole^ eventSession, String^ eventName);
		event BindCLRObjToWebPage^ OnBindCLRObjToWebPage;
		void Fire_OnBindCLRObjToWebPage(Object^ SourceObj, Cosmos::Wormhole^ eventSession, String^ eventName)
		{
			OnBindCLRObjToWebPage(SourceObj, eventSession, eventName);
		}

		String^ SendIPCMessage(String^ strTo, String^ strPayload, String^ strExtra, String^ strMsgId)
		{
			BSTR bstrRet;
			HRESULT hr = m_pGrid->SendIPCMessage(STRING2BSTR(strTo), STRING2BSTR(strPayload), STRING2BSTR(strExtra), STRING2BSTR(strMsgId), &bstrRet);
			if (hr == S_OK)
			{
				return BSTR2STRING(bstrRet);
			}
			return "";
		}

		property Wormhole^ Wormhole
		{
			Cosmos::Wormhole^ get();
		}

	public:
		void Init();
		Grid^ Observe(String^ layerName, String^ layerXML);
		Grid^ ObserveChild(int rowNum, int colName, String^ layerName, String^ layerXML);

		property String^ Caption
		{
			String^ get();
			void set(String^ strCaption)
			{
				BSTR bstrCap = STRING2BSTR(strCaption);
				m_pGrid->put_Caption(bstrCap);
				::SysFreeString(bstrCap);
			}
		}

		property String^ Name
		{
			String^ get()
			{
				if (m_pGrid)
				{
					CComBSTR bstrCap("");
					m_pGrid->get_Name(&bstrCap);
					return BSTR2STRING(bstrCap);
				}
				return "";
			}

			void set(String^ newVal)
			{
				if (m_pGrid)
				{
					m_pGrid->put_Name(STRING2BSTR(newVal));
				}
			}
		}

		property String^ URL
		{
			String^ get();
			void set(String^ newVal);
		}

		[BrowsableAttribute(false)]
		property Object^ XObject
		{
			Object^ get()
			{
				if (m_pHostObj != nullptr)
					return m_pHostObj;

				VARIANT var;
				m_pGrid->get_XObject(&var);

				try
				{
					m_pHostObj = Marshal::GetObjectForNativeVariant((System::IntPtr) & var);
				}
				catch (InvalidOleVariantTypeException^ e)
				{
					e->Message;
				}
				catch (...)
				{

				}
				return m_pHostObj;
			}
		}

		property System::Drawing::Color rgbMiddle
		{
			System::Drawing::Color get()
			{
				OLE_COLOR rgb;
				m_pGrid->get_rgbMiddle(&rgb);
				return System::Drawing::ColorTranslator::FromOle(rgb);
			}
			void set(System::Drawing::Color newVal)
			{
				m_pGrid->put_rgbMiddle(System::Drawing::ColorTranslator::ToOle(newVal));
			}
		}

		property System::Drawing::Color rgbLeftTop
		{
			System::Drawing::Color get()
			{
				OLE_COLOR rgb;
				m_pGrid->get_rgbLeftTop(&rgb);
				return System::Drawing::ColorTranslator::FromOle(rgb);
			}
			void set(System::Drawing::Color newVal)
			{
				m_pGrid->put_rgbLeftTop(System::Drawing::ColorTranslator::ToOle(newVal));
			}
		}

		property System::Drawing::Color rgbRightBottom
		{
			System::Drawing::Color get()
			{
				OLE_COLOR rgb;
				m_pGrid->get_rgbRightBottom(&rgb);
				return System::Drawing::ColorTranslator::FromOle(rgb);
			}
			void set(System::Drawing::Color newVal)
			{
				m_pGrid->put_rgbRightBottom(System::Drawing::ColorTranslator::ToOle(newVal));
			}
		}

		[BrowsableAttribute(false)]
		property Object^ Extender
		{
			Object^ get()
			{
				Object^ pRetObject = nullptr;
				IDispatch* pDisp = NULL;
				m_pGrid->get_Extender(&pDisp);
				if (pDisp)
					pRetObject = Marshal::GetObjectForIUnknown((IntPtr)pDisp);
				return pRetObject;
			}
			void set(Object^ obj)
			{
				IDispatch* pDisp = (IDispatch*)Marshal::GetIUnknownForObject(obj).ToPointer();
				m_pGrid->put_Extender(pDisp);
			}
		}

		property Object^ Tag
		{
			Object^ get()
			{
				Object^ pRetObject = nullptr;
				VARIANT var;
				m_pGrid->get_Tag(&var);
				try
				{
					pRetObject = Marshal::GetObjectForNativeVariant((System::IntPtr) & var);
				}
				catch (InvalidOleVariantTypeException^)
				{

				}
				catch (NotSupportedException^)
				{

				}
				return pRetObject;
			}

			void set(Object^ obj)
			{
				try
				{
					VARIANT var;
					Marshal::GetNativeVariantForObject(obj, (System::IntPtr) & var);
					m_pGrid->put_Tag(var);
				}
				catch (ArgumentException^ e)
				{
					e->Data->ToString();
				}
			}
		}

		[BrowsableAttribute(false)]
		property int ActivePage
		{
			int get()
			{
				int nPage = 0;
				m_pGrid->get_ActivePage(&nPage);

				return nPage;
			}

			void set(int nPage)
			{
				m_pGrid->put_ActivePage(nPage);
			}
		}

		[BrowsableAttribute(false)]
		property Quasar^ Quasar
		{
			Cosmos::Quasar^ get();
		}

		[BrowsableAttribute(false)]
		property Cosmos::Quasar^ HostQuasar
		{
			Cosmos::Quasar^ get()
			{
				CComPtr<IQuasar> pHubbleFrame;
				m_pGrid->get_HostQuasar(&pHubbleFrame);
				if (pHubbleFrame)
				{
					Cosmos::Quasar^ pQuasar = theAppProxy._createObject<IQuasar, Cosmos::Quasar>(pHubbleFrame);
					return pQuasar;
				}
				return nullptr;
			}
		}

		[BrowsableAttribute(false)]
		property Object^ PlugIn[String^]
		{
			Object ^ get(String ^ strName);
		}

			[BrowsableAttribute(false)]
		property Grid^ RootGrid
		{
			Grid^ get()
			{
				CComPtr<IGrid> pRootGrid;
				m_pGrid->get_RootGrid(&pRootGrid);

				return theAppProxy._createObject<IGrid, Grid>(pRootGrid);
			}
		}

		[BrowsableAttribute(false)]
		property Grid^ ParentGrid
		{
			Grid^ get()
			{
				CComPtr<IGrid> pGrid;
				m_pGrid->get_ParentGrid(&pGrid);

				return theAppProxy._createObject<IGrid, Grid>(pGrid);
			}
		}

		[BrowsableAttribute(false)]
		property Grid^ HostGrid
		{
			Grid^ get()
			{
				IGrid* pGrid = nullptr;
				m_pGrid->get_HostGrid(&pGrid);
				if (pGrid == NULL)
					return nullptr;
				return theAppProxy._createObject<IGrid, Grid>(pGrid);
			}
		}

		[BrowsableAttribute(false)]
		property int Col
		{
			int get()
			{
				long nValue = 0;
				m_pGrid->get_Col(&nValue);
				return nValue;
			}
		}

		[BrowsableAttribute(false)]
		property int Row
		{
			int get()
			{
				long nValue = 0;
				m_pGrid->get_Row(&nValue);
				return nValue;
			}
		}

		property int Hmin
		{
			int get()
			{
				int nValue = 0;
				m_pGrid->get_Hmin(&nValue);
				return nValue;
			}
			void set(int newVal)
			{
				m_pGrid->put_Hmin(newVal);
			}
		}

		property int Hmax
		{
			int get()
			{
				int nValue = 0;
				m_pGrid->get_Hmax(&nValue);
				return nValue;
			}
			void set(int newVal)
			{
				m_pGrid->put_Hmax(newVal);
			}
		}

		property int Vmin
		{
			int get()
			{
				int nValue = 0;
				m_pGrid->get_Vmin(&nValue);
				return nValue;
			}
			void set(int newVal)
			{
				m_pGrid->put_Vmin(newVal);
			}
		}

		property int Vmax
		{
			int get()
			{
				int nValue = 0;
				m_pGrid->get_Vmax(&nValue);
				return nValue;
			}
			void set(int newVal)
			{
				m_pGrid->put_Vmax(newVal);
			}
		}

		property int Rows
		{
			int get()
			{
				long nValue = 0;
				m_pGrid->get_Rows(&nValue);
				return nValue;
			}
		}

		property int Cols
		{
			int get()
			{
				long nValue = 0;
				m_pGrid->get_Cols(&nValue);
				return nValue;
			}
		}

		property ::GridType GridType
		{
			::GridType get()
			{
				::GridType type;
				m_pGrid->get_GridType(&type);
				return (::GridType)type;
			}
		}

		property __int64 Handle
		{
			__int64 get()
			{
				if (m_hWnd)
					return (__int64)m_hWnd;
				__int64 h = 0;
				m_pGrid->get_Handle(&h);
				m_hWnd = (HWND)h;
				return h;
			}
		}

		[BrowsableAttribute(false)]
		property String^ Attribute[String^]
		{
			String ^ get(String ^ strKey)
			{
				BSTR bstrVal;
				m_pGrid->get_Attribute(STRING2BSTR(strKey),&bstrVal);

				return BSTR2STRING(bstrVal);
			}

			void set(String ^ strKey, String ^ strVal)
			{
				m_pGrid->put_Attribute(
					STRING2BSTR(strKey),
					STRING2BSTR(strVal));
			}
		}

	public:
		Grid^ GetGrid(int nRow, int nCol)
		{
			IGrid* pGrid;
			m_pGrid->GetGrid(nRow, nCol, &pGrid);
			return theAppProxy._createObject<IGrid, Grid>(pGrid);
		}

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>

		HWND m_hWnd;
		Dictionary<String^, MethodInfo^>^ m_pHubbleCLRMethodDic = nullptr;
		Dictionary<String^, Object^>^ m_pPlugInDic = nullptr;
	};

	public ref class Quasar : public Dictionary<String^, Grid^>
	{
	public:
		Quasar(IQuasar* pQuasar)
		{
			m_pQuasar = pQuasar;
		}
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Quasar()
		{
			LONGLONG nValue = (LONGLONG)m_pQuasar;
			theAppProxy._removeObject(nValue);
		}

		IQuasar* m_pQuasar;

	public:
		Grid^ Observe(String^ layerName, String^ layerXML);

		void SendMessage(String^ strFrom, String^ strTo, String^ strMsgId, String^ strMsgContent, String^ strExtra);

		void Attach(bool bAttach)
		{
			if (m_pQuasar)
			{
				if (bAttach)
				{
					m_pQuasar->Attach();
				}
				else
				{
					m_pQuasar->Detach();
				}
			}
		}

		property IntPtr Handle
		{
			IntPtr get()
			{
				__int64 nHandle;
				m_pQuasar->get_HWND(&nHandle);
				return (IntPtr)nHandle;
			}
		}

		property Grid^ VisibleGrid
		{
			Cosmos::Grid^ get()
			{
				IGrid* pGrid = nullptr;
				m_pQuasar->get_VisibleGrid(&pGrid);
				return theAppProxy._createObject<IGrid, Cosmos::Grid>(pGrid);
			}
		}

		//property Grid^ Grid[Object^]
		//{
		//	Cosmos::Grid ^ get(Object ^ index)
		//	{
		//		VARIANT var;
		//		Marshal::GetNativeVariantForObject(index,(System::IntPtr) & var);
		//		IGrid* pGrid = nullptr;
		//		m_pQuasar->get_Grid(var, &pGrid);
		//		return theAppProxy._createObject<IGrid, Cosmos::Grid>(pGrid);
		//	}
		//}

		//property GalaxyCluster^ GalaxyCluster
		//{
		//	Cosmos::GalaxyCluster^ get()
		//	{
		//		CComPtr<IGalaxyCluster> pGalaxyCluster = NULL;
		//		m_pQuasar->get_GalaxyCluster(&pGalaxyCluster);

		//		return theAppProxy._createObject<IGalaxyCluster, Cosmos::GalaxyCluster>(pGalaxyCluster);
		//	}
		//}
	};

	public ref class Hubble
	{
	public:
		Hubble(IHubble* pHubble);
		~Hubble();
	private:
		Hubble();
		static bool IsAppInit = false;
		static bool IsChromeRunning = false;

		static Hubble^ m_pHubble;
		static Dictionary<String^, MethodInfo^>^ m_pHubbleCLRMethodDic = gcnew Dictionary<String^, MethodInfo^>();
		static Dictionary<String^, Type^>^ m_pHubbleCLRTypeDic = gcnew Dictionary<String^, Type^>();
		static Dictionary<Object^, Wormhole^>^ m_pCloudEventDic = gcnew Dictionary<Object^, Wormhole^>();

		static Hubble^ InitHubbleApp(bool bSupportCrashReporting, CosmosAppType AppType);
		static bool WebRuntimeInit();
	public:
		static int HubbleInit(String^ strInit);
		static System::Drawing::Icon^ m_pDefaultIcon = nullptr;
		static Form^ m_pMainForm = nullptr;
		static Dictionary<String^, Object^>^ m_pHubbleCLRObjDic = gcnew Dictionary<String^, Object^>();
		static Dictionary<Object^, Grid^>^ m_pFrameworkElementDic = gcnew Dictionary<Object^, Grid^>();

		static Object^ CreateObject(String^ ObjID);
		static Form^ CreateForm(IWin32Window^ parent, String^ ObjID);
		static Type^ GetType(String^ ObjID);
		static Object^ ActiveMethod(String^ strObjID, String^ strMethod, cli::array<Object^, 1>^ p);
		static Object^ ActiveObjectMethod(Object^ pObj, String^ strMethod, cli::array<Object^, 1>^ p);
		static Browser^ ActiveBrowser();
		static Browser^ GetHostBrowser(Object^ obj);
		static Grid^ GetGridFromHandle(IntPtr handle);
		static Grid^ GetNodeFromControl(Control^ ctrl);
		static Grid^ Observe(Control^ ctrl, String^ key, String^ strGridXml);
		static void UpdateNewTabPageLayout(String^ newTabPageLayout);
		static void BindObjToWebPage(IntPtr hWebPage, Object^ pObj, String^ name);

		static Hubble^ GetHubble();

		static void Run();
		static void Run(Form^ Mainform);
		static void Run(ApplicationContext^ context);
		static Grid^ GetGridFromObj(Object^ obj)
		{
			Grid^ pGrid = nullptr;
			if (m_pFrameworkElementDic->TryGetValue(obj, pGrid))
			{
				return pGrid;
			}
			return nullptr;
		}


		static String^ GetObjAssemblyName(Object^ obj)
		{
			if (obj == nullptr)
				return L"";
			Assembly^ a = nullptr;
			String^ strName = L"";
			try
			{
				a = Assembly::GetAssembly(obj->GetType());
			}
			catch (System::ArgumentNullException^)
			{

			}
			finally
			{
				if (a != nullptr)
				{
					strName = a->FullName;
					strName = strName->Substring(0, strName->IndexOf(","));
				}
			}
			return strName;
		}

		static String^ LoadCLRResource(Object^ destObj, String^ resourceName)
		{
			Assembly^ a = nullptr;
			if (resourceName == L"" || resourceName == nullptr)
				resourceName = L"tangramresource.xml";
			String^ strName = L"";
			try
			{
				if (destObj == nullptr)
					a = Assembly::GetExecutingAssembly();
				else
				{
					if (destObj->GetType()->IsSubclassOf(Assembly::typeid))
						a = (Assembly^)destObj;
					else
						a = Assembly::GetAssembly(destObj->GetType());
				}
			}
			catch (ArgumentNullException^)
			{

			}
			finally
			{
				if (a != nullptr)
				{
					strName = a->FullName;
					strName = strName->Substring(0, strName->IndexOf(","));
				}
			}

			System::IO::Stream^ sm = nullptr;
			try
			{
				sm = a->GetManifestResourceStream(strName + "." + resourceName);
			}
			catch (...)
			{

			}
			finally
			{
				if (sm != nullptr)
				{
					cli::array<byte, 1>^ bs = gcnew cli::array<byte, 1>((int)sm->Length);
					sm->Read(bs, 0, (int)sm->Length);
					sm->Close();

					System::Text::UTF8Encoding^ con = gcnew System::Text::UTF8Encoding();

					strName = con->GetString(bs);
					delete sm;
				}
			}
			return strName;
		}

		delegate void HubbleActionDelegate(Grid^ SourceObj, String^ strInfo);
		static event HubbleActionDelegate^ OnHubbleActionDelegate;
		static void Fire_OnHubbleActionDelegate(Grid^ SourceObj, String^ strInfo)
		{
			OnHubbleActionDelegate(SourceObj, strInfo);
		}

		static property Dictionary<Object^, Cosmos::Wormhole^>^ WebBindEventDic
		{
			Dictionary<Object^, Cosmos::Wormhole^>^ get()
			{
				return m_pCloudEventDic;
			};
		}

		static property Form^ MainForm
		{
			Form^ get();
			void set(Form^ frm);
		}

		static property Browser^ HostWebBrowser
		{
			Browser^ get();
		}

		static property System::Drawing::Icon^ DefaultIcon
		{
			System::Drawing::Icon^ get();
		}

		static property bool SupportCrashReporting
		{
			bool get();
			void set(bool bSupportCrashReporting);
		}

		static property CosmosAppType AppType
		{
			CosmosAppType get();
			void set(CosmosAppType nType);
		}

		static property Grid^ CreatingGrid
		{
			Grid^ get();
		}

		static Browser^ CreateBrowser(IntPtr ParentHandle, String^ strUrls);

		static property String^ AppDataPath
		{
			String^ get();
		}

		static property String^ NTPXml
		{
			String^ get();
			void set(String^ strXml);
		}

		delegate void Close();
		static event Close^ OnClose;
		static void Fire_OnClose();

		delegate void CloudAppIdle();
		static event CloudAppIdle^ OnCloudAppIdle;
		static void Fire_OnCloudAppIdle();

		delegate bool TangramAppInit();
		static TangramAppInit^ pOnAppInit;
		// Ref https://docs.microsoft.com/en-us/previous-versions/58cwt3zh%28v%3dvs.140%29#custom-accessor-methods
		static event TangramAppInit^ OnAppInit
		{
			void add(TangramAppInit^ p)
			{
				pOnAppInit = static_cast<TangramAppInit^>(Delegate::Combine(pOnAppInit, p));
			}
			void remove(TangramAppInit^ p)
			{
				pOnAppInit = static_cast<TangramAppInit^>(Delegate::Remove(pOnAppInit, p));
			}
			bool raise()
			{
				if (pOnAppInit != nullptr)
				{
					return pOnAppInit->Invoke();
				}
				return true;
			}
		};
		static bool Fire_OnAppInit();

		delegate System::Drawing::Icon^ GetAppIcon(int nIndex);
		static event GetAppIcon^ OnGetAppIcon;
		static System::Drawing::Icon^ Fire_OnGetAppIcon(int nIndex)
		{
			return OnGetAppIcon(nIndex);
		}

		delegate Object^ GetSubObjForWebPage(Object^ SourceObj, String^ subObjName);
		static event GetSubObjForWebPage^ OnGetSubObjForWebPage;
		static Object^ Fire_OnGetSubObjForWebPage(Object^ SourceObj, String^ subObjName);

		delegate void BindCLRObjToWebPage(Object^ SourceObj, Cosmos::Wormhole^ eventSession, String^ eventName);
		static event BindCLRObjToWebPage^ OnBindCLRObjToWebPage;
		static void Fire_OnBindCLRObjToWebPage(Object^ SourceObj, Cosmos::Wormhole^ eventSession, String^ eventName);

		delegate void OpenComplete(IntPtr hWnd, String^ bstrUrl, Grid^ pRootGrid);
		static event OpenComplete^ OnOpenComplete;
		static void Fire_OnOpenComplete(IntPtr hWnd, String^ bstrUrl, Grid^ pRootGrid)
		{
			OnOpenComplete(hWnd, bstrUrl, pRootGrid);
		}

		delegate void AppMsgLoop(IntPtr hWnd, IntPtr msg, IntPtr wParam, IntPtr lParam);
		static event AppMsgLoop^ OnAppMsgLoop;
		static void Fire_OnAppMsgLoop(IntPtr hWnd, IntPtr msg, IntPtr wParam, IntPtr lParam)
		{
			OnAppMsgLoop(hWnd, msg, wParam, lParam);
		}

		delegate void HubbleMsg(IntPtr hWnd, String^ strType, String^ strParam1, String^ strParam2);
		static event HubbleMsg^ OnHubbleMsg;
		static void Fire_OnHubbleMsg(IntPtr hWnd, String^ strType, String^ strParam1, String^ strParam2)
		{
			OnHubbleMsg(hWnd, strType, strParam1, strParam2);
		}

		delegate void HubbleMsgReceived(Cosmos::Wormhole^ cloudSession);
		static event HubbleMsgReceived^ OnHubbleMsgReceived;
		static void Fire_OnHubbleMsgReceived(Cosmos::Wormhole^ cloudSession)
		{
			OnHubbleMsgReceived(cloudSession);
		}

		delegate void RenderHTMLElement(IntPtr hWnd, String^ strRuleName, String^ strHTML);
		static event RenderHTMLElement^ OnRenderHTMLElement;
		static void Fire_OnRenderHTMLElement(IntPtr hWnd, String^ strRuleName, String^ strHTML)
		{
			OnRenderHTMLElement(hWnd, strRuleName, strHTML);
		}

		delegate void FormNodeCreated(String^ bstrObjID, Form^ pForm, Grid^ pGrid);
		static event FormNodeCreated^ OnFormNodeCreated;
		static void Fire_OnFormNodeCreated(String^ bstrObjID, Form^ pForm, Grid^ pGrid)
		{
			OnFormNodeCreated(bstrObjID, pForm, pGrid);
		}

		property String^ AppKeyValue[String^]
		{
			String ^ get(String ^ iIndex);
			void set(String^ iIndex, String^ newVal);
		}
	};
}
