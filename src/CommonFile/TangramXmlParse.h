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
#ifndef __TANGRAMXMLPARSE_H__
#define __TANGRAMXMLPARSE_H__

#include <vector>
using namespace std;

#include <msxml2.h>
#pragma comment(lib,"msxml2.Lib")


class CTangramXmlParse
{
public:
	CTangramXmlParse* m_pParentParse;
	CTangramXmlParse(void);
	~CTangramXmlParse(void);

	CTangramXmlParse(CComPtr<IXMLDOMNode> _node)
	{
		//m_pUnknown = NULL;
		m_pParentParse = NULL;
		_CTangramXmlParse(_node);
	}
	CTangramXmlParse(CComPtr<IXMLDOMElement> _elem)
	{
		//m_pUnknown = NULL;
		_CTangramXmlParse(_elem);
	}
	CTangramXmlParse(CComPtr<IXMLDOMNodeList> _nlist)
	{
		//m_pUnknown = NULL;
		_CTangramXmlParse(_nlist);
	}

private:
	void Initialize();

protected:
	void _CTangramXmlParse(CComPtr<IXMLDOMNode> _node);
	void _CTangramXmlParse(CComPtr<IXMLDOMElement> _elem);
	void _CTangramXmlParse(CComPtr<IXMLDOMNodeList> _nlist);

protected:
	//CArray<CTangramXmlParse*>  m_aChildElements;
	vector<CTangramXmlParse*>  m_aChildElements;
	CComPtr<IXMLDOMElement> elem;
	CComPtr<IXMLDOMDocument> m_pDoc;
	//CComPtr<IUnknown> m_pUnknown;
	//IUnknown*	m_pUnknown;

public:
	//CComPtr<IXMLDOMElement> ReturnCurrentElement(){return elem;}
	CComPtr<IXMLDOMElement> GetElement() { return elem;}
	CString name();	
	CString xml();
	CString text();
	CString attr(const CString name,CString def) const;

	DWORD attr(const CString name,DWORD def) const;

	bool attrBool(const CString name,bool def = false) const;
	int attrInt(const CString name, int def = 0) const;
	__int64 attrInt64(const CString name, __int64 def = 0) const;
	int GetCount(){return (int)m_aChildElements.size();}
	bool Reflash();

	CTangramXmlParse* AddNode(CTangramXmlParse* pParse, CString strName);
	CTangramXmlParse* ReplaceNode(CTangramXmlParse* pOldParse, CTangramXmlParse* pNewParse, CString strName);
	CTangramXmlParse* AddNode(CString name);
	BOOL ModifyNode(int nRow, int nCol, CString strName, CString strXml);
	CTangramXmlParse* AddTabNode(CString strName, CString strObjTypeID, CString strObjID, CString strStyle, int nPages);
	CTangramXmlParse* AddSplitterNode(int nRow, int nCol, CString strName, int nWidth, int nHeight);
	HRESULT RemoveNode(CString name);
	HRESULT RemoveNode(int index);
	HRESULT RemoveNode(CTangramXmlParse* pItem);
	HRESULT RemoveSubNodes();
	bool put_attr(CString name,CString value);
	bool put_attr(CString name,int value);
	bool put_attr(CString name,__int64 value);
	bool put_attr(CString name,bool value);
	bool put_text(CString text);

public:
	CTangramXmlParse* GetChild(CString strName);
	CTangramXmlParse* GetChild(int nIndex);
	CTangramXmlParse& operator[](CString strName);
	CTangramXmlParse& operator[](int nIndex);

	CTangramXmlParse* FindItem(LPCTSTR strItemname);
	CTangramXmlParse* FindItemByName(LPCTSTR strItemname);

	CTangramXmlParse* GetParseByElement(IXMLDOMElement* pEle);
	bool operator==(CTangramXmlParse& nItem);

	CString val() const;

	DWORD vall() const;
	bool LoadXml(CString strXML);
	bool LoadFile(CString strFile);
	bool SaveFile(CString strFile = _T(""));

private:
	CString	m_strFile;	
	bool	m_bCanSave;

protected:
	void ModifyNameAttrByFix(CString strNameFix);
	CTangramXmlParse* _FindParseByEle(CTangramXmlParse* _pParent, IUnknown* pEle);
	bool Clear();
};
#endif