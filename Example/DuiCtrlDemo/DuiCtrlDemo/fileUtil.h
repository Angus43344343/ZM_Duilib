/*
@ 2021-09-08  zm
@ �ļ�������ļ���дʱͳһ���ö�ڵ�ķ�ʽ������,���ٴ�����
@ ע���ļ���ʽҲҪ��ANSI,������������һ��
@ �������ʱ��������Unicode��ʽ����
@ ��խ�ֽڵ�ת��: ATL::CW2A objW2A(wchar_t*) �� ATL::CA2W objA2W(char*) ֱ��ʹ��
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
	/*�ļ���д��������ԭ�Ӳ���,ʹ�õ��������ͱ��ظ����в����ķ���*/
	static CFileUtil& GetInstance() { static CFileUtil objFileUtil; return objFileUtil; }

	/*�����ļ�*/
	HANDLE CreateFile(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwCreationDisposition,
		DWORD dwShareMode = 0, LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr,
		DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL, HANDLE hTemplateFile = nullptr);

	/*д�ļ�*/
	BOOL WriteFile(HANDLE hFile,LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
		LPOVERLAPPED lpOverlapped = nullptr);

	/*���ļ�*/
	BOOL ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
		LPOVERLAPPED lpOverlapped = nullptr);

	/*�ر��ļ����*/
	BOOL CloseHandle( HANDLE hObject );

	/*��ȡ�ļ���С*/
	DWORD GetFileSize(HANDLE  hFile, LPDWORD lpFileSizeHigh = nullptr);

	/*�����ļ�ָ��λ��*/
	BOOL SetFilePointer(HANDLE hFile, LONGLONG llOffset, DWORD dwMoveMethod, PLARGE_INTEGER lpNewFilePointer = nullptr);

	/*ɾ���ļ�*/
	BOOL DeleteFile(LPCTSTR lpFileName);

	/*��ȡ�ļ���׺�������������ļ���·���з������׺��*/
	BOOL PathFindFileExt(PTSTR ptFilePath, PTSTR ptExtBuf, UINT uiBufSize);

	/*��ȡ�ļ��������������ļ���·���з�����ļ���*/
	PTSTR PathFindFileName(PTSTR ptFilePath);

	/*�ļ��еĴ���*/
	BOOL CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr);

	/*�ļ��е�ɾ��*/
	BOOL RemoveDirectory(LPCTSTR lpPathName);

	/*�ļ�����������ָ��Ŀ¼������ָ�����͵��ļ�(������Ŀ¼�ı���)*/
	void EnumFile(TCHAR* pcDirPath, TCHAR* pcFileExt);

	/*ѡ���ļ�:���ļ��ĶԻ���*/
	void OpenDlgFile(HWND hWnd = nullptr, DWORD dwFlags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST, LPCTSTR lpFileType = _T("*.*"));

	/*ѡ���ļ��У����ļ��еĶԻ���*/
	void OpenDlgDir(LPCTSTR lpTitle = _T("ѡ���ļ���"), UINT uiFlags = BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_USENEWUI);

public:
	ICallBackFile* GetCallbackFile() const;
	void SetCallbackFile(ICallBackFile* pCallbackFile);

private:
	ICallBackFile* m_pCallBackFile;

public:
	CFileUtil() : m_pCallBackFile(nullptr) {};
	CFileUtil(const CFileUtil&) = delete;//���ำֵ
	CFileUtil& operator=(const CFileUtil&) = delete;//���࿽��

	~CFileUtil() = default;
};

#endif /*_FILEUTIL_H__*/