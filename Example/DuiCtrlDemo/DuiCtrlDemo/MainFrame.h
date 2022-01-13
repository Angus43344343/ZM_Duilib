#pragma once

#include <vector>

#include "CtrlBasics.h"
#include "CtrlSenior.h"
#include "CtrlList.h"
#include "CtrlActiveX.h"
#include "CtrlControlEx.h"

class CMainFrame : public CWndImplBase, public IObserver, public ICallBackTray
{
public:
	//��Դ�ļ�����
	virtual LPCTSTR GetWindowClassName(void) const override;
	virtual CDuiString GetSkinFolder() override;
	virtual CDuiString GetSkinFile() override;

	//���岼�ֵ�һ����ȷ��ʼ������õĽӿڣ���ʱ���ڽ��ύˢ��/�������ڴ�����ʾǰ����
	virtual void OnPrepare(void) override;

	//��ʼ������
	virtual void OnInitWindow(void) override;

	//����ر���Ϣ����
	virtual void OnFinalMessage(HWND hWnd) override;

	//uilib �ؼ�֪ͨ��Ϣ
	virtual void Notify(TNotifyUI &msg) override;

	//������Ϣ���͵����ڹ���ǰ�Ĺ��˴���
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool &bHandled) override;

	//ϵͳ���̵Ĵ�����Ϣ
	virtual HRESULT MessageTray(CSystemTray* pobjSystemTray, WPARAM wParam, LPARAM lParam) override;


	//����۲����仯�¼�
	virtual bool OnSubjectUpdate(WPARAM p1, WPARAM p2 = NULL, LPARAM p3 = NULL, CSubjectBase *pSub = NULL) override;

	//���ⴰ�ڹ���
	void RegisterVirtualWnd(LPCTSTR strName, CVirtualWnd* pobjVirtualWnd);
	void UnRegisterVirtualWnd();

private:
	void OnInitSystemTray();
	void OnMenuClick(TNotifyUI& msg);
	void OnCreateMenu(TNotifyUI& msg);
	void OnBkSkin(TNotifyUI& msg);

	void CheckProExist();
private:
	void SetLanguage(int nLangType, const LPCTSTR xml, LPCTSTR szResType = NULL);

//private:
//	void OnTabOption(TNotifyUI &msg);

public:
	CMainFrame();
	virtual ~CMainFrame();

private:
	std::vector<CVirtualWnd*> m_vecVirtualWnd;

	CSystemTray	m_objSysTray;// ϵͳ����ͼ��
};

