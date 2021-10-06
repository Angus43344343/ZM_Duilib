#pragma once

#include "Duilib.h"

class CWndModal : public CWndImplBase
{
public:
	//��Դ�ļ�����
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;

	//��ʼ������
	virtual void OnInitWindow(void) override;

	//����ر���Ϣ����
	virtual void OnFinalMessage(HWND hWnd) override;

	// INotifyUI �ӿڣ�duilib �ؼ�֪ͨ��Ϣ
	virtual void Notify(TNotifyUI &msg) override;

	//IMessageFilterUI �ӿڣ�������Ϣ���͵����ڹ���ǰ�Ĺ��˴���
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;

public:
	CWndModal() = default;
	virtual ~CWndModal() = default;
};

//====================================================================================
class CWndNonModal : public CWndImplBase
{
public:
	//��Դ�ļ�����
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;

	//��ʼ������
	virtual void OnInitWindow(void) override;

	//����ر���Ϣ����
	virtual void OnFinalMessage(HWND hWnd) override;

	// INotifyUI �ӿڣ�duilib �ؼ�֪ͨ��Ϣ
	virtual void Notify(TNotifyUI &msg) override;

	//IMessageFilterUI �ӿڣ�������Ϣ���͵����ڹ���ǰ�Ĺ��˴���
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;

public:
	CWndNonModal() = default;
	virtual ~CWndNonModal() = default;
};



