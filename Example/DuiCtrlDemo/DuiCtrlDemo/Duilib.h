#ifndef _DUILIB_H__
#define _DUILIB_H__

#include <mutex>

#include "UIlib.h"
using namespace DuiLib;

#ifdef _DEBUG
#	ifdef _UNICODE
#		pragma comment(lib, "DuiLib_ud.lib")
#	else
#		pragma comment(lib, "DuiLib_d.lib")
#	endif
#else
#	ifdef _UNICODE
#		pragma comment(lib, "DuiLib_u.lib")
#	else
#		pragma comment(lib, "DuiLib.lib")
#	endif
#endif

#include "mutex.h"		//添加互斥锁
#include "fileUtil.h"

//定义虚拟窗口
#define DUI_VIRTUAL_WND_BASICS		_T("pageBasics")
#define DUI_VIRTUAL_WND_SENIOR		_T("pageSenior")
#define DUI_VIRTUAL_WND_LIST		_T("pageList")
#define DUI_VIRTUAL_WND_ACTIVEX		_T("pageActiveX")
#define DUI_VIRTUAL_WND_CONTROLEX	_T("pageControlEx")

//定义语言类型
typedef enum
{
	EN_LANG_TYPE_CHINESE = 1,
	EN_LANG_TYPE_ENGLISH,
	EN_LANG_TYPE_JAPANESE,
}EN_LANG_TYPE;

#endif /*_DUILIB_H__*/