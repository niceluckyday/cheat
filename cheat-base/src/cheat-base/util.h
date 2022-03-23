#pragma once
#include <string>
#include <stdexcept>
#include <memory>

#include <SimpleIni.h>

#define LOG_LAST_ERROR(msg) LOG_ERROR("%s. Error: %s", msg, util::GetLastErrorAsString().c_str())

namespace util 
{
	std::string SelectFile(const char* filter, const char* title);
	std::string SelectDirectory(const char* title);
	std::string GetOrSelectPath(CSimpleIni& ini, const char* section, const char* name, const char* friendName, const char* filter);

	std::string GetLastErrorAsString();
	bool GetResourceMemory(HINSTANCE hInstance, int resId, LPBYTE& pDest, DWORD& size);
	int64_t GetCurrentTimeMillisec();

	std::string to_hex_string(uint8_t* barray, int length);

	template<typename ... Args>
	std::string string_format(const std::string& format, Args ... args)
	{
		int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
		if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
		auto size = static_cast<size_t>(size_s);
		auto buf = std::make_unique<char[]>(size);
		std::snprintf(buf.get(), size, format.c_str(), args ...);
		return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	}
}
