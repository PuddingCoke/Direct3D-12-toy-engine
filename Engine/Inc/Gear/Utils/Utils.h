#pragma once

#ifndef _UTILS_H_
#define _UTILS_H_

#define NOMINMAX

#include<string>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<Windows.h>
#include<dxgiformat.h>

class Utils
{
public:

	Utils() = delete;

	Utils(const Utils&) = delete;

	static std::string getRootFolder();
	
	static uint32_t getPixelSize(const DXGI_FORMAT format);

	class File
	{
	public:

		File() = delete;

		File(const File&) = delete;

		static std::string backslashToSlash(const std::string& filePath);

		static std::string getParentFolder(const std::string& filePath);

		static std::string getExtension(const std::string& filePath);

		static std::string readAllText(const std::string& filePath);

	};

private:

	friend class Gear;

	static std::string exeRootPath;

};

#endif // !_UTILITY_H_