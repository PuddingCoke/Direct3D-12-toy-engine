#pragma once

#ifndef _UTILS_H_
#define _UTILS_H_

#define NOMINMAX

#include"Logger.h"

#include<string>

#include<vector>

class Utils
{
public:

	Utils() = delete;

	Utils(const Utils&) = delete;

	static std::wstring getRootFolder();

	class File
	{
	public:

		File() = delete;

		File(const File&) = delete;

		static std::wstring backslashToSlash(const std::wstring& filePath);

		static std::wstring getParentFolder(const std::wstring& filePath);

		static std::wstring getExtension(const std::wstring& filePath);

		static std::wstring readAllText(const std::wstring& filePath);

		static std::vector<uint8_t> readAllBinary(const std::wstring& filePath);

	};

private:

	friend class Gear;

	static std::wstring exeRootPath;

};

#endif // !_UTILITY_H_