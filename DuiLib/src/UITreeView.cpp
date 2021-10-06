﻿#include "StdAfx.h"

#pragma warning( disable: 4251 )
namespace DuiLib {
//************************************
// 函数名称: CTreeNodeUI
// 返回类型:
// 参数信息: CTreeNodeUI * _ParentNode
// 函数说明:
//************************************
CTreeNodeUI::CTreeNodeUI(CTreeNodeUI *_ParentNode /*= NULL*/)
{
    m_dwItemTextColor       = 0x00000000;
    m_dwItemHotTextColor    = 0;
    m_dwSelItemTextColor    = 0;
    m_dwSelItemHotTextColor = 0;

    pTreeView       = NULL;
    m_bIsVisable    = true;
    m_bIsCheckBox   = false;
    pParentTreeNode = NULL;
	m_bIsDotline    = false;//zm

    pHoriz          = new CHorizontalLayoutUI();
    pExpandButton   = new CCheckBoxUI();
    pDottedLine     = new CLabelUI();
    pCheckBox       = new CCheckBoxUI();
    pItemButton     = new COptionUI();

    this->SetFixedHeight(18);
    //this->SetFixedWidth(250);
    pExpandButton->SetFixedWidth(GetFixedHeight());
    pDottedLine->SetFixedWidth(2);
    pCheckBox->SetFixedWidth(GetFixedHeight());
    pItemButton->SetAttribute(_T("align"), _T("left"));

    pDottedLine->SetVisible(false);
    pCheckBox->SetVisible(false);
    pItemButton->SetMouseEnabled(false);

    if (_ParentNode)
    {
        if (_tcsicmp(_ParentNode->GetClass(), DUI_CTR_TREENODE) != 0) { return; }

        pDottedLine->SetVisible(_ParentNode->IsVisible());
        pDottedLine->SetFixedWidth(_ParentNode->GetDottedLine()->GetFixedWidth() + 16);
        this->SetParentNode(_ParentNode);
    }

    pHoriz->Add(pDottedLine);
    pHoriz->Add(pExpandButton);
    pHoriz->Add(pCheckBox);
    pHoriz->Add(pItemButton);
    Add(pHoriz);
}

//************************************
// 函数名称: ~CTreeNodeUI
// 返回类型:
// 参数信息: void
// 函数说明:
//************************************
CTreeNodeUI::~CTreeNodeUI(void)
{

}

//************************************
// 函数名称: GetClass
// 返回类型: LPCTSTR
// 函数说明:
//************************************
LPCTSTR CTreeNodeUI::GetClass() const
{
    return DUI_CTR_TREENODE;
}

//************************************
// 函数名称: GetInterface
// 返回类型: LPVOID
// 参数信息: LPCTSTR pstrName
// 函数说明:
//************************************
LPVOID CTreeNodeUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_TREENODE) == 0) { return static_cast<CTreeNodeUI *>(this); }

    return CListContainerElementUI::GetInterface(pstrName);
}

//************************************
// 函数名称: DoEvent
// 返回类型: void
// 参数信息: TEventUI & event
// 函数说明:
//************************************
void CTreeNodeUI::DoEvent(TEventUI &event)
{
    if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
    {
        if (m_pOwner != NULL) { m_pOwner->DoEvent(event); }
        else { CContainerUI::DoEvent(event); }

        return;
    }

    CListContainerElementUI::DoEvent(event);

	//2021-10-06 zm 修复option + tablayout在xml实现转换的bug
	if (event.Type == UIEVENT_BUTTONUP || event.Type == UIEVENT_RBUTTONUP)
	{
		if (IsEnabled())
		{
			if (NULL != pItemButton) pItemButton->Activate();
			Invalidate();
		}

		return;
	}

    if (event.Type == UIEVENT_DBLCLICK)
    {
        if (IsEnabled())
        {
            m_pManager->SendNotify(this, DUI_MSGTYPE_ITEMDBCLICK);
            Invalidate();
        }

        return;
    }

    if (event.Type == UIEVENT_MOUSEENTER)
    {
        if (IsEnabled())
        {
            if (m_bSelected && GetSelItemHotTextColor())
            { pItemButton->SetTextColor(GetSelItemHotTextColor()); }
            else
            { pItemButton->SetTextColor(GetItemHotTextColor()); }
        }
        else
        { pItemButton->SetTextColor(pItemButton->GetDisabledTextColor()); }

        //return;
    }

    if (event.Type == UIEVENT_MOUSELEAVE)
    {
        if (IsEnabled())
        {
            if (m_bSelected && GetSelItemTextColor())
            { pItemButton->SetTextColor(GetSelItemTextColor()); }
            else if (!m_bSelected)
            { pItemButton->SetTextColor(GetItemTextColor()); }
        }
        else
        { pItemButton->SetTextColor(pItemButton->GetDisabledTextColor()); }

        //return;
    }
}

//************************************
// 函数名称: Invalidate
// 返回类型: void
// 函数说明:
//************************************
void CTreeNodeUI::Invalidate()
{
    if (!IsVisible()) { return; }

    if (GetParent())
    {
        CContainerUI *pParentContainer = static_cast<CContainerUI *>(GetParent()->GetInterface(DUI_CTR_CONTAINER));

        if (pParentContainer)
        {
            RECT rc = pParentContainer->GetPos();
            RECT rcInset = pParentContainer->GetPadding();
            rc.left += rcInset.left;
            rc.top += rcInset.top;
            rc.right -= rcInset.right;
            rc.bottom -= rcInset.bottom;
            CScrollBarUI *pVerticalScrollBar = pParentContainer->GetVerticalScrollBar();

            if (pVerticalScrollBar && pVerticalScrollBar->IsVisible()) { rc.right -= pVerticalScrollBar->GetFixedWidth(); }

            CScrollBarUI *pHorizontalScrollBar = pParentContainer->GetHorizontalScrollBar();

            if (pHorizontalScrollBar && pHorizontalScrollBar->IsVisible()) { rc.bottom -= pHorizontalScrollBar->GetFixedHeight(); }

            RECT invalidateRc = m_rcItem;

            if (!::IntersectRect(&invalidateRc, &m_rcItem, &rc)) { return; }

            CControlUI *pParent = GetParent();
            RECT rcTemp;
            RECT rcParent;

            while (pParent = pParent->GetParent())
            {
                rcTemp = invalidateRc;
                rcParent = pParent->GetPos();

                if (!::IntersectRect(&invalidateRc, &rcTemp, &rcParent)) { return; }
            }

            if (m_pManager != NULL) { m_pManager->Invalidate(invalidateRc); }
        }
        else
        {
            CContainerUI::Invalidate();
        }
    }
    else
    {
        CContainerUI::Invalidate();
    }
}

//************************************
// 函数名称: Select
// 返回类型: bool
// 参数信息: bool bSelect
// 函数说明:
//************************************
bool CTreeNodeUI::Select(bool bSelect /*= true*/,  bool bTriggerEvent)
{
    bool nRet = CListContainerElementUI::Select(bSelect, bTriggerEvent);

    if (m_bSelected) { pItemButton->SetTextColor(GetSelItemTextColor()); }
    else             { pItemButton->SetTextColor(GetItemTextColor()); }

    return nRet;
}

//************************************
// 函数名称: Add
// 返回类型: bool
// 参数信息: CControlUI * _pTreeNodeUI
// 函数说明: 通过节点对象添加节点
//************************************
bool CTreeNodeUI::Add(CControlUI *_pTreeNodeUI)
{
    if (_tcsicmp(_pTreeNodeUI->GetClass(), DUI_CTR_TREENODE) == 0)
    { return AddChildNode((CTreeNodeUI *)_pTreeNodeUI); }

    return CListContainerElementUI::Add(_pTreeNodeUI);
}

//************************************
// 函数名称: AddAt
// 返回类型: bool
// 参数信息: CControlUI * pControl
// 参数信息: int iIndex             该参数仅针对当前节点下的兄弟索引，并非列表视图索引
// 函数说明:
//************************************
bool CTreeNodeUI::AddAt(CControlUI *pControl, int iIndex)
{
    if (NULL == static_cast<CTreeNodeUI *>(pControl->GetInterface(DUI_CTR_TREENODE))) { return false; }

    CTreeNodeUI *pIndexNode = static_cast<CTreeNodeUI *>(mTreeNodes.GetAt(iIndex));

    if (!pIndexNode)
    {
        if (!mTreeNodes.Add(pControl)) { return false; }
    }
    else if (pIndexNode && !mTreeNodes.InsertAt(iIndex, pControl))
    { return false; }

    if (!pIndexNode && pTreeView && pTreeView->GetItemAt(GetTreeIndex() + 1))
    { pIndexNode = static_cast<CTreeNodeUI *>(pTreeView->GetItemAt(GetTreeIndex() + 1)->GetInterface(DUI_CTR_TREENODE)); }

    pControl = CalLocation((CTreeNodeUI *)pControl);

    //if (pTreeView && pIndexNode)
    //{ return pTreeView->AddAt((CTreeNodeUI *)pControl, pIndexNode); }
    //else
    //{ return pTreeView->Add((CTreeNodeUI *)pControl); }
    if (pTreeView)
    {
        if (pIndexNode) { return pTreeView->AddAt((CTreeNodeUI *)pControl, pIndexNode); }
        else { return pTreeView->Add((CTreeNodeUI *)pControl); }
    }

    return true;
}

//************************************
// 函数名称: SetVisibleTag
// 返回类型: void
// 参数信息: bool _IsVisible
// 函数说明:
//************************************
void CTreeNodeUI::SetVisibleTag(bool _IsVisible)
{
    m_bIsVisable = _IsVisible;
}

//************************************
// 函数名称: GetVisibleTag
// 返回类型: bool
// 函数说明:
//************************************
bool CTreeNodeUI::GetVisibleTag()
{
    return m_bIsVisable;
}

//************************************
// 函数名称: SetItemText
// 返回类型: void
// 参数信息: LPCTSTR pstrValue
// 函数说明:
//************************************
void CTreeNodeUI::SetItemText(LPCTSTR pstrValue)
{
    pItemButton->SetText(pstrValue);
}

//************************************
// 函数名称: GetItemText
// 返回类型: UiLib::CDuiString
// 函数说明:
//************************************
CDuiString CTreeNodeUI::GetItemText()
{
    return pItemButton->GetText();
}

//************************************
// 函数名称: CheckBoxSelected
// 返回类型: void
// 参数信息: bool _Selected
// 函数说明:
//************************************
void CTreeNodeUI::CheckBoxSelected(bool _Selected)
{
    pCheckBox->Selected(_Selected);
}

//************************************
// 函数名称: IsCheckBoxSelected
// 返回类型: bool
// 函数说明:
//************************************
bool CTreeNodeUI::IsCheckBoxSelected() const
{
    return pCheckBox->IsSelected();
}

//************************************
// 函数名称: IsHasChild
// 返回类型: bool
// 函数说明:
//************************************
bool CTreeNodeUI::IsHasChild() const
{
    return !mTreeNodes.IsEmpty();
}

//************************************
// 函数名称: AddChildNode
// 返回类型: bool
// 参数信息: CTreeNodeUI * _pTreeNodeUI
// 函数说明:
//************************************
bool CTreeNodeUI::AddChildNode(CTreeNodeUI *_pTreeNodeUI)
{
    if (!_pTreeNodeUI) { return false; }

    if (_tcsicmp(_pTreeNodeUI->GetClass(), DUI_CTR_TREENODE) != 0) { return false; }

    _pTreeNodeUI = CalLocation(_pTreeNodeUI);

    bool nRet = true;

    if (pTreeView)
    {
        CTreeNodeUI *pNode = static_cast<CTreeNodeUI *>(mTreeNodes.GetAt(mTreeNodes.GetSize() - 1));

        if (!pNode || !pNode->GetLastNode())
        { nRet = pTreeView->AddAt(_pTreeNodeUI, GetTreeIndex() + 1) >= 0l; }
        else { nRet = pTreeView->AddAt(_pTreeNodeUI, pNode->GetLastNode()->GetTreeIndex() + 1l) >= 0; }
    }

    if (nRet) { mTreeNodes.Add(_pTreeNodeUI); }

    return nRet;
}

//************************************
// 函数名称: RemoveAt
// 返回类型: bool
// 参数信息: CTreeNodeUI * _pTreeNodeUI
// 函数说明:
//************************************
bool CTreeNodeUI::RemoveAt(CTreeNodeUI *_pTreeNodeUI)
{
    int nIndex = mTreeNodes.Find(_pTreeNodeUI);
    CTreeNodeUI *pNode = static_cast<CTreeNodeUI *>(mTreeNodes.GetAt(nIndex));

    if (pNode && pNode == _pTreeNodeUI)
    {
        while (pNode->IsHasChild())
        { pNode->RemoveAt(static_cast<CTreeNodeUI *>(pNode->mTreeNodes.GetAt(0))); }

        mTreeNodes.Remove(nIndex);

        if (pTreeView)
        { pTreeView->Remove(_pTreeNodeUI); }

        return true;
    }

    return false;
}

//************************************
// 函数名称: SetParentNode
// 返回类型: void
// 参数信息: CTreeNodeUI * _pParentTreeNode
// 函数说明:
//************************************
void CTreeNodeUI::SetParentNode(CTreeNodeUI *_pParentTreeNode)
{
    pParentTreeNode = _pParentTreeNode;
}

//************************************
// 函数名称: GetParentNode
// 返回类型: CTreeNodeUI*
// 函数说明:
//************************************
CTreeNodeUI *CTreeNodeUI::GetParentNode()
{
    return pParentTreeNode;
}

//************************************
// 函数名称: GetCountChild
// 返回类型: long
// 函数说明:
//************************************
long CTreeNodeUI::GetCountChild()
{
    return mTreeNodes.GetSize();
}

//************************************
// 函数名称: SetTreeView
// 返回类型: void
// 参数信息: CTreeViewUI * _CTreeViewUI
// 函数说明:
//************************************
void CTreeNodeUI::SetTreeView(CTreeViewUI *_CTreeViewUI)
{
    pTreeView = _CTreeViewUI;
}

//************************************
// 函数名称: GetTreeView
// 返回类型: CTreeViewUI*
// 函数说明:
//************************************
CTreeViewUI *CTreeNodeUI::GetTreeView()
{
    return pTreeView;
}

//************************************
// 函数名称: SetAttribute
// 返回类型: void
// 参数信息: LPCTSTR pstrName
// 参数信息: LPCTSTR pstrValue
// 函数说明:
//************************************
void CTreeNodeUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("text")) == 0) { pItemButton->SetText(pstrValue); }    // 2019-01-10 zhuyadong 不做任何处理
    else if (_tcscmp(pstrName, _T("horizattr")) == 0) { pHoriz->SetAttributeList(pstrValue); }
    else if (_tcscmp(pstrName, _T("dotlineattr")) == 0) { pDottedLine->SetAttributeList(pstrValue); }
    else if (_tcscmp(pstrName, _T("expandattr")) == 0) { pExpandButton->SetAttributeList(pstrValue); }//zm
    else if (_tcscmp(pstrName, _T("checkboxattr")) == 0) { pCheckBox->SetAttributeList(pstrValue); }
    else if (_tcscmp(pstrName, _T("optionattr")) == 0) { pItemButton->SetAttributeList(pstrValue); }//zm
    else if (_tcscmp(pstrName, _T("itemtextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetItemTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("itemhottextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetItemHotTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("selitemtextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetSelItemTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("selitemhottextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetSelItemHotTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("autowidth")) == 0) { DUITRACE(_T("不支持属性:autowidth")); }
    else { CListContainerElementUI::SetAttribute(pstrName, pstrValue); }
}

//************************************
// 函数名称: GetTreeNodes
// 返回类型: UiLib::CDuiPtrArray
// 函数说明:
//************************************
CDuiPtrArray CTreeNodeUI::GetTreeNodes()
{
    return mTreeNodes;
}

//************************************
// 函数名称: GetChildNode
// 返回类型: CTreeNodeUI*
// 参数信息: int _nIndex
// 函数说明:
//************************************
CTreeNodeUI *CTreeNodeUI::GetChildNode(int _nIndex)
{
    return static_cast<CTreeNodeUI *>(mTreeNodes.GetAt(_nIndex));
}

//************************************
// 函数名称: SetVisibleFolderBtn
// 返回类型: void
// 参数信息: bool _IsVisibled
// 函数说明:
//************************************
void CTreeNodeUI::SetVisibleFolderBtn(bool _IsVisibled)
{
    pExpandButton->SetVisible(_IsVisibled);
}

//************************************
// 函数名称: GetVisibleFolderBtn
// 返回类型: bool
// 函数说明:
//************************************
bool CTreeNodeUI::GetVisibleFolderBtn()
{
    return pExpandButton->IsVisible();
}

//************************************
// 函数名称: SetVisibleCheckBtn
// 返回类型: void
// 参数信息: bool _IsVisibled
// 函数说明:
//************************************
void CTreeNodeUI::SetVisibleCheckBtn(bool _IsVisibled)
{
    pCheckBox->SetVisible(_IsVisibled);
}

//************************************
// 函数名称: GetVisibleCheckBtn
// 返回类型: bool
// 函数说明:
//************************************
bool CTreeNodeUI::GetVisibleCheckBtn()
{
    return pCheckBox->IsVisible();
}

//************************************
// 函数名称: GetNodeIndex
// 返回类型: int
// 函数说明: 取得全局树视图的索引
//************************************
int CTreeNodeUI::GetTreeIndex()
{
    if (!pTreeView) { return -1; }

    for (int nIndex = 0; nIndex < pTreeView->GetCount(); nIndex++)
    {
        if (this == pTreeView->GetItemAt(nIndex)) { return nIndex; }
    }

    return -1;
}

//************************************
// 函数名称: GetNodeIndex
// 返回类型: int
// 函数说明: 取得相对于兄弟节点的当前索引
//************************************
int CTreeNodeUI::GetNodeIndex()
{
    CTreeNodeUI *pNode = GetParentNode();

    if (!pNode && !pTreeView) { return -1; }

    if (!pNode && pTreeView) { return GetTreeIndex(); }

    return pNode ? pNode->GetTreeNodes().Find(this) : 0;
}

//************************************
// 函数名称: GetLastNode
// 返回类型: CTreeNodeUI*
// 函数说明:
//************************************
CTreeNodeUI *CTreeNodeUI::GetLastNode()
{
    if (!IsHasChild()) { return this; }

    CTreeNodeUI *nRetNode = NULL;

    for (int nIndex = 0; nIndex < GetTreeNodes().GetSize(); nIndex++)
    {
        CTreeNodeUI *pNode = static_cast<CTreeNodeUI *>(GetTreeNodes().GetAt(nIndex));

        if (!pNode) { continue; }

        CDuiString aa = pNode->GetItemText();

        if (pNode->IsHasChild()) { nRetNode = pNode->GetLastNode(); }
        else                     { nRetNode = pNode; }
    }

    return nRetNode;
}

//************************************
// 函数名称: CalLocation
// 返回类型: CTreeNodeUI*
// 参数信息: CTreeNodeUI * _pTreeNodeUI
// 函数说明: 缩进计算
//************************************
CTreeNodeUI *CTreeNodeUI::CalLocation(CTreeNodeUI *_pTreeNodeUI)
{
    _pTreeNodeUI->GetDottedLine()->SetVisible(true);
    _pTreeNodeUI->GetDottedLine()->SetFixedWidth(pDottedLine->GetFixedWidth() + 16);
    _pTreeNodeUI->SetParentNode(this);
    _pTreeNodeUI->GetItemButton()->SetGroup(pItemButton->GetGroup());
    _pTreeNodeUI->SetTreeView(pTreeView);

    return _pTreeNodeUI;
}

//************************************
// 函数名称: SetTextColor
// 返回类型: void
// 参数信息: DWORD _dwTextColor
// 函数说明:
//************************************
void CTreeNodeUI::SetItemTextColor(DWORD _dwItemTextColor)
{
    m_dwItemTextColor   = _dwItemTextColor;
    pItemButton->SetTextColor(m_dwItemTextColor);
}

//************************************
// 函数名称: GetTextColor
// 返回类型: DWORD
// 函数说明:
//************************************
DWORD CTreeNodeUI::GetItemTextColor() const
{
    return m_dwItemTextColor;
}

//************************************
// 函数名称: SetTextHotColor
// 返回类型: void
// 参数信息: DWORD _dwTextHotColor
// 函数说明:
//************************************
void CTreeNodeUI::SetItemHotTextColor(DWORD _dwItemHotTextColor)
{
    m_dwItemHotTextColor = _dwItemHotTextColor;
    Invalidate();
}

//************************************
// 函数名称: GetTextHotColor
// 返回类型: DWORD
// 函数说明:
//************************************
DWORD CTreeNodeUI::GetItemHotTextColor() const
{
    return m_dwItemHotTextColor;
}

//************************************
// 函数名称: SetSelItemTextColor
// 返回类型: void
// 参数信息: DWORD _dwSelItemTextColor
// 函数说明:
//************************************
void CTreeNodeUI::SetSelItemTextColor(DWORD _dwSelItemTextColor)
{
    m_dwSelItemTextColor = _dwSelItemTextColor;
    Invalidate();
}

//************************************
// 函数名称: GetSelItemTextColor
// 返回类型: DWORD
// 函数说明:
//************************************
DWORD CTreeNodeUI::GetSelItemTextColor() const
{
    return m_dwSelItemTextColor;
}

//************************************
// 函数名称: SetSelHotItemTextColor
// 返回类型: void
// 参数信息: DWORD _dwSelHotItemTextColor
// 函数说明:
//************************************
void CTreeNodeUI::SetSelItemHotTextColor(DWORD _dwSelHotItemTextColor)
{
    m_dwSelItemHotTextColor = _dwSelHotItemTextColor;
    Invalidate();
}

//************************************
// 函数名称: GetSelHotItemTextColor
// 返回类型: DWORD
// 函数说明:
//************************************
DWORD CTreeNodeUI::GetSelItemHotTextColor() const
{
    return m_dwSelItemHotTextColor;
}

void CTreeNodeUI::PaintBorder(HDC hDC)
{
	CControlUI::PaintBorder(hDC);

	if (!m_bIsDotline) return;

	RECT rcDotline = { 0 };
	if (pDottedLine) rcDotline = pDottedLine->GetPos();

	//绘制左边虚线
	if (rcDotline.left > 0 && GetParentNode())
	{
		RECT rcLeftDotline = rcDotline;
		RECT rcFolderButton = GetParentNode()->GetFolderButton()->GetPos();
		rcLeftDotline.left = rcFolderButton.left + (rcFolderButton.right - rcFolderButton.left) / 2;
		if (IsHasChild() || GetParentNode()->GetCountChild() == (GetNodeIndex() + 1))//有子节点或当前节点上最后一个节点时绘制一半虚线 
		{
			rcLeftDotline.bottom -= (rcLeftDotline.bottom - rcLeftDotline.top) / 2;
		}
		rcLeftDotline.right = rcLeftDotline.left;
		CRenderEngine::DrawLine(hDC, rcLeftDotline, 1, 0xFF000000, PS_DOT);
	}

	//绘制顶部虚线
	if (rcDotline.top > 0 && GetParentNode())
	{
		RECT rcTopDotline = rcDotline;
		RECT rcFolderButton = GetParentNode()->GetFolderButton()->GetPos();
		rcTopDotline.left = rcFolderButton.left + (rcFolderButton.right - rcFolderButton.left) / 2;
		rcTopDotline.top += (rcTopDotline.bottom - rcTopDotline.top) / 2;
		rcTopDotline.bottom = rcTopDotline.top;
		CRenderEngine::DrawLine(hDC, rcTopDotline, 1, 0xFF000000, PS_DOT);
	}
}

void CTreeNodeUI::SetVisibleDotlineBtn(bool _IsVisibled)
{
	if (m_bIsDotline == _IsVisibled) return;

	m_bIsDotline = _IsVisibled;
	Invalidate();
}

bool CTreeNodeUI::GetVisibleDotlineBtn()
{
	return m_bIsDotline;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

//************************************
// 函数名称: CTreeViewUI
// 返回类型:
// 参数信息: void
// 函数说明:
//************************************
CTreeViewUI::CTreeViewUI(void) 
: m_bVisibleFolderBtn(true)
, m_bVisibleCheckBtn(false)
, m_uItemMinWidth(0)
, m_bVisibleIndentBtn(true)
, m_bVisibleDotlineBtn(false)
{
    this->GetHeader()->SetVisible(false);
}

//************************************
// 函数名称: ~CTreeViewUI
// 返回类型:
// 参数信息: void
// 函数说明:
//************************************
CTreeViewUI::~CTreeViewUI(void)
{

}

//************************************
// 函数名称: GetClass
// 返回类型: LPCTSTR
// 函数说明:
//************************************
LPCTSTR CTreeViewUI::GetClass() const
{
    return DUI_CTR_TREEVIEW;
}

LPVOID CTreeViewUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_TREEVIEW) == 0) { return static_cast<CTreeViewUI *>(this); }

    return CListUI::GetInterface(pstrName);
}

bool CTreeViewUI::Add(CControlUI *pControl)
{
    if (!pControl) { return false; }

    CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(pControl->GetInterface(DUI_CTR_TREENODE));

    if (pTreeNode == NULL) { return false; }

    pTreeNode->OnNotify += MakeDelegate(this, &CTreeViewUI::OnDBClickItem);
    pTreeNode->GetFolderButton()->OnNotify += MakeDelegate(this, &CTreeViewUI::OnFolderChanged);
    pTreeNode->GetCheckBox()->OnNotify += MakeDelegate(this, &CTreeViewUI::OnCheckBoxChanged);

    pTreeNode->SetVisibleFolderBtn(m_bVisibleFolderBtn);
    pTreeNode->SetVisibleCheckBtn(m_bVisibleCheckBtn);

    if (m_uItemMinWidth > 0) { pTreeNode->SetMinWidth(m_uItemMinWidth); }

    CListUI::Add(pTreeNode);

    if (pTreeNode->GetCountChild() > 0)
    {
        int nCount = pTreeNode->GetCountChild();

        for (int nIndex = 0; nIndex < nCount; nIndex++)
        {
            CTreeNodeUI *pNode = pTreeNode->GetChildNode(nIndex);

            if (pNode) { Add(pNode); }
        }
    }

    pTreeNode->SetTreeView(this);
    return true;
}

bool CTreeViewUI::AddAt(CControlUI *pControl, int iIndex)
{
    if (!pControl) { return false; }

    CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(pControl->GetInterface(DUI_CTR_TREENODE));

    if (pTreeNode == NULL) { return false; }

    return AddAt(pTreeNode, iIndex) >= 0l;
}

bool CTreeViewUI::Remove(CControlUI *pControl, bool bDoNotDestroy)
{
    if (!pControl) { return false; }

    CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(pControl->GetInterface(DUI_CTR_TREENODE));

    if (pTreeNode == NULL) { return CListUI::Remove(pControl, bDoNotDestroy); }

    if (pTreeNode->GetCountChild() > 0)
    {
        int nCount = pTreeNode->GetCountChild();

        for (int nIndex = 0; nIndex < nCount; nIndex++)
        {
            CTreeNodeUI *pNode = pTreeNode->GetChildNode(nIndex);

            if (pNode)
            {
                pTreeNode->Remove(pNode, true);
            }
        }
    }

    return CListUI::Remove(pControl, bDoNotDestroy);
}

bool CTreeViewUI::RemoveAt(int iIndex, bool bDoNotDestroy)
{
    CControlUI *pControl = GetItemAt(iIndex);

    if (pControl == NULL) { return false; }

    CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(pControl->GetInterface(DUI_CTR_TREENODE));

    if (pTreeNode == NULL) { return CListUI::Remove(pControl, bDoNotDestroy); }

    return Remove(pTreeNode);
}

void CTreeViewUI::RemoveAll()
{
    CListUI::RemoveAll();
}

long CTreeViewUI::AddAt(CTreeNodeUI *pControl, int iIndex)
{
    if (!pControl) { return -1; }

    CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(pControl->GetInterface(DUI_CTR_TREENODE));

    if (pTreeNode == NULL) { return -1; }

    CTreeNodeUI *pParent = static_cast<CTreeNodeUI *>(GetItemAt(iIndex));

    if (!pParent) { return -1; }

    pTreeNode->OnNotify += MakeDelegate(this, &CTreeViewUI::OnDBClickItem);
    pTreeNode->GetFolderButton()->OnNotify += MakeDelegate(this, &CTreeViewUI::OnFolderChanged);
    pTreeNode->GetCheckBox()->OnNotify += MakeDelegate(this, &CTreeViewUI::OnCheckBoxChanged);

    pTreeNode->SetVisibleFolderBtn(m_bVisibleFolderBtn);
    pTreeNode->SetVisibleCheckBtn(m_bVisibleCheckBtn);

    if (m_uItemMinWidth > 0) { pTreeNode->SetMinWidth(m_uItemMinWidth); }

    CListUI::AddAt(pTreeNode, iIndex);

    if (pTreeNode->GetCountChild() > 0)
    {
        int nCount = pTreeNode->GetCountChild();

        for (int nIndex = 0; nIndex < nCount; nIndex++)
        {
            CTreeNodeUI *pNode = pTreeNode->GetChildNode(nIndex);

            if (pNode) { return AddAt(pNode, iIndex + 1); }
        }
    }
    else
    { return iIndex + 1; }

    return -1;
}

bool CTreeViewUI::AddAt(CTreeNodeUI *pControl, CTreeNodeUI *_IndexNode)
{
    if (!_IndexNode && !pControl) { return false; }

    int nItemIndex = -1;

    for (int nIndex = 0; nIndex < GetCount(); nIndex++)
    {
        if (_IndexNode == GetItemAt(nIndex))
        {
            nItemIndex = nIndex;
            break;
        }
    }

    if (nItemIndex == -1) { return false; }

    return AddAt(pControl, nItemIndex) >= 0l;
}

//************************************
// 函数名称: Notify
// 返回类型: void
// 参数信息: TNotifyUI & msg
// 函数说明:
//************************************
void CTreeViewUI::Notify(TNotifyUI &msg)
{

}

//************************************
// 函数名称: OnCheckBoxChanged
// 返回类型: bool
// 参数信息: void * param
// 函数说明:
//************************************
bool CTreeViewUI::OnCheckBoxChanged(void *param)
{
    TNotifyUI *pMsg = (TNotifyUI *)param;

    if (pMsg->sType == DUI_MSGTYPE_SELECTCHANGED)
    {
        CCheckBoxUI *pCheckBox = (CCheckBoxUI *)pMsg->pSender;
        CTreeNodeUI *pItem = (CTreeNodeUI *)pCheckBox->GetParent()->GetParent();
        SetItemCheckBox(pCheckBox->GetCheck(), pItem);
        return true;
    }

    return true;
}

//************************************
// 函数名称: OnFolderChanged
// 返回类型: bool
// 参数信息: void * param
// 函数说明:
//************************************
bool CTreeViewUI::OnFolderChanged(void *param)
{
    TNotifyUI *pMsg = (TNotifyUI *)param;

    if (pMsg->sType == DUI_MSGTYPE_SELECTCHANGED)
    {
        CCheckBoxUI *pFolder = (CCheckBoxUI *)pMsg->pSender;
        CTreeNodeUI *pItem = (CTreeNodeUI *)pFolder->GetParent()->GetParent();
        pItem->SetVisibleTag(!pFolder->GetCheck());
        SetItemExpand(!pFolder->GetCheck(), pItem);
        return true;
    }

    return true;
}

//************************************
// 函数名称: OnDBClickItem
// 返回类型: bool
// 参数信息: void * param
// 函数说明:
//************************************
bool CTreeViewUI::OnDBClickItem(void *param)
{
    TNotifyUI *pMsg = (TNotifyUI *)param;

    if (pMsg->sType == DUI_MSGTYPE_ITEMDBCLICK)
    {
        CTreeNodeUI *pItem      = static_cast<CTreeNodeUI *>(pMsg->pSender);
        CCheckBoxUI *pFolder    = pItem->GetFolderButton();
        pFolder->Selected(!pFolder->IsSelected());
        pItem->SetVisibleTag(!pFolder->GetCheck());
        SetItemExpand(!pFolder->GetCheck(), pItem);
        return true;
    }

    return false;
}

//************************************
// 函数名称: SetItemCheckBox
// 返回类型: bool
// 参数信息: bool _Selected
// 参数信息: CTreeNodeUI * _TreeNode
// 函数说明:
//************************************
bool CTreeViewUI::SetItemCheckBox(bool _Selected, CTreeNodeUI *_TreeNode /*= NULL*/)
{
    if (_TreeNode)
    {
        if (_TreeNode->GetCountChild() > 0)
        {
            int nCount = _TreeNode->GetCountChild();

            for (int nIndex = 0; nIndex < nCount; nIndex++)
            {
                CTreeNodeUI *pItem = _TreeNode->GetChildNode(nIndex);
                pItem->GetCheckBox()->Selected(_Selected);

                if (pItem->GetCountChild()) { SetItemCheckBox(_Selected, pItem); }
            }
        }

        return true;
    }
    else
    {
        int nIndex = 0;
        int nCount = GetCount();

        while (nIndex < nCount)
        {
            CTreeNodeUI *pItem = (CTreeNodeUI *)GetItemAt(nIndex);
            pItem->GetCheckBox()->Selected(_Selected);

            if (pItem->GetCountChild()) { SetItemCheckBox(_Selected, pItem); }

            nIndex++;
        }

        return true;
    }

    return false;
}

//************************************
// 函数名称: SetItemExpand
// 返回类型: void
// 参数信息: bool _Expanded
// 参数信息: CTreeNodeUI * _TreeNode
// 函数说明:
//************************************
void CTreeViewUI::SetItemExpand(bool _Expanded, CTreeNodeUI *_TreeNode /*= NULL*/)
{
    if (_TreeNode)
    {
        if (_TreeNode->GetCountChild() > 0)
        {
            int nCount = _TreeNode->GetCountChild();

            for (int nIndex = 0; nIndex < nCount; nIndex++)
            {
                CTreeNodeUI *pItem = _TreeNode->GetChildNode(nIndex);
                pItem->SetVisible(_Expanded);

                if (pItem->GetCountChild() && !pItem->GetFolderButton()->IsSelected())
                { SetItemExpand(_Expanded, pItem); }
            }
        }
    }
    else
    {
        int nIndex = 0;
        int nCount = GetCount();

        while (nIndex < nCount)
        {
            CTreeNodeUI *pItem = (CTreeNodeUI *)GetItemAt(nIndex);

            pItem->SetVisible(_Expanded);

            if (pItem->GetCountChild() && !pItem->GetFolderButton()->IsSelected())
            { SetItemExpand(_Expanded, pItem); }

            nIndex++;
        }
    }
}

//************************************
// 函数名称: SetVisibleFolderBtn
// 返回类型: void
// 参数信息: bool _IsVisibled
// 函数说明:
//************************************
void CTreeViewUI::SetVisibleFolderBtn(bool _IsVisibled)
{
    m_bVisibleFolderBtn = _IsVisibled;
    int nCount = this->GetCount();

    for (int nIndex = 0; nIndex < nCount; nIndex++)
    {
        CTreeNodeUI *pItem = static_cast<CTreeNodeUI *>(this->GetItemAt(nIndex));
        pItem->GetFolderButton()->SetVisible(m_bVisibleFolderBtn);
    }
}

//************************************
// 函数名称: GetVisibleFolderBtn
// 返回类型: bool
// 函数说明:
//************************************
bool CTreeViewUI::GetVisibleFolderBtn()
{
    return m_bVisibleFolderBtn;
}

//************************************
// 函数名称: SetVisibleCheckBtn
// 返回类型: void
// 参数信息: bool _IsVisibled
// 函数说明:
//************************************
void CTreeViewUI::SetVisibleCheckBtn(bool _IsVisibled)
{
    m_bVisibleCheckBtn = _IsVisibled;
    int nCount = this->GetCount();

    for (int nIndex = 0; nIndex < nCount; nIndex++)
    {
        CTreeNodeUI *pItem = static_cast<CTreeNodeUI *>(this->GetItemAt(nIndex));
        pItem->GetCheckBox()->SetVisible(m_bVisibleCheckBtn);
    }
}

//************************************
// 函数名称: GetVisibleCheckBtn
// 返回类型: bool
// 函数说明:
//************************************
bool CTreeViewUI::GetVisibleCheckBtn()
{
    return m_bVisibleCheckBtn;
}

//************************************
// 函数名称: SetItemMinWidth
// 返回类型: void
// 参数信息: UINT _ItemMinWidth
// 函数说明:
//************************************
void CTreeViewUI::SetItemMinWidth(UINT _ItemMinWidth)
{
    m_uItemMinWidth = _ItemMinWidth;

    for (int nIndex = 0; nIndex < GetCount(); nIndex++)
    {
        CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(GetItemAt(nIndex));

        if (pTreeNode) { pTreeNode->SetMinWidth(GetItemMinWidth()); }
    }

    Invalidate();
}

//************************************
// 函数名称: GetItemMinWidth
// 返回类型: UINT
// 函数说明:
//************************************
UINT CTreeViewUI::GetItemMinWidth()
{
    return m_uItemMinWidth;
}

//************************************
// 函数名称: SetItemTextColor
// 返回类型: void
// 参数信息: DWORD _dwItemTextColor
// 函数说明:
//************************************
void CTreeViewUI::SetItemTextColor(DWORD _dwItemTextColor)
{
    for (int nIndex = 0; nIndex < GetCount(); nIndex++)
    {
        CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(GetItemAt(nIndex));

        if (pTreeNode) { pTreeNode->SetItemTextColor(_dwItemTextColor); }
    }
}

//************************************
// 函数名称: SetItemHotTextColor
// 返回类型: void
// 参数信息: DWORD _dwItemHotTextColor
// 函数说明:
//************************************
void CTreeViewUI::SetItemHotTextColor(DWORD _dwItemHotTextColor)
{
    for (int nIndex = 0; nIndex < GetCount(); nIndex++)
    {
        CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(GetItemAt(nIndex));

        if (pTreeNode) { pTreeNode->SetItemHotTextColor(_dwItemHotTextColor); }
    }
}

//************************************
// 函数名称: SetSelItemTextColor
// 返回类型: void
// 参数信息: DWORD _dwSelItemTextColor
// 函数说明:
//************************************
void CTreeViewUI::SetSelItemTextColor(DWORD _dwSelItemTextColor)
{
    for (int nIndex = 0; nIndex < GetCount(); nIndex++)
    {
        CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(GetItemAt(nIndex));

        if (pTreeNode) { pTreeNode->SetSelItemTextColor(_dwSelItemTextColor); }
    }
}

//************************************
// 函数名称: SetSelItemHotTextColor
// 返回类型: void
// 参数信息: DWORD _dwSelHotItemTextColor
// 函数说明:
//************************************
void CTreeViewUI::SetSelItemHotTextColor(DWORD _dwSelHotItemTextColor)
{
    for (int nIndex = 0; nIndex < GetCount(); nIndex++)
    {
        CTreeNodeUI *pTreeNode = static_cast<CTreeNodeUI *>(GetItemAt(nIndex));

        if (pTreeNode) { pTreeNode->SetSelItemHotTextColor(_dwSelHotItemTextColor); }
    }
}

//************************************
// 函数名称: SetAttribute
// 返回类型: void
// 参数信息: LPCTSTR pstrName
// 参数信息: LPCTSTR pstrValue
// 函数说明:
//************************************
void CTreeViewUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcscmp(pstrName, _T("visibleexpandbtn")) == 0)//zm
    { SetVisibleFolderBtn(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("visiblecheckbtn")) == 0)
    { SetVisibleCheckBtn(ParseBool(pstrValue)); }
    else if (_tcscmp(pstrName, _T("itemminwidth")) == 0)
    { SetItemMinWidth(ParseDWord(pstrValue)); }
    else if (_tcscmp(pstrName, _T("itemtextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetItemTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("itemhottextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetItemHotTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("selitemtextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetSelItemTextColor(clr);
    }
    else if (_tcscmp(pstrName, _T("selitemhottextcolor")) == 0)
    {
        DWORD clr = ParseColor(pstrValue);
        SetSelItemHotTextColor(clr);
    }
	//2021-10-06 zm 添加“缩进显示”和“网络线显示”
	else if (_tcscmp(pstrName, _T("visibleindentbtn")) == 0)
	{
		SetVisibleIndentBtn(ParseBool(pstrValue));
	}
	else if (_tcscmp(pstrName, _T("visibledotlinebtn")) == 0)
	{
		SetVisibleDotlineBtn(ParseBool(pstrValue));
	}
    else { CListUI::SetAttribute(pstrName, pstrValue); }
}

void CTreeViewUI::SetVisibleDotlineBtn(bool _IsVisibled)
{
	m_bVisibleDotlineBtn = _IsVisibled;
	int nCount = this->GetCount();

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CTreeNodeUI *pItem = static_cast<CTreeNodeUI *>(this->GetItemAt(nIndex));
		pItem->SetVisibleDotlineBtn(m_bVisibleDotlineBtn);
	}
}

bool CTreeViewUI::GetVisibleDotlineBtn()
{
	return m_bVisibleDotlineBtn;
}

void CTreeViewUI::SetVisibleIndentBtn(bool _IsVisibled)
{
	m_bVisibleIndentBtn = _IsVisibled;
	int nCount = this->GetCount();

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CTreeNodeUI *pItem = static_cast<CTreeNodeUI *>(this->GetItemAt(nIndex));
		pItem->GetDottedLine()->SetVisible(m_bVisibleIndentBtn);
	}
}

bool CTreeViewUI::GetVisibleIndentBtn()
{
	return m_bVisibleIndentBtn;
}

}
