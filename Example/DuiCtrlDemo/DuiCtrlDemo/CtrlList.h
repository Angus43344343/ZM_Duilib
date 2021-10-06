#pragma once

#include "Duilib.h"

class CCtrlList : public CVirtualWnd, public IListCallbackUI, public IListCmbCallbackUI
{
public:
	virtual void Notify(TNotifyUI& msg) override;
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

	//list的回调,用于设置单元格信息：动态修改文本, 文本色, 单元格背景色
	virtual LPCTSTR GetItemText(CControlUI *pList, int iItem, int iSubItem) override;
	virtual DWORD GetItemTextColor(CControlUI* pList, int iItem, int iSubItem) override;
	virtual DWORD GetItemBkColor(CControlUI* pList, int iItem, int iSubItem) override;
	//设计Combo在list单元格上
	virtual void GetComboItems(CControlUI *pCtrl, int iItem, int iSubItem) override;

	//表头排序 
	static int CALLBACK CompareItemHigh(UINT_PTR item1Addr, UINT_PTR item2Addr, UINT_PTR CompareData);
	static int CALLBACK CompareItemLow(UINT_PTR item1Addr, UINT_PTR item2Addr, UINT_PTR CompareData);

private:
	void OnInitWindow(TNotifyUI& msg);
	void OnItemClicked(TNotifyUI& msg);
	void OnItemDBClicked(TNotifyUI& msg);
	void OnRButtonMenu(TNotifyUI& msg);
	void OnHeaderSort(TNotifyUI& msg);

private:
	void OnDoInitList();
	void OnDoInitTree();

public:
	CCtrlList(CPaintManagerUI* pobjManager);
	virtual ~CCtrlList();
};