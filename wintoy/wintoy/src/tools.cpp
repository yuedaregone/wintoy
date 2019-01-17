#include "tools.h"
#include <windows.h>

std::string GetPath(std::string p)
{
	return p.substr(0, p.find_last_of("\\"));
}

std::string GetTotalName(std::string p)
{
	return p.substr(p.find_last_of("/") + 1, p.length());
}

std::string GetOnlyNameFromTotalName(std::string name)
{
	return name.substr(0, name.find_last_of("."));
}

std::string GetHouzhui(std::string p)
{
	return p.substr(p.find_last_of("."), p.length());
}

std::string GetCurrentPath()
{
	char buff[512] = { 0 };
	GetCurrentDirectory(512, buff);
	return buff;
}

void LoadAllFileNames(const char* path, std::vector<std::string>& files, bool subdir, const char* subfix)
{
	std::string findPath = path;
	findPath += "\\*.*";

	WIN32_FIND_DATA findData;	
	HANDLE handle = FindFirstFile(findPath.c_str(), &findData);
	if (handle == INVALID_HANDLE_VALUE)
		return;

	do 
	{
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!subdir)
			{
				continue;
			}
			if (strcmp(findData.cFileName, ".") == 0 || strcmp(findData.cFileName, "..") == 0)
			{
				continue;
			}
			std::string strpath = path;
			strpath += "\\";
			strpath += findData.cFileName;
			LoadAllFileNames(strpath.c_str(), files, subdir, subfix);
		}
		else
		{			
			std::string houzhui = GetHouzhui(findData.cFileName);
			if (subfix == NULL || strcmp(houzhui.c_str(), subfix) == 0)
			{
				std::string strpath = path;
				strpath += "\\";
				strpath += findData.cFileName;
				files.push_back(strpath);
			}				
		}
	} while (FindNextFile(handle, &findData));
	FindClose(handle);
}

