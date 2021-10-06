#pragma once

#include "Duilib.h"

//��ӻ����۲����
class CSubjectSkin : public CSubjectBase
{
public:
	static CSubjectSkin& GetInstance() { static CSubjectSkin s_objSubjectSkin; return s_objSubjectSkin; }

public:
	CSubjectSkin(void){};
	CSubjectSkin(CSubjectSkin&) = delete;
	CSubjectSkin& operator=(CSubjectSkin&) = delete;
};


//=====================================================
class CBkSkin : public CWndImplBase
{
public:
	//���廻��������ͼ�򱳾�ɫ
	enum{ EN_SKIN_BKCOLOR = 1, EN_SKIN_BKIMAGE };

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

	//��ʧ���㣬�͹رմ���
	virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled) override;

public:
	CBkSkin(const RECT& rectControl, HWND hWnd) : m_rectControl(rectControl), m_hParent(hWnd) {}
	virtual ~CBkSkin() = default;

private:
	HWND m_hParent;
	RECT m_rectControl;
};
