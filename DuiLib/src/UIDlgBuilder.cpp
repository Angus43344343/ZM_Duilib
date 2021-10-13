﻿#include "StdAfx.h"

namespace DuiLib {

CDialogBuilder::CDialogBuilder() : m_pCallback(NULL), m_pstrtype(NULL)
{

}

CControlUI *CDialogBuilder::Create(STRINGorID xml, LPCTSTR type, IDialogBuilderCallback *pCallback,
                                   CPaintManagerUI *pManager, CControlUI *pParent)
{
    //资源ID为0-65535，两个字节；字符串指针为4个字节
    //字符串以<开头认为是XML字符串，否则认为是XML文件

    if (HIWORD(xml.m_lpstr) != NULL)
    {
        if (*(xml.m_lpstr) == _T('<'))
        {
            if (!m_xml.Load(xml.m_lpstr)) { return NULL; }
        }
        else
        {
            if (!m_xml.LoadFromFile(xml.m_lpstr)) { return NULL; }
        }
    }
    else
    {
        HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), xml.m_lpstr, type);

        if (hResource == NULL) { return NULL; }

        HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);

        if (hGlobal == NULL)
        {
            FreeResource(hResource);
            return NULL;
        }

        m_pCallback = pCallback;

        if (!m_xml.LoadFromMem((BYTE *)::LockResource(hGlobal),
                               ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource)))
        {
            return NULL;
        }

        ::FreeResource(hResource);
        m_pstrtype = type;
    }

    return Create(pCallback, pManager, pParent);
}

CControlUI *CDialogBuilder::Create(IDialogBuilderCallback *pCallback, CPaintManagerUI *pManager,
                                   CControlUI *pParent)
{
    m_pCallback = pCallback;
    CMarkupNode root = m_xml.GetRoot();

    if (!root.IsValid()) { return NULL; }

    if (pManager)
    {
        LPCTSTR pstrClass = NULL;
        int nAttributes = 0;
        LPCTSTR pstrName = NULL;
        LPCTSTR pstrValue = NULL;
        LPTSTR pstr = NULL;

        CMarkupNode node = root.GetChild(_T("Include"));

        if (node.IsValid())
        {
            LPCTSTR pstrStyle = node.GetAttributeValue(_T("style"));

            if (pstrStyle && ParseBool(pstrStyle))
            {
                // 解析默认属性
                _ParseInclude(node, pParent, pManager);
            }
        }

        for (CMarkupNode node = root.GetChild(); node.IsValid(); node = node.GetSibling())
        {
            pstrClass = node.GetName();

            if (_tcsicmp(pstrClass, _T("Image")) == 0)
            {
                nAttributes = node.GetAttributeCount();
                CDuiString sImageName;
                CDuiString sImageResType;
                DWORD mask = 0;
                bool bUseHSL = false;
                bool shared = false;

                for (int i = 0; i < nAttributes; i++)
                {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);

                    if (_tcsicmp(pstrName, _T("file")) == 0)
                    {
                        sImageName = ParseString(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("res")) == 0)
                    {
                        sImageName = ParseString(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("restype")) == 0)
                    {
                        sImageResType = ParseString(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("mask")) == 0)
                    {
                        mask = ParseColor(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("hsl")) == 0)
                    {
                        bUseHSL = ParseBool(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("shared")) == 0)
                    {
                        shared = ParseBool(pstrValue);
                    }
                }

                if (!sImageName.IsEmpty())
                {
                    const TImageInfo *data = pManager->GetImage(sImageName);

                    if (!data)
                    {
                        pManager->AddImage(sImageName, sImageResType, mask, bUseHSL, shared);
                    }
                }
            }
            else if (_tcsicmp(pstrClass, _T("Font")) == 0)
            {
                nAttributes = node.GetAttributeCount();
                int id = -1;
                CDuiString sFontName;
                int size = 12;
                bool bold = false;
                bool underline = false;
                bool italic = false;
                bool defaultfont = false;
                bool shared = false;

                for (int i = 0; i < nAttributes; i++)
                {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);

                    if (_tcsicmp(pstrName, _T("id")) == 0)
                    {
                        id = ParseInt(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("name")) == 0)
                    {
                        sFontName = ParseString(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("size")) == 0)
                    {
                        size = ParseInt(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("bold")) == 0)
                    {
                        bold = ParseBool(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("underline")) == 0)
                    {
                        underline = ParseBool(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("italic")) == 0)
                    {
                        italic = ParseBool(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("default")) == 0)
                    {
                        defaultfont = ParseBool(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("shared")) == 0)
                    {
                        shared = ParseBool(pstrValue);
                    }
                }

                if (id >= 0 && !sFontName.IsEmpty())
                {
                    pManager->AddFont(id, sFontName, size, bold, underline, italic, shared);

                    if (defaultfont) { pManager->SetDefaultFont(sFontName, size, bold, underline, italic, shared); }
                }
            }
            else if (_tcsicmp(pstrClass, _T("Default")) == 0)
            {
                nAttributes = node.GetAttributeCount();
                CDuiString sControlName;
                CDuiString sControlValue;
                bool shared = false;

                for (int i = 0; i < nAttributes; i++)
                {
                    pstrName = node.GetAttributeName(i);
                    pstrValue = node.GetAttributeValue(i);

                    if (_tcsicmp(pstrName, _T("name")) == 0)
                    {
                        sControlName = ParseString(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("value")) == 0)
                    {
                        sControlValue = ParseString(pstrValue);
                    }
                    else if (_tcsicmp(pstrName, _T("shared")) == 0)
                    {
                        shared = ParseBool(pstrValue);
                    }
                }

                if (!sControlName.IsEmpty())
                {
                    pManager->AddDefaultAttributeList(sControlName, sControlValue, shared);
                }
            }
			//2021-09-06 zm 添加样式表
			else if (_tcsicmp(pstrClass, _T("Style")) == 0)
			{
				nAttributes = node.GetAttributeCount();
				LPCTSTR pName = NULL;
				LPCTSTR pStyle = NULL;
				bool shared = false;
				for (int i = 0; i < nAttributes; i++)
				{
					pstrName = node.GetAttributeName(i);
					pstrValue = node.GetAttributeValue(i);
					if (_tcsicmp(pstrName, _T("name")) == 0)
					{
						pName = pstrValue;
					}
					else if (_tcsicmp(pstrName, _T("value")) == 0)
					{
						pStyle = pstrValue;
					}
					else if (_tcsicmp(pstrName, _T("shared")) == 0)
					{
						shared = (_tcsicmp(pstrValue, _T("true")) == 0);
					}
				}
				if (pName)
				{
					pManager->AddStyle(pName, pStyle, shared);
				}
			}
        }

        pstrClass = root.GetName();

        if (_tcsicmp(pstrClass, _T("Window")) == 0)
        {
            if (pManager->GetPaintWindow())
            {
                pManager->SetWindowAttributeList(pManager->GetDefaultAttributeList(_T("Window"), true));
                pManager->SetWindowAttributeList(pManager->GetDefaultAttributeList(_T("Window"), false));
                int nAttributes = root.GetAttributeCount();

                for (int i = 0; i < nAttributes; i++)
                {
                    pstrName = root.GetAttributeName(i);
                    pstrValue = root.GetAttributeValue(i);
                    pManager->SetWindowAttribute(pstrName, pstrValue);
                }
            }
        }
    }

    return _Parse(&root, pParent, pManager);
}

CMarkup *CDialogBuilder::GetMarkup()
{
    return &m_xml;
}

void CDialogBuilder::GetLastErrorMessage(LPTSTR pstrMessage, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorMessage(pstrMessage, cchMax);
}

void CDialogBuilder::GetLastErrorLocation(LPTSTR pstrSource, SIZE_T cchMax) const
{
    return m_xml.GetLastErrorLocation(pstrSource, cchMax);
}

CControlUI *CDialogBuilder::_Parse(CMarkupNode *pRoot, CControlUI *pParent, CPaintManagerUI *pManager)
{
    IContainerUI *pContainer = NULL;
    CControlUI *pReturn = NULL;

    for (CMarkupNode node = pRoot->GetChild(); node.IsValid(); node = node.GetSibling())
    {
        LPCTSTR pstrClass = node.GetName();

        if (NULL == pstrClass || _tcsicmp(pstrClass, _T("Image")) == 0 || _tcsicmp(pstrClass, _T("Font")) == 0
            || _tcsicmp(pstrClass, _T("Default")) == 0)
        {
            continue;
        }

        CControlUI *pControl = NULL;

        if (_tcsicmp(pstrClass, _T("Include")) == 0)
        {
            if (!node.HasAttributes()) { continue; }

            LPCTSTR pstrStyle = node.GetAttributeValue(_T("style"));

            // 默认属性在前面已经处理完毕
            if (pstrStyle && ParseBool(pstrStyle)) { continue; }

            // 生成窗体
            pControl = _ParseInclude(node, pParent, pManager);
            continue;
        }
        //树控件XML解析
        else if (_tcsicmp(pstrClass, _T("TreeNode")) == 0)
        {
            CTreeNodeUI *pParentNode    = static_cast<CTreeNodeUI *>(pParent->GetInterface(_T("TreeNode")));
            CTreeNodeUI *pNode          = new CTreeNodeUI();

            if (pParentNode)
            {
                if (!pParentNode->Add(pNode))
                {
                    delete pNode;
                    continue;
                }
            }

            // 若有控件默认配置先初始化默认属性
            if (pManager)
            {
                pNode->SetManager(pManager, NULL, false);
                LPCTSTR pDefAttr = pManager->GetDefaultAttributeList(pstrClass, true);
                pNode->SetAttributeList(pDefAttr);
                pDefAttr = pManager->GetDefaultAttributeList(pstrClass, false);
                pNode->SetAttributeList(pDefAttr);
                // LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
                // if (pDefaultAttributes)
                // {
                //     pNode->SetAttributeList(pDefaultAttributes);
                // }
            }

            // 解析所有属性并覆盖默认属性
            if (node.HasAttributes())
            {
                TCHAR szValue[500] = { 0 };
                SIZE_T cchLen = LENGTHOF(szValue) - 1;
                // Set ordinary attributes
                int nAttributes = node.GetAttributeCount();

                for (int i = 0; i < nAttributes; i++)
                {
                    pNode->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
                }
            }

            //检索子节点及附加控件
            if (node.HasChildren())
            {
                CControlUI *pSubControl = _Parse(&node, pNode, pManager);

                if (pSubControl && _tcsicmp(pSubControl->GetClass(), _T("TreeNodeUI")) != 0)
                {
                    //                  pSubControl->SetFixedWidth(30);
                    //                  CHorizontalLayoutUI* pHorz = pNode->GetTreeNodeHoriznotal();
                    //                  pHorz->Add(new CEditUI());
                    //                  continue;
                }
            }

            if (!pParentNode)
            {
                CTreeViewUI *pTreeView = static_cast<CTreeViewUI *>(pParent->GetInterface(_T("TreeView")));
                ASSERT(pTreeView);

                if (pTreeView == NULL) { return NULL; }

                if (!pTreeView->Add(pNode))
                {
                    delete pNode;
                    continue;
                }
            }

            continue;
        }
        else
        {
#ifdef _DEBUG
            DUITRACE(_T("Create Control: %s"), pstrClass);
#endif
            SIZE_T cchLen = _tcslen(pstrClass);

            switch (cchLen)
            {
            case 4:
                if (_tcsicmp(pstrClass, DUI_CTR_EDIT) == 0)                  { pControl = new CEditUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_LIST) == 0)             { pControl = new CListUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_TEXT) == 0)             { pControl = new CTextUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_TREE) == 0)             { pControl = new CTreeViewUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_HBOX) == 0)             { pControl = new CHorizontalLayoutUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_VBOX) == 0)             { pControl = new CVerticalLayoutUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_MENU) == 0)             { pControl = new CMenuUI; }

                break;

            case 5:
                if (_tcsicmp(pstrClass, DUI_CTR_COMBO) == 0)                 { pControl = new CComboUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_LABEL) == 0)            { pControl = new CLabelUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_FLASH) == 0)            { pControl = new CFlashUI; }

                break;

            case 6:
                if (_tcsicmp(pstrClass, DUI_CTR_BUTTON) == 0)                { pControl = new CButtonUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_OPTION) == 0)           { pControl = new COptionUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_SLIDER) == 0)           { pControl = new CSliderUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_HOTKEY) == 0)           { pControl = new CHotKeyUI; }

                break;

            case 7:
                if (_tcsicmp(pstrClass, DUI_CTR_CONTROL) == 0)               { pControl = new CControlUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_ACTIVEX) == 0)          { pControl = new CActiveXUI; }
                else if (_tcscmp(pstrClass, DUI_CTR_GIFANIM) == 0)           { pControl = new CGifAnimUI; }
				else if (_tcscmp(pstrClass, DUI_CTR_PALETTE) == 0)           { pControl = new CColorPaletteUI; }//zm

                break;

            case 8:
                if (_tcsicmp(pstrClass, DUI_CTR_PROGRESS) == 0)              { pControl = new CProgressUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_RICHEDIT) == 0)         { pControl = new CRichEditUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_CHECKBOX) == 0)         { pControl = new CCheckBoxUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_RADIOBOX) == 0)         { pControl = new CRadioBoxUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_COMBOBOX) == 0)         { pControl = new CComboUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_DATETIME) == 0)         { pControl = new CDateTimeUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_TREEVIEW) == 0)         { pControl = new CTreeViewUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_TREENODE) == 0)         { pControl = new CTreeNodeUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_PWDCHECK) == 0)         { pControl = new CPwdCheckUI; }
				else if (_tcsicmp(pstrClass, DUI_CTR_ROLLTEXT) == 0)         { pControl = new CRollTextUI; }//zm
				else if (_tcsicmp(pstrClass, DUI_CTR_GROUPBOX) == 0)         { pControl = new CGroupBoxUI; }//zm

                break;

            case 9:
                if (_tcsicmp(pstrClass, DUI_CTR_CONTAINER) == 0)             { pControl = new CContainerUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_TABLAYOUT) == 0)        { pControl = new CTabLayoutUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_SCROLLBAR) == 0)        { pControl = new CScrollBarUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_IPADDRESS) == 0)        { pControl = new CIPAddressUI; }

                break;

            case 10:
                if (_tcsicmp(pstrClass, DUI_CTR_LISTHEADER) == 0)            { pControl = new CListHeaderUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_TILELAYOUT) == 0)       { pControl = new CTileLayoutUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_WEBBROWSER) == 0)       { pControl = new CWebBrowserUI; }//zm
				else if (_tcscmp(pstrClass, DUI_CTR_WKEBROWSER) == 0)        { pControl = new CWkeBrowserUI; }//zm

                break;

            case 11:
                if (_tcsicmp(pstrClass, DUI_CTR_CHILDLAYOUT) == 0)           { pControl = new CChildLayoutUI; }

                if (_tcsicmp(pstrClass, DUI_CTR_MENUELEMENT) == 0)           { pControl = new CMenuElementUI; }

                break;

            case 13:
                if (_tcsicmp(pstrClass, DUI_CTR_HWEIGHTLAYOUT) == 0)         { pControl = new CHWeightLayoutUI; }

                break;

            case 14:
                if (_tcsicmp(pstrClass, DUI_CTR_VERTICALLAYOUT) == 0)        { pControl = new CVerticalLayoutUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_LISTHEADERITEM) == 0)   { pControl = new CListHeaderItemUI; }
				else if (_tcsicmp(pstrClass, DUI_CTR_CIRCLEPROGRESS) == 0)   { pControl = new CCircleProgressUI; }//2021-09-07 zm

                break;

            case 15:
                if (_tcsicmp(pstrClass, DUI_CTR_LISTTEXTELEMENT) == 0)       { pControl = new CListTextElementUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_LISTHBOXELEMENT) == 0)  { pControl = new CListHBoxElementUI; }

                break;

            case 16:
                if (_tcsicmp(pstrClass, DUI_CTR_HORIZONTALLAYOUT) == 0)      { pControl = new CHorizontalLayoutUI; }
                else if (_tcsicmp(pstrClass, DUI_CTR_LISTLABELELEMENT) == 0) { pControl = new CListLabelElementUI; }

                break;

            case 20:
                if (_tcsicmp(pstrClass, DUI_CTR_LISTCONTAINERELEMENT) == 0)  { pControl = new CListContainerElementUI; }

                break;
            }

            // User-supplied control factory
            if (pControl == NULL)
            {
                CDuiPtrArray *pPlugins = CPaintManagerUI::GetPlugins();
                LPCREATECONTROL lpCreateControl = NULL;

                for (int i = 0; i < pPlugins->GetSize(); ++i)
                {
                    lpCreateControl = (LPCREATECONTROL)pPlugins->GetAt(i);

                    if (lpCreateControl != NULL)
                    {
                        pControl = lpCreateControl(pstrClass);

                        if (pControl != NULL) { break; }
                    }
                }
            }

            if (pControl == NULL && m_pCallback != NULL)
            {
                pControl = m_pCallback->CreateControl(pstrClass);
            }
        }

#ifndef _DEBUG
        ASSERT(pControl);
#endif // _DEBUG

        if (pControl == NULL)
        {
#ifdef _DEBUG
            DUITRACE(_T("Unknow Control:%s"), pstrClass);
#endif
            continue;
        }

        // Add children
        if (node.HasChildren())
        {
            _Parse(&node, pControl, pManager);
        }

        TCHAR szValue[256] = { 0 };
        int cchLen = LENGTHOF(szValue) - 1;

        // Attach to parent
        // 因为某些属性和父窗口相关，比如selected，必须先Add到父窗口
        if (pParent != NULL)
        {
            LPCTSTR lpValue = szValue;

            if (node.GetAttributeValue(_T("cover"), szValue, cchLen) && _tcscmp(lpValue, _T("true")) == 0)
            {
                pParent->SetCover(pControl);
            }
            else
            {
                CTreeNodeUI *pContainerNode = static_cast<CTreeNodeUI *>(pParent->GetInterface(DUI_CTR_TREENODE));

                if (pContainerNode)
                {
                    pContainerNode->GetTreeNodeHoriznotal()->Add(pControl);
                }
                else
                {
                    if (pContainer == NULL) { pContainer = static_cast<IContainerUI *>(pParent->GetInterface(DUI_CTR_ICONTAINER)); }

                    ASSERT(pContainer);

                    if (pContainer == NULL) { return NULL; }

                    if (!pContainer->Add(pControl))
                    {
                        pControl->Delete();
                        continue;
                    }
                }
            }
        }

        // Init default attributes
        if (pManager)
        {
            pControl->SetManager(pManager, NULL, false);
            LPCTSTR pDefAttr = pManager->GetDefaultAttributeList(pstrClass, true);
            pControl->SetAttributeList(pDefAttr);
            pDefAttr = pManager->GetDefaultAttributeList(pstrClass, false);
            pControl->SetAttributeList(pDefAttr);
            // LPCTSTR pDefaultAttributes = pManager->GetDefaultAttributeList(pstrClass);
            // if (pDefaultAttributes)
            // {
            //     pControl->SetAttributeList(pDefaultAttributes);
            // }
        }

        // Process attributes
        if (node.HasAttributes())
        {
            // Set ordinary attributes
            int nAttributes = node.GetAttributeCount();

            for (int i = 0; i < nAttributes; i++)
            {
                pControl->SetAttribute(node.GetAttributeName(i), node.GetAttributeValue(i));
            }
        }

        if (pManager)
        {
            pControl->SetManager(NULL, NULL, false);
        }

        // Return first item
        if (pReturn == NULL) { pReturn = pControl; }
    }

    return pReturn;
}

CControlUI *CDialogBuilder::_ParseInclude(CMarkupNode &node, CControlUI *pParent, CPaintManagerUI *pManager)
{
    CControlUI *pControl = NULL;
    int count = 1;
    LPTSTR pstr = NULL;
    TCHAR szValue[500] = { 0 };
    SIZE_T cchLen = LENGTHOF(szValue) - 1;

    if (node.GetAttributeValue(_T("count"), szValue, cchLen))
    {
        count = _tcstol(szValue, &pstr, 10);
    }

    cchLen = LENGTHOF(szValue) - 1;

    if (!node.GetAttributeValue(_T("source"), szValue, cchLen)) { return NULL; }

    CDialogBuilder builder;

    for (int i = 0; i < count; ++i)
    {
        if (!builder.GetMarkup()->IsValid())
        {
            if (m_pstrtype != NULL)    // 使用资源dll，从资源中读取
            {
                WORD id = (WORD)_tcstol(szValue, &pstr, 10);
                pControl = builder.Create((UINT)id, m_pstrtype, m_pCallback, pManager, pParent);
            }
            else
            {
                pControl = builder.Create((LPCTSTR)szValue, (UINT)0, m_pCallback, pManager, pParent);
            }
        }
        else
        {
            pControl = builder.Create(m_pCallback, pManager, pParent);
        }
    }

    return pControl;
}

} // namespace DuiLib
