 /*
@2021-09-07 zm
@虚拟窗口用于代码的模块化处理，使逻辑层次更清晰，使用方法：
@1. XML布局：在主XML里 Include source="virtualWnd.xml"
@2. 代码布局：主界面代码RegisterVirtualWnd, AddDuiMessage, UnRegisterVirtualWnd
@		    虚拟窗口界面代码：Notify, MessageHandler里截获消息进行处理
*/

#pragma once

namespace DuiLib {
class CVirtualWnd : public CNotifyPump, public INotifyUI, public IMessageFilterUI
{
public:
	/*@注册虚拟窗口*/
	virtual void RegisterVirtualWnd(LPCTSTR strName)
	{
		m_strName = strName;
		AddVirtualWnd(m_strName, this);
	}
	
	/*@注销虚拟窗口*/
	virtual void UnRegisterVirtualWnd() { RemoveVirtualWnd(m_strName); }

	/*@添加窗口消息到虚拟窗口*/
	virtual void AddDuiMessage()
	{
		if (nullptr == m_pobjManager)return;

		m_pobjManager->AddNotifier(this);
		m_pobjManager->AddMessageFilter(this);
	}

	/*@移除窗口消息到虚拟窗口*/
	virtual void RemoveDuiMessage()
	{
		if (nullptr == m_pobjManager)return;

		m_pobjManager->RemoveNotifier(this);
		m_pobjManager->RemoveMessageFilter(this);
	}

	/*@Duilib内部消息响应*/
	virtual void Notify(TNotifyUI& msg) = 0;

	/*@Windows窗口消息响应*/
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