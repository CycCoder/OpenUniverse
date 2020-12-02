﻿#include "hubble.h"
#include "hubble_xobj.h"
#include "hubble_node.h"
#include "hubble_event.h"
#include "hubble_galaxy.h"
#include "hubble_winform.h"
#include "hubble_control.h"
#include "hubble_compositor.h"
#include <stdlib.h>
#include <string>
#include <iostream>

#include "base/guid.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/dom_token_list.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/dom/node.h"
#include "third_party/blink/renderer/core/dom/node_list.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/core/html/html_head_element.h"
#include "third_party/blink/renderer/core/dom/class_collection.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/platform/bindings/callback_function_base.h"

#include "third_party/blink/renderer/bindings/core/v8/v8_hubble_callback.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_application_callback.h"
#include "third_party/blink/renderer/core/xml/dom_parser.h"
#include "third_party/blink/renderer/bindings/core/v8/string_or_trusted_html.h"

#include "../../third_party/ChromeRenderDomProxy.h"

namespace blink {

	Hubble::Hubble(LocalFrame* frame) : DOMWindowClient(frame) {
		url_ = L"";
		is_pending_ = false;
		innerXobj_ = newVar(L"");
		DOMParser_ = nullptr;
		m_pRenderframeImpl = nullptr;
		m_pVisibleContentElement = nullptr;
	}

	Hubble::~Hubble() {
	}

	void Hubble::Trace(blink::Visitor* visitor) {
		EventTargetWithInlineData::Trace(visitor);
		ScriptWrappable::Trace(visitor);
		visitor->Trace(DOMParser_);
		DOMWindowClient::Trace(visitor);
		visitor->Trace(m_mapHubbleNode);
		visitor->Trace(m_mapHubbleGalaxy);
		visitor->Trace(m_mapWinForm);
		visitor->Trace(m_mapHubbleCompositor);
		visitor->Trace(mapHubbleCallback_);
		visitor->Trace(mapCallbackFunction_);
		visitor->Trace(m_pVisibleContentElement);
		visitor->Trace(innerXobj_);
		visitor->Trace(mapCloudSession_);
	}

	String Hubble::url()
	{
		return DomWindow()->document()->Url().GetString();
	}

	HubbleXobj* Hubble::xobj()
	{
		return innerXobj_;
	}

	void Hubble::setStr(const String& strKey, const String& value)
	{
		WebString str = strKey;
		WebString val = value;
		innerXobj_->session_.m_mapString[str.Utf16()] = val.Utf16();
		auto it = innerXobj_->session_.m_mapint64.find(WebString(strKey).Utf16());
		if (it != innerXobj_->session_.m_mapint64.end())
		{
			setStr(L"msgID", L"MODIFY_CTRL_VALUE");
			setStr(L"currentsubobjformodify", strKey);
			m_pRenderframeImpl->SendHubbleMessageEx(innerXobj_->session_);
		}
	}

	String Hubble::getStr(const String& strKey)
	{
		WebString str = strKey;
		auto it = innerXobj_->session_.m_mapString.find(str.Utf16());
		if (it != innerXobj_->session_.m_mapString.end())
		{
			return String(it->second.c_str());
		}
		return L"";
	}

	void Hubble::setLong(const String& strKey, long value)
	{
		WebString str = strKey;
		innerXobj_->session_.m_mapLong[str.Utf16()] = value;
	}

	long Hubble::getLong(const String& strKey)
	{
		WebString str = strKey;
		auto it = innerXobj_->session_.m_mapLong.find(str.Utf16());
		if (it != innerXobj_->session_.m_mapLong.end())
		{
			return it->second;
		}
		return 0;
	}

	void Hubble::setInt64(const String& strKey, int64_t value)
	{
		WebString str = strKey;
		auto it = innerXobj_->session_.m_mapint64.find(str.Utf16());
		if (it != innerXobj_->session_.m_mapint64.end())
		{
			innerXobj_->session_.m_mapint64.erase(it);
		}
		innerXobj_->session_.m_mapint64[str.Utf16()] = value;
	}

	int64_t Hubble::getInt64(const String& strKey)
	{
		WebString str = strKey;
		auto it = innerXobj_->session_.m_mapint64.find(str.Utf16());
		if (it != innerXobj_->session_.m_mapint64.end())
		{
			return it->second;
		}
		return 0;
	}

	void Hubble::setFloat(const String& strKey, float value)
	{
		WebString str = strKey;
		innerXobj_->session_.m_mapFloat[str.Utf16()] = value;
	}

	float Hubble::getFloat(const String& strKey)
	{
		WebString str = strKey;
		auto it = innerXobj_->session_.m_mapFloat.find(str.Utf16());
		if (it != innerXobj_->session_.m_mapFloat.end())
			return it->second;
		return 0;
	}

	void Hubble::wait(bool bwait)
	{
		if (bwait)
			run_loop_.Run();
		else
			run_loop_.Quit();
	}

	void Hubble::AddedEventListener(const AtomicString& event_type,
		RegisteredEventListener& registered_listener) {
		EventTargetWithInlineData::AddedEventListener(event_type,
			registered_listener);
	}

	void Hubble::sendMessage(const String& id, const String& param1, const String& param2, const String& param3, const String& param4, const String& param5) {
		if (m_pRenderframeImpl == nullptr)
		{
			m_pRenderframeImpl = WebLocalFrameImpl::FromFrame(GetFrame())->Client();
			innerXobj_->m_pRenderframeImpl = m_pRenderframeImpl;
		}
		if (m_pRenderframeImpl) {
			if (is_pending_) {
				String message = id + "%%%" + param1 + "%%%" + param2 + "%%%" + param3 + "%%%" + param4 + "%%%" + param5;
				pending_messages_.push_back(message);
			}
			else {
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
				m_pRenderframeImpl->SendHubbleMessage(u16_id, u16_param1, u16_param2, u16_param3, u16_param4, u16_param5);
			}
		}
	}

	// Define a element
	void Hubble::defineElement(const String& tagName, const String& html) {
		sendMessage(L"DEFINE_ELEMENT", tagName, html, L"", L"", L"");
	}

	// Render an element.
	void Hubble::renderElement(const String& tagName, const String& html) {
		sendMessage(L"RENDER_ELEMENT", tagName, html, L"", L"", L"");
	}

	void Hubble::waitMessage() {
		is_pending_ = true;
	}

	void Hubble::releaseMessage() {
		is_pending_ = false;
		if (m_pRenderframeImpl) {
			if (pending_messages_.size() > 0) {
				String stringBuffer = "";
				for (wtf_size_t i = 0; i < pending_messages_.size(); i++) {
					if (i > 0) {
						stringBuffer = stringBuffer + "$$$";
					}
					stringBuffer = stringBuffer + pending_messages_[i];
				}
				WebString bundle = stringBuffer;
				std::wstring type = L"AGGREGATED_MESSAGE";
				std::wstring param1 = bundle.Utf16();
				m_pRenderframeImpl->SendHubbleMessage(type, param1, L"", L"", L"", L"");
				pending_messages_.clear();
			}
		}
	}

	HubbleXobj* Hubble::newVar(const String& strName)
	{
		HubbleXobj* var = HubbleXobj::Create(strName);
		var->m_pRenderframeImpl = m_pRenderframeImpl;
		var->id_ = WTF::CreateCanonicalUUIDString();
		var->session_.m_mapString[L"sessionid"] = WebString(var->id_).Utf16();
		var->hubble_ = this;
		return var;
	}

	HubbleWinform* Hubble::createWinForm(const String& strFormXml, const long FormType, V8ApplicationCallback* callback)
	{
		HubbleWinform* form = HubbleWinform::Create(DomWindow()->GetFrame(), strFormXml);
		form->hubble_ = this;
		form->innerXobj_ = newVar(L"");
		form->handle_ = (int64_t)form;
		if (callback)
			mapCallbackFunction_.insert(form->handle_, callback);
		form->m_pRenderframeImpl = m_pRenderframeImpl;
		m_mapWinForm.insert(form->handle_, form);
		if (m_pRenderframeImpl) {
			form->innerXobj_->setStr(L"msgID", L"CREATE_WINFORM");
			form->innerXobj_->setStr(L"objID", L"WinForm");
			form->innerXobj_->setInt64(L"form", (int64_t)form);
			form->innerXobj_->setInt64(L"innerXobj", (int64_t)form->innerXobj_.Get());
			form->innerXobj_->setStr(L"formXml", strFormXml);
			form->innerXobj_->setLong(L"formType", FormType);
			m_pRenderframeImpl->SendHubbleMessageEx(form->innerXobj_->session_);
		}
		return form;
	}

	HubbleWinform* Hubble::newWinForm(int64_t handle, HubbleXobj* obj)
	{
		HubbleWinform* form = HubbleWinform::Create(DomWindow()->GetFrame(), L"");
		form->hubble_ = this;
		form->innerXobj_ = obj;
		form->handle_ = handle;
		form->innerXobj_->setStr(L"objID", L"WinForm");
		form->m_pRenderframeImpl = m_pRenderframeImpl;
		m_mapWinForm.insert(form->handle_, form);
		return form;
	}

	HubbleWinform* Hubble::CreateForm(int64_t handle, HubbleXobj* obj)
	{
		HubbleWinform* form = HubbleWinform::Create(DomWindow()->GetFrame(), obj->getStr(L"tagName"));
		form->hubble_ = this;
		form->innerXobj_ = obj;
		form->handle_ = handle;
		form->innerXobj_->setStr(L"objID", L"WinForm");
		form->m_pRenderframeImpl = m_pRenderframeImpl;
		m_mapWinForm.insert(form->handle_, form);
		long nFormType = obj->getLong(L"WinFormType");
		switch (nFormType)
		{
		case 0:
			DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kLoadwinform, obj));
			break;
		case 1:
			DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kLoadmdiwinform, obj));
			break;
		case 2:
		{
			HubbleWinform* parentform = form->mdiParent();
			if (parentform)
			{
				parentform->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kLoadmdichildwinform, obj));
				//form->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kMdichildactivate, obj));
				//parentform->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kMdichildactivate, obj));
			}
		}
		break;
		default:
			break;
		}
		return form;
	}

	void Hubble::MdiChildActive(HubbleXobj* xobj)
	{
		__int64 handle = xobj->getInt64(L"active_mdichildhandle");
		if (handle)
		{
			HubbleWinform* form = nullptr;
			HubbleWinform* parentmdiform = nullptr;
			auto it = m_mapWinForm.find(handle);
			if (it != m_mapWinForm.end())
			{
				form = it->value.Get();
				parentmdiform = form->mdiParent();
				ExceptionState exception_state(nullptr,
					ExceptionState::kExecutionContext,
					"MdiChildActive",
					"");
				if (form->m_pContentElement == nullptr)
				{
					ClassCollection* contentCollection = DomWindow()->document()->getElementsByClassName(/*parentmdiform->name() + */"applicationcontents");
					if (contentCollection)
					{
						HTMLCollection* contentsElements = contentCollection->item(0)->Children();
						if (contentsElements)
						{
							String strname = form->name();
							WebString _strName = strname;
							for (Element* contentElement : *contentsElements)
							{
								if (contentElement->classList().contains(_strName))
								{
									contentElement->classList().remove({ "hidden" }, exception_state);
									contentElement->classList().add({ "show" }, exception_state);
									form->m_pContentElement = contentElement;
									if(m_pVisibleContentElement==nullptr)
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
					if (m_pVisibleContentElement != form->m_pContentElement)
					{
						form->m_pContentElement->classList().remove({ "hidden" }, exception_state);
						form->m_pContentElement->classList().add({ "show" }, exception_state);
						m_pVisibleContentElement->classList().remove({ "show" }, exception_state);
						m_pVisibleContentElement->classList().add({ "hidden" }, exception_state);
						m_pVisibleContentElement = form->m_pContentElement;
					}
				}
				if (parentmdiform && form)
				{
					form->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kMdichildactivate, xobj));
					parentmdiform->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kMdichildactivate, xobj));
				}
			}
		}
	}

	void Hubble::AllMdiChildRemoved(HubbleXobj* xobj)
	{
		__int64 nHandle = xobj->getInt64(L"formhandle");
		HubbleWinform* form = nullptr;
		auto it = m_mapWinForm.find(nHandle);
		if (it != m_mapWinForm.end())
		{
			form = it->value.Get();
			if (m_pVisibleContentElement)
			{
				ExceptionState exception_state(nullptr,
					ExceptionState::kExecutionContext,
					"AllMdiChildRemoved",
					"");
				m_pVisibleContentElement->classList().remove({ "show" }, exception_state);
				m_pVisibleContentElement->classList().add({ "hidden" }, exception_state);
				m_pVisibleContentElement = nullptr;
				ClassCollection* contentCollection = DomWindow()->document()->getElementsByClassName(/*form->name() + */"applicationcontents");
				if (contentCollection)
				{
					HTMLCollection* contentsElements = contentCollection->item(0)->Children();
					if (contentsElements)
					{
						for (Element* contentElement : *contentsElements)
						{
							if (contentElement->classList().contains(WebString(form->name())))
							{
								contentElement->classList().remove({ "hidden" }, exception_state);
								contentElement->classList().add({ "show" }, exception_state);
								break;
							}
						}
					}
				}
			}
		
			sendMessage("TANGRAM_UI_MESSAGE", "__VIEWPORT_DEFAULT__", L"", L"", L"", L"");
			form->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kAllmdichildremoved, xobj));
		}
	}

	DOMParser* Hubble::xmlParse()
	{
		if (DOMParser_ == nullptr)
		{
			DOMParser_ = DOMParser::Create(*(DomWindow()->document()));
		}
		return DOMParser_.Get();
	}

	void Hubble::OnMessage(Element* e, const String& eventName)
	{
		Element* element = static_cast<Element*>(e->childNodes()->item(1));
		if (!!element) {
			for (unsigned int i = 1; i < element->childNodes()->length(); i++) {
				Element* elem = static_cast<Element*>(element->childNodes()->item(i));
				if (elem)
				{
					Node* pNode = elem;
					if (pNode->getNodeType() == 1) {
						AtomicString target = "";
						if (elem->hasAttribute("target"))
						{
							target = elem->getAttribute("target");
							if (target!="") {
								AtomicString galaxy = elem->getAttribute("galaxy");
								AtomicString cluster = elem->getAttribute("cluster");
								if (galaxy == "")
									galaxy = "default";
								if (cluster == "")
									cluster = "__viewport_default__";
								HubbleNode* gridfortarget = getGrid(galaxy, cluster, target);
								if (!!gridfortarget) {
									gridfortarget->setWorkElement(elem);
									String strMsgID = e->GetIdAttribute() + "_" + eventName;
									gridfortarget->setMsgID(strMsgID);
									gridfortarget->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kCloudmessageforgrid, gridfortarget->xobj()));
								}
							}
						}
					}
				}
			}
		}
		//var element = e.childNodes[1];
		//if (!!element) {
		//    alert(element.childNodes.length);
		//    for (var i = 1; i < element.childNodes.length; i++) {
		//        var elem = element.childNodes[i];
		//        alert(elem.nodeType);
		//        if (elem.nodeType == 1) {
		//           var target = elem.getAttribute("target");
		//           var galaxy = elem.getAttribute("galaxy");
		//           var cluster = elem.getAttribute("cluster");
		//            if (!!target && !!galaxy && !!cluster) {
		//                var gridfortarget = apppage.getGrid(galaxy, cluster, target);
		//                if (!!gridfortarget) {
		//                    gridfortarget.workElement = elem;
		//                    gridfortarget.msgID = e.id + "_OnClick";
		//                    gridfortarget.sendMessageToGrid(gridfortarget);
		//                }
		//            }
		//        }
		//    }
		//}
	}

	void Hubble::DispatchGridEvent(HubbleXobj* xObj, const String& ctrlName, const String& eventName)
	{
		String ctrlName_ = ctrlName;
		if (ctrlName.IsNull() || ctrlName == "")
		{ 
			ctrlName_ = xObj->getStr(L"name@page");
		}
		HubbleNode* grid = xObj->grid();
		if (grid)
		{
			grid->setMsgID(ctrlName_ + "_" + eventName);
			grid->xobj()->setSender(xObj);
			grid->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kCloudmessageforgrid, grid->xobj()));
		}
		else
		{
			xObj->fireEvent(ctrlName + "@" + eventName, xObj);
		}

		String strXml = xObj->getStr(eventName + "Xml");
		if (strXml.IsNull() || strXml == "")
			return;
		if (DOMParser_ == nullptr)
		{
			DOMParser_ = DOMParser::Create(*(DomWindow()->document()));
		}
		if (DOMParser_)
		{
			ExceptionState exception_state(nullptr,
				ExceptionState::kExecutionContext,
				"DOMParser",
				"");
			Document* doc = DOMParser_->parseFromString(blink::StringOrTrustedHTML::FromString(xObj->getStr(eventName + "Xml")), "application/xml", exception_state);
			if (doc)
			{
				String eventName_ = eventName.LowerASCII();
				AtomicString name = AtomicString(eventName_);
				ContainerNode* pContainerNode = (ContainerNode*)doc->firstChild();
				HTMLCollection* list = pContainerNode->getElementsByTagName(name);
				for (unsigned int index = 0; index < list->length(); index++)
				{
					Element* workItem = list->item(index);
					for (unsigned int i = 1; i < workItem->childNodes()->length(); i++)
					{ 
						Element* elem = (Element*)workItem->childNodes()->item(i);
						Node* pNode = elem;
						if (pNode->getNodeType() == 1&& elem->hasAttribute("target")) {
							AtomicString target = elem->getAttribute("target");
							if (target!="") {
								AtomicString galaxy = elem->getAttribute("galaxy");
								if (galaxy == "")
									galaxy = "default";
								AtomicString cluster = elem->getAttribute("cluster");
								if (cluster == "")
								{
									cluster = "__viewport_default__";
								}
								HubbleNode* gridfortarget = getGrid(galaxy, cluster, target);
								if (gridfortarget == nullptr)
								{
									if (xObj->grid())
									{
										HubbleWinform* form = xObj->grid()->parentForm();
										if(form)
											gridfortarget = form->getGrid(galaxy, cluster, target);
									}
								}
								if (!!gridfortarget) {
									gridfortarget->element_ = elem;
									gridfortarget->setMsgID(ctrlName_ + "_" + eventName);
									gridfortarget->xobj()->setSender(xObj);
									gridfortarget->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kCloudmessageforgrid, gridfortarget->xobj()));
								}
							}
						}
					}
				}
			}
		}
		//var domparser = new DOMParser();
		//var doc = domparser.parseFromString(e.getStr("OnAfterSelectXml"), "application/xml");
		//if (!!doc) {
		//    var webcontent = e.getStr("webcontent");
		//    var selectCluster = e.getStr(eventName+"_key");
		//    var list = doc.firstChild.getElementsByTagName(eventName.toLowerCase());
		//    if (list.length) {
		//        for (var index = 0; index < list.length;index++) {
		//            var onafterselect = list[index];
		//            for (var i = 1; i < onafterselect.childNodes.length; i++) {
		//                var elem = onafterselect.childNodes[i];
		//                if (elem.nodeType == 1) {
		//                    var target = elem.getAttribute("target");
		//                    var galaxy = elem.getAttribute("galaxy");
		//                    var cluster = elem.getAttribute("cluster");
		//                    if (!!target&&!!galaxy && !!cluster) {
		//                        var gridfortarget = apppage.getGrid(galaxy, cluster, target);
		//                        if (!!gridfortarget) {
		//                            gridfortarget.workElement = elem;
		//                            gridfortarget.setStr("targetCluster", selectCluster);
		//                            gridfortarget.msgID = "treeView1_" + eventName;
		//                            if (gridfortarget.objtype == "nucleus") {
		//                                gridfortarget.setStr("content_show", webcontent);
		//                                gridfortarget.setStr("content_parent", "contents");
		//                            }
		//                            gridfortarget.sendMessageToGrid(gridfortarget);
		//                        }
		//                    }
		//                }
		//            }
		//        }
		//    }
		//}
	}

	void Hubble::ProcessMessage(HubbleXobj* xobj)
	{
		__int64 nHandle = xobj->getInt64(L"formhandle");
		if (nHandle)
		{
			HubbleWinform* form = nullptr;
			auto it = m_mapWinForm.find(nHandle);
			if (it != m_mapWinForm.end())
			{
				form = it->value.Get();
				form->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kCloudmessageforwinform, xobj));
			}
		}
		nHandle = xobj->getInt64(L"gridhandle");
		if (nHandle)
		{
			auto it = m_mapHubbleNode.find(nHandle);
			if (it != m_mapHubbleNode.end())
			{
				HubbleNode* grid = it->value.Get();
				grid->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kCloudmessageforgrid, xobj));
				grid->ProcessNodeMessage(xobj->getStr(L"msgID"));
			}
		}
		DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kHubblemessage, xobj));
	}

	void Hubble::BindNativeObj(HubbleXobj* xobj)
	{
		if (DOMParser_ == nullptr)
		{
			DOMParser_ = DOMParser::Create(*(DomWindow()->document()));
		}
		if (DOMParser_)
		{
			ExceptionState exception_state(nullptr,
				ExceptionState::kExecutionContext,
				"DOMParser",
				"");
			Document* doc = DOMParser_->parseFromString(blink::StringOrTrustedHTML::FromString(xobj->getStr(L"BindObjData")), "application/xml", exception_state);
			if (doc)
			{
				ContainerNode* pContainerNode = (ContainerNode*)doc->firstChild();
				HTMLCollection* list = pContainerNode->Children();
				for (unsigned int index = 0; index < list->length(); index++)
				{
					Element* workItem = list->item(index);
					Node* pNode = workItem;
					if (pNode->getNodeType() == 1)
					{
						String eventName = workItem->tagName();
						if (!eventName.IsNull())
						{
							__int64 nHandle = xobj->getInt64(L"gridhandle");
							if (nHandle)
							{
								auto it = m_mapHubbleNode.find(nHandle);
								if (it != m_mapHubbleNode.end())
								{
									xobj->setStr(L"Bindevent", eventName);
									m_pRenderframeImpl->SendHubbleMessageEx(xobj->session_);
								}
							}
						}
					}
				}
			}
		}
	}

	void Hubble::MdiChildReady(HubbleXobj* xobj)
	{
		__int64 handle = xobj->getInt64(L"ready_mdichildhandle");
		if (handle)
		{
			HubbleWinform* form = nullptr;
			HubbleWinform* parentmdiform = nullptr;
			auto it = m_mapWinForm.find(handle);
			if (it != m_mapWinForm.end())
			{
				form = it->value.Get();
				parentmdiform = form->mdiParent();
			}
			if (parentmdiform && form)
			{
				form->isReady_ = true;
				form->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kMdichildwinformready, xobj));
				parentmdiform->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kMdichildwinformready, xobj));
			}
		}
	}

	HubbleNode* Hubble::createHubbleNode(HubbleXobj* xobj)
	{
		String strname = xobj->getStr(L"name@page");
		HubbleNode* node = HubbleNode::Create(DomWindow()->GetFrame(), strname);
		__int64 handle = xobj->getInt64(L"gridhandle");
		node->hubble_ = this;
		node->innerXobj_ = xobj;
		node->handle_ = handle;
		node->m_pRenderframeImpl = m_pRenderframeImpl;
		m_mapHubbleNode.insert(handle, node);

		__int64 nGalaxyHandle = xobj->getInt64(L"Galaxyhandle");
		blink::HubbleGalaxy* pGalaxy = nullptr;
		HubbleNode* m_pRootNode = nullptr;
		String strGalaxyname = xobj->getStr(L"galaxy");
		String strClustername = xobj->getStr(L"cluster");;
		bool bNewGalaxy = false;
		if (nGalaxyHandle)
		{
			auto it1 = m_mapHubbleGalaxy.find(nGalaxyHandle);
			if (it1 != m_mapHubbleGalaxy.end())
			{
				__int64 nRootHandle = xobj->getInt64(L"rootgridhandle");
				auto it = m_mapHubbleNode.find(nRootHandle);
				if (it != m_mapHubbleNode.end())
				{
					m_pRootNode = it->value;
				}
				pGalaxy = it1->value;
			}
			else
			{
				bNewGalaxy = true;
				m_pRootNode = node;
			}
		}
		if (m_pRootNode)
		{
			WebString str = strname;
			m_pRootNode->m_mapGrid[str.Utf16()] = node;

			String strMessageXml = xobj->getStr(L"hubblexml");
			xobj->setStr(L"hubblexml", L"");
			if (strMessageXml.IsNull() == false && strMessageXml != "")
			{
				node->innerDOMParser_ = DOMParser::Create(*(DomWindow()->document()));
				if (node->innerDOMParser_ != nullptr)
				{
					ExceptionState exception_state(nullptr,
						ExceptionState::kExecutionContext,
						"DOMParser",
						"");
					node->innerdoc_ = node->innerDOMParser_->parseFromString(blink::StringOrTrustedHTML::FromString(strMessageXml), "application/xml", exception_state);
				}
			}else
				node->innerdoc_ = m_pRootNode->innerdoc_;
		}
		if (bNewGalaxy)
		{
			pGalaxy = HubbleGalaxy::Create(DomWindow()->GetFrame(), strGalaxyname);
			pGalaxy->hubble_ = this;
			pGalaxy->innerXobj_ = xobj;
			pGalaxy->handle_ = nGalaxyHandle;
			pGalaxy->m_pRenderframeImpl = m_pRenderframeImpl;
			m_mapHubbleGalaxy.insert(nGalaxyHandle, pGalaxy);
			WebString str = strGalaxyname;
			m_mapHubbleGalaxy2[str.Utf16()] = pGalaxy;
			str = strClustername;
			pGalaxy->m_mapRootNode[str.Utf16()] = node;
			DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kHubblegalaxycreated, xobj));
		}

		if (node->innerdoc_)
		{
			Element* e = node->innerdoc_->Children()->item(0);
			for (auto* it : *(e->Children()))
			{
				AtomicString name = AtomicString(it->tagName());
				if (it->tagName() == "layout")
				{
					node->gridElem_ = node->innerdoc_->getElementById(AtomicString(strname));
				}
				else
				{
					HTMLCollection* list = e->getElementsByTagName(name);
					if (list->length())
					{
						list = list->item(0)->getElementsByTagName(AtomicString(strname.LowerASCII()));
						if (list->length())
						{
							node->m_mapElement[WebString(it->tagName()).Utf16()] = list->item(0);
							if (it->tagName() == "message")
							{
								node->messageElem_ = list->item(0);
							}
							else if (it->tagName() == "event")
							{
								node->eventElem_ = list->item(0);
								for (auto* it : *(list->item(0)->Children()))
								{
									Element* e = it;
									Node* pNode = e;
									if (pNode->getNodeType() == 1)
									{
										for (auto* it2 : *(e->Children()))
										{
											Element* elem = it2;
											Node* pNode2 = e;
											if (pNode2->getNodeType() == 1)
											{
												node->setStr(L"msgID", L"BIND_CTRL_EVENT");
												node->setStr(L"ctrlname", elem->tagName());
												node->setStr(L"eventname", e->tagName());
												m_pRenderframeImpl->SendHubbleMessageEx(node->xobj()->session_);
											}
										}
									}
								}


							}
							else if (it->tagName() == "ui")
							{
								node->uiElem_ = list->item(0);
							}
							else if (it->tagName() == "property")
							{
								node->propertyElem_ = list->item(0);
							}
						}
					}
				}
			}
		}
		__int64 nPHandle = xobj->getInt64(L"parentgridhandle");
		if (nPHandle)
		{
			auto it = m_mapHubbleNode.find(nPHandle);
			if (it != m_mapHubbleNode.end())
			{
				WebString str = strname;
				it->value->m_mapChildNode2[str.Utf16()] = node;
			}
		}

		if (pGalaxy)
		{
			strClustername = strClustername + L"__";
			strClustername = strClustername + strname;
			pGalaxy->m_mapHubbleNode[handle] = node;
			WebString str = strClustername;
			pGalaxy->m_mapHubbleNode2[str.Utf16()] = node;
			pGalaxy->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kGridcreated, xobj));
		}

		nPHandle = xobj->getInt64(L"parentFormHandle");
		if (nPHandle)
		{
			HubbleWinform* parentform = nullptr;
			HubbleWinform* parentmdiform = nullptr;
			auto it = m_mapWinForm.find(nPHandle);
			if (it != m_mapWinForm.end())
			{
				parentform = it->value.Get();
				node->m_pParentForm = parentform;
				if (parentform->m_nMdiwebbindgridhandle == handle)
					parentform->m_pWebBindMdiNode = node;
				parentform->m_mapHubbleGalaxy[WebString(strGalaxyname).Utf16()] = pGalaxy;
				if (bNewGalaxy)
					parentform->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kHubblegalaxycreated, xobj));
			}
			it = m_mapWinForm.find(xobj->getInt64(L"parentMDIFormHandle"));
			if (it != m_mapWinForm.end())
			{
				parentmdiform = it->value.Get();
			}

			if (parentmdiform && parentform)
			{
				HubbleNode* pNode = parentform->mdibindgrid();
				if (pNode == nullptr)
				{
					int64_t nHandle = xobj->getInt64(L"BindMdiGridHandle");
					if (nHandle)
					{
						auto it1 = m_mapHubbleNode.find(nHandle);
						if (it1 != m_mapHubbleNode.end())
						{
							parentform->m_pBindMdiNode = it1->value.Get();
						}
					}

					nHandle = xobj->getInt64(L"mdiwebbindgridhandle");
					if (nHandle)
					{
						parentform->m_nMdiwebbindgridhandle = nHandle;
						auto it1 = m_mapHubbleNode.find(nHandle);
						if (it1 != m_mapHubbleNode.end())
						{
							parentform->m_pWebBindMdiNode = it1->value.Get();
						}
					}
				}
				parentform->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kGridcreated, xobj));
				return node;
			}
			else if (parentmdiform)
			{
				parentmdiform->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kGridcreated, xobj));
				return node;
			}
			else if (parentform)
			{
				parentform->DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kGridcreated, xobj));
				return node;
			}
		}

		DispatchEvent(*blink::HubbleEvent::Create(blink::event_type_names::kGridcreated, xobj));

		return node;
	}

	HubbleNode* Hubble::getGrid(const int64_t nodeHandle)
	{
		auto it = m_mapHubbleNode.find(nodeHandle);
		if (it != m_mapHubbleNode.end())
			return it->value;
		return nullptr;
	}

	HubbleNode* Hubble::getGrid(const String& galaxyName, const String& clusterName, const String& gridName)
	{
		String clusterName_ = clusterName;
		if (clusterName_ == "undefined")
			clusterName_ = "default";
		auto it = m_mapHubbleGalaxy2.find(WebString(galaxyName).Utf16());
		if (it != m_mapHubbleGalaxy2.end())
		{
			if (gridName == "undefined")
			{
				auto it2 = it->second->m_mapRootNode.find(WebString(clusterName_).Utf16());
				if (it2 != it->second->m_mapRootNode.end())
					return it2->second;
				return nullptr;
			}

			clusterName_ = clusterName + "__" + gridName;
			auto it2 = it->second->m_mapHubbleNode2.find(WebString(clusterName_).Utf16());
			if (it2 != it->second->m_mapHubbleNode2.end())
				return it2->second;
		}
		return nullptr;
	}

	HubbleGalaxy* Hubble::getGalaxy(const String& wndName)
	{
		WebString str = wndName;
		auto it = m_mapHubbleGalaxy2.find(str.Utf16());
		if (it != m_mapHubbleGalaxy2.end())
			return it->second;
		return nullptr;
	}

	HubbleGalaxy* Hubble::getGalaxy(const int64_t wndHandle)
	{
		if (wndHandle)
		{
			auto it = m_mapHubbleGalaxy.find(wndHandle);
			if (it != m_mapHubbleGalaxy.end())
				return it->value;
		}
		return nullptr;
	}

	const AtomicString& Hubble::InterfaceName() const {
		return event_target_names::kHubble;
	}

	ExecutionContext* Hubble::GetExecutionContext() const {
		return DomWindow()->document();
	}

	void Hubble::Observe(const String& key, const String& strXml, V8ApplicationCallback* callback)
	{
		sendMessage("TANGRAM_UI_MESSAGE", key, L"", strXml, L"", L"");
	}

	void Hubble::addEventListener(const String& eventName, V8ApplicationCallback* callback)
	{
		if (callback)
		{
			innerXobj_->mapHubbleEventCallback_.insert(eventName, callback);
		}
	}

	void Hubble::removeEventListener(const String& eventName)
	{
		auto it = innerXobj_->mapHubbleEventCallback_.find(eventName);
		if (it != innerXobj_->mapHubbleEventCallback_.end())
			innerXobj_->mapHubbleEventCallback_.erase(it);
	}

	void Hubble::disConnect()
	{
		int nSize = innerXobj_->mapHubbleEventCallback_.size();
		if (nSize)
		{
			while (innerXobj_->mapHubbleEventCallback_.size())
			{
				auto it = innerXobj_->mapHubbleEventCallback_.begin();
				innerXobj_->mapHubbleEventCallback_.erase(it);
			}
		}
	}

	void Hubble::invokeWinFormCreatedCallback(HubbleWinform* form)
	{
		auto itcallback = mapCallbackFunction_.find((int64_t)form);
		if (itcallback != mapCallbackFunction_.end())
		{
			blink::V8ApplicationCallback* callback = (blink::V8ApplicationCallback*)itcallback->value.Get();
			ScriptState* callback_relevant_script_state = callback->CallbackRelevantScriptState();
			ScriptState::Scope callback_relevant_context_scope(callback_relevant_script_state);
			callback->InvokeAndReportException(nullptr, form->innerXobj_);
			mapCallbackFunction_.erase(itcallback);
		}
	}

	void Hubble::fireEvent(const String& eventName, HubbleXobj* eventParam)
	{
		auto itcallback = innerXobj_->mapHubbleEventCallback_.find(eventName);
		if (itcallback != innerXobj_->mapHubbleEventCallback_.end())
		{
			blink::V8ApplicationCallback* callback = (blink::V8ApplicationCallback*)itcallback->value.Get();
			ScriptState* callback_relevant_script_state = callback->CallbackRelevantScriptState();
			ScriptState::Scope callback_relevant_context_scope(callback_relevant_script_state);
			callback->InvokeAndReportException(nullptr, eventParam);
		}
	}

	void Hubble::sendMessage(HubbleXobj* msg, V8ApplicationCallback* callback, bool bwait = false)
	{
		if (m_pRenderframeImpl)
		{
			if (msg == nullptr)
				msg = innerXobj_;
			msg->setStr(L"senderid", innerXobj_->getid());
			if (callback)
			{
				String callbackid_ = WTF::CreateCanonicalUUIDString();
				msg->setStr(L"callbackid", callbackid_);
				innerXobj_->mapHubbleEventCallback_.insert(callbackid_, callback);
				WebString strID = callbackid_;
				m_pRenderframeImpl->m_mapHubbleSession[strID.Utf16()] = this;
			}
			m_pRenderframeImpl->SendHubbleMessageEx(msg->session_);
		}
		//if (bwait)
		//	run_loop_.Run();
	}

	void Hubble::openUrl(const String& url, long nBrowserWndOpenDisposition, V8ApplicationCallback* callback, bool bwait = false)
	{
		if (m_pRenderframeImpl)
		{
			innerXobj_->setStr(L"senderid", innerXobj_->getid());
			innerXobj_->setStr(L"msgID", L"OPEN_URL");
			innerXobj_->setStr(L"openurl", url);
			innerXobj_->setLong(L"BrowserWndOpenDisposition", nBrowserWndOpenDisposition);
			if (callback)
			{
				String callbackid_ = WTF::CreateCanonicalUUIDString();
				innerXobj_->setStr(L"callbackid", callbackid_);
				innerXobj_->mapHubbleEventCallback_.insert(callbackid_, callback);
				WebString strID = callbackid_;
				m_pRenderframeImpl->m_mapHubbleSession[strID.Utf16()] = this;
			}
			m_pRenderframeImpl->SendHubbleMessageEx(innerXobj_->session_);
		}
		//if (bwait)
		//	run_loop_.Run();
	}

	HubbleXobj* Hubble::getNamedItem(const AtomicString& name) const {
		auto it = mapCloudSession_.find(name);
		if (it != mapCloudSession_.end())
			return it->value.Get();
		return nullptr;
	}

	void Hubble::setNamedItem(const String& name, HubbleXobj* var, ExceptionState&)
	{
		DCHECK(var);
		mapCloudSession_.insert(name, var);
	}

	HubbleXobj* Hubble::setNamedItem(HubbleXobj* var, ExceptionState& exception_state) {
		DCHECK(var);
		mapCloudSession_.insert(var->name(), var);
		return var;
	}

	void Hubble::NamedPropertyEnumerator(Vector<String>& names,
		ExceptionState&) const {
		for (auto it : mapCloudSession_) {
			names.UncheckedAppend(it.value->name());
		}
	}

	bool Hubble::NamedPropertyQuery(const AtomicString& name,
		ExceptionState& exception_state) const {
		Vector<String> properties;
		NamedPropertyEnumerator(properties, exception_state);
		return properties.Contains(name);
	}

	void Hubble::Close()
	{
		while (mapCloudSession_.size())
		{
			mapCloudSession_.erase(mapCloudSession_.begin());
		}
		while (m_mapHubbleNode.size())
		{
			m_mapHubbleNode.erase(m_mapHubbleNode.begin());
		}
		while (m_mapHubbleGalaxy.size())
		{
			m_mapHubbleGalaxy.erase(m_mapHubbleGalaxy.begin());
		}
		while (m_mapWinForm.size())
		{
			m_mapWinForm.erase(m_mapWinForm.begin());
		}
		while (m_mapHubbleCompositor.size())
		{
			m_mapHubbleCompositor.erase(m_mapHubbleCompositor.begin());
		}
		while (mapHubbleCallback_.size())
		{
			mapHubbleCallback_.erase(mapHubbleCallback_.begin());
		}
		while (mapCallbackFunction_.size())
		{
			mapCallbackFunction_.erase(mapCallbackFunction_.begin());
		}
	}

}  // namespace blink
