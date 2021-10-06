#include "stdafx.h"
#include "MainFrame.h"

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	//内存检测
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);//程序退出后自动打印内存泄漏的块信息
	//_CrtSetBreakAlloc(6565);//在指定的内存申请点时断点

	CPaintManagerUI::SetInstance(hInstance);
	CPaintManagerUI::SetResourcePath(CPaintManagerUI::GetInstancePath() + _T("skin//duictrldemo"));

	//如果存在多国语言切换,就需要初始化加载语言表
	CPaintManagerUI::LoadLanguage(EN_LANG_TYPE_CHINESE, _T("language/chinese.xml"), _T("xml"));

	CMainFrame* pobjMainFrame = new CMainFrame();
	ASSERT(nullptr != pobjMainFrame);

	pobjMainFrame->Create(NULL, _T("Duilib control demo"), UI_WNDSTYLE_FRAME, UI_WNDSTYLE_EX_FRAME);
	pobjMainFrame->CenterWindow();
	pobjMainFrame->ShowWindow(true);

	//消息循环
	CPaintManagerUI::MessageLoop();

	//程序结束时销毁资源
	CPaintManagerUI::Term();

	return 0;
}



