﻿#include<Gear/Utils/File.h>

#include<Gear/Utils/Internal/FileInternal.h>

#include<Gear/Utils/Logger.h>

#include<fstream>

#include<sstream>

#include<algorithm>

#include<vector>

namespace
{
    struct FilePrivate
    {
        std::wstring rootFolder;
    }pvt;
}

std::wstring Utils::File::getRootFolder()
{
    return pvt.rootFolder;
}

std::wstring Utils::File::backslashToSlash(const std::wstring& filePath)
{
	std::wstring result = filePath;

    std::replace(result.begin(), result.end(), L'\\', L'/');

	return result;
}

std::wstring Utils::File::getParentFolder(const std::wstring& filePath)
{
	size_t idx = filePath.find_last_of(L'\\');

	if (idx == std::wstring::npos)
	{
		idx = filePath.find_last_of(L'/');

		if (idx == std::wstring::npos)
		{
            return L"";
		}
	}

	return filePath.substr(0, idx + 1);
}

std::wstring Utils::File::getExtension(const std::wstring& filePath)
{
	const size_t idx = filePath.find_last_of(L'.');

    if (idx == std::wstring::npos)
    {
        return L"";
    }

	return filePath.substr(idx + 1, filePath.size() - idx - 1);
}

std::wstring Utils::File::readAllText(const std::wstring& filePath)
{
    std::wifstream file(filePath);

    if (!file.is_open())
    {
        LOGERROR(L"open file", filePath, L"failed");
    }

    std::wstringstream stringStream;

    stringStream << file.rdbuf();

    return stringStream.str();
}

std::vector<uint8_t> Utils::File::readAllBinary(const std::wstring& filePath)
{
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        LOGERROR(L"open file", filePath, L"failed");
    }

    const size_t fileSize = static_cast<size_t>(file.tellg());

    std::vector<uint8_t> bytes = std::vector<uint8_t>(fileSize);

    file.seekg(0);

    file.read(reinterpret_cast<char*>(bytes.data()), fileSize);

    file.close();

    return bytes;
}

void Utils::File::Internal::setRootFolder(const std::wstring& rootFolder)
{
    pvt.rootFolder = rootFolder;
}