﻿#pragma once

#ifndef _GEAR_UTILS_FILE_H_
#define _GEAR_UTILS_FILE_H_

#include<string>

#include<vector>

#define NOMINMAX

namespace Gear
{
	namespace Utils
	{
		namespace File
		{

			std::wstring getRootFolder();

			std::wstring backslashToSlash(const std::wstring& filePath);

			std::wstring getParentFolder(const std::wstring& filePath);

			std::wstring getExtension(const std::wstring& filePath);

			std::wstring readAllText(const std::wstring& filePath);

			std::vector<uint8_t> readAllBinary(const std::wstring& filePath);

		}
	}
}

#endif // !_GEAR_UTILS_FILE_H_
