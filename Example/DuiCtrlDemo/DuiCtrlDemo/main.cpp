#include "stdafx.h"
#include "MainFrame.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	//�ڴ���
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);//�����˳����Զ���ӡ�ڴ�й©�Ŀ���Ϣ
	//_CrtSetBreakAlloc(6565);//��ָ�����ڴ������ʱ�ϵ�

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin//duictrldemo"));

	//������ڶ�������л�,����Ҫ��ʼ���������Ա�
	CPaintManagerUI::LoadLanguage(EN_LANG_TYPE_CHINESE, _T("language/chinese.xml"), _T("xml"));

	CMainFrame* pobjMainFrame = new CMainFrame();
	ASSERT(nullptr != pobjMainFrame);

	pobjMainFrame->Create(NULL, _T("Duilib control demo"), UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_FRAME);
	pobjMainFrame->CenterWindow();
	pobjMainFrame->ShowWindow(true);

	//��Ϣѭ��
	CPaintManagerUI::MessageLoop();

	//�������ʱ������Դ
	CPaintManagerUI::Term();

	return 0;
}



