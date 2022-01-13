#include "stdafx.h"
#include "fileUtil.h"

#include <shlwapi.h>

HANDLE CFileUtil::CreateFile(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwCreationDisposition,
	DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	return ::CreateFile(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

BOOL CFileUtil::WriteFile(HANDLE hFile, LPCVOID lpBuffer, DWORD nNumberOfBytesToWrite, LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped)
{
	return ::WriteFile(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}

BOOL CFileUtil::ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead,
	LPOVERLAPPED lpOverlapped)
{
	return ::ReadFile(hFile, lpBuffer, nNumberOfBytesToRead, lpNumberOfBytesRead, lpOverlapped);
}

BOOL CFileUtil::CloseHandle(HANDLE hObject)
{
	return ::CloseHandle(hObject);
}

DWORD CFileUtil::GetFileSize(HANDLE  hFile, LPDWORD lpFileSizeHigh)
{
	return ::GetFileSize(hFile, lpFileSizeHigh);
}

BOOL CFileUtil::SetFilePointer(HANDLE hFile, LONGLONG llOffset, DWORD dwMoveMethod, PLARGE_INTEGER lpNewFilePointer)
{
	LARGE_INTEGER stOffset;
	stOffset.QuadPart = llOffset;
	return ::SetFilePointerEx(hFile, stOffset, lpNewFilePointer, dwMoveMethod);
}

BOOL CFileUtil::DeleteFile(LPCTSTR lpFileName)
{
	if (-1 != _taccess(lpFileName, 0))
	{
		return ::DeleteFile(lpFileName);
	}

	return FALSE;
}

BOOL CFileUtil::PathFindFileExt(PTSTR ptFilePath, PTSTR ptExtBuf, UINT uiBufSize)
{
	if (-1 != _taccess(ptFilePath, 0))
	{
		return (0 == ::_tsplitpath_s(ptFilePath, nullptr, 0, nullptr, 0, nullptr, 0, ptExtBuf, uiBufSize)) ? TRUE : FALSE;
	}

	return FALSE;
}

PTSTR CFileUtil::PathFindFileName(PTSTR ptFilePath)
{
	if (-1 != _taccess(ptFilePath, 0))
	{
		return ::PathFindFileName(ptFilePath);
	}

	return nullptr;
}

BOOL CFileUtil::CreateDirectory(LPCTSTR lpPathName, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	if (!::PathIsDirectory(lpPathName))//检查指定的目录是否存在
	{
		return ::CreateDirectory(lpPathName, lpSecurityAttributes);
	}

	return FALSE;
}

BOOL CFileUtil::RemoveDirectory(LPCTSTR lpPathName)
{
	if (::PathIsDirectory(lpPathName))//检查指定的目录是否存在
	{
		return ::RemoveDirectory(lpPathName);
	}
	return FALSE;
}

void CFileUtil::EnumFile(TCHAR* pcDirPath, TCHAR* pcFileExt)
{
	if ((nullptr == pcDirPath) || (nullptr == pcFileExt)) return;

	WIN32_FIND_DATA fd = { 0 };
	TCHAR acDirPath[MAX_PATH] = { 0 };
	_stprintf_s(acDirPath, _T("%s\\%s"), pcDirPath, pcFileExt);

	HANDLE hFind = ::FindFirstFile(acDirPath, &fd);
	if (hFind == INVALID_HANDLE_VALUE) return;

	BOOL bRet = FALSE;
	do
	{
		bRet = ::FindNextFile(hFind, &fd);
		if (FALSE == bRet) break;

		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)//如果为目录
		{
			if (_tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0) continue;

			TCHAR acCurPath[MAX_PATH] = { 0 };
			_stprintf_s(acCurPath, _T("%s\\%s"), pcDirPath, fd.cFileName);
			CFileUtil::EnumFile(acCurPath, pcFileExt);//子目录继续遍历
		}
		else// 如果为文件
		{
			if (nullptr != m_pCallBackFile) m_pCallBackFile->CallBackEnumFiles(fd.cFileName);
		}

	} while (bRet);

	::FindClose(hFind);
}

void CFileUtil::OpenDlgFile(HWND hWnd, DWORD dwFlags, LPCTSTR lpFileType)
{
	OPENFILENAME ofn;
	TCHAR acFile[MAX_PATH] = {0};
	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = acFile;
	ofn.nMaxFile = sizeof(acFile);
	ofn.lpstrFilter = lpFileType;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = dwFlags;

	if (GetOpenFileName(&ofn))
	{
		if (nullptr != m_pCallBackFile) m_pCallBackFile->CallBackOpenDlgFile(acFile);
	}
}

void CFileUtil::OpenDlgDir(LPCTSTR lpTitle, UINT uiFlags)
{
	BROWSEINFO stDirInfo;
	TCHAR acDirPath[MAX_PATH] = { 0 };
	::ZeroMemory(&stDirInfo, sizeof(stDirInfo));

	stDirInfo.pidlRoot = 0;
	stDirInfo.lpszTitle = lpTitle;
	stDirInfo.ulFlags = uiFlags;
	stDirInfo.lpfn = nullptr;

	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&stDirInfo);
	if (nullptr != lpidlBrowse)
	{
		if (::SHGetPathFromIDList(lpidlBrowse, acDirPath))
		{
			if (nullptr != m_pCallBackFile) m_pCallBackFile->CallBackOpenDlgDir(acDirPath);
		}

		::CoTaskMemFree(lpidlBrowse);
	}
}

ICallBackFile* CFileUtil::GetCallbackFile() const
{
	return m_pCallBackFile;
}

void CFileUtil::SetCallbackFile(ICallBackFile* pCallbackFile)
{
	if (nullptr != pCallbackFile) return;

	m_pCallBackFile = pCallbackFile;
}