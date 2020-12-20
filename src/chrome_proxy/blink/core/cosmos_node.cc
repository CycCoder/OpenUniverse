﻿#include "cosmos.h"
#include "cosmos_node.h"
#include "cosmos_event.h"
#include "cosmos_winform.h"
#include "cosmos_galaxy.h"
#include "cosmos_control.h"
#include "cosmos_compositor.h"

#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/dom_token_list.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/dom/document_fragment.h"
#include "third_party/blink/renderer/core/dom/node.h"
#include "third_party/blink/renderer/core/dom/node_list.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/core/html/html_head_element.h"
#include "third_party/blink/renderer/core/dom/class_collection.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_application_callback.h"
#include "third_party/blink/renderer/core/xml/dom_parser.h"
#include "third_party/blink/renderer/bindings/core/v8/string_or_trusted_html.h"

namespace blink {

	CosmosNode::CosmosNode() :CosmosXobj() {
		rootNode_ = nullptr;
		m_pParentForm = nullptr;
	}

	CosmosNode::~CosmosNode() {
		Cosmos* pCosmos = cosmos_.Get();
		if (pCosmos)
		{
			auto it = pCosmos->m_mapCosmosNodeforEvent.find(handle_);
			if (it != pCosmos->m_mapCosmosNodeforEvent.end())
				pCosmos->m_mapCosmosNodeforEvent.erase(it);

			auto it2 = pCosmos->m_mapCosmosNode.find(handle_);
			if (it2 != pCosmos->m_mapCosmosNode.end())
				pCosmos->m_mapCosmosNode.erase(it2);
		}
		mapCosmosEventCallback_.clear();
	}

	CosmosNode* CosmosNode::Create(const String& strNodeName) {
		return MakeGarbageCollected<CosmosNode>(strNodeName);
	}

	CosmosNode::CosmosNode(const String& strNodeName)
	{
		name_ = strNodeName;
	}

	String CosmosNode::getid()
	{
		return id_;
	}

	long CosmosNode::row()
	{
		return getLong(L"row");
	}

	long CosmosNode::col()
	{
		return getLong(L"col");
	}

	long CosmosNode::rows()
	{
		return getLong(L"rows");
	}

	long CosmosNode::cols()
	{
		return getLong(L"cols");
	}

	CosmosWinform* CosmosNode::parentForm()
	{
		return m_pParentForm;
	}

	CosmosNode* CosmosNode::getChild(long nIndex)
	{
		return getChild(0, nIndex);
	}

	CosmosNode* CosmosNode::getChild(long row, long col)
	{
		int nIndex = col + row * getLong(L"rows");
		std::wstring strIndex = std::to_wstring(nIndex);
		String strname = strIndex.c_str();
		int64_t nHandle = getInt64(strname);
		if (nHandle)
		{
			auto it = cosmos_->m_mapCosmosNode.find(nHandle);
			if (it != cosmos_->m_mapCosmosNode.end())
			{
				return it->value.Get();
			}
		}
		return nullptr;
	}

	CosmosNode* CosmosNode::getChild(const String& strName)
	{
		WebString str = strName;
		auto it = m_mapChildNode2.find(str.Utf16());
		if (it != m_mapChildNode2.end())
			return it->second;
		return nullptr;
	}

	CosmosNode* CosmosNode::getGrid(const String& strName)
	{
		CosmosNode* root_ = root();
		WebString str = strName;
		auto it = root_->m_mapGrid.find(str.Utf16());
		if (it != root_->m_mapGrid.end())
			return it->second;
		return nullptr;
	}

	void CosmosNode::Trace(blink::Visitor* visitor) {
		EventTargetWithInlineData::Trace(visitor);
		ScriptWrappable::Trace(visitor);
		CosmosXobj::Trace(visitor);
		visitor->Trace(cosmos_);
		visitor->Trace(m_pParentForm);
		visitor->Trace(rootNode_);
		visitor->Trace(mapCosmosEventCallback_);
	}

	void CosmosNode::ShowWebContent(const String& strParentDivName, const String& strDivName)
	{
		String contentname = strDivName;
		String contentparent = strParentDivName;
		Element* visibleElem2 = getVisibleElement(contentname);
		ExceptionState exception_state(nullptr,
			ExceptionState::kExecutionContext,
			"MdiChildActive",
			"");
		if (visibleElem2 == nullptr)
		{
			ClassCollection* contentCollection = ((Document*)GetExecutionContext())->getElementsByClassName(WebString(contentparent));
			if (contentCollection)
			{
				HTMLCollection* contentsElements = contentCollection->item(0)->Children();
				if (contentsElements)
				{
					String strname = contentname;
					WebString _strName = strname;
					for (Element* contentElement : *contentsElements)
					{
						if (contentElement->classList().contains(_strName))
						{
							contentElement->classList().remove({ "hidden" }, exception_state);
							contentElement->classList().add({ "show" }, exception_state);
							setVisibleElement(contentname, contentElement);
							if (m_pVisibleContentElement == nullptr)
								m_pVisibleContentElement = contentElement;
							else
							{
								m_pVisibleContentElement->classList().remove({ "show" }, exception_state);
								m_pVisibleContentElement->classList().add({ "hidden" }, exception_state);
								m_pVisibleContentElement = contentElement;
								break;
							}
						}
						else
						{
							contentElement->classList().remove({ "show" }, exception_state);
							contentElement->classList().add({ "hidden" }, exception_state);
						}

					}
				}
			}
		}
		else
		{
			if (m_pVisibleContentElement != visibleElem2)
			{
				visibleElem2->classList().remove({ "hidden" }, exception_state);
				visibleElem2->classList().add({ "show" }, exception_state);
				m_pVisibleContentElement->classList().remove({ "show" }, exception_state);
				m_pVisibleContentElement->classList().add({ "hidden" }, exception_state);
				m_pVisibleContentElement = visibleElem2;
			}
		}
		//var visibleElem = null;
		//var visibleElem2 = sender.xobj.getVisibleElement(contentname);
		//if (visibleElem2 == null) {
		//    var contentElements = document.getElementsByClassName(contentparent)[0].children;
		//    for (var i = 0; i < contentElements.length; i++) {
		//        var classList = contentElements[i].classList;
		//        var bContains = classList.contains(contentname);
		//        if (bContains) {
		//            classList.remove("hidden");
		//            classList.add("show");
		//            sender.xobj.setVisibleElement(contentname, contentElements[i]);
		//            if (visibleElem == null)
		//                visibleElem = contentElements[i];
		//            else {
		//                visibleElem.classList.remove("show");
		//                visibleElem.classList.add("hidden");
		//                visibleElem = contentElements[i];
		//                break;
		//            }
		//        } else {
		//            classList.remove("show");
		//            classList.add("hidden");
		//        }
		//    }
		//} else {
		//    if (visibleElem != visibleElem2) {
		//        visibleElem.classList.remove("show");
		//        visibleElem.classList.add("hidden");
		//        visibleElem2.classList.remove("hidden");
		//        visibleElem2.classList.add("show");
		//        visibleElem = visibleElem2;
		//    }
		//}
	}

	void CosmosNode::Observe(const String& strKey, const String& xml, V8ApplicationCallback* callback)
	{
		if (m_pRenderframeImpl)
		{
			setStr(L"senderid", id_);
			String callbackid_ = WTF::CreateCanonicalUUIDString();
			setStr(L"msgID", L"OPEN_XML");
			setStr(L"open_callbackid", callbackid_);
			setStr(L"openkey", strKey);
			setStr(L"openxml", xml);
			WebString strID = callbackid_;
			m_pRenderframeImpl->m_mapCosmosSession[strID.Utf16()] = this;
			if (callback)
			{
				mapCosmosEventCallback_.insert(callbackid_, callback);
			}
			m_pRenderframeImpl->SendCosmosMessageEx(session_);
		}
	}

	void CosmosNode::ObserveEx(const String& strKey, const String& xml, long row, long col, V8ApplicationCallback* callback)
	{
		if (m_pRenderframeImpl)
		{
			setStr(L"senderid", id_);
			setStr(L"msgID", L"OPEN_XML_SPLITTER");
			setStr(L"openkey", strKey);
			setStr(L"openxml", xml);
			setLong(L"opencol", col);
			setLong(L"openrow", row);
			String callbackid_ = WTF::CreateCanonicalUUIDString();
			setStr(L"opencallbackid", callbackid_);
			WebString strID = callbackid_;
			m_pRenderframeImpl->m_mapCosmosSession[strID.Utf16()] = this;
			if (callback)
			{
				mapCosmosEventCallback_.insert(callbackid_, callback);
			}
			m_pRenderframeImpl->SendCosmosMessageEx(session_);
		}
	}

	void CosmosNode::ObserveCtrl(const String& strCtrlName, const String& strKey, const String& xml, V8ApplicationCallback* callback)
	{
		if (m_pRenderframeImpl)
		{
			setStr(L"senderid", id_);
			setStr(L"msgID", L"OPEN_XML_CTRL");
			setStr(L"ctrlName", strCtrlName);
			setStr(L"openkey", strKey);
			setStr(L"openxml", xml);
			String callbackid_ = WTF::CreateCanonicalUUIDString();
			setStr(L"opencallbackid", callbackid_);
			WebString strID = callbackid_;
			m_pRenderframeImpl->m_mapCosmosSession[strID.Utf16()] = this;
			if (callback)
			{
				mapCosmosEventCallback_.insert(callbackid_, callback);
			}
			m_pRenderframeImpl->SendCosmosMessageEx(session_);
		}
	}

	void CosmosNode::sendMessageToGrid(CosmosXobj* msg)
	{
		if (msg)
		{
			Cosmos* pCosmos = cosmos_.Get();
			__int64 nHandle = msg->getInt64(L"gridhandle");
			CosmosNode* grid = nullptr;
			if (nHandle)
			{
				auto it = pCosmos->m_mapCosmosNode.find(nHandle);
				if (it != pCosmos->m_mapCosmosNode.end())
				{
					grid = it->value.Get();
					grid->DispatchEvent(*blink::CosmosEvent::Create(blink::event_type_names::kCloudmessageforgrid, msg));
				}
			}
		}
	}

	void CosmosNode::sendMessageToGrid(CosmosNode* grid)
	{
		grid->DispatchEvent(*blink::CosmosEvent::Create(blink::event_type_names::kCloudmessageforgrid, grid));
	}

	void CosmosNode::setControlVal(const String& CtrlID, int64_t CtrlHandle, const String& strVal)
	{
		int64_t nHandle = 0;
		if (CtrlID != "")
		{
			Element* const tangramelement = cosmos_->DomWindow()->document()->getElementById(WTF::AtomicString(CtrlID));
			if (tangramelement)
			{
				WTF::AtomicString handle = tangramelement->getAttribute("hwnd");
				if (handle != "")
				{
					WebString webstr = handle;
					std::wstring u16_handle = webstr.Utf16();
					nHandle = _wtoi(u16_handle.c_str());
				}
			}
		}
		else
			nHandle = handle_;
		if (nHandle)
		{
			if (m_pRenderframeImpl) {
				WebString webstr = strVal;
				std::wstring _val = webstr.Utf16();
				webstr = CtrlID;
				std::wstring _strCtrlID = webstr.Utf16();
				m_pRenderframeImpl->SendCosmosMessage(L"TANGRAM_CTRL_VALUE_MESSAGE", _strCtrlID, nHandle, 0, _val, L"");
			}
		}
	}

	int64_t CosmosNode::hwnd()
	{
		return handle_;
	}

	String CosmosNode::objtype() {
		return getStr(L"objtype");
	}

	void CosmosNode::sendMessage(const String& id, const String& param1, const String& param2, const String& param3, const String& param4, const String& param5) {
		if (m_pRenderframeImpl) {
			WebString webstr = id;
			std::wstring u16_id = webstr.Utf16();
			webstr = param1;
			std::wstring u16_param1 = webstr.Utf16();
			webstr = param2;
			std::wstring u16_param2 = webstr.Utf16();
			webstr = param3;
			std::wstring u16_param3 = webstr.Utf16();
			webstr = param4;
			std::wstring u16_param4 = webstr.Utf16();
			webstr = param5;
			std::wstring u16_param5 = webstr.Utf16();
			m_pRenderframeImpl->SendCosmosMessage(u16_id, u16_param1, u16_param2, u16_param3, u16_param4, u16_param5);
		}
	}

	void CosmosNode::GetControlsNames(V8ApplicationCallback* callback)
	{

	}

	int64_t CosmosNode::handle() {
		return handle_;
	}

	CosmosNode* CosmosNode::parentGrid()
	{
		__int64 nHandle = getInt64(L"parentgridhandle");
		if (nHandle)
		{
			auto it = cosmos_->m_mapCosmosNode.find(nHandle);
			if (it != cosmos_->m_mapCosmosNode.end())
			{
				return it->value.Get();
			}
		}
		return nullptr;
	}

	CosmosNode* CosmosNode::root()
	{
		return rootNode_;
	}

	CosmosGalaxy* CosmosNode::parentGalaxy() {
		__int64 nHandle = getInt64(L"Galaxyhandle");
		auto it = cosmos_->m_mapCosmosGalaxy.find(nHandle);
		if (it != cosmos_->m_mapCosmosGalaxy.end())
			return it->value.Get();
		return nullptr;
	}

	CosmosNode* CosmosNode::AddChild(int64_t nHandle, const String& strNodeName, blink::Cosmos* pCosmos)
	{
		CosmosNode* node = nullptr;
		auto it = pCosmos->m_mapCosmosNode.find(nHandle);
		if (it != pCosmos->m_mapCosmosNode.end())
		{
			node = it->value;
			int nSize = m_mapChildNode.size();
			m_mapChildNode[nSize] = node;
		}
		return node;
	}

}  // namespace blink