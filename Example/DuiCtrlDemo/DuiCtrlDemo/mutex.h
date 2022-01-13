#pragma once

#include <mutex>

class CMutex
{
public:
	static CMutex& GetInstance() { static CMutex s_mutex; return s_mutex; }
	std::mutex& GetMutex() { return m_mutex; }

public:
	CMutex() = default;
	CMutex(CMutex&) = delete;
	CMutex& operator=(CMutex&) = delete;

private:
	std::mutex m_mutex;
};