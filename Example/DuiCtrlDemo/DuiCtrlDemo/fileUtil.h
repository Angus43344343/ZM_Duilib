/*
@ 2021-09-08  zm
@ 文件传输和文件读写时统一采用多节点的方式来操作,减少传输量
@ 注意文件格式也要是ANSI,与代码操作保持一致
@ 界面操作时尽量采用Unicode方式编码
@ 宽窄字节的转换: ATL::CW2A objW2A(wchar_t*) 和 ATL::CA2W objA2W(char*) 直接使用
*/

#ifndef _FILEUTIL_H__
#define _FILEUTIL_H__

#include <Windows.h>
#include <ShlObj.h>
#include <commdlg.h>
#include <atlbase.h> 
#include <atlstr.h>

class ICallBackFile
{
public:
	virtual void CallBackEnumFiles(LPCTSTR lpFileName) = 0;
	virtual void CallBackOpenDlgFile(LPCTSTR lpFileName) = 0;
	virtual void CallBackOpenDlgDir(LPCTSTR lpDirName) = 0;
public:
	ICallBackFile() = default;
	virtual ~ICallBackFile() = default;
};

class CFileUtil
{
public:
	/*文件读写操作都是原子操作,使用单例来降低被重复并行操作的风险*/
	static CFileUtil& GetInstance() { static CFileUtil objFileUtil; return objFileUtil; }

	/*创建文件*/
	HANDLE CreateFile(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwCreationDisposition,
		DWORD dwShareMode = 0, LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr,
		DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL, HANDLE hTemplateFile = nullptr);

	/*写文件*/
	BOOL WriteFile(HANDLE hFile,LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
		LPOVERLAPPED lpOverlapped = nullptr);

	/*读文件*/
	BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
		LPOVERLAPPED lpOverlapped = nullptr);

	/*关闭文件句柄*/
	BOOL CloseHandle( HANDLE hObject );

	/*获取文件大小*/
	DWORD GetFileSize(HANDLE  hFile, LPDWORD lpFileSizeHigh = nullptr);

	/*设置文件指针位置*/
	BOOL SetFilePointer(HANDLE hFile, LONGLONG llOffset, DWORD dwMoveMethod, PLARGE_INTEGER lpNewFilePointer = nullptr);

	/*删除文件*/
	BOOL DeleteFile(LPCTSTR lpFileName);

	/*获取文件后缀名：从完整的文件名路径中分离出后缀名*/
	BOOL PathFindFileExt(PTSTR ptFilePath, PTSTR ptExtBuf, UINT uiBufSize);

	/*获取文件名：从完整的文件名路径中分离出文件名*/
	PTSTR PathFindFileName(PTSTR ptFilePath);

	/*文件夹的创建*/
	BOOL CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr);

	/*文件夹的删除*/
	BOOL RemoveDirectory(LPCTSTR lpPathName);

	/*文件遍历：遍历指定目录下所有指定类型的文件(包含子目录的遍历)*/
	void EnumFile(TCHAR* pcDirPath, TCHAR* pcFileExt);

	/*选择文件:打开文件的对话框*/
	void OpenDlgFile(HWND hWnd = nullptr, DWORD dwFlags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, LPCTSTR lpFileType = _T("*.*"));

	/*选择文件夹：打开文件夹的对话框*/
	void OpenDlgDir(LPCTSTR lpTitle = _T("选择文件夹"), UINT uiFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_USENEWUI);

public:
	ICallBackFile* GetCallbackFile() const;
	void SetCallbackFile(ICallBackFile* pCallbackFile);

private:
	ICallBackFile* m_pCallBackFile;

public:
	CFileUtil() : m_pCallBackFile(nullptr) {};
	CFileUtil(const CFileUtil&) = delete;//禁类赋值
	CFileUtil& operator=(const CFileUtil&) = delete;//禁类拷贝

	~CFileUtil() = default;
};

#endif /*_FILEUTIL_H__*/