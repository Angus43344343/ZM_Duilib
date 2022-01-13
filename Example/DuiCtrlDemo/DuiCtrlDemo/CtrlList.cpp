#include "stdafx.h"
#include "CtrlList.h"

#define EN_LIST_ITEM_NUM	(500)

const CDuiString g_astrList[][7] = 
{ { _T("0"), _T("����"), _T("��"), _T("65"), _T("74"), _T("35"), _T("35") }
, { _T("1"), _T("����"), _T("Ů"), _T("58"), _T("74"), _T("35"), _T("35") }
, { _T("2"), _T("����"), _T("��"), _T("78"), _T("74"), _T("35"), _T("35") }
, { _T("3"), _T("����"), _T("��"), _T("84"), _T("74"), _T("35"), _T("35") }
, { _T("4"), _T("����"), _T("Ů"), _T("82"), _T("74"), _T("35"), _T("35") }
, { _T("5"), _T("����"), _T("��"), _T("100"), _T("74"), _T("35"), _T("35") }
, { _T("6"), _T("�پ�"), _T("��"), _T("90"), _T("74"), _T("35"), _T("35") }
, { _T("7"), _T("��ʮ"), _T("Ů"), _T("65"), _T("74"), _T("35"), _T("35") }
, { _T("8"), _T("��һ"), _T("��"), _T("90"), _T("74"), _T("35"), _T("35") }
, { _T("9"), _T("���"), _T("Ů"), _T("65"), _T("74"), _T("35"), _T("35") } };


CCtrlList::CCtrlList(CPaintManagerUI* pobjManager)
{
	m_pobjManager = pobjManager;
}


CCtrlList::~CCtrlList()
{
}

void CCtrlList::OnDoInitList()
{
	CListUI* pobjList = static_cast<CListUI*>(m_pobjManager->FindControl(_T("listTest")));
	if (nullptr == pobjList) return;

	for (int iRow = 0; iRow < EN_LIST_ITEM_NUM; iRow++)
	{
		CListTextElementUI* pobjListItem = new CListTextElementUI();
		if (nullptr == pobjListItem) break;

		pobjList->Add(pobjListItem);//������Ϣ�ӵ�List

		//���õ�Ԫ���ı�
		for (int iColumn = 0; iColumn < sizeof (g_astrList[0]) / sizeof(g_astrList[0][0]); iColumn++)
		{
			if ((8 == iRow) && (3 == iColumn))
			{
				pobjListItem->SetText(iColumn, _T("��������"));
				continue;
			}

			if ((8 == iRow) && (4 == iColumn))
			{
				pobjListItem->SetText(iColumn, _T("˫������"));
				continue;
			}

			if ((8 == iRow) && (5 == iColumn))
			{
				pobjListItem->SetItemImage(iColumn, _T("list/bkimage.jpg"));
			}

			pobjListItem->SetText(iColumn, g_astrList[iRow % (sizeof (g_astrList) / sizeof (g_astrList[0]))][iColumn]);
		}
	}

	//�����ı������⴦����ڻص��ﴦ��
	pobjList->SetTextCallback(this);

	//����Combo�Ĵ������GetComboItems�ﴦ��
	pobjList->SetCmbItemCallback(this);
}

void CCtrlList::OnDoInitTree()
{
	//ֵ����
	//CTreeViewUI* pobjTreeView = static_cast<CTreeViewUI*>(m_pobjManager->FindControl(_T("treeTest")));
	//if (nullptr == pobjTreeView) return;

	//int iCount = pobjTreeView->GetCount();

	//CTreeNodeUI* pobjTreeNode = static_cast<CTreeNodeUI*>(m_pobjManager->FindControl(_T("treeTabTest")));
	//if (nullptr == pobjTreeView) return;

	//CDuiString strGroup = pobjTreeNode->GetItemButton()->GetGroup();

}

void CCtrlList::OnInitWindow(TNotifyUI& msg)
{
	OnDoInitList(); 
	OnDoInitTree();
}

void CCtrlList::OnItemClicked(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(_T("listTest")) != 0) return;
	CListTextElementUI* pobjTextElement = (CListTextElementUI*)(msg.wParam);

	int iRow = pobjTextElement->GetIndex();//��
	int iColomn = (int)(msg.lParam); //��

	if ((8 == iRow) && (3 == iColomn)) pobjTextElement->SetItemColor(iColomn, 0xFFccffcc);	
}

void CCtrlList::OnItemDBClicked(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(_T("listTest")) != 0) return;
	CListTextElementUI* pobjTextElement = (CListTextElementUI*)(msg.wParam);

	int iRow = pobjTextElement->GetIndex();//��
	int iColomn = (int)(msg.lParam); //��

	if ((8 == iRow) && (4 == iColomn)) pobjTextElement->SetItemColor(iColomn, 0xFFFF0000);
}

void CCtrlList::OnRButtonMenu(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(_T("listTest")) != 0) return;

	CDuiPoint stPoint = msg.ptMouse;
	ClientToScreen(m_pobjManager->GetPaintWindow(), &stPoint);
	CMenuWnd::CreateMenu(nullptr, _T("menu.xml"), _T("xml"), stPoint, m_pobjManager);
}

void CCtrlList::OnHeaderSort(TNotifyUI& msg)
{
	if (msg.pSender->GetName().Compare(_T("listHeaderItemTest3")) != 0) return;
	if (msg.pSender->GetParent()->GetParent()->GetName().Compare(_T("listTest")) != 0) return;//ͨ����ͷ��ȡ����
	CListUI* pobjList = static_cast<CListUI*>(m_pobjManager->FindControl(_T("listTest")));
	if (nullptr == pobjList) return;

	static bool s_bCompare = false;
	s_bCompare = !s_bCompare;

	//��ʼ�ȶ�
	if (s_bCompare) { pobjList->SortItems(CompareItemHigh, (int)(msg.lParam)); }
	else { pobjList->SortItems(CompareItemLow, (int)(msg.lParam)); }
}

void CCtrlList::Notify(TNotifyUI& msg)
{
	if (DUI_MSGTYPE_WINDOWINIT == msg.sType)//���ڳ�ʼ��
	{
		OnInitWindow(msg);
	}
	if (DUI_MSGTYPE_ITEMCLICK == msg.sType) //list ��Ԫ�񵥻��¼�
	{
		OnItemClicked(msg);
	}
	else if (DUI_MSGTYPE_ITEMDBCLICK == msg.sType) //list ��Ԫ��˫���¼�
	{
		OnItemDBClicked(msg);
	}
	else if (DUI_MSGTYPE_MENU == msg.sType) //list �Ҽ��˵�
	{
		OnRButtonMenu(msg);
	}
	else if (DUI_MSGTYPE_HEADERCLICK == msg.sType) //list �������
	{
		OnHeaderSort(msg);
	}
	//tree������list,���ﲻ����Ϣ��Ӧ�������
}

LRESULT CCtrlList::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	LRESULT lRes = 0;

	return lRes;
}

LPCTSTR CCtrlList::GetItemText(CControlUI *pList, int iItem, int iSubItem)
{
	//�������
	if (0 == iSubItem)
	{
		CDuiString strTextNum;
		strTextNum.Format(_T("%d"), iItem);
		return strTextNum.GetData();
	}

	return nullptr;
}

DWORD CCtrlList::GetItemTextColor(CControlUI* pList, int iItem, int iSubItem)
{
	//���õ�Ԫ���ı�ɫ
	if ((7 == iItem) && (4 == iSubItem)) { return 0xFFFF0000; }

	return 0;
}

DWORD CCtrlList::GetItemBkColor(CControlUI* pList, int iItem, int iSubItem)
{
	//��������ɫ
	if (1 == iItem) { return 0xFFffffcc; }

	//��������ɫ 
	if (2 == iSubItem) { return 0xFFccffcc; }

	return 0;
}

LPCTSTR CCtrlList::GetItemBkImage(CControlUI* pList, int iItem, int iSubItem)
{
	if (6 == iSubItem) { return _T("list/bkimage.jpg"); }//������ͼƬ

	return nullptr;
}

void CCtrlList::GetComboItems(CControlUI *pCtrl, int iItem, int iSubItem)
{
	CComboUI* pobjCombo = (CComboUI*)pCtrl;
	CDuiString astrInfo[] = { _T("����"), _T("����"), _T("����") };
	pobjCombo->SetAttribute(_T("itemalign"), _T("center"));

	for (int iCount = 0; iCount < sizeof (astrInfo) / sizeof (astrInfo[0]); iCount++)
	{
		CListLabelElementUI* pobjElement = new CListLabelElementUI;
		pobjElement->SetText(astrInfo[iCount]);

		pobjCombo->Add(pobjElement);
	}
}

/*�ȶԺ���:���ַ����ȶԾ���,�û����Ը�������д�Լ��ıȶԺ���*/
int CALLBACK CCtrlList::CompareItemHigh(UINT_PTR item1Addr, UINT_PTR item2Addr, UINT_PTR CompareData)//����
{
	CControlUI* pobjControl1 = (CControlUI*)(item1Addr);
	CControlUI* pobjControl2 = (CControlUI*)(item2Addr);
	CListTextElementUI* pobjItem1 = static_cast<CListTextElementUI*>(pobjControl1->GetInterface(DUI_CTR_LISTELEMENT));
	CListTextElementUI* pobjItem2 = static_cast<CListTextElementUI*>(pobjControl2->GetInterface(DUI_CTR_LISTELEMENT));

	_ASSERT((NULL != pobjItem1) && (NULL != pobjItem2));

	return _ttoi(pobjItem1->GetText(CompareData)) - _ttoi(pobjItem2->GetText(CompareData));
}

int CALLBACK CCtrlList::CompareItemLow(UINT_PTR item1Addr, UINT_PTR item2Addr, UINT_PTR CompareData)//����
{
	CControlUI* pobjControl1 = (CControlUI*)(item1Addr);
	CControlUI* pobjControl2 = (CControlUI*)(item2Addr);
	CListTextElementUI* pobjItem1 = static_cast<CListTextElementUI*>(pobjControl1->GetInterface(DUI_CTR_LISTELEMENT));
	CListTextElementUI* pobjItem2 = static_cast<CListTextElementUI*>(pobjControl2->GetInterface(DUI_CTR_LISTELEMENT));

	_ASSERT((NULL != pobjItem1) && (NULL != pobjItem2));

	return _ttoi(pobjItem2->GetText(CompareData)) - _ttoi(pobjItem1->GetText(CompareData));
}