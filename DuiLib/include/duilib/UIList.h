﻿#ifndef __UILIST_H__
#define __UILIST_H__

#pragma once
#include "UIVerticalLayout.h"
#include "UIHorizontalLayout.h"

namespace DuiLib {
/////////////////////////////////////////////////////////////////////////////////////
//

typedef int (CALLBACK *PULVCompareFunc)(UINT_PTR, UINT_PTR, UINT_PTR);

class CListHeaderUI;

#define UILIST_MAX_COLUMNS 64

typedef struct tagTListInfoUI
{
    int       nColumns;
    RECT      rcColumn[UILIST_MAX_COLUMNS];
    UINT      uFixedHeight;
    int       nFont;
    UINT      uTextStyle;
    RECT      rcPadding;
    DWORD     dwTextColor;
    DWORD     dwBkColor;
    TDrawInfo diBk;
    bool      bAlternateBk;
    DWORD     dwSelectedTextColor;
    DWORD     dwSelectedBkColor;
    TDrawInfo diSelected;
    DWORD     dwHotTextColor;
    DWORD     dwHotBkColor;
    TDrawInfo diHot;
    DWORD     dwDisabledTextColor;
    DWORD     dwDisabledBkColor;
    TDrawInfo diDisabled;
    int       iHLineSize;
    DWORD     dwHLineColor;
    int       iVLineSize;
    DWORD     dwVLineColor;
    bool      bShowHtml;
    bool      bMultiExpandable;
    bool      bCheckBox;
} TListInfoUI;


/////////////////////////////////////////////////////////////////////////////////////
//

class IListCallbackUI
{
public:
    virtual LPCTSTR GetItemText(CControlUI *pList, int iItem, int iSubItem) = 0;
	//2021-10-04 zm增加动态修改文本色,单元格背景色和单元格背景图片
	virtual DWORD GetItemTextColor(CControlUI* pList, int iItem, int iSubItem) = 0;
	virtual DWORD GetItemBkColor(CControlUI* pList, int iItem, int iSubItem) = 0;
	virtual LPCTSTR GetItemBkImage(CControlUI* pList, int iItem, int iSubItem) = 0;
};

class IListCmbCallbackUI
{
public:
    virtual void GetComboItems(CControlUI *pCtrl, int iItem, int iSubItem) = 0;
};

class IListOwnerUI
{
public:
    virtual TListInfoUI *GetListInfo() = 0;
    virtual int GetCurSel() const = 0;

	virtual void DoEvent(TEventUI &event) = 0;
	virtual bool ExpandItem(int iIndex, bool bExpand = true) = 0;
	virtual int GetExpandedItem() const = 0;

	//2021-10-17 zm添加多选(键盘shift,ctrl和鼠标框选等功能)
	virtual bool IsPtInSelItem(POINT& stPoint) = 0;

	virtual bool IsMultiSelect() const = 0;
    virtual bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true) = 0;
	virtual bool SelectMultiItem(int iIndex, bool bTakeFocus = false) = 0;
	virtual bool UnSelectItem(int iIndex, bool bOthers = false) = 0;

	virtual void DragBegin(TEventUI& event) = 0;
	virtual void Draging(TEventUI& event) = 0;
	virtual void DragEnd(TEventUI& event) = 0;
};

class IListUI : public IListOwnerUI
{
public:
    virtual CListHeaderUI *GetHeader() const = 0;
    virtual CContainerUI *GetList() const = 0;
    virtual IListCallbackUI *GetTextCallback() const = 0;
    virtual void SetTextCallback(IListCallbackUI *pCallback) = 0;

    virtual void ShowEdit(int nRow, int nColumn, RECT &rt, CDuiString &sItemTxt) = 0;
    virtual void HideEdit() = 0;
    virtual IListCmbCallbackUI *GetCmbItemCallback() const = 0;
    virtual void SetCmbItemCallback(IListCmbCallbackUI *pCallback) = 0;
	virtual void ShowCombo(int nRow, int nColumn, RECT &rt, CDuiString &sItemTxt) = 0;
    virtual void HideCombo() = 0;
};

class IListItemUI
{
public:
    virtual int GetIndex() const = 0;
    virtual void SetIndex(int iIndex) = 0;
    virtual int GetDrawIndex() const = 0;
    virtual void SetDrawIndex(int iIndex) = 0;
    virtual IListOwnerUI *GetOwner() = 0;
    virtual void SetOwner(CControlUI *pOwner) = 0;
    virtual bool IsSelected() const = 0;
    virtual bool Select(bool bSelect = true, bool bTriggerEvent = true) = 0;
	virtual bool SelectMulti(bool bSelect = true) = 0;//zm
    virtual bool IsExpanded() const = 0;
    virtual bool Expand(bool bExpand = true) = 0;
    virtual void DrawItemText(HDC hDC, const RECT &rcItem) = 0;
};

//2021-10-17 zm 设置checkbox的属性
class ICheckBoxUI
{
public:
	virtual TDrawInfo &GetUnSelImage(void) = 0;
	virtual TDrawInfo &GetSelImage(void) = 0;
	virtual int GetCheckBoxWidth() const = 0;
	virtual int GetCheckBoxHeight() const = 0;
	virtual RECT GetCheckBoxRect(RECT rc) = 0;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class CListBodyUI;
class CListHeaderUI;
class CComboUI;

class DUILIB_API CListUI : public CVerticalLayoutUI, public IListUI, public ICheckBoxUI
{
public:
    CListUI();

    virtual LPCTSTR GetClass() const override;
    virtual UINT GetControlFlags() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual void SetEnabled(bool bEnable = true) override;

    bool GetScrollSelect();
    void SetScrollSelect(bool bScrollSelect);
    virtual int GetCurSel() const override;
    virtual bool SelectItem(int iIndex, bool bTakeFocus = false, bool bTriggerEvent = true) override;

    virtual CControlUI *GetItemAt(int iIndex) const override;
    virtual int GetItemIndex(CControlUI *pControl) const override;
    virtual bool SetItemIndex(CControlUI *pControl, int iIndex) override;
    virtual bool SetMultiItemIndex(CControlUI *pStartControl, int iCount, int iNewStartIndex) override;
    virtual int GetCount() const override;
    virtual bool Add(CControlUI *pControl) override;
    virtual bool AddAt(CControlUI *pControl, int iIndex) override;
    virtual bool Remove(CControlUI *pControl, bool bDoNotDestroy = false) override;
    virtual bool RemoveAt(int iIndex, bool bDoNotDestroy = false) override;
    virtual void RemoveAll() override;
    bool RemoveCount(int iIndex, int iCount, bool bDoNotDestroy = false);

    int FindItemByTag(UINT_PTR pTag);
    int FindItemByUserData(LPCTSTR pstrText);
    CControlUI *GetItemByTag(UINT_PTR pTag);
    CControlUI *GetItemByUserData(LPCTSTR pstrText);

    void EnsureVisible(int iIndex);
    void Scroll(int dx, int dy);

    virtual int GetChildMargin() const override;
    virtual void SetChildMargin(int iMargin) override;

    virtual CListHeaderUI *GetHeader() const override;
    virtual CContainerUI *GetList() const override;
    virtual TListInfoUI *GetListInfo() override;

    UINT GetItemFixedHeight();
    void SetItemFixedHeight(UINT nHeight);
    int GetItemFont(int index);
    void SetItemFont(int index);
    UINT GetItemTextStyle();
    void SetItemTextStyle(UINT uStyle);
    RECT GetItemPadding() const;
    void SetItemPadding(RECT rc);
    DWORD GetItemTextColor() const;
    void SetItemTextColor(DWORD dwTextColor);
    DWORD GetItemBkColor() const;
    void SetItemBkColor(DWORD dwBkColor);
    LPCTSTR GetItemBkImage() const;
    void SetItemBkImage(LPCTSTR pStrImage);
    bool IsAlternateBk() const;
    void SetAlternateBk(bool bAlternateBk);
    DWORD GetSelectedItemTextColor() const;
    void SetSelectedItemTextColor(DWORD dwTextColor);
    DWORD GetSelectedItemBkColor() const;
    void SetSelectedItemBkColor(DWORD dwBkColor);
    LPCTSTR GetSelectedItemImage() const;
    void SetSelectedItemImage(LPCTSTR pStrImage);
    DWORD GetHotItemTextColor() const;
    void SetHotItemTextColor(DWORD dwTextColor);
    DWORD GetHotItemBkColor() const;
    void SetHotItemBkColor(DWORD dwBkColor);
    LPCTSTR GetHotItemImage() const;
    void SetHotItemImage(LPCTSTR pStrImage);
    DWORD GetDisabledItemTextColor() const;
    void SetDisabledItemTextColor(DWORD dwTextColor);
    DWORD GetDisabledItemBkColor() const;
    void SetDisabledItemBkColor(DWORD dwBkColor);
    LPCTSTR GetDisabledItemImage() const;
    void SetDisabledItemImage(LPCTSTR pStrImage);
    int GetItemHLineSize() const;
    void SetItemHLineSize(int iSize);
    DWORD GetItemHLineColor() const;
    void SetItemHLineColor(DWORD dwLineColor);
    int GetItemVLineSize() const;
    void SetItemVLineSize(int iSize);
    DWORD GetItemVLineColor() const;
    void SetItemVLineColor(DWORD dwLineColor);
    bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);

    void SetMultiExpanding(bool bMultiExpandable);
    virtual int GetExpandedItem() const override;
    virtual bool ExpandItem(int iIndex, bool bExpand = true) override;

    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual void Move(SIZE szOffset, bool bNeedInvalidate = true) override;
    virtual void DoEvent(TEventUI &event) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    virtual IListCallbackUI *GetTextCallback() const override;
    virtual void SetTextCallback(IListCallbackUI *pCallback) override;

    virtual SIZE GetScrollPos() const override;
    virtual SIZE GetScrollRange() const override;
    virtual void SetScrollPos(SIZE szPos) override;
    virtual void LineUp() override;
    virtual void LineDown() override;
    virtual void PageUp() override;
    virtual void PageDown() override;
    virtual void HomeUp() override;
    virtual void EndDown() override;
    virtual void LineLeft() override;
    virtual void LineRight() override;
    virtual void PageLeft() override;
    virtual void PageRight() override;
    virtual void HomeLeft() override;
    virtual void EndRight() override;
    virtual void EnableScrollBar(bool bEnableVertical = true, bool bEnableHorizontal = false) override;
    virtual CScrollBarUI *GetVerticalScrollBar() const override;
    virtual CScrollBarUI *GetHorizontalScrollBar() const override;
    bool SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);

	//2021-10-17 zm 设置checkbox的属性
    virtual TDrawInfo &GetUnSelImage(void) override;
    virtual TDrawInfo &GetSelImage(void) override;
	virtual int GetCheckBoxWidth() const override;  
	virtual int GetCheckBoxHeight() const override;      
	virtual RECT GetCheckBoxRect(RECT rc) override;

    void GetAllSelectedItem(CDuiValArray &arySelIdx, int nColumn = 0);
    void SetAllItemSelected(bool bSelect, int nColumn = 0);
    virtual void DoInit() override;

    void GetLastModifiedItem(int &nRow, int &nColumn);
    virtual void ShowEdit(int nRow, int nColumn, RECT &rt, CDuiString &sItemTxt) override;
    virtual void HideEdit() override;
    CEditUI *GetEditUI();

    virtual IListCmbCallbackUI *GetCmbItemCallback() const override;
    virtual void SetCmbItemCallback(IListCmbCallbackUI *pCallback) override;
	virtual void ShowCombo(int nRow, int nColumn, RECT &rt, CDuiString &sItemTxt) override;
    virtual void HideCombo() override;
    CComboUI *GetComboUI();

    int GetMouseColumn(POINT pt);                       // 返回鼠标所在的列

	//2021-10-16 zm添加多选,键盘多选和鼠标框选
	void SetMultiSelect(bool bMultiSel);
	virtual bool IsMultiSelect() const override;

	void SelectAllItems();
	void UnSelectAllItems();

	virtual bool IsPtInSelItem(POINT& stPoint) override;

	int GetMinSelItemIndex();
	int GetMaxSelItemIndex();

	virtual bool SelectMultiItem(int iIndex, bool bTakeFocus = false) override;
	virtual bool UnSelectItem(int iIndex, bool bOthers = false) override;

	int GetComboIndex(CDuiString& strItemText);

	virtual void DragBegin(TEventUI& event) override;
	virtual void Draging(TEventUI& event) override;
	virtual void DragEnd(TEventUI& event) override;

protected:
    bool OnHeaderCheckBoxNotify(void *pParam);
    bool OnEditNotify(void *pParam);
    bool OnComboNotify(void *pParam);
    bool OnScrollNotify(void *pParam);

protected:
    bool             m_bScrollSelect;
    int              m_iCurSel;
    int              m_iExpandedItem;
    IListCallbackUI *m_pCallback;
    CListBodyUI     *m_pList;
    CListHeaderUI   *m_pHeader;
    TListInfoUI      m_ListInfo;

    TDrawInfo        m_diUnSel;         // 复选框未选中状态图片
    TDrawInfo        m_diSel;           // 复选框选中状态图片

	//2021-10-17 zm
	bool			m_bMultiSel;		// 是否多选
	CDuiPtrArray	m_aSelItems;		// 多选item的序号数组
	int				m_iDragBeginIndex;	// 鼠标框选时的item起始序号

	SIZE			m_szCheckBox;

    // 2019-05-19 zhuyadong 编辑框、下拉框使用说明：
    // 1. ListHeaderItem 的属性 editable/comboable，用于标识该列是否可编辑、下拉框
    // 2. 编辑框、下拉框的消息，需要开发人员处理。比如编辑框的过滤、回车键，下拉框的展开、收起、选项改变等等
    // 3. 用户：双击时显示编辑框、下拉框；
    // 4. 编辑框中按回车键，自动更新List、隐藏编辑框，开发人员需要处理 DUI_MSGTYPE_RETURN 保存更新后的数据
    int                 m_nRow;         // 编辑框所在的行、列
    int                 m_nColumn;
    CEditUI            *m_pEdit;
    IListCmbCallbackUI *m_pCmbCallback; // 用于获取下拉框可选项列表
    CComboUI           *m_pCombo;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CListHeaderUI : public CHorizontalLayoutUI
{
public:
    CListHeaderUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    virtual SIZE EstimateSize(SIZE szAvailable) override;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CListHeaderItemUI : public CContainerUI
{
public:
    CListHeaderItemUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual UINT GetControlFlags() const override;

    virtual void SetEnabled(bool bEnable = true) override;

    bool IsDragable() const;
    void SetDragable(bool bDragable);
    DWORD GetSepWidth() const;
    void SetSepWidth(int iWidth);
    DWORD GetTextStyle() const;
    void SetTextStyle(UINT uStyle);
    DWORD GetSepColor() const;
    void SetSepColor(DWORD dwSepColor);
    void SetFont(int index);
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);
    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetSepImage() const;
    void SetSepImage(LPCTSTR pStrImage);

    virtual void SetPadding(RECT rc) override;
    virtual void DoEvent(TEventUI &event) override;
    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    RECT GetThumbRect() const;

    virtual void PaintText(HDC hDC) override;
    virtual void PaintStatusImage(HDC hDC) override;

    void SetEditable(bool bEditable);
    bool IsEditable();

    void SetComboable(bool bComboable);
    bool IsComboable();

    void SetCheckable(bool bCheckable);
    bool IsCheckable();

	//2021-10-02 zm 添加获取当前表头位置，用于进行排序使用
	int GetIndex();

	void SetCheckState(bool bChecked);
	bool GetCheckState() const;

	void SetOwner(ICheckBoxUI* pCheckBox);
protected:
    POINT     m_ptLastMouse;
    bool      m_bDragable;
    bool      m_bShowHtml;
    bool      m_bEditable;      // 当前列是否支持编辑
    bool      m_bComboable;     // 当前列是否支持下拉框
    bool      m_bCheckable;     // 当前列是否支持复选框
    UINT      m_uButtonState;
    int       m_iSepWidth;
    DWORD     m_dwSepColor;
    int       m_iFont;
    UINT      m_uTextStyle;
    TDrawInfo m_diNormal;
    TDrawInfo m_diHot;
    TDrawInfo m_diPushed;
    TDrawInfo m_diFocused;
    TDrawInfo m_diSep;

	bool m_bChecked;//zm
	ICheckBoxUI* m_pOwner;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CListElementUI : public CControlUI, public IListItemUI
{
public:
    CListElementUI();

    virtual LPCTSTR GetClass() const override;
    virtual UINT GetControlFlags() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    virtual void SetEnabled(bool bEnable = true) override;

    virtual int GetIndex() const override;
    virtual void SetIndex(int iIndex) override;
    virtual int GetDrawIndex() const override;
    virtual void SetDrawIndex(int iIndex) override;

    virtual IListOwnerUI *GetOwner() override;
    virtual void SetOwner(CControlUI *pOwner) override;
    virtual bool SetVisible(bool bVisible = true) override;

    virtual bool IsSelected() const override;
    virtual bool Select(bool bSelect = true, bool bTriggerEvent = true) override;
	virtual bool SelectMulti(bool bSelect = true) override;//zm
    virtual bool IsExpanded() const override;
    virtual bool Expand(bool bExpand = true) override;

    virtual void Invalidate() override; // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
    virtual bool Activate() override;

    virtual void DoEvent(TEventUI &event) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    virtual void DrawItemBk(HDC hDC, const RECT &rcItem);
	virtual void DrawItemImage(HDC hDC, const RECT &rcItem);

    int GetMouseColumn(POINT pt);                       // 返回鼠标所在的列
    // 返回鼠标所在列的矩形。
    // bList=true 表示相对当前列表的位置；false 表示相对当前窗体的位置
    RECT GetSubItemPos(int nColumn, bool bList = false);

    virtual void SetCheckBoxState(bool bSelect, int nColumn = 0) { }
    virtual bool GetCheckBoxState(int nColumn = 0) { return false; }

	//2021-10-17 zm Shift多选
	void SetShiftEnble(bool bShiftEnble);
	bool GetShiftEnble();

protected:
    int m_iIndex;
    int m_iDrawIndex;
    bool m_bSelected;
    UINT m_uButtonState;
    IListOwnerUI *m_pOwner;
    bool m_bCustomBk;

	bool m_bShiftEnble;//zm
};


/////////////////////////////////////////////////////////////////////////////////////
// CListLabelElementUI 不支持复选框

class DUILIB_API CListLabelElementUI : public CListElementUI
{
public:
    CListLabelElementUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    virtual void SetOwner(CControlUI *pOwner) override;

    virtual void SetFixedWidth(int cx) override;
    virtual void SetFixedHeight(int cy) override;
    virtual void SetText(LPCTSTR pstrText) override;

    virtual void DoEvent(TEventUI &event) override;
    virtual SIZE EstimateSize(SIZE szAvailable) override;
    virtual bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl) override;

    virtual void DrawItemText(HDC hDC, const RECT &rcItem) override;
protected:
    SIZE    m_cxyFixedLast;
    bool    m_bNeedEstimateSize;

    SIZE    m_szAvailableLast;
    UINT    m_uFixedHeightLast;
    int     m_nFontLast;
    UINT    m_uTextStyleLast;
    RECT    m_rcTextPaddingLast;
};


/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CListTextElementUI : public CListLabelElementUI
{
public:
    CListTextElementUI();
    virtual ~CListTextElementUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;
    virtual UINT GetControlFlags() const override;

    LPCTSTR GetText(int iIndex) const;
    void SetText(int iIndex, LPCTSTR pstrText);

    virtual void SetOwner(CControlUI *pOwner) override;
    CDuiString *GetLinkContent(int iIndex);

    virtual void DoEvent(TEventUI &event) override;
    virtual SIZE EstimateSize(SIZE szAvailable) override;

    virtual void DrawItemText(HDC hDC, const RECT &rcItem) override;

    virtual void SetCheckBoxState(bool bSelect, int nColumn = 0) override;
    virtual bool GetCheckBoxState(int nColumn = 0) override;

	//2021-09-17 zm 单独设置单元格的背景色和文本色
	DWORD GetTextColor(int iIndex) const;
	void SetTextColor(int iIndex, DWORD dwTextColor);

	DWORD GetItemColor(int iIndex) const;
	void SetItemColor(int iIndex, DWORD dwItemColor);

	LPCTSTR GetItemImage(int iIndex) const;
	void SetItemImage(int iIndex, LPCTSTR lpImage);

	virtual void DrawItemBk(HDC hDC, const RECT& rcItem) override;
	virtual void DrawItemImage(HDC hDC, const RECT &rcItem) override;
protected:
    enum { MAX_LINK = 8 };
    int m_nLinks;
    RECT m_rcLinks[MAX_LINK];
    CDuiString m_sLinks[MAX_LINK];
    int m_nHoverLink;
    IListUI *m_pOwner;
    CDuiPtrArray m_aTexts;

    CDuiString m_sTextLast;
    bool m_bCheckBoxSelect;

	//2021-09-17 zm
	CDuiStringPtrMap m_mapTextColors;
	CDuiStringPtrMap m_mapItemColors;
	CDuiStringPtrMap m_mapItemImage;
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CListContainerElementUI : public CContainerUI, public IListItemUI
{
public:
    CListContainerElementUI();

    virtual LPCTSTR GetClass() const override;
    virtual UINT GetControlFlags() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    virtual int GetIndex() const override;
    virtual void SetIndex(int iIndex) override;
    virtual int GetDrawIndex() const override;
    virtual void SetDrawIndex(int iIndex) override;

    virtual IListOwnerUI *GetOwner() override;
    virtual void SetOwner(CControlUI *pOwner) override;
    virtual bool SetVisible(bool bVisible = true) override;
    virtual void SetEnabled(bool bEnable = true) override;

    virtual bool IsSelected() const override;
    virtual bool Select(bool bSelect = true, bool bTriggerEvent = true) override;
	virtual bool SelectMulti(bool bSelect = true) override;//zm
    virtual bool IsExpanded() const override;
    virtual bool Expand(bool bExpand = true) override;
    bool IsExpandable() const;
    void SetExpandable(bool bExpandable);

    virtual void Invalidate() override; // 直接CControl::Invalidate会导致滚动条刷新，重写减少刷新区域
    virtual bool Activate() override;

    virtual void DoEvent(TEventUI &event) override;
    virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    virtual bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl) override;

    virtual void DrawItemText(HDC hDC, const RECT &rcItem) override;
    void DrawItemBk(HDC hDC, const RECT &rcItem);

    virtual SIZE EstimateSize(SIZE szAvailable) override;

    virtual void SetCheckBoxState(bool bSelect, int nColumn = 0);
    virtual bool GetCheckBoxState(int nColumn = 0);

	//2021-10-17 zm Shift多选
	void SetShiftEnble(bool bShiftEnble);
	bool GetShiftEnble();

protected:
    int m_iIndex;
    int m_iDrawIndex;
    bool m_bSelected;
    bool m_bExpandable;
    bool m_bExpand;
    UINT m_uButtonState;
    IListOwnerUI *m_pOwner;
	bool m_bShiftEnble;//zm
};

/////////////////////////////////////////////////////////////////////////////////////
//

class DUILIB_API CListHBoxElementUI : public CListContainerElementUI
{
public:
    CListHBoxElementUI();

    virtual LPCTSTR GetClass() const override;
    virtual LPVOID GetInterface(LPCTSTR pstrName) override;

    virtual void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    virtual bool DoPaint(HDC hDC, const RECT &rcPaint, CControlUI *pStopControl) override;

    virtual void SetCheckBoxState(bool bSelect, int nColumn = 0) override;
    virtual bool GetCheckBoxState(int nColumn = 0) override;

};

} // namespace DuiLib

#endif // __UILIST_H__
