 /*
@2021-09-07 zm
@���ⴰ�����ڴ����ģ�黯����ʹ�߼���θ�������ʹ�÷�����
@1. XML���֣�����XML�� Include source="virtualWnd.xml"
@2. ���벼�֣����������RegisterVirtualWnd, AddDuiMessage, UnRegisterVirtualWnd
@		    ���ⴰ�ڽ�����룺Notify, MessageHandler��ػ���Ϣ���д���
*/

#pragma once

namespace DuiLib {
class CVirtualWnd : public CNotifyPump, public INotifyUI, public IMessageFilterUI
{
public:
	/*@ע�����ⴰ��*/
	virtual void RegisterVirtualWnd(LPCTSTR strName)
	{
		m_strName = strName;
		AddVirtualWnd(m_strName, this);
	}
	
	/*@ע�����ⴰ��*/
	virtual void UnRegisterVirtualWnd() { RemoveVirtualWnd(m_strName); }

	/*@��Ӵ�����Ϣ�����ⴰ��*/
	virtual void AddDuiMessage()
	{
		if (nullptr == m_pobjManager)return;

		m_pobjManager->AddNotifier(this);
		m_pobjManager->AddMessageFilter(this);
	}

	/*@�Ƴ�������Ϣ�����ⴰ��*/
	virtual void RemoveDuiMessage()
	{
		if (nullptr == m_pobjManager)return;

		m_pobjManager->RemoveNotifier(this);
		m_pobjManager->RemoveMessageFilter(this);
	}

	/*@Duilib�ڲ���Ϣ��Ӧ*/
	virtual void Notify(TNotifyUI& msg) = 0;

	/*@Windows������Ϣ��Ӧ*/
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) = 0;

public:
	CVirtualWnd() : m_strName(nullptr), m_pobjManager(nullptr) {}
	CVirtualWnd(CPaintManagerUI* pobjManager) : m_strName(nullptr), m_pobjManager(pobjManager) {}
	virtual ~CVirtualWnd() = default;

protected:
	CDuiString m_strName;
	CPaintManagerUI* m_pobjManager;
};
}